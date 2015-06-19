#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "commonLib.h"
#include "hi3531_rtcLib.h"
#include <sys/time.h>
#include "ntp.h"
#include<pthread.h>
#include "myThreadLib.h"

#define JAN_1970     		0x83aa7e80
#define NTPFRAC(x) (4294 * (x) + ((1981 * (x))>>11))
#define USEC(x) (((x) >> 12) - 759 * ((((x) >> 10) + 32768) >> 16))
#define Data(i) ntohl(((unsigned int *)data)[i])
#define LI 0
#define VN 3
#define MODE 3
#define STRATUM 0
#define POLL 4 
#define PREC -6


typedef struct _TimeZoneMap {
	char key;
	signed char hour;
	signed char minutes;	
}TimeZoneMap;

struct NtpTime 
{
	unsigned int coarse;
	unsigned int fine;
};

static TimeZoneMap g_stZones[] = 
{
	{0	,-12	,0		},
	{1	,-11	,-30	},
	{2	,-11	,0		},
	{3	,-10	,-30	},
	{4	,-10	,0		},
	{5	,-9 	,-30	},
	{6	,-9 	,0		},
	{7	,-8 	,-30	},
	{8	,-8 	,0		},
	{9	,-7 	,-30	},
	{10 ,-7 	,0		},
	{11 ,-6 	,-30	},
	{12 ,-6 	,0		},
	{13 ,-5 	,-30	},
	{14 ,-5 	,0		},
	{15 ,-4 	,-30	},
	{16 ,-4 	,0		},
	{17 ,-3 	,-30	},
	{18 ,-3 	,0		},
	{19 ,-2 	,-30	},
	{20 ,-2 	,0		},
	{21 ,-1 	,-30	},
	{22 ,-1 	,0		},
	{23 ,0		,-30	},
	{24 ,0		,0		},
	{25 ,0		,30 	},
	{26 ,1		,0		},
	{27 ,1		,30 	},
	{28 ,2		,0		},
	{29 ,2		,30 	},
	{30 ,3		,0		},
	{31 ,3		,30 	},
	{32 ,4		,0		},
	{33 ,4		,30 	},
	{34 ,5		,0		},
	{35 ,5		,30 	},
	{36 ,6		,0		},
	{37 ,6		,30 	},
	{38 ,7		,0		},
	{39 ,7		,30 	},
	{40 ,8		,0		},
	{41 ,8		,30 	},
	{42 ,9		,0		},
	{43 ,9		,30 	},
	{44 ,10 	,0		},
	{45 ,10 	,30 	},
	{46 ,11 	,0		},
	{47 ,11 	,30 	},
	{48 ,12 	,0		},
};

static pthread_t g_ntpThreadId;
static char *g_hostName = NULL;
static int g_ntpPort = 123;
static int g_enable = 0;
static struct hostent *g_hostIp = NULL;


int GetTimeZone( char * timezone,int * hour ,int * minute)
{
    int tz = DbGetTimeZone();
  if(tz<0 || tz >48)
  	{
      return  -1;
    }
  if(tz <=22)
  	{
    	sprintf(timezone, "%s%02d:%02d","GMT-",abs(g_stZones[tz].hour),abs(g_stZones[tz].minutes));				
  	}
  else
  	{
	  sprintf(timezone, "%s%02d:%02d","GMT+",abs(g_stZones[tz].hour),abs(g_stZones[tz].minutes));     
    }
  *hour = abs(g_stZones[tz].hour);
  * minute = abs(g_stZones[tz].minutes);

  return 0;
   
}



void sendPacket(int fd)
{
	unsigned int data[12];
	struct timeval now;

	if (sizeof(data) != 48) 
	{
    	fprintf(stderr,"size error\n");
    	return;
	}

	memset((char*)data, 0, sizeof(data));
	data[0] = htonl((LI << 30) | (VN << 27) | (MODE << 24) | (STRATUM << 16) | (POLL << 8) | (PREC & 0xff));
	data[1] = htonl(1<<16);
	data[2] = htonl(1<<16);
	gettimeofday(&now, NULL);
	data[10] = htonl(now.tv_sec + JAN_1970);
	data[11] = htonl(NTPFRAC(now.tv_usec));
	send(fd, data, 48, 0);
}

void getNewTime(unsigned int *data,struct timeval *ptimeval)
{
	struct NtpTime trantime;
	trantime.coarse = Data(10);
	trantime.fine   = Data(11);
	
	ptimeval->tv_sec 	= trantime.coarse - JAN_1970;
	ptimeval->tv_usec 	= USEC(trantime.fine);
}

