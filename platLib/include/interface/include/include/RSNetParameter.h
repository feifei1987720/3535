/*********************************

  		RS NetParamether
		mai 20110224

*********************************/

#ifndef __RS_NETPARAMETHER_H__
#define __RS_NETPARAMETHER_H__

#include "RSNetProtocol.h"
#include "RSPLParameter.h" //RSPL_MAXNAMELEN

#define ParamToNet true // caiwang213 20110603
#define NetToParam false
#define CheckParam 2
#define ParamError (-2)

#define RS_Open 1
#define RS_Close 0


/* ******************** *
   
	

* ******************** */

//TvSystem
#define RS_PAL 0
#define RS_NTSC 1
typedef struct {
	UINT StructSize; //

	//
	BYTE DeviceName[MaxNameLen];
	UINT DeviceID;
    
	BYTE TvSystem; /*: <RS_PAL, RS_NTSC>*/
	
	//
	BYTE DeviceType[MaxNameLen]; //
	BYTE MacAddr[MaxNameLen];    
	BYTE SoftwareVer[MaxNameLen]; //
	BYTE HardwareVer[MaxNameLen]; //
	BYTE IEVer[MaxNameLen]; 	  //IE		/*Yiguang 11.06.15*/
	BYTE IpAddr[MaxIpLen];
	
	UINT MobilePort;//
	UINT Port;
	UINT WebPort;
	
	BYTE DdnsHostStatus[MaxNameLen];	
	BYTE HddStatus[MaxNameLen];
	
	BYTE P2pKeyID[12];
	BYTE P2pUID[40];               // kguard

}RSNet_SystemBaseInfo_t;



/* ******************** *
   
		

* ******************** */

//HddState
#define RS_HDDS_NONE 		0
#define RS_HDDS_UNFORMAT 	1
#define RS_HDDS_OK 	    	2
#define RS_HDDS_BAD 		3
#define RS_HDDS_RONLR 		4
#define RS_HDDS_BACKUP  	5   //
#define RS_HDDS_INVALID 	6

//OverWrite
#define RS_OverWrite_Close 	0
#define RS_OverWrite_Auto 	1
#define RS_OverWrite_1Hour 	2
#define RS_OverWrite_3Hours 3
#define RS_OverWrite_1Day 	4
#define RS_OverWrite_7Days 	5
#define RS_OverWrite_30Days 6
#define RS_OverWrite_90Days 7

typedef struct{
	UINT StructSize; //
	UINT HddMask; 		 //

	/**/
	unsigned long long HddState;  
	unsigned long long HddTotalSize;/*:*/
	unsigned long long HddFreeSize;
	unsigned long long HddFreeRecord; //
}RSNet_SoleHddInfo_t;

typedef struct {
	UINT StructSize; //

	//

	/**/
    BYTE  OverWrite;       
	BYTE RecordState;
	BYTE reservel[2];
}RSNet_HddInfo_t;




/* ******************** *
   
		

* ******************** */

//Protocol
#define RS_PelcoD 0
#define RS_PelcoP 1

//Baudrate
#define RS_Baudrate_1200 0
#define RS_Baudrate_2400 1
#define RS_Baudrate_4800 2
#define RS_Baudrate_9600 3

//DataBit
#define RS_DataBit_8 0
#define RS_DataBit_7 1
#define RS_DataBit_6 2
#define RS_DataBit_5 3

//StopBit
#define RS_StopBit_1 0
#define RS_StopBit_2 1

//Check
#define RS_CheckBit_None  0
#define RS_CheckBit_Odd   1
#define RS_CheckBit_Even  2
#define RS_CheckBit_Mark  3
#define RS_CheckBit_Space 4

//KbSwitch
#define RS_SerialKB  0
#define RS_SerialChn 1

typedef struct {
	UINT StructSize; //

    /**/
    BYTE Protocol;   
    /**/
    BYTE Baudrate;
    /**/
    BYTE DataBit;
    /**/
    BYTE StopBit;
    /**/
    BYTE Check;
	/*PS:OR 0:1:  */
	BYTE KbSwitch;
	BYTE reservel[2];
}RSNet_SerialInfo_t;




//

#define RS_MAX_USER_NUMBER 7
//#define RS_USERRIGHT_MAX_PAGE 128


// 

//UserStatus
#define RS_UserLiveOff 0
#define RS_UserLiveOn  1

/*
//
enum RSNet_UserPageRight{
	RS_LIVE_PAGE = 0,
	RS_OUTPUT_PAGE,
	RS_RECPARA_PAGE,
	RS_SCHEDULE_PAGE,
	RS_RECSEARCH_PAGE,
	RS_NETWORK_PAGE,
	RS_DDNS_PAGE,
	RS_EMAIL_PAGE,
	RS_MOBILE_PAGE,
	RS_NTP_PAGE,
	RS_HDD_PAGE,
	RS_ALARM_PAGE,
	RS_PTZ_PAGE,
	RS_PTZ_CTRL,
	RS_SERIAL_PAGE,
	RS_GENERAL_PAGE,
	RS_MOTION_PAGE,
	RS_VERSION_PAGE,
	RS_STOP_RECORD,
	RS_MAINTAIN_PAGE,
	RS_REBOOT,
	RS_POWER_OFF,
	RS_ABNORMAL_PAGE,
	RS_LOG_PAGE,
	RS_MAINSTREAM_PAGE,
	RS_SUBSTREAM_PAGE,
	RS_START_RECORD,
	RS_VIDEOCOVER_PAGE,
	RS_BACKTOFACTORYSET_PAGE,
};
*/


typedef struct 
{
	UINT StructSize; 	//
	UINT UserMask; 	// MAX_USER_NUM

	BYTE Password[8];		// 
	BYTE Password2[8];		// 
	BYTE UserName[10];		// 

	BYTE HaveSwitch;	// 	<RS_Open, RS_Close>

	BYTE HaveUser;		//  <RS_Open, RS_Close> 

	BYTE UserStatus;		// 		//1,0
	BYTE reservel[3];

	UserRightWrap UserRights; //
	BYTE c32Password[32];		// 
	BYTE c32Password2[32];		// 
}RSNet_SoleUserInfo_t;

typedef struct {
	UINT StructSize; 	//
	UINT MaxUerNum; 	//
}RSNet_UserSet_t;




//DST
//TimeMode
#define RS_TimeMode_24 0
#define RS_TimeMode_12 1

//DateMode
#define RS_DateMode_MDY 0
#define RS_DateMode_YMD 1
#define RS_DateMode_DMY 2

//Offset
#define RS_Dst1Hour 0
#define RS_Dst2Hour 1

//DstMode
#define RS_DstWeek 0
#define RS_DstDate 1


//StartMonth, EndMonth
#define RS_DstMonth_January 	0
#define RS_DstMonth_February 	1
#define RS_DstMonth_March 		2
#define RS_DstMonth_April 		3
#define RS_DstMonth_May 		4
#define RS_DstMonth_June 		5
#define RS_DstMonth_July 		6
#define RS_DstMonth_August 		7
#define RS_DstMonth_September 	8
#define RS_DstMonth_October 	9
#define RS_DstMonth_November 	10
#define RS_DstMonth_December 	11

