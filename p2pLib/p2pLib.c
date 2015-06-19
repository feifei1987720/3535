#include "commonLib.h"
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "hs_miscLib.h"
#include "p2pLib.h"
#include "configLib.h"
#include "streamLib.h"
#include "networkLib.h"
#include "hs_rtsp_commonLib.h"
#include "sdk_struct.h"
#include <sqlite3.h>
#include "onvif_client.h"
#define MODLE_NAME	"P2P"

#define OPEN_SERVER		2
#define OPEN_UDP_SERVER		1
#define OPEN_TCP_SERVER		2

#define SERV_PORT  9395

#define MAX_MSG_LEN	2048
#define 	INNOV_FAIL					(-1)
#define 	INNOV_SUCCESS				(0)

#define MAX_VOD_CHANNEL 4
#define P2P_FREEIP 1
#define P2P_DANALE 2

#define FREE_IP_ADDR       "www.freeipc.com"
typedef struct p2p_info_t
{
	pthread_t  p2pId;//id
	pthread_mutex_t p2pMutex;//
}p2p_info_t;


static  int  g_Status =0 ;
//static p2p_info_t  p2p_info;
static unsigned char g_p2pFlg;

//static P2P_VOD_CHANNEL g_p2pVodServer[MAX_VOD_CHANNEL];


void P2pExchFlg(sdk_p2p_conf_t *p2pCfg)
{
	if(p2pCfg->P2pEn)
	{
		g_p2pFlg = P2P_FREEIP;
	}
	p2pCfg++;
	if(p2pCfg->P2pEn)
	{
		g_p2pFlg = P2P_DANALE;
	}
}


void *SendRealStream(void *mediainfo)
{
	struct  m_mediainfo* pmediainfo  = (struct  m_mediainfo *)(mediainfo);
	int MessionId = pmediainfo->msessionid;
       int res; 
	int channelNo =pmediainfo->channelNo; 
	int streamNo = 1;
	stream_frame_queue_t *pFrameQueue = NULL;
	stream_frame_t * pFrame = NULL;
	int readBegin = -1;
	int readEnd = -1;
	stream_frame_head_t frameHead;
	int waitIFrame = 0;
	int frameNo = 0;
	int noFrame = 0;
	memset(&frameHead, 0, sizeof(stream_frame_head_t));
	pFrameQueue = (stream_frame_queue_t *)networkGetStreamFrameQueue(channelNo, streamNo);
	int freamNum=0;
	while(pmediainfo->run)
	{
		pFrame = streamGetFrameFromNetPool(pFrameQueue, &readBegin, &readEnd);
		if(pFrame == NULL)
		{
			usleep(20 * 1000);
			if(noFrame >= 100)
			{
				//goto __RtpTcpThrFxnExit;
			}
			noFrame++;
			continue;
		}
		noFrame = 0;
		freamNum++;
		if(waitIFrame == 0)
		{
			if((pFrame->pFrameBuf[4] & 0x1F) == NAL_TYPE_SPS)
			{
				waitIFrame = 1;
				frameNo = pFrame->frameHead.frameNo;
			}
			else
			{
				streamFreeFrameBuffer(pFrameQueue, pFrame);
				continue;
			}
		}
		else
		{
			if(((frameNo + 1)&0xFFFF) != pFrame->frameHead.frameNo)
			{
				//Printf("dev %d, channel %d, lost frame %d ~ %d\r\n", channelNo, streamNo, frameNo, pFrame->frameHead.frameNo);
				waitIFrame = 0;
				streamFreeFrameBuffer(pFrameQueue, pFrame);
				continue;
			}
		}
		frameNo = pFrame->frameHead.frameNo;
	
		if(pFrame->frameHead.type==FRAME_TYPE_AUDIO)
		{

		}
		else if(pFrame->frameHead.type==FRAME_TYPE_VIDEO)
		{
			if(waitIFrame==1)
			{
				res = P2pStartSendData(MessionId,pFrame->pFrameBuf, pFrame->frameHead.len, RealStream_Data);	
				if(res== -2)
				{
					break;
				}	
				streamFreeFrameBuffer(pFrameQueue, pFrame);
				usleep(25*1000);
			}
		}
	}
	Printf("#############pthread SendRealStream exit ...........\r\n");
	return NULL;
	
}


