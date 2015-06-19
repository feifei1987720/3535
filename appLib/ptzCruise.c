#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include "global.h"
#include "sdk_struct.h"
#include "main.h"
#include "commonLib.h"
#include "databaseLib.h"
#include "midWare.h"
#include "onvif_client.h"
#include "myThreadLib.h"



static sdk_cruise_line_t  gp_ptzCruise[MAX_DISPLAY_NUM];
static int	g_ptzCruiseThreadStatus[MAX_DISPLAY_NUM];  


#define THREAD_FLAG_STOP			 -1 
#define THREAD_FLAG_START			 1 	
#define THREAD_FLAG_RUN 	  		 2
#define THREAD_FLAG_PAUSE			 3 
#define THREAD_FLAG_EXIT			 4

static pthread_mutex_t g_CruiseMutex;

static pthread_mutex_t g_CruiseStatusMutex;


int g_ptzchannelstatus[MAX_DISPLAY_NUM];

int ptzInitCruiseInfo( int channel )
{
	int  chno = channel;   
	return 	databaseOperate(SDK_MAIN_MSG_CRUISECFG, SDK_PARAM_GET, &chno, sizeof(chno), 
			(void*)&gp_ptzCruise[channel].cruise_line,sizeof(sdk_cruise_line_t));
	return 0;

}	

int ptzSetCruiseDefault(int channel)
{
	int i,j ;
	sdk_cruise_param_t  cruise_param;
	memset(&cruise_param,0,sizeof(sdk_cruise_param_t));
	
	for(i=0;i!=MAX_CRUISE_LINE_NUM;++i)
	{
	      for(j=0;j!=MAX_CRUISE_PRESET_NUM ;++j)
		{
                 cruise_param.cruise_preset[j].num = j +1;
	       }
		cruise_param.line = i;
		databaseOperate(SDK_MAIN_MSG_CRUISECFG, SDK_PARAM_SET, &channel, sizeof(channel), 
				(void*)&cruise_param, sizeof(sdk_cruise_param_t) );
		usleep(100);
	} 
	return 0;
}

int ptzSetPresetDefault(int channel)
{
	int  chno = channel;
	sdk_preset_t preset_param;
	memset(&preset_param,0,sizeof(sdk_preset_t));
	int i;
	for(i=0;i!=MAX_PRESET_NUM;++i)
	{
		preset_param.num = i;
		databaseOperate(SDK_MAIN_MSG_ALLPRESETS, SDK_PARAM_SET, &channel, sizeof(channel), 
				(void*)&preset_param, sizeof(sdk_preset_t) );
		usleep(100);
	}
	return 0;
}


//获取所有的巡航 线信息
int ptzInitPresetsInfo( int channel )
{
	int  chno = channel;
	databaseOperate(SDK_MAIN_MSG_ALLPRESETS, SDK_PARAM_GET, &chno, sizeof(chno), 
			(void*)&gp_ptzCruise[channel].preset_all, sizeof(sdk_preset_all_t) ); 

	return 0;


}	

void ptzSetStatus(int channel ,int flag)
{
	pthread_mutex_lock(&g_CruiseStatusMutex);
	g_ptzchannelstatus[channel] = flag ;
	pthread_mutex_unlock(&g_CruiseStatusMutex);

}

void  ptzGetStatus(int channel , int * flag)
{
	pthread_mutex_lock(&g_CruiseStatusMutex);
	*flag = g_ptzchannelstatus[channel]  ;
	pthread_mutex_unlock(&g_CruiseStatusMutex);
}


//辅助接口      返回此通道使能的巡航路线，若返回零，表示此通道没有使能巡航路线
int GetEnableFlagFromMemory(int channel)
{
	if ( channel < 0 || channel >= MAX_DISPLAY_NUM) 
	{
		return THREAD_FLAG_STOP;		
	}
	int line = 0;   
	//pthread_mutex_lock(&g_CruiseMutex);
	for(line=0;line!=MAX_CRUISE_LINE_NUM  ;++line)
	{
		if(gp_ptzCruise[channel].cruise_line[line ].enable)
		{                
			// pthread_mutex_unlock(&g_CruiseMutex);
			return line;
		}      
	}
	// pthread_mutex_unlock(&g_CruiseMutex);
	return THREAD_FLAG_STOP;
}





