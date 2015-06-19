#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "global.h"
#include "appLib.h"
#include "procMsgLib.h"
#include "diskLib.h"
#include "commonLib.h"
#include "databaseLib.h"
#include "playbackLib.h"
#include "recordLib.h"
#include "networkLib.h"
#include "appFunTable.h"
//#include "ptzCruise.h"
#include "onvif_client.h"
#include "suKey.h"
#include "myThreadLib.h"
#ifndef X86
#include "decodeLib.h"
#include "hiLib.h"
#endif
#include "param.h"
#include<time.h>

//hash_table_t *g_pDevChannelHash = NULL;//

dev_channel_info_t g_devChannelInfo[0xFF][0x3F];
static pthread_t g_appThreadId;
static int gMsgId = 0;
static int gSemId = 0;


static void onvifEvent(char *pRemoteIp, int eventType)
{
	//Printf("pRemoteIp = (%s), eventType %d\r\n", pRemoteIp, eventType);
}

int setSystemReboot()
{
	int ret =0;
	ret=cmdOperate(SDK_MAIN_MSG_CLOSE_SYSTEM, SDK_CLOSE_SYS_REBOOT, NULL, 0, NULL, 0);
	if(ret <0)
	{
		Printf("cmdOperate SDK_MAIN_MSG_CLOSE_SYSTEM fail\n");
		return -1;
	}
	return ret;
}

void autoMaintain(void)
{
	int ret = 0;
	sdk_autoMaintain_attr_t AutoMaintainAttrCfg;
	time_t timep;
        struct tm *p;
        time(&timep);
        //timep = 1402243200;
        p=localtime(&timep); /*取得当地时间*/
        //Printf ("%d%d%d  week= %d  hour =%d\n", (1900+p->tm_year),(1+p->tm_mon), p->tm_mday,p->tm_wday,p->tm_hour);

	ret = databaseOperate(SDK_MAIN_MSG_AUTO_MAINTAIN, SDK_PARAM_GET, NULL, 0, &AutoMaintainAttrCfg, sizeof(sdk_autoMaintain_attr_t));
	if(ret <0)
	{
		Printf("databaseOperate SDK_MAIN_MSG_AUTO_MAINTAIN fail\n");
	}
	//Printf("77777777777777AutoMaintainAttrCfg.interval =%d AutoMaintainAttrCfg.hour=%d week=%d \n",AutoMaintainAttrCfg.interval,AutoMaintainAttrCfg.hour,p->tm_wday);
	if(AutoMaintainAttrCfg.interval == SDK_MAIN_AUTO_NEVER)
	{
		return ;
	}
	else if(AutoMaintainAttrCfg.interval == SDK_MAIN_AUTO_EVERY_DAY)
	{
		if((AutoMaintainAttrCfg.hour == p->tm_hour)&&(p->tm_min<1))
		{
			setSystemReboot();
			Printf("AutoMaintainAttrCfg.interval =%d AutoMaintainAttrCfg.hour=%d\n",AutoMaintainAttrCfg.interval,AutoMaintainAttrCfg.hour);
		}
	}
	else if((AutoMaintainAttrCfg.interval == SDK_MAIN_AUTO_MONDAY)&&(AutoMaintainAttrCfg.interval == (p->tm_wday +1)))
	{
		if((AutoMaintainAttrCfg.hour == p->tm_hour)&&(p->tm_min<1))
		{
			setSystemReboot();
			Printf("AutoMaintainAttrCfg.interval =%d AutoMaintainAttrCfg.hour=%d\n",AutoMaintainAttrCfg.interval,AutoMaintainAttrCfg.hour);
		}
	}
	else if((AutoMaintainAttrCfg.interval == SDK_MAIN_AUTO_TUESDAY)&&(AutoMaintainAttrCfg.interval == (p->tm_wday +1)))
	{
		if((AutoMaintainAttrCfg.hour == p->tm_hour)&&(p->tm_min<1))
		{
			setSystemReboot();
			Printf("AutoMaintainAttrCfg.interval =%d AutoMaintainAttrCfg.hour=%d\n",AutoMaintainAttrCfg.interval,AutoMaintainAttrCfg.hour);
		}
	}
	else if((AutoMaintainAttrCfg.interval == SDK_MAIN_AUTO_WEDNESDAY)&&(AutoMaintainAttrCfg.interval == (p->tm_wday +1)))
	{
		if((AutoMaintainAttrCfg.hour == p->tm_hour)&&(p->tm_min<1))
		{
			setSystemReboot();
			Printf("AutoMaintainAttrCfg.interval =%d AutoMaintainAttrCfg.hour=%d\n",AutoMaintainAttrCfg.interval,AutoMaintainAttrCfg.hour);
		}
	}
	else if((AutoMaintainAttrCfg.interval == SDK_MAIN_AUTO_THURSDAY)&&(AutoMaintainAttrCfg.interval == (p->tm_wday +1)))
	{
		if((AutoMaintainAttrCfg.hour == p->tm_hour)&&(p->tm_min<1))
		{
			setSystemReboot();
			Printf("AutoMaintainAttrCfg.interval =%d AutoMaintainAttrCfg.hour=%d\n",AutoMaintainAttrCfg.interval,AutoMaintainAttrCfg.hour);
		}
	}
	else if((AutoMaintainAttrCfg.interval == SDK_MAIN_AUTO_FRIDAY)&&(AutoMaintainAttrCfg.interval == (p->tm_wday +1)))
	{
		if((AutoMaintainAttrCfg.hour == p->tm_hour)&&(p->tm_min<1))
		{
			setSystemReboot();
			Printf("AutoMaintainAttrCfg.interval =%d AutoMaintainAttrCfg.hour=%d\n",AutoMaintainAttrCfg.interval,AutoMaintainAttrCfg.hour);
		}
	}
	else if((AutoMaintainAttrCfg.interval == SDK_MAIN_AUTO_STAURDAY)&&(AutoMaintainAttrCfg.interval == (p->tm_wday +1)))
	{
		if((AutoMaintainAttrCfg.hour == p->tm_hour)&&(p->tm_min<1))
		{
			setSystemReboot();
			Printf("AutoMaintainAttrCfg.interval =%d AutoMaintainAttrCfg.hour=%d\n",AutoMaintainAttrCfg.interval,AutoMaintainAttrCfg.hour);
		}
	}
	else if((AutoMaintainAttrCfg.interval == SDK_MAIN_AUTO_SUNDAY)&&(AutoMaintainAttrCfg.interval == (p->tm_wday +8)))
	{
		if((AutoMaintainAttrCfg.hour == p->tm_hour)&&(p->tm_min<1))
		{
			setSystemReboot();
			Printf("AutoMaintainAttrCfg.interval =%d AutoMaintainAttrCfg.hour=%d\n",AutoMaintainAttrCfg.interval,AutoMaintainAttrCfg.hour);
		}
	}
	
}