int getNtpTime(struct hostent* phost,struct timeval *ptimeval)
{
	if(phost == NULL)
	{
		Printf("err:host is null!\n");
		return -1;
	}
	int sockfd;
	struct sockaddr_in addr_src,addr_dst;
	fd_set fds;
	int ret;
	int recv_len;
	unsigned int buf[12];
	memset(buf,0,sizeof(buf));
	int addr_len;
	int count = 0;
	
	struct timeval timeout;

	addr_len = sizeof(struct sockaddr_in);

	memset(&addr_src, 0, addr_len);
	addr_src.sin_family = AF_INET;
	addr_src.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_src.sin_port = htons(0);

	memset(&addr_dst, 0, addr_len);
	addr_dst.sin_family = AF_INET;
	memcpy(&(addr_dst.sin_addr.s_addr), phost->h_addr_list[0], 4);
	addr_dst.sin_port = htons(g_ntpPort);

	if(-1==(sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)))
	{		
		Printf("create socket error!\n");
		return -1;
	}

	ret = bind(sockfd, (struct sockaddr*)&addr_src, addr_len);
	if(-1==ret)	
	{		
		Printf("bind error!\n");		
		close(sockfd);		
		return -1;
	}
	
	ret = connect(sockfd, (struct sockaddr*)&addr_dst, addr_len);
	if(-1==ret)	
	{		
		Printf("connect error!\n");		
		close(sockfd);		
		return -1;
	}	
	while (count < 300)
	{
		FD_ZERO(&fds);
		FD_SET(sockfd, &fds);

		timeout.tv_sec = 0;
		timeout.tv_usec = 1000;
		ret = select(sockfd + 1, &fds, NULL, NULL, &timeout);
		if (0 == ret || !FD_ISSET(sockfd, &fds))
		{
			count++;
			sendPacket(sockfd);
			continue;
		}
		recv_len = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&addr_dst, &addr_len);
		if(-1==recv_len)		
		{			
			Printf("recvfrom error\n");			
			close(sockfd);			
			return -1;
		}
		else if(recv_len > 0)
		{
			getNewTime(buf,ptimeval);
			break;
		}
	}
	close(sockfd);
	return 0;
}

