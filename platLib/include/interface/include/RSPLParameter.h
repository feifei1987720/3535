
//


#ifndef __RSPL_PARAMERTER_H_
#define __RSPL_PARAMERTER_H_

#define RSPL_Open 1
#define RSPL_Close 0

#define RSPL_MAXNAMELEN	36


/* ******************** *
   
	

* ******************** */

//TvSystem, VideoFormat
#define RSPL_PAL 0
#define RSPL_NTSC 1

//GetSystemBaseInfo
typedef struct {
	//
	//char DeviceName[RSPL_MAXNAMELEN];
	//unsigned int DeviceID;
	char PuID[RSPL_MAXNAMELEN];
    char TvSystem; /*: <RSPL_PAL, RSPL_NTSC>*/

	//
	char DeviceType[RSPL_MAXNAMELEN]; //
	char MacAddr[RSPL_MAXNAMELEN];    
	char SoftwareVer[RSPL_MAXNAMELEN]; //
	char HardwareVer[RSPL_MAXNAMELEN]; //
	char IEVer[RSPL_MAXNAMELEN]; 	  //IE		/*Yiguang 11.06.15*/
	char IpAddr[RSPL_MAXNAMELEN];
	unsigned int MobilePort;//
	unsigned int Port;
	unsigned int WebPort;
}RSPlat_SystemBaseInfo_t;



/* ******************** *
   
		

* ******************** */


//OverWrite
#define RSPL_OverWrite_Close 	0
#define RSPL_OverWrite_Auto 	1
#define RSPL_OverWrite_1Hour 	2
#define RSPL_OverWrite_3Hours 	3
#define RSPL_OverWrite_1Day 	4
#define RSPL_OverWrite_7Days 	5
#define RSPL_OverWrite_30Days 	6
#define RSPL_OverWrite_90Days 	7

//HddState
#define RSPL_HDDS_NONE 		0
#define RSPL_HDDS_UNFORMAT 	1
#define RSPL_HDDS_OK 	   	2
#define RSPL_HDDS_BAD 		3
#define RSPL_HDDS_RONLR 	4
#define RSPL_HDDS_BACKUP  	5   //
#define RSPL_HDDS_INVALID 	6

//GetHddInfo
//SetHddInfo
typedef struct{
	/**/
	unsigned long long HddState;  
	unsigned long long HddTotalSize;/*:*/
	unsigned long long HddFreeSize;
	unsigned long long HddFreeRecord; //
}RSPlat_SoleHddInfo_t;

typedef struct {
	//

	/**/
    char  OverWrite;       
	char reservel[3];
}RSPlat_HddInfo_t;




/* ******************** *
   
		

* ******************** */

//Protocol
#define RSPL_PelcoD 0
#define RSPL_PelcoP 1

//Baudrate
#define RSPL_Baudrate_1200 0
#define RSPL_Baudrate_2400 1
#define RSPL_Baudrate_4800 2
#define RSPL_Baudrate_9600 3

//DataBit
#define RSPL_DataBit_8 0
#define RSPL_DataBit_7 1
#define RSPL_DataBit_6 2
#define RSPL_DataBit_5 3

//StopBit
#define RSPL_StopBit_1 0
#define RSPL_StopBit_2 1

//Check
#define RSPL_CheckBit_None  0
#define RSPL_CheckBit_Odd   1
#define RSPL_CheckBit_Even  2
#define RSPL_CheckBit_Mark  3
#define RSPL_CheckBit_Space 4

//KbSwitch
#define RSPL_SerialKB  0
#define RSPL_SerialChn 1

//GetSerialInfo
//SetSerialInfo
typedef struct {
    /**/
    char Baudrate;
    /**/
    char DataBit;
    /**/
    char StopBit;
    /**/
    char Check;
}RSPlat_SerialInfo_t;



//DST
//TimeMode
#define RSPL_TimeMode_24 0
#define RSPL_TimeMode_12 1

//DateMode
#define RSPL_DateMode_MDY 0
#define RSPL_DateMode_YMD 1
#define RSPL_DateMode_DMY 2