//StartWeek, EndWeek
#define RS_DstFristWeek 	0
#define RS_DstSecondWeek 	1
#define RS_DstThirdWeek 	2
#define RS_DstFourthWeek 	3
#define RS_DstFifthWeek 	4

//StartWeekDay, EndWeekDay
#define RS_DstSunday		0
#define RS_DstMonday 		1
#define RS_DstTuesday		2
#define RS_DstWednesday 	3
#define RS_DstThursday		4
#define RS_DstFriday		5
#define RS_DstSaturday		6


typedef struct {
	UINT StructSize; //

    DateTime time; //
    /**/
    BYTE TimeMode;
    /*0MM/DD/YY1YY-MM-DD2DD/MM/YY*/
    BYTE DateMode;

    /*<RS_Close, RS_Open>*/
    BYTE Dst;
	/* Dst0-1hour1-2hour */
    BYTE Offset;
    /*0-Week1-Date*/
    BYTE DstMode;
	/*12:0---11*/
    BYTE StartMonth;    
    /*0-1-2-3-,4-*/
    BYTE StartWeek;
	/*0-Sun,1-Mon, ...6-Sat*/
	BYTE StartWeekDay;
	/*12:0---11*/
    BYTE EndMonth;
    /*0-1-2-3-*/
    BYTE EndWeek;
	/*0-Sun,1-Mon, ...6-Sat*/
	BYTE EndWeekDay;

	BYTE reservel[5];

	/*Dst */
    BYTE StartDate[4];
	/*Dst */
    BYTE StartHour[4];
	/*Dst  */
    BYTE EndDate[4];
	/*Dst  */
    BYTE EndHour[4];
}RSNet_DSTSet_t;

//Ntp

/*NtpIPAddr  NTP*/
#define RS_Ntp_windows   0
#define RS_Ntp_nist 	 1
#define RS_Ntp_ntp 		 2
#define RS_Ntp_lorexddns 3
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
#define RS_TimeZone_GMT_b12 	0  
#define RS_TimeZone_GMT_b11 	1 
#define RS_TimeZone_GMT_b10 	2 
#define RS_TimeZone_GMT_b09 	3 
#define RS_TimeZone_GMT_b08 	4 
#define RS_TimeZone_GMT_b07 	5 
#define RS_TimeZone_GMT_b06 	6 
#define RS_TimeZone_GMT_b05 	7 
#define RS_TimeZone_GMT_b0430 	8 
#define RS_TimeZone_GMT_b04 	9 
#define RS_TimeZone_GMT_b0330 	10 
#define RS_TimeZone_GMT_b03 	11 
#define RS_TimeZone_GMT_b02 	12 
#define RS_TimeZone_GMT_b01 	13 
#define RS_TimeZone_GMT 		14
#define RS_TimeZone_GMT_f01 	15 
#define RS_TimeZone_GMT_f02 	16 
#define RS_TimeZone_GMT_f03 	17 
#define RS_TimeZone_GMT_f0330 	18 
#define RS_TimeZone_GMT_f04 	19 
#define RS_TimeZone_GMT_f0430 	20 
#define RS_TimeZone_GMT_f05 	21 
#define RS_TimeZone_GMT_f0530 	22 
#define RS_TimeZone_GMT_f0545 	23 
#define RS_TimeZone_GMT_f06 	24 
#define RS_TimeZone_GMT_f0630 	25 
#define RS_TimeZone_GMT_f07 	26 
#define RS_TimeZone_GMT_f08 	27 
#define RS_TimeZone_GMT_f09 	28 
#define RS_TimeZone_GMT_f0930 	29 
#define RS_TimeZone_GMT_f10 	30 
#define RS_TimeZone_GMT_f11 	31 
#define RS_TimeZone_GMT_f12 	32 
#define RS_TimeZone_GMT_f13 	33 


//IPC
typedef struct {
	UINT StructSize; //
	UINT ChannelMask;//

	/*IP*/   
	BYTE IPAddr[4];
	/**/
	UINT Port;
	/**/
	BYTE UserName[36];
	/**/
	BYTE UserPW[36];    
	/*,dvr*/
	BYTE ChnNo;
	/*0:raysharp,1:hikvison,2:sony*/
	BYTE Protocol;	
	/**/
	BYTE Enable;
	/*ip*/
	BYTE ChnMax;
	
	/*ip, */
	BYTE IPChnStatus;
	BYTE Version;
	BYTE reservel[2];

	unsigned long long u64devType;  //
	BYTE macAddr[36];    //mac 
}RSNet_SoleIPCSet_t;

typedef struct {
	UINT StructSize; //

//	/**/
//	UINT  NtpPort;
//	/*-:65535*/
//	UINT SyncPeriod;
	/**/
	BYTE NtpIPAddr[4];
	/*NTP <RS_Close, RS_Open>*/
	BYTE UseNtp;
    /* */
    BYTE TimeZone;
	BYTE reservel[2];
}RSNet_NtpSet_t;

// 
typedef struct {
	UINT StructSize; //
	RSNet_DSTSet_t DstSet;
	RSNet_NtpSet_t NtpSet;
	UINT           LanguageMask;
	BYTE           Language;    // LANGUAGE_E
	BYTE           VideoFormat;
	BYTE           AutoLogout;
	BYTE           reservel[5];
}RSNet_GeneralInfoSet_t;



//
typedef struct {
	UINT StructSize; //
	UINT ChannelMask; //

	BYTE Chroma; 		//, <0 - 63>
	BYTE Brightness; 	//, <0 - 63>
	BYTE contrast; 	//, <0 - 63>
	BYTE saturation; 	//, <0 - 63>

	BYTE DefaultColor; 	//
	BYTE reserved[3];
}RSNet_SoleColorSet_t;





//
#define MAX_VIDEOCOVER_NUM  32
//,
typedef struct {/**/
	UINT StructSize; //

	int CoverSwitch; //, <RS_Open, RS_Close>
    int x, y, w, h;
}RSNet_VideoCoverArea_t;


typedef struct {
	UINT StructSize; //
	UINT ChannelMask; 		 //
	
	BYTE VideoCoverSwitch;/* , <RS_Open, RS_Close>*/
	BYTE reservel[3];
	RSNet_VideoCoverArea_t VideoCoverArea[4]; //, 
}RSNet_SoleVideoCover_t;





//PTZ

/* Serial
//Protocol
#define RS_PelcoD 0
#define RS_PelcoP 1

//Baudrate
#define RS_Baudrate_1200 0
#define RS_Baudrate_2400 1
#define RS_Baudrate_4800 2
#define RS_Baudrate_9600 3

//DataBit
#define RS_DataBit_8 0
#define RS_DataBit_7 1
#define RS_DataBit_6 2
#define RS_DataBit_5 3

//StopBit
#define RS_StopBit_1 0
#define RS_StopBit_2 1

//Check
#define RS_CheckBit_None  0
#define RS_CheckBit_Odd   1
#define RS_CheckBit_Even  2
#define RS_CheckBit_Mark  3
#define RS_CheckBit_Space 4
*/

