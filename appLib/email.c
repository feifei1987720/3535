/********************************************
  SMTP程序说明:
  1. 本程序实现了邮件发送的基本功能，如SSLj加密、带附件发送、抄送等。
  2. 由于本程序中使用了SSL加密技术，会用到OpenSSL的头文件和静态库文件，
  所以，在编译此程序前，应该正确编译OpenSSL，OpenSSL的编译请参照相关说明。
  3. 当有多个抄送人时，每个抄送人之间必须使用分号;相隔,不然将无法解析
 ********************************************/
#include <stdlib.h>    
#include <stdio.h>    
#include <string.h>    
#include <netdb.h>    
#include <sys/types.h>    
#include <sys/stat.h>    
#include <unistd.h>     
#include <fcntl.h>    
#include <sys/ioctl.h>    
#include <netinet/in.h>    
#include <sys/socket.h>    
#include <linux/if_ether.h>    
#include <net/if.h>    
#include <errno.h>  
#include <time.h>
#include <pthread.h>
#include <time.h>
#include "myThreadLib.h"
#include "commonLib.h"
#include "sdk_struct.h"
#include "main.h"
#include "global.h"
#include "databaseLib.h"

#include "midWare.h"
#include "smtp.h"
#include "DbSystem.h"


static unsigned char g_smtpRun = 0;
static unsigned char g_smtpCfgFlag = 0;
static sdk_email_conf_t p_SmtpConf;
static int p_SendEmail[MAX_DISPLAY_NUM]={0,};

static int sendEmailSetting(int channel)
{
	if(channel<0&&channel>MAX_DISPLAY_NUM)
	{
		Printf("channel is  fail \n");
		return -1;
	}
	p_SendEmail[channel] = 1;
	return 0;
}

int updateSmtpSetting(unsigned char flag)
{
	g_smtpCfgFlag = flag;
	return 0;
}


void *MD_SendEmailThread(char *arg, int argLen)
{
	int i=0;
	char sys_time[16];
	char message[512];
	char fileDir[50];
	st_smtp smtp_send;

	while(g_smtpRun)	
	{
		for(i=0;i<configGetDisplayNum();i++)
		{
			if(p_SendEmail[i] == 1)
			{
				p_SendEmail[i] = 0;
				time_t tnow = time(NULL);	
				struct tm *ptm = localtime(&tnow);
			
				memset(sys_time ,0 ,sizeof(sys_time));	
				memset(&smtp_send ,0 ,sizeof(smtp_send));		
				memset(message ,0 ,sizeof(message));	
			
				/* 只发消息 */	
				memset(sys_time, 0 ,sizeof(sys_time));
				sprintf(sys_time, "%04d-%02d-%02d %02d:%02d:%02d", 
						ptm->tm_year+1900 ,ptm->tm_mon+1 ,ptm->tm_mday ,ptm->tm_hour ,ptm->tm_min ,ptm->tm_sec);
			
				memset(message ,0 ,sizeof(message));
			
				/* 发送的消息内容 */
				sprintf(message,"NVR mail:moving object were found in the motion detect area,time is:%s\n"
						"Channel Number:%d\n",sys_time,i+1);
				memset(fileDir,0,sizeof(fileDir));
				sprintf(fileDir,"/tmp/snap_%d.jpg",i+1);
				//Printf("smtp_send.pFileName=%s\n",fileDir);
				smtp_send.pHost = p_SmtpConf.smtp_server;
				smtp_send.pLoginName = p_SmtpConf.user;
				smtp_send.pLoginPassword = p_SmtpConf.passwd;
				smtp_send.pSendFrom = p_SmtpConf.sender;
				smtp_send.pSendTo = p_SmtpConf.receiver;
				smtp_send.pSendCc = NULL;
				smtp_send.pDate =  sys_time;
				smtp_send.pEmailSubject = p_SmtpConf.theme;
				smtp_send.pMessage = message;
				smtp_send.port = p_SmtpConf.port;				
			
#ifndef HI3531
				if(p_SmtpConf.file_enable == 1)
				{
					/*发附件 */ 
					smtp_send.pFileName = fileDir;
				}
				else
#endif
				{
					/* 只发消息 */	
					smtp_send.pFileName = NULL;
				}
				if(SendEmail(&smtp_send) == -1)
				{
					Printf("Smtp SendEmail error!\n");
					usleep(20*1000);
				}
				usleep(50*1000);
			}
			usleep(30*1000);
		}
		usleep(500*1000);
	}
	return NULL;
}