//Offset
#define RSPL_Dst1Hour 0
#define RSPL_Dst2Hour 1

//DstMode
#define RSPL_DstWeek 0
#define RSPL_DstDate 1


//StartMonth, EndMonth
#define RSPL_DstMonth_January 	0
#define RSPL_DstMonth_February 	1
#define RSPL_DstMonth_March 		2
#define RSPL_DstMonth_April 		3
#define RSPL_DstMonth_May 		4
#define RSPL_DstMonth_June 		5
#define RSPL_DstMonth_July 		6
#define RSPL_DstMonth_August 		7
#define RSPL_DstMonth_September 	8
#define RSPL_DstMonth_October 	9
#define RSPL_DstMonth_November 	10
#define RSPL_DstMonth_December 	11

//StartWeek, EndWeek
#define RSPL_DstFristWeek 	0
#define RSPL_DstSecondWeek 	1
#define RSPL_DstThirdWeek 	2
#define RSPL_DstFourthWeek 	3
#define RSPL_DstFifthWeek 	4

//StartWeekDay, EndWeekDay
#define RSPL_DstSunday		0
#define RSPL_DstMonday 		1
#define RSPL_DstTuesday		2
#define RSPL_DstWednesday 	3
#define RSPL_DstThursday		4
#define RSPL_DstFriday		5
#define RSPL_DstSaturday		6

//Ntp
/*NtpIPAddr  NTP*/
#define RSPL_Ntp_windows   0
#define RSPL_Ntp_nist 	 1
#define RSPL_Ntp_ntp 		 2
#define RSPL_Ntp_lorexddns 3
/*
time.windows.com
time.nist.gov
pool.ntp.org
ntp.lorexddns.net
*/

//TimeZone
//PS:b12 = GMT-12:00
//PS:f8  = GMT+8:00
//PS:b0430  = GMT-4:30
#define RSPL_TimeZone_GMT_b12 	0  
#define RSPL_TimeZone_GMT_b11 	1 
#define RSPL_TimeZone_GMT_b10 	2 
#define RSPL_TimeZone_GMT_b09 	3 
#define RSPL_TimeZone_GMT_b08 	4 
#define RSPL_TimeZone_GMT_b07 	5 
#define RSPL_TimeZone_GMT_b06 	6 
#define RSPL_TimeZone_GMT_b05 	7 
#define RSPL_TimeZone_GMT_b0430 	8 
#define RSPL_TimeZone_GMT_b04 	9 
#define RSPL_TimeZone_GMT_b0330 	10 
#define RSPL_TimeZone_GMT_b03 	11 
#define RSPL_TimeZone_GMT_b02 	12 
#define RSPL_TimeZone_GMT_b01 	13 
#define RSPL_TimeZone_GMT 		14
#define RSPL_TimeZone_GMT_f01 	15 
#define RSPL_TimeZone_GMT_f02 	16 
#define RSPL_TimeZone_GMT_f03 	17 
#define RSPL_TimeZone_GMT_f0330 	18 
#define RSPL_TimeZone_GMT_f04 	19 
#define RSPL_TimeZone_GMT_f0430 	20 
#define RSPL_TimeZone_GMT_f05 	21 
#define RSPL_TimeZone_GMT_f0530 	22 
#define RSPL_TimeZone_GMT_f0545 	23 
#define RSPL_TimeZone_GMT_f06 	24 
#define RSPL_TimeZone_GMT_f0630 	25 
#define RSPL_TimeZone_GMT_f07 	26 
#define RSPL_TimeZone_GMT_f08 	27 
#define RSPL_TimeZone_GMT_f09 	28 
#define RSPL_TimeZone_GMT_f0930 	29 
#define RSPL_TimeZone_GMT_f10 	30 
#define RSPL_TimeZone_GMT_f11 	31 
#define RSPL_TimeZone_GMT_f12 	32 
#define RSPL_TimeZone_GMT_f13 	33 