#define RS_MAX_CRUISE_POINT 256
typedef struct {
	UINT StructSize; //
	UINT ChannelMask; 		 //

    /**/
    BYTE Protocol;
    /**/
    BYTE Baudrate;
    /**/
    BYTE DataBit;
    /**/
    BYTE StopBit;
    /**/
    BYTE Check;
	/* PTZ  RS_Close, RS_Open*/
	BYTE EnablePtzCruise;
    BYTE reservel[2];
    /*,UIADDRESS1-255*/
    UINT Number;
    ///*, 1--255, 0 ! */
    //BYTE ChnCruiseTime[RS_MAX_CRUISE_POINT];	

}RSNet_SolePTZSet_t;

typedef struct {
	UINT StructSize; //

    /* PTZ  RS_Close, RS_Open*/
    UINT  EnablePtzCruise;     /*This para is reserved for another page later!*/
}RSNet_PTZSet_t;



//OSD
//Position
#define RS_PositionLeftUp 	 0
#define RS_PositionLeftDown  1
#define RS_PositionRightUp 	 2
#define RS_PositionRightDown 3

typedef struct {
	UINT StructSize; //
	UINT ChannelMask; 		 //

    /* 0-1-2-3-*/
    BYTE Position;

    /*osd RS_Open, RS_Close*/
    BYTE Preview;

    /*OSD RS_Open, RS_Close*/
    BYTE PreviewTimeSet;

    /*OSD RS_Open, RS_Close*/
    BYTE RecordTimeSet;

    /**/
    BYTE ChnName[20];

}RSNet_SoleOsdSet_t;

typedef struct {
	UINT StructSize; //

    /*OSD RS_Open, RS_Close*/
    BYTE PreviewTimeSet;
    /*OSD RS_Open, RS_Close*/
    BYTE RecordTimeSet;
    BYTE reservel[2];
}RSNet_OsdSet_t;


//
//NetworkMode
#define RS_DHCP   0
#define RS_PPPOE  1
#define RS_Static 2

//PPPoEState
#define PPPOE_CONNECTING   0
#define PPPOE_SUCCESS      1
#define PPPOE_FAIL         2

typedef struct {
	UINT StructSize; //

    /*IP*/
    BYTE IPAddr[4];
    /**/
    BYTE NetMask[4];
    /**/
    BYTE GateWay[4];
    /**/
    UINT Port;
    /*Web*/
    UINT WebPort;
    /*<RS_DHCP, RS_PPPOE, RS_Static>*/
    BYTE NetworkMode;
    /* UPNP     RS_Open, RS_Close*/
    BYTE UseUPNP;
	/**/
	WORD MobilePort;
    /*DNS1*/
    BYTE dns1[4];
    /*DNS2*/
    BYTE dns2[4];
    //ppoe
    /*PPPOE*/
    BYTE PPPOEuser[MaxNameLen];
    /*PPPOE*/
	BYTE Password[MaxNameLen];

    //3G
	BYTE APN[MaxNameLen];
	BYTE DialCode[MaxNameLen];
	BYTE WirelessUser[MaxNameLen];
	BYTE WirelessPwd[MaxNameLen];
	
	BYTE EncodeFlag; //Enable:1 Disable:0
	/*WAN : 0 WAN  ;1  LAN*/
	BYTE WANWkIn;
	
	/*PPPOE state  0:connecting.. 1:success 2:fail */
	BYTE PPPoEState;
	/*PPPoE IP */
	BYTE PPPoEIPaddr[4];
	BYTE BandWidth;
	BYTE Reserved[12];
}RSNet_NetworkBaseSet_t;



//sub_bitrate, mobile_bitrate
typedef enum{
	RS_SubBitrate_8	= 0,
	RS_SubBitrate_16,
	RS_SubBitrate_24,
	RS_SubBitrate_32,
	RS_SubBitrate_48,
	RS_SubBitrate_64,
	RS_SubBitrate_80,
	RS_SubBitrate_96,
	RS_SubBitrate_128,
	RS_SubBitrate_160,
	RS_SubBitrate_192, //10
	RS_SubBitrate_224,
	RS_SubBitrate_256,	
	RS_SubBitrate_320,	
	RS_SubBitrate_384,	
	RS_SubBitrate_448,	
	RS_SubBitrate_512,	
	RS_SubBitrate_768,	
	RS_SubBitrate_1024,	
	RS_SubBitrate_1536, //19
}RS_SubBitrate_e;

//
typedef struct {
	UINT StructSize; //
	UINT ChannelMask; //
	//
	RS_SubBitrate_e sub_bitrate;
	
	BYTE sub_video; 	/* ,<RS_Close, RS_Open>*/ 

	/*RS_Encode720P
	  RS_EncodeD1
	  RS_EncodeHD1,
	  RS_EncodeCIF*/
	BYTE sub_resolution; //DVRCIFIPCD1

	/*PAL = 1~25, NTSC = 1~30,
	  , PAL=1~5, NTSC=1~6
	  0*/ 
	BYTE sub_fps;  

	BYTE sub_audio;/*:<RS_Close, RS_Open>*/
	
}RSNet_SoleSubStreamSet_t;

//
typedef struct {
	UINT StructSize; //
	UINT ChannelMask; //

	//
	RS_SubBitrate_e mobile_bitrate;

	BYTE mobile_video; 	/* ,<RS_Close, RS_Open>*/ 

	/*RS_Encode720P
	  RS_EncodeD1
	  RS_EncodeHD1,
	  RS_EncodeCIF*/
	BYTE mobile_resolution;/*PS:CIF, */

	/*PAL = 1~25, NTSC = 1~30,
	  , PAL=1~5, NTSC=1~6
	  0*/ 
	BYTE mobile_fps;  

	BYTE mobile_audio; /* ,<RS_Close, RS_Open>*/

}RSNet_SoleMobileStreamSet_t;

//DDNS

//ServerAddr
#define RS_DDNS_MYQ_SEE     0
#define RS_DDNS_LTSCCTV     1
#define RS_DDNS_SystemPort  2
#define RS_DDNS_3322        3
#define RS_DDNS_DYNDNS      4
#define RS_DDNS_EASTERNDNS  5
#define RS_DDNS_NEWDDNS     6
#define RS_DDNS_NIGHTOWLDVR 7
#define RS_DDNS_LOREXDDNS   8

typedef struct {
	UINT StructSize; //

    /*HOST16*/
    BYTE HostName[MaxNameLen];
    /*16*/
    BYTE UserName[MaxNameLen];
    /*8*/
    BYTE UserPW[MaxNameLen];
	/*bit  DDNS ,1:,0:
	  DDNS
	 DDNS
	 DDNS*/
    UINT DDNSUseFlag;
    /**/
    BYTE ServerAddr;
    /*DDNS<RS_Close, RS_Open>*/
    BYTE UseDDNS;
	BYTE reservel[2];
	BYTE DDNSID[12];
}RSNet_DDNSSet_t;




