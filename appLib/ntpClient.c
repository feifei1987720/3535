
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>
#include <pthread.h>
#include "sdk_struct.h"
#include "commonLib.h"
#include "main.h"
#include "myThreadLib.h"
#include <errno.h>

#include "midWare.h"

static int g_NtpThreadRun = 0;
static int g_NtpUpdatFlg = 1;	//NTP信息发生改变的标志


int updateNtpSetting(int flag)
{
	g_NtpUpdatFlg = flag;
	Printf("===========g_NtpUpdatFlg:%d\n", g_NtpUpdatFlg);

	return 0;
}

static void * NtpClientThread(char *arg, int argLen)
{
	time_t tnow, tbef=0;
	sdk_ntp_conf_t p_ntpConf;

	g_NtpUpdatFlg = 1;

	while(1==g_NtpThreadRun)
	{
		tnow = time(NULL);	

		if(1==g_NtpUpdatFlg)
		{
			/////读取NTP相关参数
			databaseOperate(SDK_MAIN_MSG_NTP, SDK_PARAM_GET, NULL, 0, &p_ntpConf, sizeof(sdk_ntp_conf_t));
			g_NtpUpdatFlg = 0;	
		}

		if(!p_ntpConf.enable || ((tnow-tbef) < p_ntpConf.interval*60))
		{
			usleep(1000*1000);
			continue;
		}
		else
		{
			tbef = time(NULL);	
			Printf("+++++ SetNtpTime +++++\n");
			SetNtpTime(p_ntpConf.time_zone, p_ntpConf.ntp_server, p_ntpConf.port);	
			//////还要加各个IPC同步时间的功能
		}
		usleep(1000*1000);
		
	}
	
	return NULL;
}


/////NTP接口层
int startNtpClientThread(void)
{
	if(g_NtpThreadRun == 1)
	{
		return 0;
	}

	g_NtpThreadRun = 1; // 只开启一个线程
	if(threadPoolAddWorker(NULL, NtpClientThread, NULL, 0) != 0)
	{
		Printf("ntp thread create error !\n");
		return -1;
	}

	return 0;
}

int stopNtpClientThread(void)
{
	if(g_NtpThreadRun == 0)
	{
		return 0;
	}

	g_NtpThreadRun = 0;
	return 0;
}