#if 0
//
enum tag_LANGUAGE
{
	LAN_CHINESE	 		= 0,	//
	LAN_CHINESECOMPLEX  = 1,	//
	LAN_CZECH			= 2,	//
	LAN_DANISH			= 3,	//
	LAN_ENGLISH			= 4,	//
	LAN_FINNISH			= 5,	//
	LAN_FRENSH			= 6,	//
	LAN_GERMAN			= 7,	//
	LAN_GREEK			= 8,	//
	LAN_HEBREW			= 9,	//
	LAN_HUNGARIAN		= 10,	//
	LAN_INTALIAN		= 11,	//
	LAN_JAPANESE		= 12,	//
	LAN_POLISH			= 13,	//
	LAN_PORTUGUESE		= 14,	//
	LAN_RUSSIAN			= 15,	//
	LAN_SPANISH			= 16,	//
	LAN_THAI			= 17,	//
    LAN_SLOVENIA        = 18,   //
    LAN_ROMANIA         = 19,   //
	LAN_BULGARIAN		= 20,	//
	LAN_LANGUAGE_MAX,
} LANGUAGE_E;
#endif

// 
//GetGeneralInfoSet
//SetGeneralInfoSet
typedef struct {
	//DstSet
    DateTime time; //
	/*Dst */
    char StartDate[4];
	/*Dst */
    char StartHour[4];
	/*Dst  */
    char EndDate[4];
	/*Dst  */
    char EndHour[4];
    /*, 1224*/
    char TimeMode;
    /*0MM/DD/YY1YY-MM-DD2DD/MM/YY*/
    char DateMode;
    /*<RSPL_Close, RSPL_Open>*/
    char Dst;
	/* Dst0-1hour1-2hour */
    char Offset;
    /*0-Week1-Date*/
    char DstMode;
	/*12:0---11*/
    char StartMonth;    
    /*0-1-2-3-,4-*/
    char StartWeek;
	/*0-Sun,1-Mon, ...6-Sat*/
	char StartWeekDay;
	/*12:0---11*/
    char EndMonth;
    /*0-1-2-3-*/
    char EndWeek;
	/*0-Sun,1-Mon, ...6-Sat*/
	char EndWeekDay;

	//NtpSet
	/*NTP <RSPL_Close, RSPL_Open>*/
	char UseNtp;
    /* */
    char TimeZone;
	char Language;    // LANGUAGE_E
	char VideoFormat;
	char AutoLogout; //
//	/**/
//	unsigned int  NtpPort;
//	/*-:65535*/
//	unsigned int SyncPeriod;
	/* */
	char NtpIPAddr[4];
	/*LanguageMask*/
	unsigned int           LanguageMask;
}RSPlat_GeneralInfoSet_t;



//
//GetColorSet
//SetColorSet
typedef struct {
	char Chroma; 		//, <0 - 63>
	char Brightness; 	//, <0 - 63>
	char contrast; 	//, <0 - 63>
	char saturation; 	//, <0 - 63>

	char DefaultColor; 	//
}RSPlat_SoleColorSet_t;





//
//GetVideoCoverSet
//SetVideoCoverSet
#define MAX_VIDEOCOVER_NUM  32
//,
typedef struct {/**/
	int CoverSwitch; //, <RSPL_Open, RSPL_Close>
    int x, y, w, h;
}RSPlat_VideoCoverArea_t;


typedef struct {
	char VideoCoverSwitch;/* , <RSPL_Open, RSPL_Close>*/
	char reservel[3];
	RSPlat_VideoCoverArea_t VideoCoverArea[4]; //, 
}RSPlat_SoleVideoCover_t;





//PTZ

/* Serial
//Protocol
#define RSPL_PelcoD 0
#define RSPL_PelcoP 1

//Baudrate
#define RSPL_Baudrate_1200 0
#define RSPL_Baudrate_2400 1
#define RSPL_Baudrate_4800 2
#define RSPL_Baudrate_9600 3

//DataBit
#define RSPL_DataBit_8 0
#define RSPL_DataBit_7 1
#define RSPL_DataBit_6 2
#define RSPL_DataBit_5 3

//StopBit
#define RSPL_StopBit_1 0
#define RSPL_StopBit_2 1

//Check
#define RSPL_CheckBit_None  0
#define RSPL_CheckBit_Odd   1
#define RSPL_CheckBit_Even  2
#define RSPL_CheckBit_Mark  3
#define RSPL_CheckBit_Space 4
*/