//PPPOE
typedef struct {
	UINT StructSize; //

    /*PPPOE*/
    BYTE PPPOEuser[MaxNameLen];
    /*PPPOE*/
    BYTE Password[MaxNameLen];
    /*DNS1*/
    BYTE dns1[4];   // PPPOE
    /*DNS2*/
    BYTE dns2[4];
}RSNet_PPPoESet_t;




//Email
//intervaltime
#define RS_IntervalTime_60  0
#define RS_IntervalTime_180 1
#define RS_IntervalTime_300 2
#define RS_IntervalTime_600 3
typedef struct {
	UINT StructSize; //

    /*SMTP*/
    BYTE SMTP[MaxNameLen];
    /**/
    BYTE SendEmail[MaxNameLen];
    /**/
    BYTE SendEmailPW[MaxNameLen];
    /*, */
    BYTE RecvEmail[3][MaxNameLen];
    /**/
    UINT Port;
    /*email,<RS_Close, RS_Open>*/
    BYTE EmailSwitch;
    /*SSL<RS_Close, RS_Open>*/
    BYTE SSLSwitch;
    /**/
    BYTE intervaltime;
	BYTE reservel;
}RSNet_EmailSet_t;

typedef struct {
	UINT StructSize; 	//
	int MotionScheduleHour[16][8];
	int AlarmScheduleHour[16][8];
	int EventScheduleHour[8];
	BYTE reservel[4];
	int MotionScheduleHourEx[32][8];
	int AlarmScheduleHourEx[32][8];
}RSNet_EmailScheduleSet_t;

//Mobile
typedef struct {
	UINT StructSize; //
	//
    BYTE MobileUserName[MaxNameLen];/* 16*/    
    BYTE MobileUserPwd[MaxNameLen];/* 16*/     
    UINT MobilePort; /* */ 
	BYTE reservel[4];
}RSNet_MobileSet_t;


//Router Lan 
typedef struct {
	UINT StructSize; //
    BYTE IPAddr[4];  /* IP */
    BYTE NetMask[4]; /* */
    BYTE SSID[MaxNameLen];  /*  */    
    BYTE Passwd[MaxNameLen];/*  */     
	BYTE SecureType;        /*  */
	BYTE WiFiChannel; /**///add by st 20120509
	BYTE WpaAlgorithm;
	BYTE reservel[17];
}RSNet_RouterLanSet_t;

//

//Sensitivity
#define RS_Sensitivity_8 0
#define RS_Sensitivity_7 1
#define RS_Sensitivity_6 2
#define RS_Sensitivity_5 3
#define RS_Sensitivity_4 4
#define RS_Sensitivity_3 5
#define RS_Sensitivity_2 6
#define RS_Sensitivity_1 7

//AlarmOutTime
//Motion,Abnormal,IOAlarm
#define RS_AlarmTime_10s 1
#define RS_AlarmTime_20s 2
#define RS_AlarmTime_40s 3
#define RS_AlarmTime_60s 4


//BuzzerMooTime
//Motion,Abnormal,IOAlarm
#define RS_BuzzerTime_Off 0
#define RS_BuzzerTime_10s 1
#define RS_BuzzerTime_20s 2
#define RS_BuzzerTime_40s 3
#define RS_BuzzerTime_60s 4


// RecordDelayTime
//Motion,IOAlarm
#define RS_RecordDelayTime_30   0
#define RS_RecordDelayTime_1Min 1
#define RS_RecordDelayTime_2Min 2
#define RS_RecordDelayTime_5Min 3

#define MAX_ALARMOUT_NUM 16

typedef struct {
	UINT StructSize; //
	UINT ChannelMask; 	//

	/*8*/
	BYTE Sensitivity;

	/*<RS_Close, RS_Open>*/
	BYTE ChnSwitch;

	/*Send Email <RS_Close, RS_Open>*/
	BYTE SendEmail;

	/**/
	BYTE BuzzerMooTime;

	/*Record Channel, */
	UINT RecordChannel;

	/**/
	BYTE RecordDelayTime;
	/*Record channel switch  <RS_Close, RS_Open>*/
	BYTE Record;

	/*Show Message <RS_Close, RS_Open>*/
	BYTE ShowMessage;

	/*FullScreen  <RS_Close, RS_Open>*/
	BYTE FullScreen;

	/*18032*/
	BYTE RegionSetting[32];

	BYTE reservel[2];

	/*Alarm Out <RS_Close, RS_Open>*/
	BYTE AlarmOut;
	/**/
	BYTE AlarmOutTime;
	BYTE AlarmOutManager[MAX_ALARMOUT_NUM];

	/*20032*/
	BYTE RegionSetting3531[200];
}RSNet_SoleMotionSet_t;





//
/*
   ,
0videoloss
1:HddInvalidAlarm
2:HddNoSpaceAlarm
*/
typedef struct {
	UINT StructSize; 	//
	UINT EventTypeMask; 	//

	// RS_Close, RS_Open
	BYTE Enable;
	/**/
	BYTE BuzzerMooTime;

	/* <RS_Close, RS_Open>*/
	BYTE ShowMessage;
	/*Send Email <RS_Close, RS_Open>*/
	BYTE SendEmail;

	/*Alarm Out <RS_Close, RS_Open>*/
	BYTE AlarmOut;
	/**/
	BYTE AlarmOutTime;
	BYTE reservel[2];
	/*Alarm Out <RS_Close, RS_Open>*/
	BYTE AlarmOutManager[MAX_ALARMOUT_NUM];

}RSNet_SoleAbnormalSet_t;


//IO

//IoAlarmSet
#define RS_IOAlarm_NO    0
#define RS_IOAlarm_NC    1
#define RS_IOAlarm_Close 2

typedef struct {
	UINT StructSize; 	//
	UINT ChannelMask; 	//

	//IO
	BYTE IoAlarmSet;

	/*Alarm Out <RS_Close, RS_Open>*/
	BYTE AlarmOut;

	/**/
	BYTE AlarmOutTime;
	/**/
	BYTE BuzzerMooTime;

	/*Record Channel, PS:, 1:, 0:*/
	UINT RecordChannel;

	/**/
	BYTE RecordDelayTime;

	/* <RS_Close, RS_Open>*/
	BYTE ShowMessage;
	/*Send Email <RS_Close, RS_Open>*/
	BYTE SendEmail;
	/*FullScreen  <RS_Close, RS_Open>*/
	BYTE FullScreen;

	/*Alarm Out <RS_Close, RS_Open>*/
	BYTE AlarmOutManager[MAX_ALARMOUT_NUM];

	/*Record channel switch  <RS_Close, RS_Open>*/
	BYTE Record;

	/*Yiguang 2012.02.16 ,0-IPC;1-*/
	BYTE AlarmIO;

	BYTE reservel[2];

}RSNet_SoleIOAlarmSet_t;