//辅助接口
int getPtzToken(int channel, char *pToken, Remote_Device_Info *pRemoteIpc)
{
	Onvif_Ptz_Config ptzConfig;
	int ret = 0;
	Onvif_Profiles p_getProfilesRespone ;
	DbMainMsgChannelInfo(channel,pRemoteIpc->ip,&pRemoteIpc->port,pRemoteIpc->userName,pRemoteIpc->password);
	if(-1==ret)
	{
		Printf( " DbMainMsgChannelInfo    get channel failed\n");
		return -1;
	}
	// Printf( " ##############Channel: %d   ip: %s \r\n", channel, pRemoteIpc->ip );

	ONVIF_AddRemoteDevice(channel,pRemoteIpc);
	ONVIF_GetProfilesFromCache( pRemoteIpc, &p_getProfilesRespone);
	int sizeProfiles = p_getProfilesRespone.sizeOfProfiles;
	memset(&ptzConfig, 0, sizeof(Onvif_Ptz_Config));
	if ( sizeProfiles > 0 ) 
	{
		ret = ONVIF_PtzGetConfiguration(pRemoteIpc, &ptzConfig);	
		strncpy(pToken, p_getProfilesRespone.Onvif_Profiles[0].profileToken, sizeof(p_getProfilesRespone.Onvif_Profiles[0].profileToken));
	}				
	return ret;

}


//辅助接口
void GetPresetEnableFlag(int channel, int line ,int num , sdk_preset_param_t * preset_param)
{
	// Printf("channel =%d,enable =%d,line=%d ,pointnum=%d ,presetnum =%d\r\n",channel,gp_ptzCruise[channel].cruise_line[line].cruise_preset[num].enable,line,num,gp_ptzCruise[channel].cruise_line[line].cruise_preset[num].preset.num);
	//pthread_mutex_lock(&g_CruiseMutex);
	//strcpy(preset_param->name , gp_ptzCruise[channel].cruise_line[line-1].cruise_preset[num].name);
	preset_param->enable = gp_ptzCruise[channel].cruise_line[line].cruise_preset[num].enable;
	preset_param->num = gp_ptzCruise[channel].cruise_line[line].cruise_preset[num].preset.num;
	preset_param->dwell_time = gp_ptzCruise[channel].cruise_line[line].cruise_preset[num].dwell_time;
	preset_param->speed = gp_ptzCruise[channel].cruise_line[line].cruise_preset[num].speed;
	//pthread_mutex_unlock(&g_CruiseMutex);

}

static void * PtzCruiseThread(char *arg, int argLen)
{  
	unsigned int Channel = 0;
	if(argLen != sizeof(Channel))
	{
		Printf("thread arg len is error, %d.%d\r\n", argLen, sizeof(Channel));
	}
	memcpy(&Channel, arg, sizeof(Channel));
	int i;
	int flag = THREAD_FLAG_STOP;
	int flag1 = THREAD_FLAG_STOP;
	int status = 0;
	int ret = 0;
	int res = 0;
	char ptzToken[128] = { 0 };	
	Remote_Device_Info remoteIpc;
	memset(&remoteIpc, 0, sizeof(remoteIpc));
	char preSetToken[32];	
	do
	{
		res = getPtzToken(Channel, ptzToken, &remoteIpc);
		sleep(1);  
	}while(!res);
	sdk_preset_param_t  preset_param;
	setPthreadPriority(PTHREAD_SCHEDULED_PRIORITY_LOW_EST);
	while ( 1 )
	{
		flag = GetEnableFlagFromMemory(Channel);
		if ( flag >THREAD_FLAG_STOP )
		{     
			for (i = 0; i < MAX_CRUISE_PRESET_NUM; i++ )
			{
				ptzGetStatus(Channel,&ret);
				if(!ret )
				{
					flag1 = GetEnableFlagFromMemory(Channel);
					if ( flag ==flag1 )
					{     
						GetPresetEnableFlag( Channel, flag1 ,i , &preset_param);
						if (preset_param.enable) 
						{				           
							sprintf(preSetToken, "%d",preset_param.num);
							//Printf("%s:preset  %s  start  token =%s.........\r\n",remoteIpc.ip,preSetToken,ptzToken);
							ONVIF_PtzGotoPreset(&remoteIpc, ptzToken, preSetToken,preset_param.speed);
							if ( preset_param.dwell_time > 0) 
							{
								sleep(preset_param.dwell_time );
							}
							else
							{
								usleep(2000*1000);
							}
						}
						else
						{
							usleep(100*1000);
						}
					}
					else
					{
						usleep(500*1000);
						break;
					}
				}
				else
				{
					int j;
					for(j=0;j!=MAX_CRUISE_LINE_NUM;++j)
					{
						ChangeCruiseLineEnableFlag( Channel , j ,0);
					}
					ptzSetPresetDefault(Channel);
					ptzSetCruiseDefault(Channel);
					ptzSetStatus(Channel ,0);
					break;
					usleep(100);
				}	  
			}
			usleep(1000);
		}
		else
		{

			break;
		}
		usleep(1000);
	}
	Printf("PTZ cruise  thread  exit ................. \r\n");
	return NULL;
}