#define RSPL_MAX_CRUISE_POINT 256

//GetPTZSet
//SetPTZSet
typedef struct {
    /**/
    char Protocol;
    /**/
    char Baudrate;
    /**/
    char DataBit;
    /**/
    char StopBit;
    /**/
    char Check;
	/* PTZ  RSPL_Close, RSPL_Open*/
	char EnablePtzCruise;
    char reservel[2];
    /*,UIADDRESS1-255*/
    unsigned int Number;
}RSPlat_SolePTZSet_t;


//OSD
//Position
#define RSPL_PositionLeftUp 	 0
#define RSPL_PositionLeftDown  1
#define RSPL_PositionRightUp 	 2
#define RSPL_PositionRightDown 3

//GetOsdSet
//SetOsdSet
typedef struct {
    /* 0-1-2-3-*/
    char Position;

    /*osd RSPL_Open, RSPL_Close*/
    char Preview;
	char reservel[2];

    /**/
    char ChnName[20];

}RSPlat_SoleOsdSet_t;

typedef struct {
    /*OSD RSPL_Open, RSPL_Close*/
    char PreviewTimeSet;
    /*OSD RSPL_Open, RSPL_Close*/
    char RecordTimeSet;
    char reservel[2];
}RSPlat_OsdSet_t;


//
//NetworkMode
#define RSPL_DHCP		0
#define RSPL_PPPOE  	1
#define RSPL_STATIC		2
#define RSPL_WCDMA		3
#define RSPL_CDMA2000	4
#define RSPL_WIFI		5

//GetNetworkSet
//SetNetworkSet
typedef struct {
    /*IP*/
    char IPAddr[4];
    /**/
    char NetMask[4];
    /**/
    char GateWay[4];
    /**/
    unsigned int Port;
    /*Web*/
    unsigned int WebPort;
    /*<RSPL_DHCP, RSPL_PPPOE, RSPL_Static, RSPL_WCDMA, RSPL_CDMA2000, RSPL_WIFI>*/
    char NetworkMode;
    short NetworkModeMask; //differ NetworkMode show Mask
    /* UPNP     RSPL_Open, RSPL_Close*/
    char UseUPNP;
    /*DNS1*/
    char Dns1[4];
    /*DNS2*/
    char Dns2[4];
    //ppoe
    /*PPPOE*/
    char PPPOEuser[RSPL_MAXNAMELEN];
    /*PPPOE*/
	char Password[RSPL_MAXNAMELEN];

	//RSPL_WCDMA, RSPL_CDMA2000
	char APN[32];			//3G APN
	char DialCode[16];		//3G DialCode
	char WirelessUser[32];	//3G UserName
	char WirelessPwd[32];  	//3G Password
	char WirelessIPaddr[4];    // 3G IP
	//RSPL_WIFI
	char RouterName[32];	//Wifi ESSID
	char RouterPwd[32];  	//Wifi Password
}RSPlat_NetworkBaseSet_t;




//
//GetSubStreamSet
//SetSubStreamSet
typedef struct {
	//
	unsigned int SubBitrate;
	char SubVideo; 	/* ,<RSPL_Close, RSPL_Open>*/ 
	char SubResolution;/*PS:CIF, */
	/*PAL = 1~25, NTSC = 1~30,
	  , PAL=1~5, NTSC=1~6
	  0*/ 
	char SubFps;  
	char SubAudio;/*:<RSPL_Close, RSPL_Open>*/
}RSPlat_SoleSubStreamSet_t;



//DDNS

//ServerAddr
#define RSPL_DDNS_MYQ_SEE     0
#define RSPL_DDNS_LTSCCTV     1
#define RSPL_DDNS_SystemPort  2
#define RSPL_DDNS_3322        3
#define RSPL_DDNS_DYNDNS      4
#define RSPL_DDNS_EASTERNDNS  5
#define RSPL_DDNS_NEWDDNS     6
#define RSPL_DDNS_NIGHTOWLDVR 7
#define RSPL_DDNS_LOREXDDNS   8