/*,EyeHome-Host*/
/*8*/
#define MAX_AREA_NUMBER         8
/*,0-;1-*/
//ControlWay
#define RS_SCHEDULE_CONTROL   	0
#define RS_MANUAL_CONTROL	  	1

/*,0-;1-;2-;:,*/
//ControlMode
#define RS_MANUAL_LEAVEHOME		0
#define RS_MANUAL_SLEEP			1
#define RS_MANUAL_ATHOME		2

typedef struct {
	UINT StructSize;
    BYTE ControlWay;            /*,0-,1-*/
    BYTE ControlMode;           /*,0-;1-;2-*/
	BYTE ModeSchedule[8][24];   /*,0-;1-;2-*/
	BYTE AreaSwitch[4];         /*,0-;1-;2-;0718,0-;1-*/   	
	BYTE Reserved[6];
}RSNet_TELALARM_AreaParameter_t;

/*,,8,EyeHome-Host*/
typedef struct {
	UINT StoreSize;
	BYTE Name[20];
	BYTE AlarmOut;				/*Alarm Out <RS_Close, RS_Open>*/
	BYTE AlarmOutTime[8];		/**/
	BYTE BuzzerMooTime[8];		/**/
	BYTE DialNumber[8];			/**/
	UINT RecordChannel[8];		/*Record Channel, PS:, 1:, 0:*/
	UINT FullScreen[8];			/*FullScreen  <RS_Close, RS_Open>*/
	BYTE RecordDelayTime[8];	/**/
	BYTE ShowMessage;			/* <RS_Close, RS_Open>*/
	BYTE SendEmail;				/*Send Email <RS_Close, RS_Open>*/
	BYTE Record;				/*Record channel switch  <RS_Close, RS_Open>*/
	BYTE Reserved[4];
}RSNet_TELALARM_SoleAreaParameter_t;

/*,EyeHome-Host*/
#define MAX_WIRE_SENSOR_NUMBER  4       /**/
#define MAX_WIRELESS_SENSOR_NUMBER  12  /**/

#define RS_TRIGGLE_NC    0
#define RS_TRIGGLE_NO    1

#define RS_ALARMTYPE_DOOR   0           /**/
#define RS_ALARMTYPE_RAY    1           /**/
#define RS_ALARMTYPE_SILENT 2           /**/
#define RS_ALARMTYPE_FIRE   3           /**/
#define RS_ALARMTYPE_GAS    4           /**/

typedef struct {
	UINT StoreSize;
    BYTE Triggle;                   /**/
    BYTE AlarmType;                 /**/
    BYTE Area;                      /**/
    BYTE AlarmDisplay;              /**/
	BYTE Reserved[4];
}RSNet_TELALARM_SoleWireSensor_t;

typedef struct {
	UINT StoreSize;
    BYTE LearnCode;                 /**/
    BYTE AlarmType;                 /**/
    BYTE Area;                      /**/
    BYTE AlarmDisplay;              /**/
	BYTE Reserved[4];
}RSNet_TELALARM_SoleWirelessSensor_t;

/*,EyeHome-Host*/
#define MAX_PHONENUMBER_GROUP   5   /**/

typedef struct {
	UINT StoreSize;
	BYTE Name[20];                  /**/
	BYTE Number[40];                /**/
	BYTE DialTime;                  /**/
	BYTE Reserved[3];
}RSNet_SolePhoneNumberSetting_t;


//
/****************  huoxiaolong20110512  *****************************/

typedef struct{
	UINT StructSize;//
	/**/
	BYTE ContrailLength;
    BYTE reservel[3];
}RSNet_IntelligentSet_t;



typedef struct{
	UINT StructSize; 	//
	BYTE AlarmOut;
	/*Show Message */
	BYTE ShowMessage;
	/*Send Email */
	BYTE SendEmail;
	/*FullScreen */
	BYTE FullScreen;
	/*Record Channel*/
	BYTE Record;
	/*Record Delay Time*/
	BYTE RecordDelayTime;/*:30s1min,2min,5min*/
	BYTE BuzzerMooTime;/*:0,10204060s*/
	BYTE AlarmOutTime;/*:10204060s*/
	BYTE AlarmOutManager[MAX_ALARMOUT_NUM];
	UINT RecordChannel;
	BYTE reservel[4];

}RSNet_SoleIntAlarmSet_t;


#define RS_MAX_RULENUM  4

#define RS_Intelligent_Tripwire  0
#define RS_Intelligent_Perimeter 1
#define MAX_PERIMETER_POINT 8
#define MAX_TRIPWIRE  2
typedef struct{
	UINT StructSize; 	//
	/**/
	BYTE RuleEnable;
	/*0-1-*/
	BYTE RuleType;
	/**/
	BYTE Direction;
	/**/
	short Forbidden;
	BYTE reservel[3];
}RSNet_RuleNum_t;

//added by zgm 20110118
typedef struct{
	short x;//X
	short y;//Y
	short RuleNum;//
	short ForbiddenDirection;//
	BYTE reservel[4];
}RSNet_Point_t;

typedef struct{
	UINT StructSize; 	//
	UINT ChannelMask; 	//
	/*0-1-*/
	BYTE Enable;
	BYTE reservel[3];
	RSNet_SoleIntAlarmSet_t IntAlarm;
	/**/
	RSNet_Point_t TriWire[2 * RS_MAX_RULENUM];//add 20110118
	/**/
	RSNet_Point_t Perimeter[MAX_PERIMETER_POINT * RS_MAX_RULENUM];//add 20110118
	/**/
	RSNet_RuleNum_t RuleNumber[RS_MAX_RULENUM];
	//RSNet_SoleIntAlarmSet_t IntAlarm;
}RSNet_SoleIntelligentSet_t;

//level
#define  RS_level_1 0
#define  RS_level_2 1
#define  RS_level_3 2
#define  RS_level_4 3
#define  RS_level_5 4

//Avdtype
#define RS_MAXAVDTYPE_NUM   8

#define RS_Avdtype_BrightAbnormal   0
#define RS_Avdtype_ClarityAbnormal  1
#define RS_Avdtype_ColorAbnormal    2
#define RS_Avdtype_NoiseAbnormal    3
#define RS_Avdtype_MenuFreeze       4
#define RS_Avdtype_Nosignal         5
#define RS_Avdtype_StageChange      6
#define RS_Avdtype_Interfere        7
#define RS_Avdtype_PtzLoseControl   8
typedef struct{
	UINT StructSize; 	//
	/*AVD*/
	BYTE TypeEnable;
	/*5 1-5-*/
	BYTE level;
	BYTE reservel[2];
}RSNet_Avdtype_t;   