//辅助接口
int ChangeCruiseParamToDB(int channel, sdk_cruise_param_t  * cruise_param)
{
	if ( channel < 0 || channel >= MAX_DISPLAY_NUM) 
	{
		return THREAD_FLAG_STOP;		
	}

	int ret  = 0;
	ret = databaseOperate(SDK_MAIN_MSG_CRUISECFG, SDK_PARAM_SET, &channel, sizeof(channel), 
			(void*)cruise_param, sizeof(sdk_cruise_param_t) );
	return sizeof(sdk_cruise_param_t);

}

//设置某个通道的某一个预置点到数据库
int ChangePresetParamToDB(int channel, sdk_preset_t  * preset_param)
{
	if ( channel < 0 || channel >= MAX_DISPLAY_NUM) 
	{
		return THREAD_FLAG_STOP;		
	}
	int ret  = 0;
	ret = databaseOperate(SDK_MAIN_MSG_ALLPRESETS, SDK_PARAM_SET, &channel, sizeof(channel), 
			(void*)preset_param, sizeof(sdk_preset_t) );
	return ret;

}





//辅助接口
void ChangeCruiseLineEnableFlag(int channel ,int line , int flag)
{
	if ( channel < 0 || channel >= MAX_DISPLAY_NUM) 
	{
		return THREAD_FLAG_STOP;		
	}
	// pthread_mutex_lock(&g_CruiseMutex);
	gp_ptzCruise[channel].cruise_line[line ].enable = flag;
	// pthread_mutex_unlock(&g_CruiseMutex);
}

//辅助接口
int  ChangeCruiseLineParam(int channel ,int line ,sdk_cruise_param_t  * cruise_param)
{

	if ( channel < 0 || channel >= MAX_DISPLAY_NUM) 
	{
		return THREAD_FLAG_STOP;		
	}
	//pthread_mutex_lock(&g_CruiseMutex);
	memcpy(&gp_ptzCruise[channel].cruise_line[cruise_param->line ],cruise_param,sizeof(sdk_cruise_param_t));
	//pthread_mutex_unlock(&g_CruiseMutex);
	return 0 ;
}


//辅助接口 设置巡航参数到内存
int ChangeCruiseParamToMemory(int channel, sdk_cruise_param_t  * cruise_param)
{

	if ( channel < 0 || channel >= MAX_DISPLAY_NUM) 
	{
		return -1;
	}	

	int line = GetEnableFlagFromMemory(channel);
	if(line!=-1 &&line !=cruise_param->line)
	{  
		if(cruise_param->enable)
		{ 
			ChangeCruiseLineEnableFlag(channel,line,0);
			ChangeCruiseParamToDB(channel,&gp_ptzCruise[channel].cruise_line[line]) ;
			
		}
		  ChangeCruiseLineParam(channel,cruise_param->line,cruise_param);
		  ChangeCruiseParamToDB(channel,cruise_param) ;
		
	}
	else
	{
		ChangeCruiseLineParam(channel,cruise_param->line,cruise_param);
		  ChangeCruiseParamToDB(channel,cruise_param) ;
	} 
	return 0;
}



//更改巡航线路的参数，包括使能与否，预置点的添加和设置
int SetPtzCruiseAllPreset(int channel,sdk_cruise_param_t  * cruise_param )
{    
	if ( channel < 0 || channel >= MAX_DISPLAY_NUM) 
	{
		return THREAD_FLAG_STOP;		
	}

	int line = GetEnableFlagFromMemory(channel);
	ChangeCruiseParamToMemory(channel, cruise_param);
	if(-1==line && cruise_param->enable)
	{     
		if(threadPoolAddWorker(NULL, PtzCruiseThread, (char *)&channel, sizeof(channel)) != 0)
		{
			Printf("##############channel %d  ptz cruise thread start  err  !!\r\n ",channel+1);
			return  -1;
		}
		else
		{
			Printf("##############channel %d  ptz cruise thread start \r\n ",channel+1);
		}
	}

	return 0;
}