//GetDDNSSet
//SetDDNSSet
typedef struct {
    /*HOST16*/
    char HostName[RSPL_MAXNAMELEN];
    /*16*/
    char UserName[RSPL_MAXNAMELEN];
    /*8*/
    char UserPW[RSPL_MAXNAMELEN];
	/*bit  DDNS ,1:,0:
	  DDNS
	 DDNS
	 DDNS*/
    unsigned int DDNSUseFlag;
    /**/
    char ServerAddr;
    /*DDNS<RSPL_Close, RSPL_Open>*/
    char UseDDNS;
	char reservel[2];
}RSPlat_DDNSSet_t;




//Email
//intervaltime
#define RSPL_IntervalTime_60  0
#define RSPL_IntervalTime_180 1
#define RSPL_IntervalTime_300 2
#define RSPL_IntervalTime_600 3
//GetEmailSet
//SetEmailSet
typedef struct {
    /*SMTP*/
    char SMTP[RSPL_MAXNAMELEN];
    /**/
    char SendEmail[RSPL_MAXNAMELEN];
    /**/
    char SendEmailPW[RSPL_MAXNAMELEN];
    /*, */
    char RecvEmail[3][RSPL_MAXNAMELEN];
    /**/
    unsigned int Port;
    /*email,<RSPL_Close, RSPL_Open>*/
    char EmailSwitch;
    /*SSL<RSPL_Close, RSPL_Open>*/
    char SSLSwitch;
    /**/
    char intervaltime;
	char reservel;
}RSPlat_EmailSet_t;


//Mobile
//GetMobileSet
//SetMobileSet
typedef struct {
	//
    char MobileUserName[RSPL_MAXNAMELEN];/* 16*/    
    char MobileUserPwd[RSPL_MAXNAMELEN];/* 16*/     
    unsigned int MobilePort; /* */ 
	char reservel[4];
}RSPlat_MobileSet_t;


//

//Sensitivity
#define RSPL_Sensitivity_8 0
#define RSPL_Sensitivity_7 1
#define RSPL_Sensitivity_6 2
#define RSPL_Sensitivity_5 3
#define RSPL_Sensitivity_4 4
#define RSPL_Sensitivity_3 5
#define RSPL_Sensitivity_2 6
#define RSPL_Sensitivity_1 7

//AlarmOutTime
//Motion,Abnormal,IOAlarm
#define RSPL_AlarmTime_10s 1
#define RSPL_AlarmTime_20s 2
#define RSPL_AlarmTime_40s 3
#define RSPL_AlarmTime_60s 4


//BuzzerMooTime
//Motion,Abnormal,IOAlarm
#define RSPL_BuzzerTime_Off 0
#define RSPL_BuzzerTime_10s 1
#define RSPL_BuzzerTime_20s 2
#define RSPL_BuzzerTime_40s 3
#define RSPL_BuzzerTime_60s 4


// RecordDelayTime
//Motion,IOAlarm
#define RSPL_RecordDelayTime_30   0
#define RSPL_RecordDelayTime_1Min 1
#define RSPL_RecordDelayTime_2Min 2
#define RSPL_RecordDelayTime_5Min 3

#define MAX_ALARMOUT_NUM 16

#define MAX_ABNORMAL_LIST 3

//GetMotionSet
//SetMotionSet
typedef struct {
	unsigned int AlarmOutNum; //
	unsigned int VideoFormat; //
}RSPlat_MotionSet_t;

typedef struct {
	/*8*/
	char Sensitivity;

	/*<RSPL_Close, RSPL_Open>*/
	char ChnSwitch;

	/*Send Email <RSPL_Close, RSPL_Open>*/
	char SendEmail;

	/**/
	char BuzzerMooTime;

	/*Record Channel, */
	unsigned int RecordChannel;

	/**/
	char RecordDelayTime;
	/*Record channel switch  <RSPL_Close, RSPL_Open>*/
	char Record;

	/*Show Message <RSPL_Close, RSPL_Open>*/
	char ShowMessage;

	/*FullScreen  <RSPL_Close, RSPL_Open>*/
	char FullScreen;

	/*18032*/
	unsigned char RegionSetting[32];

	char reservel[2];

	/*Alarm Out <RSPL_Close, RSPL_Open>*/
	char AlarmOut;
	/**/
	char AlarmOutTime;
	char AlarmOutManager[MAX_ALARMOUT_NUM];
}RSPlat_SoleMotionSet_t;





