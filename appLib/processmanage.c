

#include <stdio.h>
#include <stdlib.h>


#include "appLib.h"
#include "commonLib.h"
#include "databaseLib.h"


static pthread_t g_processManageId;
static int g_runProcessManage=0;
static int g_changNtpSetting=0;
static int g_changEmailSetting=0;
static int g_changDdnsSetting=0;


int changEmailSetting(int value)
{
	g_changEmailSetting=value;
	return 0;
}

int changNtpSetting(int value)
{
	g_changNtpSetting=value;
	return 0;
}

int changDdnsSetting(int value)
{
	g_changDdnsSetting=value;
	return 0;
}

void processManageThread()
{
	sdk_email_conf_t pSmtpConf;
	sdk_ddns_cfg_t pDdnsConf;
	sdk_ntp_conf_t p_ntpConf;
	int len=0;
	unsigned char i=0;

	while(1==g_runProcessManage)
	{
		if(1==g_changNtpSetting)
		{
			g_changNtpSetting = 0;
			len = databaseOperate(SDK_MAIN_MSG_NTP, SDK_PARAM_GET, NULL, 0, &p_ntpConf, sizeof(sdk_ntp_conf_t));

			if(len != sizeof(sdk_email_conf_t))
			{
				///g_changNtpSetting = 1;
				Printf("get ntp info error, len = %d\r\n", len);
			}
			if(p_ntpConf.enable == 1)
			{
				startNtpClientThread();
			}
			else
			{
				stopNtpClientThread();
			}	
		}
		
		if(1==g_changEmailSetting)
		{
			g_changEmailSetting = 0;
			len = databaseOperate(SDK_MAIN_MSG_SMTP, SDK_PARAM_GET, NULL, 0, &pSmtpConf, sizeof(pSmtpConf));
			if(len != sizeof(sdk_email_conf_t))
			{
				///g_changEmailSetting = 1;
				Printf("get smtp info error, len = %d\r\n", len);
			}

			if(pSmtpConf.email_enable)
			{
				startSmtpThread();
			}
			else
			{
				stopSmtpThread();
			}

		}

		if(1==g_changDdnsSetting)
		{
			g_changDdnsSetting = 0;

			for(i = 0; i< MAX_DDNS_TYPE; i++)
			{
				pDdnsConf.type = i;
				len = databaseOperate(SDK_MAIN_MSG_DDNS_CFG, SDK_PARAM_GET, &i, 0, &pDdnsConf, sizeof(pDdnsConf));
				if(len != sizeof(sdk_ddns_cfg_t))
				{
					///g_changDdnsSetting = 1;
					Printf("get smtp info error, len = %d\r\n", len);
				}
				if(pDdnsConf.enable)
				{
					StartDdnsThread();
					break;
				}
			}
						
			if(i >= MAX_DDNS_TYPE)
			{
				StopDdnsThread();
			}
		}
		usleep(1000*1000);
	}
}

int startProcessManage()
{
	////开机要从数据库读一次配置
	g_changEmailSetting = 1;
	g_changDdnsSetting = 1;
	g_changNtpSetting = 1;
	DdnsUpdate(1);
	updateSmtpSetting(1);
	updateNtpSetting(1);

	if(g_runProcessManage == 1)
	{
		return 0;
	}

	g_runProcessManage = 1;
	if(pthread_create(&g_processManageId, NULL, processManageThread, NULL) < 0)
	{
		Printf("smtp thread create error !\n");
		return -1;
	}
	
	return 0;
}

int stopProcessManage()
{
	if(g_runProcessManage == 0)
	{
		return 0;
	}
	g_runProcessManage = 0;
	pthread_join(g_processManageId, NULL);

	return 0;
}






