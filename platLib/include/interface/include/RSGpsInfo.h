
#ifndef __RSGPSINFO_H_
#define __RSGPSINFO_H_

#ifdef GPS_MODULE

#include "gps.h"

#else

typedef struct _DATEGPS_MDVR
{
	float speed;				// 
	float longitude; 			//  0-180
	float latitude; 			//  0-180
	float azimuth; 				//  0 0~360
	char latitude_direct;		//  'N' or 'S'
	char longitude_direct;   	//  'E' or 'W'
	char state;					// 'A',  'V'
	char reserve[13];
}GPS_MDVR_DATA;

#endif

typedef struct _DATETIME_MDVR
{
	unsigned char year; //2000+
	unsigned char month; //1~12
	unsigned char day;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
	unsigned char week;
	char reserve;
}DATETIME_MDVR;

typedef struct _SYSSTATEDATA_MDVR
{
	DATETIME_MDVR datetime;
	GPS_MDVR_DATA gpsinfo;
	int signal_3G;///3G
	int temperature;
	float speed_from_sensor; //
	unsigned int total_mileage;   //  
	unsigned char alarm[8];  // 0:IO alarm[0]=1<<chn; 1:motion alarm[1]=1<<chn; 2:video loss alarm[2]=1<<chn;3:hdd alarm[3]=1<<bit ,no hdd bit = 2,hdd err bit = 1,hdd full bit = 0;	
	unsigned char alarmiotype[8];
	char release[16];
}SYSSTATEDATA_MDVR;

#endif