//
/*
   ,
0videoloss
1:HddInvalidAlarm
2:HddNoSpaceAlarm
*/
//GetAbnormalSet
//SetAbnormalSet
typedef struct {
	unsigned int AlarmOutNum; 	//
}RSPlat_AbnormalSet_t;

typedef struct {
	// RSPL_Close, RSPL_Open
	char Enable;
	/**/
	char BuzzerMooTime;

	/* <RSPL_Close, RSPL_Open>*/
	char ShowMessage;
	/*Send Email <RSPL_Close, RSPL_Open>*/
	char SendEmail;

	/*Alarm Out <RSPL_Close, RSPL_Open>*/
	char AlarmOut;
	/**/
	char AlarmOutTime;
	char reservel[2];
	/*Alarm Out <RSPL_Close, RSPL_Open>*/
	char AlarmOutManager[MAX_ALARMOUT_NUM];

}RSPlat_SoleAbnormalSet_t;


//IO

//IoAlarmSet
#define RSPL_IOAlarm_NO    0
#define RSPL_IOAlarm_NC    1
#define RSPL_IOAlarm_Close 2

//GetIOAlarmSet
//SetIOAlarmSet
typedef struct {
	unsigned int AlarmInNum; 	//
	unsigned int AlarmOutNum; 	//
}RSPlat_IOAlarmSet_t;

typedef struct {
	//IO
	char IoAlarmSet;

	/*Alarm Out <RSPL_Close, RSPL_Open>*/
	char AlarmOut;

	/**/
	char AlarmOutTime;
	/**/
	char BuzzerMooTime;

	/*Record Channel, PS:, 1:, 0:*/
	unsigned int RecordChannel;

	/**/
	char RecordDelayTime;

	/* <RSPL_Close, RSPL_Open>*/
	char ShowMessage;
	/*Send Email <RSPL_Close, RSPL_Open>*/
	char SendEmail;
	/*FullScreen  <RSPL_Close, RSPL_Open>*/
	char FullScreen;

	/*Alarm Out <RSPL_Close, RSPL_Open>*/
	char AlarmOutManager[MAX_ALARMOUT_NUM];

	/*Record channel switch  <RSPL_Close, RSPL_Open>*/
	char Record;
	char reservel[3];

}RSPlat_SoleIOAlarmSet_t;






//
//



//
//main_resolution, sub_resolution
#define RSPL_EncodeD1  0
#define RSPL_EncodeHD1 1
#define RSPL_EncodeCIF 2

//main_bitrate, sub_bitrate
#define RSPL_Bitrate_32    	0
#define RSPL_Bitrate_48 		1
#define RSPL_Bitrate_64 		2
#define RSPL_Bitrate_80 		3
#define RSPL_Bitrate_96 		4
#define RSPL_Bitrate_128 		5
#define RSPL_Bitrate_160 		6
#define RSPL_Bitrate_192 		7
#define RSPL_Bitrate_224 		8
#define RSPL_Bitrate_256 		9
#define RSPL_Bitrate_320 		10
#define RSPL_Bitrate_384 		11
#define RSPL_Bitrate_448 		12
#define RSPL_Bitrate_512 		13
#define RSPL_Bitrate_640 		14
#define RSPL_Bitrate_768 		15
#define RSPL_Bitrate_896 		16
#define RSPL_Bitrate_1024 	17
#define RSPL_Bitrate_1280 	18
#define RSPL_Bitrate_1536 	19
#define RSPL_Bitrate_1792 	20
#define RSPL_Bitrate_2048 	21

//GetMainStreamSet
//SetMainStreamSet
typedef struct{
	unsigned long long DeviceType;
	unsigned int VideoFormat;
	unsigned int TotalFps;
}RSPlat_MainStreamSet_t;