static void *appThread()
{
	int count = 0;
	int cnt = 1;
	setPthreadPriority(PTHREAD_SCHEDULED_PRIORITY_LOW_EST);
	
	while(1)
	{
		if(cnt%(30)==17)
		{
			autoMaintain();
		}
		if(cnt%(60 * 5) == 0)
		{
			cnt = 0;
			Printf("============Database Backup!\n");
			DbBackup();
		}
		if(count%(60*30) == 0)
		{//30
			syncSystemTime();
		}
		sleep(1);
		count++;
		cnt++;
	}

	return NULL;
}

void appLibStartThread()
{
	if(pthread_create(&g_appThreadId, NULL, appThread, NULL) < 0)
	{
		Printf("app  thread error\r\n");
		return;
	}
}

void appLibStart()
{
	appStartDecode();
}

void appNtpSettime(void)
{
	sdk_ntp_conf_t p_ntp_conf;
	databaseOperate(SDK_MAIN_MSG_NTP, SDK_PARAM_GET, NULL, 0, &p_ntp_conf, sizeof(sdk_ntp_conf_t));
	usleep(1000);
	databaseOperate(SDK_MAIN_MSG_NTP, SDK_PARAM_SET, NULL, 0, &p_ntp_conf, sizeof(sdk_ntp_conf_t));
}
void appLibInit()
{
	
	int channelNo = 0;
	int streamNo = 0;
	memset(&g_devChannelInfo, 0, sizeof(g_devChannelInfo));
	/*  */
	g_devChannelInfo[channelNo][streamNo].channelNo = channelNo;
	g_devChannelInfo[channelNo][streamNo].streamNo = streamNo;

	//	procMsgInit(&gMsgId, &gSemId);
	//	printf("gMsgId === %d. gSemId = %d\r\n", gMsgId, gSemId);

	//	startGuiSerVer();
	//   int num=GetDisplayNum();
	//    Printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@diskplay_num =%d\r\n",num);
	//   if(num<4 && num >32)
	//   {
	//      num =32;
	//  }
	//  ONVIF_SetDisplayNum(num);
	
	TCPComTools();
	appLibStartThread();

	appParamInit();


#if 0
	sdk_cruise_param_t   cruise_param;
	int i= 0,j,n;
	for(i=0;i!=1;++i)
	{
		for(j=0;j!=3;++j)
		{
			for(n=0;n!=32;++n)
			{
				cruise_param.line = j;
				cruise_param.enable = 1;
				cruise_param.cruise_preset[n].dwell_time = 1;
				cruise_param.cruise_preset[n].enable = 1;
				cruise_param.cruise_preset[n].speed = 1;
				cruise_param.cruise_preset[n].num = 1;
				cruise_param.cruise_preset[n].preset.num = 2;
				cruise_param.cruise_preset[n].preset.enable = 1;

			}
			SetPtzCruiseAllPreset( i, &cruise_param );
		}
	}

	sdk_preset_t preset;

	for(i=0;i!=1;++i)
	{
		for(j=0;j!=128;++j)
		{
			preset.enable = 1;
			preset.num = j;
			SetPreset(i,&preset);


		}
	}
	Printf("@@@@@@@@@@ptzCruiseInit   finish set preset\r\n");

#endif

	ptzCruiseInit();


	// ONVIF_EventServerStart(onvifEvent);
}

void appLibDestroy()
{
	ONVIF_EventServerStop();
}