typedef struct{
	UINT StructSize; 	//
	/*Alarm Out */
	BYTE AlarmOut;
	/*Show Message */
	BYTE ShowMessage;
	/*Send Email */
	BYTE SendEmail;
	/*FullScreen */
	BYTE FullScreen;
	/*Record Channel*/
	BYTE Record;
	/*Record Delay Time*/
	BYTE RecordDelayTime;/*:30s,1min,2min,5min*/
	BYTE AlarmOutTime;/*:10204060s*/
	BYTE BuzzerMooTime;/*:010204060s*/
	UINT RecordChannel;
	BYTE AlarmOutManager[MAX_ALARMOUT_NUM];
	BYTE reservel[4];

}RSNet_SoleAvdAlarmSet_t;



typedef struct{
	UINT StructSize; 	//
	UINT ChannelMask; 	//
	/*0-1-*/
	BYTE Enable;
	BYTE reservel[3];
	/*160*/
	ULONG  RotateTime;
	RSNet_Avdtype_t Avdtype[RS_MAXAVDTYPE_NUM];
	RSNet_SoleAvdAlarmSet_t AvdAlarm;
}RSNet_SoleIntAvdSet_t;
//

//MediaConType
#define RSPlat_UDP 0
#define RSPlat_TCP 1
#define RSPL_MAXNAMELEN 36

#define RSPL_FORM_SHOW *(UINT*)"PLEn"
#define RSPL_FORM_HIDE 0x00000000
typedef struct{
	UINT StructSize;   		//
	
	//RsPlatform
	BYTE PuID[RSPL_MAXNAMELEN];
	BYTE PlatformAddr[RSPL_MAXNAMELEN]; //
	UINT PlatformPort;     //
	BYTE PlatformSelect;
	BYTE PlatformEnable;
	BYTE MediaConType;     //0(udp) 1(tcp)
	BYTE reservel[1];
	UINT PlatFormShowTag; //
}RSNet_PlatformSet_t;

//HUAWEI_Platform
#define RSPL_STATUS_OFFLINE  	0
#define RSPL_STATUS_ONLINE 		1

typedef struct{
	UINT StructSize;   		//
	
	BYTE Enable;			/* RS_Open, RS_Close*/
	BYTE CenterStatus;		/*RSPL_STATUS_OFFLINE, RSPL_STATUS_ONLINE*/

	BYTE ProtocolType;		/* RSPlat_UDP, RSPlat_TCP */
	BYTE DelayTime;			/* 0100 s */

	BYTE MainDevID[24];
	BYTE CenterIP[16];
	UINT CenterPort;
}RSNet_HuaweiPlatformSet_t;

typedef struct{
	UINT StructSize;   		//
	UINT ChannelMask;		//
	
	BYTE LensEnable;
	BYTE AlarmInEnable;
	BYTE AlarmOutEnable;
	BYTE SerialEnable;

	BYTE LensStatus;		/*RSPL_STATUS_OFFLINE, RSPL_STATUS_ONLINE*/
	BYTE AlarmInStatus;		/*RSPL_STATUS_OFFLINE, RSPL_STATUS_ONLINE*/
	BYTE AlarmOutStatus;	/*RSPL_STATUS_OFFLINE, RSPL_STATUS_ONLINE*/
	BYTE SerialStatus;		/*RSPL_STATUS_OFFLINE, RSPL_STATUS_ONLINE*/
	
	BYTE Reservel[8];
	
	BYTE LensID[24];
	BYTE LensName[48];

	BYTE AlarmInID[24];
	BYTE AlarmInName[48];
	
	BYTE AlarmOutID[24];
	BYTE AlarmOutName[48];
	
	BYTE SerialID[24];
	BYTE SerialName[48];
}RSNet_SoleHuaweiPlatformSet_t;


//
//main_resolution, sub_resolution
#define RS_EncodeD1   0
#define RS_EncodeHD1  1
#define RS_EncodeCIF  2
#define RS_Encode720P 3

//main_bitrate
typedef enum{
	RS_MainBitrate_32    =	0,
	RS_MainBitrate_48,	
	RS_MainBitrate_64,	
	RS_MainBitrate_80,	
	RS_MainBitrate_96,	
	RS_MainBitrate_128,	
	RS_MainBitrate_160,	
	RS_MainBitrate_192,	
	RS_MainBitrate_224,	
	RS_MainBitrate_256,	
	RS_MainBitrate_320,	
	RS_MainBitrate_384,	
	RS_MainBitrate_448,	
	RS_MainBitrate_512,	
	RS_MainBitrate_640,	
	RS_MainBitrate_768,	
	RS_MainBitrate_896,	
	RS_MainBitrate_1024 = 17,	
	RS_MainBitrate_1280,	
	RS_MainBitrate_1536,	
	RS_MainBitrate_1792,	
	RS_MainBitrate_2048 = 21,	
	RS_MainBitrate_4096 = 22,	//4M
	RS_MainBitrate_5120 = 23,	//5M
	RS_MainBitrate_6144 = 24,	//6M
	RS_MainBitrate_8192 = 25,	//8M
}RS_MainBitrate_e;

//SWAN 
#define RS_MainBitrate_Normal	RS_MainBitrate_4096;
#define RS_MainBitrate_Good		RS_MainBitrate_5120;
#define RS_MainBitrate_Better	RS_MainBitrate_6144;
#define RS_MainBitrate_Best		RS_MainBitrate_8192;

typedef struct
{
	UINT StructSize; //

	BYTE ResolutionMode; /*0-D1, 1-960*/
	
	BYTE reservel[3];
}RSNet_MainStreamSet_t;

typedef struct{
	UINT StructSize; //
	UINT ChannelMask; //

	/*: */
	RS_MainBitrate_e main_bitrate;
	
	/*
	  RS_Encode720P-720P
	  RS_EncodeD1-
	  RS_EncodeHD1-
	  RS_EncodeCIF-*/
	BYTE main_resolution;
	
	/* 	:
		PAL:1~25 NTSC:30
		0 
		PAL400NTSC480*/
	BYTE main_fps;	
	
	BYTE main_audio;/*:<RS_Close, RS_Open>*/	
	BYTE main_resolution960; 
}RSNet_SoleMainStreamSet_t;


//
//RecordMood
#define RS_RecordSchedule 0  //
#define RS_RecordAlaways  1  //
#define RS_RecordManual   2  //

//PackTime
#define RS_PackTime_15Min 0
#define RS_PackTime_30Min 1
#define RS_PackTime_45Min 2
#define RS_PackTime_60Min 3

//PreRecordTime
#define RS_PreRecordTime_0s  0
#define RS_PreRecordTime_10s 1

typedef struct {
	UINT StructSize; 	//
	UINT ChannelMask; 	//

	/*<RS_Close, RS_Open>*/
    BYTE ChnCloseOrOpen; 
	BYTE reservel[3];
}RSNet_SoleRecordSet_t;

typedef struct {
	UINT StructSize; 	//

    BYTE RecordMode;/*:0-1-2-*/
    BYTE PackTime;/*0-151-302-453-60*/    
    BYTE PreRecordTime;/*:010s*/
	BYTE reservel;
}RSNet_RecordSet_t;



