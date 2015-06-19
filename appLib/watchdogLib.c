#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <pthread.h>

#include "watchdogLib.h"
#include "commonLib.h"
#include "global.h"
#include "param.h"
#include "myThreadLib.h"


#define  HI_WD_DEVICE   "/dev/watchdog"


static Hi3531_wtd_info_t   watchdogInfo;
static int g_run = 0;

/**/
int openWtdog(int   * fd)
{
	*fd=open(HI_WD_DEVICE,O_RDWR|O_SYNC);
	if(*fd<=0)
	{
		Printf("open wtdog fail \n");
		return -1;
	}
	else
	{
		Printf("open success  fd=%d\n",*fd);
		return 0;
	}
}

/**/
int closeWtdog(int fd)
{
	if(fd>0)
	{
		close(fd);
	}
	return 0;
}

/**/
int GetWtdogSuportOption(int fd, watchdog_info *wd_info)
{
	if(ioctl(fd,WDIOC_GETSUPPORT,wd_info) == -1)
	{
		Printf("Failed to ioctrl\n");
		return -1;
	}
	else
	{
		Printf("options=%d firmware_version= %d identity= %s\n",wd_info->options,wd_info->firmware_version,wd_info->identity);
		return 0;
	}
}


/**/
int GetWtdogStatus(int fd,int *t)
{
	*t=0;
	if(ioctl(fd,WDIOC_GETSTATUS,t) == -1)
	{
		Printf("Failed to ioctrl\n");
		return -1;
	}
	else
	{
		Printf("WDIOC_GETSTATUS t=%d\n",*t);
		return 0;
	}
}

/**/
int  GetWtdogBootstatus(int fd,int  *t)
{
	//t=0;
	if(ioctl(fd,WDIOC_GETBOOTSTATUS,t) == -1)
	{
		Printf("Failed to ioctrl\n");
		return -1;
	}
	else
	{
		Printf("WDIOC_GETBOOTSTATUS t=%d\n",*t);
		return 0;
	}
}

/**/
int feedDog(int fd,int  *t)
{
	//int t=0;
	if(ioctl(fd,WDIOC_KEEPALIVE,t) == -1)
	{
		Printf("Failed to ioctrl\n");
		return -1;
	}
	else
	{
		//Printf("WDIOC_KEEPALIVE t=%d\n",*t);
		return 0;
	}
}


/**/
int setWtdogTimeout(int fd,int * t)
{
	//int t=5;
	if(ioctl(fd,WDIOC_SETTIMEOUT,t) == -1)
	{
		Printf("Failed to ioctrl\n");
		return -1;
	}
	else
	{
		Printf("WDIOC_SETTIMEOUT t=%d\n",*t);
		return 0;
	}
}


/**/
int setWtdogOption(int fd,int *t)
{
	//int t=2;
	if(ioctl(fd,WDIOC_SETOPTIONS,t) == -1)
	{
		Printf("Failed to ioctrl\n");
		return -1;
	}
	else
	{
		Printf("WDIOC_SETOPTIONS t=%d\n",*t);
		return 0;
	}
}



void *WatchdogThread()
{
	int  t=0;
	int ret=0;
	int fd =0;
	watchdog_info wd_info;

	ret=openWtdog(&fd);
	if(ret<0)
	{
		Printf("open fial\n");
	}
	else
	{
		Printf("fd=%d\n",fd);
	}

	memset(&wd_info,0,sizeof(wd_info));
	ret=GetWtdogSuportOption(fd, &wd_info);
	if(ret<0)
	{
		Printf("GetWtdogSuportOption  fail \n");
	}
	else
	{
		Printf("options=%d firmware_version= %d identity= %s\n",wd_info.options,wd_info.firmware_version,wd_info.identity);	
	}

	t=0;
	ret=GetWtdogStatus( fd, &t);
	if(ret<0)
	{
		Printf("GetWtdogStatus fail\n");
	}

	t=0;
	ret=GetWtdogBootstatus( fd,&t);
	if(ret<0)
	{
		Printf("GetWtdogBootstatus  fail\n");
	}

	t=0;
	ret=feedDog( fd, &t);
	if(ret<0)
	{
		Printf("feedDog  fail\n");
	}

	t=6;
	ret=setWtdogTimeout( fd,&t);
	if(ret<0)
	{
		Printf("setWtdogTimeout  fail\n");
	}

	while(g_run)
	{
		t=0;
		ret=feedDog( fd, &t);
		if(ret<0)
		{
			Printf("feedDog  fail\n");
			closeWtdog( fd);
			break;
		}
		else
		{
			//Printf("feedDog success  \n");
		}

		usleep(1000 * 1000);
	}

	t=20 * 60;
	ret=setWtdogTimeout( fd,&t);

	closeWtdog( fd);
	
	return NULL;
}



int  startWtdThread()
{
	g_run = 1;

	if(pthread_create(&watchdogInfo.HiwtdogId, NULL, WatchdogThread, NULL) < 0)
	{
		Printf("WatchdogThread error\r\n");
		return -1;
	}

	return 0;
}

void stopWtdthread()
{
	g_run = 0;
	if(watchdogInfo.HiwtdogId>0)
	{
		pthread_join(watchdogInfo.HiwtdogId, NULL);
	}
}