typedef struct{
	/*: */
	unsigned int MainBitrate;
	/*:
	  RSPL_EncodeD1-
	  RSPL_EncodeHD1-
	  RSPL_EncodeCIF-*/
	char MainResolution;
	/* 	:
		PAL:1~25 NTSC:30
		0 
		PAL400NTSC480*/
	char MainFps;	
	char MainAudio;/*:<RSPL_Close, RSPL_Open>*/
	char reservel;

}RSPlat_SoleMainStreamSet_t;


//
//RecordMood
#define RSPL_RecordSchedule 0  //
#define RSPL_RecordAlaways  1  //
#define RSPL_RecordManual   2  //

//PackTime
#define RSPL_PackTime_15Min 0
#define RSPL_PackTime_30Min 1
#define RSPL_PackTime_45Min 2
#define RSPL_PackTime_60Min 3

//PreRecordTime
#define RSPL_PreRecordTime_0s  0
#define RSPL_PreRecordTime_10s 1

//GetRecordSet
//SetRecordSet
typedef struct {
	/*<RSPL_Close, RSPL_Open>*/
    char ChnCloseOrOpen; 
	char reservel[3];
}RSPlat_SoleRecordSet_t;

typedef struct {
    char PackTime;/*0-151-302-453-60*/    
    char PreRecordTime;/*:010s*/
	char reservel[2];
}RSPlat_RecordSet_t;



//

//hour
/*021-*/
#define RSPL_NoRecord     0
#define RSPL_AlarmRecord  1
#define RSPL_NormalRecord 2
//GetScheduleSet
//SetScheduleSet
typedef struct {
	unsigned int NormalHour[8]; 
	unsigned int MotionHour[8];
	unsigned int AlarmHour[8];
	unsigned int IntelligentHour[8];
}RSPlat_SoleScheduleSet_t;


//
//maintainperiod1
#define RSPL_EveryDay 0
#define RSPL_EveryWeek 1
#define RSPL_EveryMonth 2

//maintainperiod2
#define RSPL_Monday 		0
#define RSPL_Tuesday		1
#define RSPL_Wednesday 	2
#define RSPL_Thursday		3
#define RSPL_Friday		4
#define RSPL_Saturday		5
#define RSPL_Sunday		6

//GetMaintainSet
//SetMaintainSet
typedef struct {
	//
    /**/
    char time[4];
    
    char automaintain; /*<RSPL_Open, RSPL_Close>*/

	/*
	  RSPL_EveryDay
	  RSPL_EveryWeek
	  RSPL_EveryMonth
	 */
	char maintainperiod1; 
	/*0--67--37
	  RSPL_Monday 		
	  RSPL_Tuesday		
	  RSPL_Wednesday 	
	  RSPL_Thursday		
	  RSPL_Friday		
	  RSPL_Saturday		
	  RSPL_Sunday		
	 */
    char maintainperiod2; 
	char reservel[5];

}RSPlat_MaintainSet_t;

//
//




//
//<>





//

//

//
//

//SetDefaultSet
typedef struct {
	char DisplaySet_flag;
	char RecordSet_flag;
	char NetworkSet_flag;
	char AlarmSet_flag;
	char DeviceSet_flag;
	char SystemSet_flag;
	char AdvanceSet_flag;
	char reservel[1];
}RSPlat_DefaultSet_t;


//
//PlatformSelect
#define RSPlatform 0
//MediaConType
#define RSPlat_UDP 0
#define RSPlat_TCP 1

//GetPlatformSet
//SetPlatformSet
#define RSPL_FORM_SHOW *(UINT*)"PLEn"
#define RSPL_FORM_HIDE 0x00000000

#define RSPL_COMPRESS_ENABLE  1
#define RSPL_COMPRESS_DISABLE 0

typedef struct {
	char PuID[RSPL_MAXNAMELEN];
	char PlatformAddr[RSPL_MAXNAMELEN]; //36
	unsigned int PlatformPort;
	char PlatformSelect; //
	char PlatformEnable; // //RSPL_Open, RSPL_Close
	char MediaConType; //
	char CompressFlag; //
	unsigned int     PlatFormShowTag; //
}RSPlat_PlatformSet_t;

