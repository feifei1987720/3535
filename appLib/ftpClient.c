

///#include "DbSystem.h "
#include <stdio.h>
#include <time.h>

#include "sdk_struct.h"
#include "DbSystem.h"
#include "commonLib.h"
#include "databaseLib.h"
#include "main.h"

static int g_changDate=1;
static sdk_ftp_config g_ftp_cfg;

static int AnalysisSendFile(char *strFileName, char *FileName)
{
	char *temp = NULL;
	time_t tCurrentTime;
	struct tm *tmnow;
	time(&tCurrentTime);
	tmnow = localtime(&tCurrentTime);

	temp = strstr(strFileName, ".avi");
	if(temp != NULL)
	{
		sprintf(FileName, "%04d%02d%02d%02d%02d%02d.avi",
		tmnow->tm_year+1900, tmnow->tm_mon+1, tmnow->tm_mday, tmnow->tm_hour,
		tmnow->tm_min, tmnow->tm_sec);

		return 0;
	}

	temp = strstr(strFileName, ".jpg");
	if(temp != NULL)
	{
		sprintf(FileName, "%04d%02d%02d%02d%02d%02d.jpg",
		tmnow->tm_year+1900, tmnow->tm_mon+1, tmnow->tm_mday, tmnow->tm_hour,
		tmnow->tm_min, tmnow->tm_sec);	

		return 0;
	}

	return -1;
}


int UpdateFtpDate(int value)
{
	g_changDate = value;
	return 0;
}

int SentToFTP(char *strFileName)
{
	int ret = -1, len=0;
	char filename[64] = {0};

	if(NULL == strFileName)
	{
		return -1;
	}

	if(g_changDate == 1)
	{
		len = databaseOperate(SDK_MAIN_MSG_FTP, SDK_PARAM_GET, NULL, 0, &g_ftp_cfg, sizeof(sdk_ftp_config));
		if(len != sizeof(sdk_ftp_config))
		{
			return -1;
		}
		
		g_changDate = 0;
	}

	if(0==g_ftp_cfg.bFtpEnable)
	{
		return 0;
	}

	if(AnalysisSendFile(strFileName, filename) == -1)
	{
		return -1;
	}
	
	//FTP·¢ËÍ
	ret = ftp_upload(g_ftp_cfg.servier_ip,g_ftp_cfg.port,g_ftp_cfg.foldername,\
			g_ftp_cfg.username,g_ftp_cfg.password,strFileName, filename);

	if(ret == 0)
	{
		Printf("=======ftp upload successful=======\n\n");
	}
	else
	{
		Printf("=======ftp upload fail %d=======\n\n", ret);
	}
		
	return ret;
}