void *SendRelayStream(void *mediainfo)
{
	struct  m_mediainfo* pmediainfo  = (struct  m_mediainfo *)(mediainfo);
	int MessionId = pmediainfo->msessionid;
       int res; 
	int channelNo =pmediainfo->channelNo; 
	int streamNo = 1;
	stream_frame_queue_t *pFrameQueue = NULL;
	stream_frame_t * pFrame = NULL;
	int readBegin = -1;
	int readEnd = -1;
	stream_frame_head_t frameHead;
	int waitIFrame = 0;
	int frameNo = 0;
	int noFrame = 0;
	memset(&frameHead, 0, sizeof(stream_frame_head_t));
	pFrameQueue = (stream_frame_queue_t *)networkGetStreamFrameQueue(channelNo, streamNo);
	int freamNum=0;
	while(pmediainfo->run)
	{
		pFrame = streamGetFrameFromNetPool(pFrameQueue, &readBegin, &readEnd);
		if(pFrame == NULL)
		{
			usleep(20 * 1000);
			if(noFrame >= 100)
			{
				//goto __RtpTcpThrFxnExit;
			}
			noFrame++;
			continue;
		}
		noFrame = 0;
		freamNum++;
		if(waitIFrame == 0)
		{
			if((pFrame->pFrameBuf[4] & 0x1F) == NAL_TYPE_SPS)
			{
				waitIFrame = 1;
				frameNo = pFrame->frameHead.frameNo;
			}
			else
			{
				streamFreeFrameBuffer(pFrameQueue, pFrame);
				continue;
			}
		}
		else
		{
			if(((frameNo + 1)&0xFFFF) != pFrame->frameHead.frameNo)
			{
				//Printf("dev %d, channel %d, lost frame %d ~ %d\r\n", channelNo, streamNo, frameNo, pFrame->frameHead.frameNo);
				waitIFrame = 0;
				streamFreeFrameBuffer(pFrameQueue, pFrame);
				continue;
			}
		}
		frameNo = pFrame->frameHead.frameNo;

		if(pFrame->frameHead.type==FRAME_TYPE_AUDIO)
		{

		}
		else if(pFrame->frameHead.type==FRAME_TYPE_VIDEO)
		{
			if(waitIFrame==1)
			{
				res = P2pStartSendRelayData(MessionId,pFrame->pFrameBuf, pFrame->frameHead.len, RealStream_Data);	
				if(res== -2)
				{
					break;
				}	
				streamFreeFrameBuffer(pFrameQueue, pFrame);
				usleep(25*1000);
			}
		}

	}
	Printf("pthread SendRelayStream exit ...........\r\n");
	return NULL;
}



int CreatThread(sthread func,void* param)
{
	int iRet=0;
       pthread_t		 threadID;
    	pthread_attr_t   attr;	
	pthread_attr_init(&attr);	
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	iRet=pthread_create(&threadID,&attr,func,param);
	if(iRet != 0)
	{
		(void)pthread_attr_destroy(&attr);
		return -1;
	}
	return 0;
}

int PlayRealStream(char *data)
{
	struct  m_mediainfo *mediainfo = (struct  m_mediainfo *)data;
	int ret = -1;

	//Printf("!!!PlayRealStream  channelNo :%d  stream=%d   \n",mediainfo->channelNo,mediainfo->streamType);
	if((mediainfo->channelNo < 0) ||(mediainfo->channelNo >= configGetDisplayNum()))
	{
		Printf("Error!!!PlayRealStream  channelNo :%d  Quit!!  \n",mediainfo->channelNo);
		return -1;
	}
	if(mediainfo->streamType == 1)  //主码流请求
	{
		Printf("PlayRealStream main stream!\n");
	}
	else if(mediainfo->streamType == 2) //辅码流请求
	{
		Printf("PlayRealStream sub stream!\n");
	}
	//printf( "  file:%s   line:%d\n\n", __FILE__, __LINE__);
	mediainfo->run = 1;
	
	ret = CreatThread(SendRealStream,(void *)(mediainfo));
	if(ret <0)
	{
		Printf("PlayRealStream CreatThread erro!\n");
		return -1;
	}
	return 0;
}

int StopRealStream(char *data)
{
	Printf("StopRealStream!\n");
	struct  m_mediainfo *mediainfo = (struct  m_mediainfo *)data;
	mediainfo->run = 0;
	return 0;
		
}