//SpeedSource
#define RSPL_SPEEDSOURCE_GPS     0
#define RSPL_SPEEDSOURCE_SENSOR  1

//DelayTime
#define RSPL_DELAYTIME_CLOSE  0
#define RSPL_DELAYTIME_1MIN   1
#define RSPL_DELAYTIME_5MIN   2
#define RSPL_DELAYTIME_10MIN  3
#define RSPL_DELAYTIME_20MIN  4
#define RSPL_DELAYTIME_30MIN  5
#define RSPL_DELAYTIME_40MIN  6
#define RSPL_DELAYTIME_50MIN  7
#define RSPL_DELAYTIME_60MIN  8

//
typedef struct {
	char CarNumber[20];							//
	unsigned int Pulse; 						//
	unsigned char RecordCarNumber;				//
	unsigned char PreviewCarNumber;				//GPS
	unsigned char RecordGps;					//GPS
	unsigned char DelayTime;					//
	unsigned char SpeedSource;					//0:GPS ,1:Sensor	
	char reserve[3];

	//
	float MaxSpeed;								//, floatint, 0,:200km/h
	unsigned int MaxSpeedSecond; 				//,:,1:60*60s
	char MaxSpeedPromptContent[128];		//
	
	//
	float ScramAcceleration;					//,:m/s2

	//
	unsigned int TireMinute;       				//:, 1:24*60 m
	unsigned int TireRestMinute;        		//,:, 1:24*60 m
	unsigned int TireAlarmFrequencyMinute;  	//
	
	unsigned int OverParkTimeMinute;			//, :min
	unsigned int GpsUpdataTime;					// gps
	unsigned int LockStateUpdatetime;			// 
	unsigned char ParkPeriod_StartHour[4];		// 
	unsigned char ParkPeriod_StartMin[4];		// 
	unsigned char ParkPeriod_EndHour[4];		// 
	unsigned char ParkPeriod_EndMin[4];			// 
	unsigned int  RecordLockState;				// 
}RSPlat_CarFunSet_t;

//
#define MAX_AIRULE_NUM 4
//RuleType
#define RSPL_RULETYPE_TRIWIRE   0
#define RSPL_RULETYPE_PERIMETER 1
//DirRestrict
#define RSPL_DIRRESTRICT_UNABLE 0
#define RSPL_DIRRESTRICT_ENABLE 1
//ShowRule
#define RSPL_SHOWRULE_UNABLE 0
#define RSPL_SHOWRULE_ENABLE 1

typedef struct {
	int x;
	int y;
}RSPlat_Coordinate;

typedef struct {
	char RuleEnable;
	char RuleType;
	char DirRestrict;
	char ShowRule;
	int Angle;
		
	union{
		RSPlat_Coordinate TriWireCo[2];
		RSPlat_Coordinate PerimeterCo[8];
	}TypeCo;
}RSPlat_AIRule;

typedef struct {
	char ChannelEnable;
	char reserve[3];
	
	RSPlat_AIRule AIRule[MAX_AIRULE_NUM];
}RSPlat_SoleAISet_t;


#define RSPL_LOCKNUM_MAX      12
#define RSPL_LOCKID_LEN   16
//LockFlag
#define RSPL_LOCKSTATE_LOCK   1
#define RSPL_LOCKSTATE_UNLCOK 0

typedef struct{
	BYTE LockFlag;
	BYTE Reservel[3];
	
	BYTE LockId[RSPL_LOCKID_LEN];	
}RSPlat_SoleELockSet_t;


#define RSPL_MAX_ICCARD_NUM		5
#define RSPL_MAX_ICCARD_LEN		12
#define RSPL_MAX_ICCARDLIST_LEN 24

typedef struct{
	BYTE ICCardID[RSPL_MAX_ICCARD_LEN]; 
	BYTE ICCardLockList[RSPL_MAX_ICCARDLIST_LEN];
}RSPlat_SoleICCardSet_t;

#endif