//

//hour
/*021-*/
#define RS_NoRecord     0
#define RS_AlarmRecord  1
#define RS_NormalRecord 2

#define RS_NormalScheduleMode 0
#define RS_SmartScheduleMode 1

#define RS_ScheduleResolution_720P	0
#define RS_ScheduleResolution_D1   1
#define RS_ScheduleResolution_CIF  2
typedef struct {
	UINT StructSize; 	//
	UINT ChannelMask; 	//
	
	//MainStream, For MainStream Record Schedule
	int NormalHour[8]; 
	int MotionHour[8];
	int AlarmHour[8];
	int IntelligentHour[8];
	
	//Smart
	BYTE SmartScheduleEnable; //1:smart,
	BYTE ScheduleMode; 
	BYTE reservel[2];
	BYTE SmartNormalResolution[8];
	BYTE SmartAlarmResolution[8]; 
	int SmartNormalHour[8]; 
	int SmartAlarmHour[8];
	
	//SubStream, For SubStream Record Schedule
	int SubNormalHour[8]; 
	int SubMotionHour[8];
	int SubAlarmHour[8];
	int SubIntelligentHour[8];
	
}RSNet_SoleScheduleSet_t;


//
//maintainperiod1
#define RS_EveryDay 0
#define RS_EveryWeek 1
#define RS_EveryMonth 2

//maintainperiod2
#define RS_Monday 		0
#define RS_Tuesday		1
#define RS_Wednesday 	2
#define RS_Thursday		3
#define RS_Friday		4
#define RS_Saturday		5
#define RS_Sunday		6

typedef struct {
	UINT StructSize; 	//

	//
    /**/
    BYTE time[4];
    
    BYTE automaintain; /*<RS_Open, RS_Close>*/

	/*
	  RS_EveryDay
	  RS_EveryWeek
	  RS_EveryMonth
	 */
	BYTE maintainperiod1; 
	/*0--67--37
	  RS_Monday 		
	  RS_Tuesday		
	  RS_Wednesday 	
	  RS_Thursday		
	  RS_Friday		
	  RS_Saturday		
	  RS_Sunday		
	 */
    BYTE maintainperiod2; 
	
	/*
	255:
	0-off; 1-admin, 2~76
	 */
	BYTE defaultuserid;
	BYTE EnabledUserList;/*	0bit-admin; 1~6bit6*/
	BYTE EnableUser;/**/
	BYTE reservel[2];

}RSNet_MaintainSet_t;

//3G
typedef struct {
	UINT StructSize;
	UINT SignalLevel;
	BYTE SignalType[32];
	BYTE IpAddr[16];
}RSNet_SystemBase3GInfo_t;

//
//




//
//<>


//

//

//
//


typedef struct {
	UINT StructSize;
	BYTE DevicePuid[64];
	UINT Result;
}RSNet_TuobaoInfo_t;


typedef struct {
	UINT StructSize; 	//

	BYTE DisplaySet_flag;
	BYTE RecordSet_flag;
	BYTE NetworkSet_flag;
	BYTE AlarmSet_flag;
	BYTE DeviceSet_flag;
	BYTE SystemSet_flag;
	BYTE AdvanceSet_flag;
	BYTE reservel[1];
}RSNet_DefaultSet_t;


typedef struct
{
	UINT StructSize; 	// 

	BYTE Switch;         	// WIFI
	BYTE EncryptType;		//   0 - 1:WEP 2:WPA-PSK 3:WPA2-PSK 4:WPA-AUTO
	BYTE AuthMode;			//  	WEP:1:OPEN, 2:SHARED   WPA:3-TKIP, 4-AES 

	BYTE Reserve1[1];
	
	//.
	//ps:WEP,5/10/13/26 .
	//WPA
	//wifi
	BYTE Pwd[32];
	BYTE SSID[32];
}RSNet_WifiSet_t;


////wifi
typedef struct
{
	UINT StructSize; 	// 
	UINT ChannelMask;
	
	BYTE MacAddr[6];	
	BYTE reserve1[2];

	BYTE SSID[32];
	BYTE Protocol[16];	  	// 802.11
	
	BYTE EncryptType;		//   0: 1:WEP 2:WPA-PSK 3:WPA2-PSK 4:WPA-AUTO
	BYTE AuthMode;			//  	WEP:1:OPEN, 2:SHARED   WPA:3-TKIP, 4-AES 
	
	BYTE SignalQuality;   	//
	BYTE Frequency;       	//  

}RSNet_SoleWifiSet_t;


//
typedef struct {
	UINT StructSize; //
	/**/
	BYTE RecordCarNumber;
	/**/
	BYTE PreviewCarNumber;
	/*GPS*/
	BYTE RecordGps;
	/**/
	BYTE SpeedSource;
	/**/
	BYTE CarNumber[MaxNameLen];
	/**/
	UINT Pulse;
	/**/
	BYTE DelayTime;
	BYTE Reserved[11];	
}RSNet_VehicleMng_t;


#define RSNet_OD_NUM 3
//StaticOrDynamic
#define RS_ViewMode_Static	0
#define RS_ViewMode_Dynamic	1
//OutputMode
#define RS_OutputMode_Single	0
#define RS_OutputMode_Quarter	1
#define RS_OutputMode_Ninth		2
#define RS_OutputMode_Sixth		3
//IsVoutOrSpot
#define RS_Output_Vout   0
#define RS_Output_Spot	 1
//Resolution
#define RS_Resolution_800_600		0
#define RS_Resolution_1024_768		1
#define RS_Resolution_1280_1024		2
#define RS_Resolution_1440_900		3
#define RS_Resolution_HDMI			4
//InitGuiDev
#define RS_InitGuiDev_VOUT		0
#define RS_InitGuiDev_VGA		1
#define RS_InitGuiDev_AUTO		2

typedef struct{
	struct tag_ChannelSelect{
		unsigned char twentychn[20]; //
		unsigned char sixteenchn[16]; //
		unsigned char eightchn[8]; //1+7
		unsigned char sixchn[6]; //1+5
		unsigned char fourchn[4]; //
		unsigned char ninechn[9]; //
		unsigned char onechn;    //
		unsigned char reserve[64];
	}ChannelSelect;/**/
	unsigned char Margin[4];  /*0:, 1:, 2:, 3:, 2*/
	unsigned char PollingTime;/*: :0~3000*/
	unsigned char StaticOrDynamic;/*0-static1-dynamic()*/
	unsigned char OutputMode;/*:0-1-2-3-*/
	unsigned char spotAudio;/*:0-, 1-ch1, 2-ch2, 3-ch3, 4-ch4 ... ... 16-ch16*/
	unsigned char reserve[24];/**/
}RSNet_OutputAttr_t;