int ClientDisconnect(char *data) //对端主动关闭连接
{
	struct  m_mediainfo *mediainfo = (struct  m_mediainfo *)data;
	mediainfo->run = 0;  //停止发送视频线程
	
	int result = P2pStopService(mediainfo->msessionid);  //关闭p2p服务
	if(result<0)
	{
		Printf("StopRealStream P2pStopSendData erro!\n");
		return -1;
	}
	
        return 0;
}

int PlayRelayStream(char *data)
{
	int ret = -1;
	
	struct  m_mediainfo *mediainfo = (struct  m_mediainfo *)data;

     Printf("!!!PlayRelayStream  channelNo :%d  stream=%d \n",mediainfo->channelNo,
	mediainfo->streamType);
	if((mediainfo->channelNo < 0) ||(mediainfo->channelNo >= configGetDisplayNum()))
	{
		Printf("Error!!!PlayRelayStream  channelNo :%d  Quit!! %s, %d \n",mediainfo->channelNo,__FILE__,__LINE__);
		return -1;
	}
	
	if(mediainfo->streamType == 1)  //主码流请求
	{
		Printf("PlayRelayStream main stream!\n");
	}
	else if(mediainfo->streamType == 2) //辅码流请求
	{
		Printf("PlayRelayStream sub stream!\n");
	}
	mediainfo->run = 1;
	ret = CreatThread(SendRelayStream,(void *)(mediainfo));
	if(ret <0)
	{
		Printf("PlayRelayStream CreatThread erro!\n");
		return -1;
	}
        return 0;
}
int StopRelayStream(char *data)  
{
	struct  m_mediainfo *mediainfo = (struct  m_mediainfo *)data;
        mediainfo->run = 0; //停止发送视频
        return 0;
		
}
int RelayClientDisconnect(char *data)   //对端主动关闭连接
{
	struct  m_mediainfo *mediainfo = (struct  m_mediainfo *)data;
	mediainfo->run = 0;
	
	int result = P2pStopRelayService(mediainfo->msessionid);  //关闭p2p服务
	if(result<0)
	{
		Printf("StopRealStream P2pStopSendData erro!\n");
		return -1;
	}
	
        return 0;
}