//设置预置点  ，channel  从零开始，preset->num从1开始
int SetPreset(int channel , sdk_preset_t * preset)
{
	int ret = 0;
	char ptzToken[128] = { 0 };	
	Remote_Device_Info remoteIpc;
	char preSetToken[32];	
	if(preset==NULL)
	{
		return -1;
	}
	memcpy(&gp_ptzCruise[channel].preset_all.preset[preset->num -1],preset,sizeof(sdk_preset_t));
	ChangePresetParamToDB(channel,preset);
	memset(&remoteIpc, 0, sizeof(remoteIpc));
#if 1
	getPtzToken(channel, ptzToken, &remoteIpc);
	sprintf(preSetToken, "%d",preset->num );	
	ret = ONVIF_PtzSetPreset(&remoteIpc, ptzToken ,preSetToken,preSetToken );
	if(ret != 1)
		return -1;
#endif

	return sizeof(sdk_preset_t);

}

//调用预置点
int RecallPreset(int channel ,sdk_preset_t *preset)
{      

	int ret = 0; 
	char ptzToken[128] = { 0 };	
	Remote_Device_Info remoteIpc;
	char preSetToken[32];	
	if(preset==NULL)
	{
		return -1;
	}
	if(!gp_ptzCruise[channel].preset_all.preset[preset->num-1].enable)
	{
		return -1;
	}
       
	memset(&remoteIpc, 0, sizeof(remoteIpc));
	getPtzToken(channel, ptzToken, &remoteIpc);
	sprintf(preSetToken, "%d",preset->num);
	ret = ONVIF_PtzGotoPreset(&remoteIpc, ptzToken, preSetToken,0);
	if(ret!=1)
		return -1;
	return sizeof(sdk_preset_t);
}

//获取某条巡航线路所有的预设点
int GetPtzCruiseAllPreset(int channel,sdk_cruise_param_t * cruise_param)
{
	if ( channel < 0 || channel >= MAX_DISPLAY_NUM) 
	 {
		 return THREAD_FLAG_STOP;		
	 }
       ptzInitCruiseInfo(channel);
      // pthread_mutex_lock(&g_CruiseMutex);
	memcpy(cruise_param,&gp_ptzCruise[channel].cruise_line[cruise_param->line],sizeof(sdk_cruise_param_t));
	//pthread_mutex_unlock(&g_CruiseMutex);
	return sizeof(sdk_cruise_param_t);

}

//获取某一通道所有的预置点
int GetAllPreset(int channel,sdk_preset_all_t *preset_set)
{
  if ( channel < 0 || channel >= MAX_DISPLAY_NUM) 
	 {
		 return THREAD_FLAG_STOP;		
	 }
  	ptzInitPresetsInfo(channel);
     memcpy(preset_set,&gp_ptzCruise[channel].preset_all,sizeof(sdk_preset_all_t));
 
     preset_set->num = 128;	
     return sizeof(sdk_preset_all_t);
}


int ptzCruiseInit()
{
	pthread_mutex_init(&g_CruiseStatusMutex,NULL);
	pthread_mutex_init(&g_CruiseMutex, NULL);
	memset(gp_ptzCruise, 0, sizeof(gp_ptzCruise));

	unsigned int i = 0;
	int flag = THREAD_FLAG_STOP;
	for ( i = 0; i < MAX_DISPLAY_NUM && i<configGetDisplayNum(); i++)
	{             
		ptzInitPresetsInfo(i);
		ptzInitCruiseInfo(i);
		flag= GetEnableFlagFromMemory(i);
		//数据库中ptz巡航使能
		if(flag>THREAD_FLAG_STOP)
		{
			//使用线程池
			if(threadPoolAddWorker(NULL, PtzCruiseThread, (char *)&i, sizeof(i)) != 0)
			{
				continue;
			}
			else
			{
				Printf("channel %d  ptz cruise thread start..................... \r\n ",i+1);
			}
		}
	}	

	return 0;
}

int ptzStartCruise(int channel)
{
	return 0;
}


int ptzStopCruise(int channel	)
{
	return 0;
}