typedef struct{
	UINT StructSize; //
	
	RSNet_OutputAttr_t OutputAttr[RSNet_OD_NUM]; /*voattr[0]-OD_VOUT, voattr[1]-OD_VSPOT, voattr[2]-OD_VGA*/
	unsigned char Transparency;/* 0~128 */
	unsigned char IsVoutOrSpot;/*0-Vout,1-Spot*/
	unsigned char Resolution;/*4*/
	unsigned char InitGuiDev;/*GUI :0-VOUT, 1-VGA ,2-AUTO*/
	unsigned char OutputDev;/*:0-CVBS,1-VGA,2-HDMI*/
	unsigned char reserve1[15];/**/
}RSNet_OutputSet_t;

//IRCutMode
#define IRCUTMODE_AUTO   0
#define IRCUTMODE_DAY    1
#define IRCUTMODE_NIGHT  2

//IRCutSensitive
#define SENSITIVE_LENGTH_HIGHT 0
#define SENSITIVE_LENGTH_MIDD  1
#define SENSITIVE_LENGTH_LOW   2

//Flip
#define FLIP_CLOSE 0
#define FLIP_OPEN  1

//Mirror
#define MIRROR_CLOSE  0
#define MIRROR_OPEN   1

typedef struct {
	UINT StoreSize;
	UINT ChannelMask;

	BYTE IRCutMode;            //0:auto;1:day;2:night
	BYTE IRCutDelay;           //0~30;unit:s 
	BYTE IRCutSensitive;       //0:Hight;1:Midd;2:Low 
	
	//predefine  
	BYTE Mirror;        	//  	0:close; 1:open	
	BYTE Flip;           	// 	0:close; 1:open
	BYTE reservel[3];
}RSNet_SoleCameraSet_t;


///////////////////////////////////////////////////////////////////////////
//, 
///////////////////////////////////////////////////////////////////////////

typedef struct {
	UINT StructSize; 	//
	
	BYTE Mac[6];
	BYTE reservel[2];
}RSNet_KeyConf_t;

//////////////////////////////////////
//IP
#define MAX_IPFILTER_NUM 16

#define FILTERSWITCH_OPEN  0
#define FILTERSWITCH_RELYON_ENABLE 1
#define FILTERSWITCH_CLOSE   2

typedef struct {
	UINT StructSize;
	
	BYTE FilterSwitch; //0 IP (IP1IP (SingleLock ) -1 IP IP)

	BYTE reserve[3];
}RSNet_IPFilter_t;

typedef struct {
	UINT StructSize;
	UINT ChannelMask;
	
	BYTE IPAddr[4];
	BYTE Enable;//IP RS_OPEN  1 ; RS_CLOSE 0 

	BYTE reservel[3];
}RSNet_SoleIPFilter_t;

//////////////////////////////////////
//RTSP
//////////////////////////////////////

//enAuthMode
#define RTSP_HTTP_AUTH_NONE   0
#define RTSP_HTTP_AUTH_BASIC  1
#define RTSP_HTTP_AUTH_DIGEST 2

typedef struct {
	UINT StructSize;

	UINT RtspPort;
	UINT enAuthMode;// 
	UINT RtspEnable;
	char  RtspUserName[32];
	char  RtspPassword[32];
}RSNet_Rtsp_t;


//////////////////////////////////////
//FTP
//////////////////////////////////////
#define    FTP_RECTYP_NORMAL  1                                           
#define    FTP_RECTYP_ALARM   2                                                 
#define    FTP_RECTYP_ALL     3     

#define    FTP_LAST_ONE_H   0                                           
#define    FTP_LAST_TWO_H   1                                                 
#define    FTP_LAST_FOUR_H  2 
#define    FTP_LAST_ONE_D   3            
typedef struct
{
	UINT StructSize;
	UINT ChannelMask;
	BYTE TimeMode;    /**/
	BYTE ReqTypeMask; /*0*/
	BYTE RecType;     /*1:,2:,3:*/
	BYTE Reserve;
}RSNet_SoleFtpSet_t;

typedef struct
{
	UINT StructSize;
	BYTE FtpIpAddr[16];
	BYTE FtpLoginName[64];
	BYTE FtpLoginPwd[64];
	UINT FtpPort;	
	BYTE UseFtp;
	BYTE Reserved[3];
}RSNet_FtpSet_t;

#define LOCKNUM_MAX      12
#define NET_LOCKID_LEN   16
//LockFlag
#define LOCKSTATE_LOCK   1
#define LOCKSTATE_UNLCOK 0

typedef struct{
	UINT StructSize;
	UINT ChannelMask;
	
	BYTE LockFlag;
	BYTE Reservel[3];
	
	BYTE LockId[NET_LOCKID_LEN];	
}RSNet_SoleElecLockSet_t;


typedef struct
{
	UINT StructSize;
	UINT ChannelMask;
	BYTE DriveName[16];
	BYTE TimeTrigger;
	BYTE Enable;
	BYTE MotionEnable;	
	BYTE Reserved[5];
	BYTE RecvEmail[3][36];  //
}RSNet_CloudStorageSet_t;

typedef struct {
	UINT StructSize;
    /*SMTP*/
    BYTE SMTP[36];

    /**/
    BYTE SendEmail[36];

    /**/
    BYTE SendEmailPW[36];

    /**/
    BYTE RecvEmail[3][36];

    /**/
    UINT Port;

    /*SSL0--1--*/
    BYTE SSLSwitch;

    /*email,0--1--*/
    BYTE EmailSwitch;

    /**/
    BYTE intervaltime;/*0:60,1:180,2:300,3:600s*/
    BYTE reserve[1];
} RSNet_cloudemailsetting_t;

//typedef struct 
//{
//	UINT StructSize;
//	BYTE p2pUid[36];
//	BYTE p2pUser[36];
//	BYTE p2pPwd[36];
//	BYTE resev[4];
//} RSNet_P2pMobileSet_t;

// P2p ID 
typedef struct 
{
	UINT StructSize;

	BYTE QrCodeID[48];
	BYTE reserved[24];
}RSNet_P2pQrCodeIdSet_t;

typedef struct{
	UINT StructSize;
	UINT ChannelMask;
	
	BYTE RecordEnable;
	BYTE MotionEnable;
	BYTE IoAlarmEnable;
	BYTE Reservel[9];
}RSNet_SoleDevAllStatusSet_t;

typedef struct {
	UINT StructSize;
	
	BYTE IDNumber[24];
	BYTE KeyContent[2048]; 
	BYTE reserver[4];
}RSNet_LorexP2pParamSet_t;

typedef struct {
	UINT StructSize;

	BYTE AutoUpgradeEnable; 
	BYTE AutoUpgradePrompt; 
	BYTE Reserve[6];
	
}RSNet_FtpAutoUpgrade_t;

// MasterSwitch 
#define MASTER_SWITCH_OFF     0
#define MASTER_SWITCH_ON      1
#define MASTER_SWITCH_FORBID  2
typedef struct {
	UINT StructSize;

	BYTE MasterSwitch;   // 0 -  1-   2- 
	BYTE Reserve[15];   // 
}RSNet_ManualRecordSet_t;	


#endif