int PtzControl(char *data)   //对端主动关闭连接
{
	PtzControlMsg *ptzcontrolmsg = (struct  PtzControlMsg *)data;
      int ret = 0;
      if (ptzcontrolmsg->channel < 0 || ptzcontrolmsg->channel>= configGetDisplayNum()  )
	  {
		return  -1;
	}
	char preSetName[16];
	char preSetToken[16];

	Remote_Device_Info remoteIpc;
	ret = DbMainMsgChannelInfo(ptzcontrolmsg->channel,remoteIpc.ip,&remoteIpc.port,remoteIpc.userName,remoteIpc.password);
	if(-1==ret)
	{
		Printf("DbMainMsgChannelInfo  get channel failed\n");
		return -1;
	}

	ONVIF_AddRemoteDevice(ptzcontrolmsg->channel,&remoteIpc);

	Onvif_Profiles getProfilesRespone;
	//memset(&getProfilesRespone, 0, sizeof(getProfilesRespone));
	ONVIF_GetProfilesFromCache(&remoteIpc, &getProfilesRespone);

	ONVIF_PTZ__ContinuousMove PtzContinuousMove;
	//memset(&PtzContinuousMove, 0, sizeof( PtzContinuousMove ));

	strncpy(PtzContinuousMove.ProfileToken, getProfilesRespone.Onvif_Profiles[0].profileToken,
			sizeof( PtzContinuousMove.ProfileToken ) - 1);
       PtzContinuousMove.Speed = 7 ;
	PtzContinuousMove.Timeout = 10000;
	PtzContinuousMove.isPan = 1;
	
	switch (ptzcontrolmsg->ptzcmd)
	{
		case  PTZCONTROLTYPE_UP_START:	
			PtzContinuousMove.direction = PTZ_UP;
			PtzContinuousMove.isZoom = 0;
		       ret = ONVIF_PtzContinueMove(&remoteIpc, &PtzContinuousMove );
			break;
		case  PTZCONTROLTYPE_UP_STOP:
			ONVIF_PtzStop(&remoteIpc, PtzContinuousMove.ProfileToken );
			break;
		case  PTZCONTROLTYPE_DOWN_START:
			PtzContinuousMove.direction = PTZ_DOWN;
			PtzContinuousMove.isZoom = 0;
			ret = ONVIF_PtzContinueMove(&remoteIpc, &PtzContinuousMove );	
			break;
		case  PTZCONTROLTYPE_DOWN_STOP:
			ONVIF_PtzStop(&remoteIpc, PtzContinuousMove.ProfileToken );
			break;	
		case  PTZCONTROLTYPE_LEFT_START:
		PtzContinuousMove.direction = PTZ_LEFT;
			PtzContinuousMove.isZoom = 0;
			ret = ONVIF_PtzContinueMove(&remoteIpc, &PtzContinuousMove );	
			break;	
		case  PTZCONTROLTYPE_LEFT_STOP:
			ONVIF_PtzStop(&remoteIpc, PtzContinuousMove.ProfileToken );	
			break;	
		case  PTZCONTROLTYPE_RIGHT_START:
			PtzContinuousMove.direction = PTZ_RIGHT;
			PtzContinuousMove.isZoom = 0;
			ret = ONVIF_PtzContinueMove(&remoteIpc, &PtzContinuousMove );
			break;	
		case  PTZCONTROLTYPE_RIGHT_STOP:
			ONVIF_PtzStop(&remoteIpc, PtzContinuousMove.ProfileToken );	
			break;

		case  PTZCONTROLTYPE_ZOOMWIDE_START:
			PtzContinuousMove.isZoom = 1;
			PtzContinuousMove.isPan = 0;
			PtzContinuousMove.direction = PTZ_ZOOM_IN ;
			ret = ONVIF_PtzContinueMove(&remoteIpc, &PtzContinuousMove );	
			break;

		case  PTZCONTROLTYPE_ZOOMWIDE_STOP:
			ONVIF_PtzStop(&remoteIpc, PtzContinuousMove.ProfileToken );	
			break;	

		case  PTZCONTROLTYPE_ZOOMTELE_START:
			PtzContinuousMove.isZoom = 1;
			PtzContinuousMove.isPan = 0;
			PtzContinuousMove.direction = PTZ_ZOOM_OUT;
			PtzContinuousMove.Speed *= -1;
			ret = ONVIF_PtzContinueMove(&remoteIpc, &PtzContinuousMove );
			break;		

		case  PTZCONTROLTYPE_ZOOMTELE_STOP:
			ONVIF_PtzStop(&remoteIpc, PtzContinuousMove.ProfileToken );	
			break;			
		default:
			break;
	}
		
        return ret;
}




int   Getdeviceinfo(char* indata,char* outdata,int outlen)
{
	#if  0
	DeviceStreamInfoResp *devinforesp=NULL;
	DeviceStreamMsgReq *devinforeq = (DeviceStreamMsgReq *)indata;
	if(devinforeq->streamType ==1)
	{
		printf("mainstream request,channel is %d\n",devinforeq->channelNo);
		if(outdata != NULL)
		{
			devinforesp = (DeviceStreamInfoResp *)outdata;
			devinforesp->streamsend_statue = 1;   //发送成功
			devinforesp->framerate = 30;
			devinforesp->streambitrate = 1000;
			strncpy(devinforesp->deviceversion,"Imax_version_1.5.0",MAX_VERSION_LENGTH);
		//	printf("success!!!\n");
		}
			
	}
	else if(devinforeq->streamType ==2)
	{
		printf("substream request,channel is %d\n",devinforeq->channelNo);
		if(outdata != NULL)
		{
			devinforesp = (DeviceStreamInfoResp *)outdata;
			devinforesp->streamsend_statue = 1;   //发送成功
			devinforesp->framerate = 30;
			devinforesp->streambitrate = 500;
			strncpy(devinforesp->deviceversion,"Imax_version_1.5.0",MAX_VERSION_LENGTH);
		//	printf("success!!!\n");
		}
			
	}
	#endif
	return 0;
}
int HandleCommand(P2pCmdType cmd,char* indata,int inlen,char* outdata,int outlen)
{
	int result = 0;	
	switch(cmd)
	{
		case  P2P_PLAY_REAL_STREAM:  //实时流
			result = PlayRealStream(indata);
			
			if(result<0)
			{
				return -1;
			}
			break;
		case  P2P_STOP_REAL_STREAM:
			result = StopRealStream(indata);
			if(result<0)
			{
				return -1;
			}
			break;
		case  P2P_PLAY_RELAY_STREAM: //实时流(走中转方式)
			result = PlayRelayStream(indata);
			
			if(result<0)
			{
				return -1;
			}
			break;
		case  P2P_STOP_RELAY_STREAM:
			result = StopRelayStream(indata);
		
			if(result<0)
			{
				return -1;
			}
			break;
		case  P2P_CLIENT_DISCONNECT:
			result = ClientDisconnect(indata);
		
			if(result<0)
			{
				return -1;
			}
			break;	
		case  P2P_RELAYCLIENT_DISCONNECT:
			result = RelayClientDisconnect(indata);
		
			if(result<0)
			{
				return -1;
			}
			break;	
		case P2P_START_PTZ_CONTROL:
			result = PtzControl(indata);
			if(result<0)
			{
				return -1;
			}
			break;
		case P2P_GET_DEVICE_STREAMINFO:			
			result = Getdeviceinfo(indata,outdata,outlen);
			if(result<0)
			{
				return -1;
			}
			break;
			
		default:
			break;
	}

	return 0;	
}