void *smtpThread(char *arg, int argLen)
{	
	time_t tnow, tbef[128]={0,};
	int count = 0 ,len=0;
	unsigned char  i=0;
	
	memset(&p_SmtpConf ,0 ,sizeof(p_SmtpConf));
	setPthreadPriority(PTHREAD_SCHEDULED_PRIORITY_LOW);
	g_smtpCfgFlag = 1;	////第一次进入线程，要从数据库读取配置

	while(g_smtpRun)
	{	
		//////获取邮件参数
		if(g_smtpCfgFlag == 1)
		{			
			//Printf("Getting smtp config....\n");
			len = databaseOperate(SDK_MAIN_MSG_SMTP, SDK_PARAM_GET, NULL, 0, &p_SmtpConf, sizeof(p_SmtpConf));
			if(len != sizeof(sdk_email_conf_t))
			{
				Printf("get smtp info error, len = %d\r\n", len);
			}
			g_smtpCfgFlag = 0;
		}

		if(p_SmtpConf.interval == 0 )
		{
			////没有开启邮件功能,就不要往检测通道。
			usleep(1000*1000);
			continue;
		}



		for(i=0; i<configGetDisplayNum(); i++)
		{
			if((VideoMDGetStatus(i)==1) && (GetPlatStatus()==0))///&&(getMotionEnble(i)==1)
			{
				////发生移动侦测
				tnow = time(NULL);	
				
				///if(tbef[i] != 0 && ((tnow-tbef[i]) >= smtp_conf.interval))
				if((tnow-tbef[i]) >= p_SmtpConf.interval*60)
				{
					tbef[i] = tnow;			
					sendEmailSetting(i);
				}
				else
				{
					///小时发邮件间隔，不用发
					continue;
				}

			}
			
		}
		
		usleep(300*1000);	
	}
	return NULL;
}

int  startSmtpThread(void)
{
	//	Printf("@@@@@@@ startSmtpThread ..... line:<%d>@@@@@@@@\n",__LINE__);

	if(g_smtpRun == 1)
	{
		return 0;
	}
	
	g_smtpRun = 1;
	
	if(threadPoolAddWorker(NULL, smtpThread, NULL, 0) != 0)
	{
		Printf("smtp thread create error !\n");
		return -1;
	}
	
	if(threadPoolAddWorker(NULL, MD_SendEmailThread, NULL, 0) != 0)
	{
		Printf("smtp thread create error !\n");
		return -1;
	}

	return 0;
}
int  stopSmtpThread(void)
{
	if(g_smtpRun == 0)
	{
		return 0;
	}

	g_smtpRun = 0;
	///pthread_join(g_smtpThreadId, NULL);
	///pthread_join(g_sendEmailThreadId, NULL);
	return 0;
}

int testSmtp(void)
{
	int len=0;
	char sys_time[16];
	char message[512];	
	time_t tnow;
	struct tm *ptm;
	st_smtp smtp_send;
	sdk_email_conf_t smtp_conf;
	memset(sys_time ,0 ,sizeof(sys_time));	
	memset(message ,0 ,sizeof(message));
	memset(&smtp_send ,0 ,sizeof(smtp_send));

	memset(&smtp_conf ,0 ,sizeof(smtp_conf));

	len = databaseOperate(SDK_MAIN_MSG_SMTP, SDK_PARAM_GET, NULL, 0, &smtp_conf, sizeof(smtp_conf));
	if(len != sizeof(sdk_email_conf_t))
	{
		Printf("get smtp info error, len = %d\r\n", len);
	}

	/* 只发消息 */	
	memset(sys_time ,0 ,sizeof(sys_time));
	tnow = time(NULL);	
	ptm = localtime(&tnow);
	sprintf(sys_time, "%04d-%02d-%02d %02d:%02d:%02d", 
		ptm->tm_year+1900 ,ptm->tm_mon+1 ,ptm->tm_mday ,ptm->tm_hour ,ptm->tm_min ,ptm->tm_sec);

	memset(message ,0 ,sizeof(message));

	/* 发送的消息内容 */
	sprintf(message,"NVR mail:this a test email,send time is:%s\n",sys_time);

	//Printf("smtp_send.pFileName=%s\n",fileDir);
	smtp_send.pHost = smtp_conf.smtp_server;
	smtp_send.pLoginName = smtp_conf.user;
	smtp_send.pLoginPassword = smtp_conf.passwd;
	smtp_send.pSendFrom = smtp_conf.sender;
	smtp_send.pSendTo = smtp_conf.receiver;
	smtp_send.pSendCc = NULL;
	smtp_send.pDate =  sys_time;
	smtp_send.pEmailSubject = smtp_conf.theme;
	smtp_send.pMessage = message;
	smtp_send.port = smtp_conf.port;				
	/* 只发消息 */	
	smtp_send.pFileName = NULL;

	if(SendEmail(&smtp_send) == -1)
	{
		Printf("Smtp SendEmail error!\n");
		return 0;
	}	
	
	return 1;
}



