#include <stdio.h>
#include <sys/ioctl.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include "hi3531_rtcLib.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "commonLib.h"



Hi3531_rtc_info_t   HiRtcInfo;


int read_rtc(HI_Rtc_Time_t *hi_rtc_time)
{
	int fd=0;
	int ret=0;
	fd = open("/dev/hi_rtc", 0);                                   
	if (fd<0)                                                           
	{                                                                               
		Printf("Open gpioi2c dev error!\n");                                                                                      
	}

        ret = ioctl(fd, RTC_GET_TIME, hi_rtc_time);
        if(ret<0)
        {
        	Printf("RTC_GET_TIME fail\n");
        	return -1;
        }
        if(fd>0)
	{
		close(fd);	
	}
	return 0;
}

int write_rtc(HI_Rtc_Time_t *hi_rtc_time)
{
	int fd=0;
	int ret=0;
	fd = open("/dev/hi_rtc", 0);                                   
	if (fd<0)                                                           
	{                                                                               
		Printf("Open gpioi2c dev error!\n");                                                                                      
	}
        ret = ioctl(fd, RTC_SET_TIME, hi_rtc_time);
        if(ret<0)
        {
        	Printf("RTC_SET_TIME fail\n");
        	return -1;
        }
        if(fd>0)
	{
		close(fd);	
	}
	return 0;
}


int setTimeTo_system(HI_Rtc_Time_t *hi_rtc_time)
{
	int ret=0;
	struct tm time_tm;
	struct timeval time_tv;
	time_t timep;
	time_tm.tm_year = hi_rtc_time->year -1900;
	time_tm.tm_mon = hi_rtc_time->month-1 ;
	time_tm.tm_wday = hi_rtc_time->week-1;
	time_tm.tm_mday = hi_rtc_time->day;
	time_tm.tm_hour=hi_rtc_time->hour;
	time_tm.tm_min=hi_rtc_time->minutes;
	time_tm.tm_sec=hi_rtc_time->seconds;
	time_tm.tm_isdst = 0;

	timep = mktime(&time_tm);
	time_tv.tv_sec = timep;
	time_tv.tv_usec = 0;


	ret = settimeofday(&time_tv, NULL);
	if(ret != 0)
	{
		fprintf(stderr, "settimeofday failed\n");
		return -1;
	}

	return 0;
}

void syncSystemTime()
{
	int ret = 0;
	HI_Rtc_Time_t hi_rtc_time;

	memset(&hi_rtc_time,0,sizeof(hi_rtc_time));

	ret = read_rtc(&hi_rtc_time);
	if(ret < 0)
	{
		Printf("read_rtc fail\n");
	}

	hi_rtc_time.year=hi_rtc_time.year+2000;

	setTimeTo_system(&hi_rtc_time);
}