int GetP2PConnectStatus()
{
     return  g_Status;
}

void  HandleStatusCommand(int Status)
{
     g_Status = Status;
}

void  P2P_end()
{
   Printf("p2p stop!!\r\n");
   P2P_StopService();
   sleep(2);
 
}


int GetFreeIpInfo(USER_INFO *UserInfo,int * enable)
{
    int result = 0;
    sqlite3 *db;
    char **dbResult;
    int nRow = 0, nColumn = 0;
    char *sqlcmd[2] = {
	"select * from userInfo",
	"select * from p2pCfgTbl"
    };

	if(SQLITE_OK != sqlite3_open(DATABASE_FILE, &db))//数据库打开失败,恢复数据库。
	{
		return -1;
	}
	
	result = sqlite3_get_table(db, sqlcmd[0], &dbResult, &nRow, &nColumn, NULL);
	
	if(SQLITE_OK != result)//查询某个表单失败，恢复数据库并退出。
	{
		sqlite3_free_table(dbResult);
		sqlite3_close(db);
		return -1;
	}

	strcpy((char *)UserInfo->ucPassWord, dbResult[nColumn + 1]);
	sqlite3_free_table(dbResult);	 
	result = sqlite3_get_table(db, sqlcmd[1], &dbResult, &nRow, &nColumn, NULL);
	
	if(SQLITE_OK != result)//查询某个表单失败，恢复数据库并退出。
	{
		sqlite3_free_table(dbResult);
		sqlite3_close(db);
		return -1;
	} 
	
	strcpy((char *)UserInfo->ucSerialNum, dbResult[nColumn + 3]);
	*enable = atoi(dbResult[nColumn + 2]);
	sqlite3_free_table(dbResult);
	sqlite3_close(db);
    return 0;
}


int p2pTHRStart(int type)
{
   
	USER_INFO UserInfo;
	int enable = 0;
	GetFreeIpInfo(&UserInfo,&enable);
	if(type ==0)
		{        
			if(enable)
			{
				Printf("Pwd:%s     sn:%s\n\n",UserInfo.ucPassWord,UserInfo.ucSerialNum);
				P2p_RegisterCommandHandle(HandleCommand);		
				P2p_RegisterStatusHandle(HandleStatusCommand);
				int result =P2P_InitService(FREE_IP_ADDR, (char *)UserInfo.ucSerialNum,"admin",(char *)UserInfo.ucPassWord);   //arg[1]P2Pwww.freeipc.com   arm[2]    "admin""admin"
				if(result<0)
				{
				Printf("P2P_InitService erro\n");
				}
			}   
	    }
	else
		{
			P2p_RegisterCommandHandle(HandleCommand);		
			P2p_RegisterStatusHandle(HandleStatusCommand);
			int result =P2P_InitService(FREE_IP_ADDR, (char *)UserInfo.ucSerialNum,"admin",(char *)UserInfo.ucPassWord);   //arg[1]P2Pwww.freeipc.com   arm[2]    "admin""admin"
			if(result<0)
			{
			Printf("P2P_InitService erro\n");
			}
	    }
	
	return INNOV_SUCCESS;
}