int setSystemTime(int timezone,struct hostent* phost)
{		
	int ret ,hour ,zone ,hour_tmp ,minutes_tmp;
	int seconds;
	int minutes;
	int day;
	int month;
	int year;
	zone = timezone;

	if(phost == NULL)
	{
		Printf("host is empty ,please give host!\n");
		return -1;
	}
	struct timeval TimeSet;
	memset(&TimeSet ,0 ,sizeof(TimeSet));
	struct tm *pTime=NULL;
	ret = getNtpTime(phost,&TimeSet);
	if(ret == -1)
	{
		Printf("err: get Ntp time err \n");
		return -1;
	}
	
	time_t timer;
	timer = (long long)TimeSet.tv_sec;
	pTime = gmtime(&timer);
	year = pTime->tm_year+1900;
	month = pTime->tm_mon + 1;
	day = pTime->tm_mday;
	hour = pTime->tm_hour;
	/* ????  ?±???????±???±?? */
	hour_tmp = hour + g_stZones[zone].hour;

	minutes_tmp = pTime->tm_min;
	seconds = pTime->tm_sec;
	//Printf("\n Ntp time is:%d-%02d-%02d %02d-%02d-%02d\n",year,month,day,hour_tmp,minutes_tmp,seconds); 
	
	HI_Rtc_Time_t  hi_rtc_time={0}; 		
	memset(&hi_rtc_time,0,sizeof(hi_rtc_time));

	if(year < 2000 || month <= 0 || day <= 0 || hour <= 0)
	{		
		Printf("err: get Ntp time err....\n");
		return -1;
	}
	hi_rtc_time.year=year - 2000;
	hi_rtc_time.month=month;
	hi_rtc_time.day=day;

	if(g_stZones[zone].minutes == 0)
	{
		minutes = minutes_tmp;
	}
	else
	{
		minutes_tmp += g_stZones[zone].minutes;
		if( minutes_tmp >= 60)
		{
			hour_tmp += 1;
			minutes = minutes_tmp - 60;
		}
		else if(minutes_tmp < 0)
		{
			hour_tmp -= 1;
			minutes = 60 + minutes_tmp;
		}
		else
		{
			minutes = minutes_tmp;
		}
	}

	/* 是否超过24 小时，或者是前一天 */
	if(hour_tmp>=24)
	{
		hi_rtc_time.day = hi_rtc_time.day + 1;		
		switch(month)
		{
			case 12:
				if(hi_rtc_time.day > 31)
				{
					hi_rtc_time.month = 1;
					hi_rtc_time.year = hi_rtc_time.year + 1;
					hi_rtc_time.day = 1;
				}				
				break;
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
				if(hi_rtc_time.day > 31)
				{
					hi_rtc_time.month = month + 1;
				}
				break;
			case 4:
			case 6:
			case 9:
			case 11:
				if(hi_rtc_time.day > 30)
				{
					hi_rtc_time.month = month + 1;
				}
				break;
			case 2:
				/*是否闰年*/
				if ((year%4 ==0 && year%100!=0) || (year%400 ==0))
				{
					if(hi_rtc_time.day > 29)
					{
						hi_rtc_time.month = month + 1;
					}
				}
				else
				{
					if(hi_rtc_time.day > 28)
					{
						hi_rtc_time.month = month + 1;
					}
				}
				break;
			default:
				break;
		}
		hi_rtc_time.hour=hour_tmp - 24;
	}
	else if(hour_tmp<0)
	{
		hi_rtc_time.day = hi_rtc_time.day - 1;		
		switch(month - 1)
		{
			case 0:
				if(hi_rtc_time.day < 1)
				{
					hi_rtc_time.day = 31;
					hi_rtc_time.month = 12;
					hi_rtc_time.year = hi_rtc_time.year - 1;
				}
				break;
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
				if(hi_rtc_time.day < 1)
				{
					hi_rtc_time.day = 31;
					hi_rtc_time.month = month - 1;
				}
				break;
			case 4:
			case 6:
			case 9:
			case 11:
				if(hi_rtc_time.day < 1)
				{
					hi_rtc_time.day = 30;
					hi_rtc_time.month = month - 1;
				}
				break;
			case 2:				
				/*是否闰年*/
				if ((year%4 ==0 && year%100!=0) || (year%400 ==0))
				{
					if(hi_rtc_time.day < 1)
					{
						hi_rtc_time.day = 29;
						hi_rtc_time.month = month - 1;
					}
				}
				else
				{
					if(hi_rtc_time.day < 1)
					{
						hi_rtc_time.day = 28;
						hi_rtc_time.month = month - 1;
					}
				}
				break;
			default:
				break;
		}
		hi_rtc_time.hour= 24 + hour_tmp;
	}
	else
	{
		hi_rtc_time.hour=hour_tmp;
	}
	
	hi_rtc_time.minutes=minutes;
	hi_rtc_time.seconds=seconds;
	
	hi_rtc_time.week=5;
	hi_rtc_time.Hour_Mode=0;
	
	//Printf("\nSet rtc time:%d-%02d-%02d %02d-%02d-%02d\n",hi_rtc_time.year,hi_rtc_time.month,hi_rtc_time.day,hi_rtc_time.hour,hi_rtc_time.minutes,hi_rtc_time.seconds); 		

	ret=write_rtc(&hi_rtc_time);
	if(ret<0)
	{
		Printf("write_rtc  fail \n");		
		return -1;
	}
	hi_rtc_time.year = hi_rtc_time.year+2000;
	ret=setTimeTo_system(&hi_rtc_time);
	if(ret<0)
	{
		Printf("setTimeTo_system  fail \n");		
		return -1;
	}
	return 0;
}


void *ntpThread()
{
	struct hostent *host = NULL;
	setPthreadPriority(PTHREAD_SCHEDULED_PRIORITY_LOW_EST);
		
	while(g_enable == 1)
	{		
		if(g_hostName != NULL)
		{
			host = NULL;
			host = gethostbyname(g_hostName);
			g_hostIp = host;
		}
		if(g_hostIp == NULL)
		{
			Printf("error:gethost %s err!\n",g_hostName);
		}
		usleep(1000);
	}
	return NULL;
}

int  startNtpThread()
{
	if(pthread_create(&g_ntpThreadId, NULL, ntpThread, NULL) < 0)
	{
		Printf("ntp thread create error !\n");
		return -1;
	}
	return 0;
}


int SetNtpTime(int timezone,char *ntpserver, int port)
{
	int cnt = 0;
	g_hostIp = NULL;
	g_hostName = ntpserver;	
	g_ntpPort = port;

	startNtpThread();

	g_enable = 1;	
	while(cnt < 50)
	{
		if(g_hostIp == NULL)
		{
			cnt++;
			usleep(1000);
			continue;
		}
		else
		{
			break;
		}
	}		

	g_enable = 0;		
	if(g_hostIp != NULL)
	{	
		setSystemTime(timezone,g_hostIp);
	}
	return 0;
}
