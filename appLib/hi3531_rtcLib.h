
#ifndef _HI3531_RTC_H
#define _HI3531_RTC_H


//#define Printf(format, arg...) myPrintf( __FILE__, __LINE__, format, ##arg)


typedef struct HI_Rtc_Time_t
{
	unsigned int seconds;
	unsigned int minutes;
	unsigned int hour;
	unsigned int week;
	unsigned int day;
	unsigned int month;
	unsigned int year;
	unsigned int Hour_Mode;  //0: 24  1:12
	unsigned int Hour_Display; //0:AM 1 :PM

}HI_Rtc_Time_t;

#define HI_RTC_IOC_MAGIC     'r'

#define    RTC_SET_TIME       _IOWR(HI_RTC_IOC_MAGIC,1,HI_Rtc_Time_t)
#define    RTC_GET_TIME       _IOWR(HI_RTC_IOC_MAGIC,2,HI_Rtc_Time_t) 

typedef struct	Hi3531_rtc_info_t
{
	pthread_t  HiwtdId;//id
	pthread_mutex_t HiwtdMutex;//
}Hi3531_rtc_info_t;

typedef  long  time_t;

//struct timeval
//{
  //      time_t tv_sec; /*??s*/
   //     time_t tv_usec; /*???us*/
//};

void startRtcThread(void);
//int read_rtc(int fd,HI_Rtc_Time_t *hi_rtc_time);
//int write_rtc(int fd,HI_Rtc_Time_t *hi_rtc_time);
int read_rtc(HI_Rtc_Time_t *hi_rtc_time);
int write_rtc(HI_Rtc_Time_t *hi_rtc_time);
int setTimeTo_system(HI_Rtc_Time_t *hi_rtc_time);
void syncSystemTime();

#if defined (__cplusplus)
}
#endif

#endif

