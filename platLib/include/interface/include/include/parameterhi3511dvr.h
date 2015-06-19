/********************************************************
  Copyright (C), 2007-2099,SZ-STREAMING Tech. Co., Ltd.
  File name:
  Author:
  Date:
  Description:
********************************************************/
#ifndef __PARAMETERHI3511DVR_H__
#define __PARAMETERHI3511DVR_H__

#include "commondatatype.h"

#define MAX_CHN_NUMPARA 16

#define MAX_USER_NUMBER 7 //add by mai for more user password 20100408
#define MAX_EVENTTYPE_NUM 3 
#define OD_MAX 3
#define USERRIGHT_MAX_PAGE 128
#define ASSIGN_FOUR_BYTES(number) (((number) + 3) / 4 * 4)

#define MAX_CRUISE_POINT	256

//#pragma pack(1)


//mai add for more user 20100623



	
typedef struct _recordmode_{
    unsigned char ChnCloseOrOpen[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)]; /**/
	unsigned char RecordState[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)]; 
    unsigned char RecordMode;/*:0-1-2-*/
    unsigned char PackTime;/*0-151-302-453-60*/    
    unsigned char PreRecordTime;/*:010s*/
    unsigned char reserve1[13];/**/
}recordmode_t;


typedef struct _mainstream_{
//	unsigned char ChnCloseOrOpen[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];
	unsigned char quality[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];/*:0-1-2-*/
	unsigned char bitrate[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];/*:0-CIF1-HD12-D1*/
	unsigned char resolution[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];/*:0-(D1)1-(HD1)2-(CIF)*/
	unsigned char fps[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];	/*:1~25/30*/
	unsigned char audio[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];/*:0-1-*/
	unsigned char ResolutionTotal;/*:0-(D1)1-(CIF)*/
	unsigned char IsFixFps;/*: 0-1-*/
	//unsigned char reserve1[12];/**/
	unsigned char reserve1[10];/**/  //mai edit 20101014
}__attribute__ ((__packed__)) mainstream_t;

typedef struct _substream_{
	unsigned char video[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)]; /* ,0,1 */ 
	unsigned char quality[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];
	unsigned int  bitrate[MAX_CHN_NUMPARA];                  /*48 ~ 256 */
														/* 0  1  2  3  4   5    6  7   8
														 * 48 64 80 96 128 160 192 224 256   */
	unsigned char resolution[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];/*CIF*/
	unsigned char fps[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];  /*1 ~ 25/30,1 ~ 5/6*/
	unsigned char audio[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];/*:0-1-*/
	unsigned char ResolutionTotal;
	unsigned char IsFixFps;
	unsigned char reserve1[12];/**/
}__attribute__ ((__packed__)) substream_t;


typedef struct _encodemode_{
	mainstream_t mainstream;
	substream_t substream;
}encodemode_t;


typedef struct _decodechn_{
    /*16chn------bit0*/
    unsigned int  selectchn;
    char          Volume;     /*-0-100*/
    char          Mute;       /*-0-1-*/
    unsigned char reserve1[6];
} decodechn_t;

typedef struct _recordschedule_{
    /*021-*/
    unsigned char hour[ASSIGN_FOUR_BYTES(7)][24];
    unsigned char reserve1[8];
}recordschedule_t;

typedef struct _hddmanage_{
    unsigned int   Redundancy;      /*32bit, bit=0,  =1,  */
    unsigned char  OverWriteMode;   /*0-1-*/
    unsigned char  OverWrite;       /*0-1-*/
    unsigned short OverWritePeriod; /**/
    unsigned char  reserve1[8];
}hddmanage_t;

typedef struct _systemlanguage_{
	/*bit ,1:,0:*/
	unsigned int LangUseFlag;
	
    /*0-1-*/
    unsigned char language;
    
    /**/
    unsigned char CurrentLang;
    unsigned char reserve1[6];

} systemlanguage_t;


#if 0   //mai hidden 20100622
typedef struct _userpassword_{
    char          UserPwd[8]; /**/
    char          UserPwd2[8];
    char          AdminPwd[8]; /**/
    char          AdminPwd2[8];
    unsigned int  DeviceID; /**/
    char 		  DeviceName[36];/* */
    unsigned char HavePwd;/*01*/
    unsigned char reserve1[23];

} userpassword_t;
#endif

typedef struct _alarmsetting_{
    /*I/O,0-1-2-*/
    unsigned char IoAlarmSet[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];
	/*Alarm Out */
	unsigned short int AlarmOut;
	unsigned short int AlarmOutManager[4];
    unsigned char AlarmOutTime[MAX_CHN_NUMPARA];/*:010204060s*/
	/*Show Message */
	unsigned short int ShowMessage;
    unsigned char BuzzerMooTime[MAX_CHN_NUMPARA];/*:010204060s*/
	/*Send Email */
	unsigned short int SendEmail;
	/*FullScreen */
	unsigned short int FullScreen;
	/*Record Channel*/
	unsigned short int Record;
	unsigned short int RecordChannel[MAX_CHN_NUMPARA];
	/*Record Delay Time*/
    unsigned char RecordDelayTime[MAX_CHN_NUMPARA];/*:010204060s*/
    unsigned char reserve1[16];
} alarmsetting_t;

typedef struct _showsetting_{
    /*0-1-2-3-*/
    unsigned char Position[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];

    /*osd*/
    unsigned char Preview[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];

    /**/
    char ChnName[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)][20];

    /**/
    unsigned char audio[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];

    /*0-1-*/
    unsigned char PreviewTimeSet;

    /*0-1-*/
    unsigned char RecordTimeSet;

    /*0-1-*/
    unsigned char voice;
    unsigned char reserve1[9];

} showsetting_t;

typedef struct _colorsetting_{
    /*0:1:2: 3:*/
    unsigned char Color[MAX_CHN_NUMPARA][4];
    unsigned char reserve1[8];

} colorsetting_t;

/*
    
    
    20% 5
    30% 10
    40% 15
    50% 20
 */
typedef struct _motiondetect_{
		/*4:0-1-2-3:*/
		unsigned char Sensitivity[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];
		/*0-1-*/
		unsigned char ChnSwitch[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];
		/*18032*/
		unsigned char RegionSetting[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)][32];
		unsigned char TriggerPercent;	   /* */
		/*Alarm Out */
		unsigned short int AlarmOut;
		unsigned short int AlarmOutManager[4];
		unsigned char AlarmOutTime[MAX_CHN_NUMPARA];/*:010204060s*/
		/*Show Message */
		unsigned short int ShowMessage;
		unsigned char BuzzerMooTime[MAX_CHN_NUMPARA];/*:010204060s*/
		/*Send Email */
		unsigned short int SendEmail;
		/*FullScreen */
		unsigned short int FullScreen;
		/*Record channel switch*/
		unsigned short int Record;
		/*Record Channel*/
		unsigned short int RecordChannel[MAX_CHN_NUMPARA];
		/*Record Delay Time*/
		unsigned char RecordDelayTime[MAX_CHN_NUMPARA];/*:010204060s*/
		unsigned char reserve[16];
	} motiondetect_t;

typedef struct _devicemaintain_{
    /**/
    unsigned char time[4];

    /*0-1-*/
    unsigned char automaintain;

    /*0-1-2-*/
    unsigned char maintainperiod1;

    /*0--67--37*/
    unsigned char maintainperiod2;
    unsigned char reserve1[9];

} devicemaintain_t;


/*20100905*/
typedef struct _ExceptionHandle_{
	/*Show Message */
	unsigned char ShowMessage;
	unsigned char BuzzerMooTime;/*:010204060s*/
	/*Alarm Out */
	unsigned char AlarmOut;
	unsigned char AlarmOutTime;/*:010204060s*/
	unsigned char AlarmOutManager[4];
	/*Event Type Switch */
	unsigned char Enable;
	/*Send Email */
	unsigned char SendEmail;
	unsigned char reserve1[22];
} ExceptionHandle_t;


/*20100905*/
typedef struct _Exception_{
	/*Event Type */
	ExceptionHandle_t VideoLossAlarm;/*0-1-*/
	ExceptionHandle_t InvalidAlarm; /*0-1-*/
	ExceptionHandle_t NoSpaceAlarm; /*0-1-*/
	ExceptionHandle_t reserve1[32];
} Exception_t;



typedef struct _PTZsetting_{
    /*0-Pelco-D1-Pelco-P*/
    unsigned char Protocol[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];

    /*0-12001-24002-48003-9600*/
    unsigned char Baudrate[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];

    /*0-81-72-63-5*/
    unsigned char DataBit[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];

    /*0-11-2*/
    unsigned char StopBit[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];

    /*0-None1-Odd2-Even3-Mark4-Space*/
    unsigned char Check[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];

    /*1--63*/
    unsigned char Number[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];

    /*, 1--255, 0 ! */
    unsigned char ChnCruiseTime[MAX_CHN_NUMPARA][ASSIGN_FOUR_BYTES(MAX_CRUISE_POINT)];	

    /* PTZ 0:1:*/
    unsigned int  EnablePtzCruise;
	
    /* PTZ 0:1
    		DVR DVR*/
    unsigned char PtzCruiseState;
    unsigned char reserve1[3];

} PTZsetting_t;

typedef struct _NetWorkSetting_{
    /*IP*/
    unsigned char IPAddr[4];

    /**/
    unsigned char NetMask[4];

    /**/
    unsigned char GateWay[4];

    /**/
    unsigned int Port;

    /*Web*/
    unsigned int WebPort;

    /*DNS1*/
    unsigned char dns1[4];

    /*DNS2*/
    unsigned char dns2[4];

    /*PPPOE16*/
    char PPPOEuser[36];

    /*PPPOE8*/
    char Password[36];

    /*0-DHCP1-PPPOE2-*/
    unsigned char NetworkMode;
	
    /* UPNP     0:1:*/
    unsigned char UseUPNP;

    unsigned char reserve1[10];

} NetWorkSetting_t;


typedef struct _ddnssetting_{
    /*HOST16*/
    char HostName[36];

    /*16*/
    unsigned char ServerAddr[36];

    /*16*/
    char UserName[36];

    /*8*/
    char UserPW[36];

	/*bit  DDNS ,1:,0:*/
    unsigned int DDNSUseFlag;

    /*DDNS0-1-*/
    unsigned char UseDDNS;
	unsigned char UpdateInterval;
    unsigned char reserve1[22];

} DdnsSetting_t;

typedef struct _networktimeprotocol_{
	/**/
	unsigned char NtpIPAddr[4];
	/**/
	unsigned int  NtpPort;
	/*-:65535*/
	unsigned int SyncPeriod;
	/*NTP 0-1-*/
	unsigned char UseNtp;
    /* */
    unsigned char TimeZone;
	unsigned char reservel[10];
	//unsigned char reservel[6]; //mai add edit 20101016
}__attribute__ ((__packed__)) networktimeprotocol_t;
	
	

typedef struct _systemsetting_{
    
	systemlanguage_t  LanguageSetting;	
	
	/*1970Dst */
	long int StartSecond;/*long int == time_t*/
	
	/*1970Dst*/
	long int EndSecond;
	
	/*Dst */
    unsigned char StartHour[4];

	/*Dst  */
    unsigned char EndHour[4];

	/*Dst */
    unsigned char StartDate[4];

	/*Dst  */
    unsigned char EndDate[4];
    
    /*0MM/DD/YY1YY-MM-DD2DD/MM/YY*/
    unsigned char DateMode;

    /*024112*/
    unsigned char TimeMode;

    /*0-1-*/
    unsigned char Dst;

	/* Dst0-1hour1-2hour */
    char Offset;

    /*0-Week1-Date*/
    unsigned char DstMode;

	/*12:0---11*/
    unsigned char StartMonth;    
    
    /*0-1-2-3-,4-*/
    unsigned char StartWeek;
    
	/*0-Sun,1-Mon, ...6-Sat*/
	unsigned char StartWeekDay;
    
	/*12:0---11*/
    unsigned char EndMonth;
    
    /*0-1-2-3-*/
    unsigned char EndWeek;

	/*0-Sun,1-Mon, ...6-Sat*/
	unsigned char EndWeekDay;
	
	/*0-Dst 1-Dst*/
	unsigned char HaveDstTime;

	unsigned char videoformat;/*:0-TS_PAL,1-TS_NTSC*/
	
	unsigned char AutoLogout;	// 

    unsigned char reserve1[6];

} systemsetting_t;


/*  : unsigned char spotaudio;
	reserve1[2] reserve1[9].   !!!
															writed by guixing at 20100303 . */
typedef struct _voattr_{
    struct _chnselect_{
        unsigned char twentychn[20]; //
        unsigned char sixteenchn[16]; //
        unsigned char eightchn[8]; //1+7
        unsigned char sixchn[6]; //1+5
        unsigned char fourchn[4]; //
        unsigned char ninechn[9]; //
        unsigned char onechn;    //
        unsigned char reserve[64];
    } chnselect;/**/
    unsigned long PollingTime;/*: 512550*/
    unsigned char Margin[4];/*0:, 1:, 2:, 3:, 2*/
    unsigned char StaticOrDynamic;/*0-static1-dynamic()*/
    unsigned char OutputMode;/*:0-1-2-3-*/
    unsigned char spotAudio;/*:0-, 1-ch1, 2-ch2, 3-ch3, 4-ch4 ... ... 16-ch16*/
    unsigned char reserve1[9];/**/
} voattr_t;

//output
typedef struct _videosetting_{
    voattr_t voattr[OD_MAX];/*voattr[0]-OD_VOUT, voattr[1]-OD_VSPOT, voattr[2]-OD_VGA*/
    unsigned char IsVoutOrSpot;/*0-Vout,1-Spot*/
    unsigned char vgaresolution;/*:0-800x600,1-1024x768,2-1280x1024,3-1440x900,4-1920x1080*/
    unsigned char Bt1120Resolution;/*:0-800x6001-1024x768, 2-1280x1024, 3-1920x1080*/
    //unsigned char cameramode;/*:0-TS_PAL,1-TS_NTSC*/
    unsigned char vgatype;/*VGA:0-VGA, 1-BT1120 */    
	unsigned char initGuiDev;/*GUI :0-VOUT, 1-VGA ,2-AUTO*/
	unsigned long transparency;/**/
 	unsigned char ChangeResolutionFlag;/* REVERT_VGARESOLUTION::0, :1, :2*/
	unsigned char oldvgaresolution;/**/
    unsigned char reserve1[17];/**/
} videosetting_t;

typedef struct _ie_client_setting_{
    /*K*/
    unsigned long BitRate;
    char          UserName[32]; //
    char          AdminName[32]; //
    char          UserPwd[8]; //
    char          AdminPwd[8]; /**/
    unsigned char HavePwd;/*01*/
    /*0(CIF 6FPS)1(QCIF 25FPS)*/
    unsigned char QualityOrFrame;
    unsigned char reserve1[22];

} ieclientsetting_t;

//add ipcamera ctrl2009.10.13
typedef struct _ipcamerasetting_{
    char           User[32];
    char           Password[32];
    char           vendor[16];
    unsigned char  IPAddr[4];
    unsigned short CameraPort;
    char           reserve1[6];
} ipcamerasetting_t;


typedef struct _langtaosetting_{    
    char WUserName[36];/* 16*/    
    char WUserPwd[36];/* 16*/     
    unsigned int WPort; /* */ 
    unsigned char IPAddr[4];/*IP*/
    unsigned char network;/*0:2.5G1:2.75G2:3G 3:WIFI*/
    unsigned char reserve[7];
} langtaosetting_t;

typedef struct _mobilesetting_{
    /*0~65535*/
    unsigned long port;
    /*0:2.5G1:2.75G2:3G 3:WIFI*/
    unsigned char network;
    unsigned char channel;
    unsigned char mode;    /*0:VBR 1:CBR*/
    unsigned char reserve[25];
} mobilesetting_t;

typedef struct _emailsetting_{
    /*SMTP*/
    char SMTP[36];

    /**/
    char SendEmail[36];

    /**/
    char SendEmailPW[36];

    /**/
    char RecvEmail[3][36];

    /**/
    unsigned int Port;

    /*SSL0--1--*/
    unsigned char SSLSwitch;

    /*email,0--1--*/
    unsigned char EmailSwitch;

    /**/
    unsigned char intervaltime;/*0:60,1:180,2:300,3:600s*/
    char          reserve[1];

} emailsetting_t;


typedef struct _KBsetting_{//edit by liuyang 2009.7.8
    /*0-Pelco-D1-Pelco-P*/
    unsigned char Protocol;   
    /*0-12001-24002-48003-9600*/
    unsigned char Baudrate;
    /*0-81-72-63-5*/
    unsigned char DataBit;
    /*0-11-2*/
    unsigned char StopBit;
    /*0-None1-Odd2-Even3-Mark4-Space*/
    unsigned char Check;
    unsigned char reserve1[7];
} KBsetting_t;

typedef struct _videocover_{
	/* 0:;1:*/
	unsigned char VideoCoverSwitch[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];
	
	/*0: 1: */
	unsigned char CoverNumber[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)][4];
	
	/**/
	rectanglearea_t CoverCoordinate[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)][4];
	unsigned char reservel[16];
} videocover_t;

/*128
  */
typedef struct _keyparameter_{
    unsigned char    MacAddr[6];
    unsigned char    TvSystem; /*:0-TS_PAL1-TS_NTSC*/
    unsigned char    reserve1[1];
    unsigned long    UpgradeAddr; /*12*/
    unsigned char AutoSEQFlag;//Yiguang for AutoSEQ 10.07.20
   // unsigned char    reserve[116];
   unsigned char    reserve[115];
} keyparameter_t;



//
enum _USER_RIGHT_BIT_{
	LIVE_PAGE = 0,
	OUTPUT_PAGE,
	RECPARA_PAGE,
	SCHEDULE_PAGE,
	RECSEARCH_PAGE,
	NETWORK_PAGE,
	DDNS_PAGE,
	EMAIL_PAGE,
	MOBILE_PAGE,
	NTP_PAGE,
	HDD_PAGE,
	ALARM_PAGE,
	PTZ_PAGE,
	PTZ_CTRL,
	SERIAL_PAGE,
	GENERAL_PAGE,
	MOTION_PAGE,
	VERSION_PAGE,
	STOP_RECORD,
	MAINTAIN_PAGE,
	REBOOT,
	POWER_OFF,
	ABNORMAL_PAGE,
	LOG_PAGE,
	MAINSTREAM_PAGE, //mai add 20100929
	SUBSTREAM_PAGE,
	START_RECORD, //mai add 20101013
	VIDEOCOVER_PAGE,//ZENGHUI 20101102
	BACKTOFACTORYSET_PAGE,
};


// 
typedef struct _UserInfo_
{	// 	10+16+3+64+11 = 104
	char UserName[10];//10		// 
	char Password[8];//8		// 
	char Password2[8];//8	// 

	unsigned char HaveSwitch;	// 	// 10

	unsigned char HaveUser;		//  // 10

	unsigned char IsLive;		// 		//1,0

	//UserRights_t userRights;	// 	//64
	
    HI_U8 au8UserRights[USERRIGHT_MAX_PAGE/8];			/*
																,
															    DisplaySet_Page = 0;	//bit 0
														        RecordSet_Page;			//bit 1
														        NetworkSet_Page;		//bit 2
														        VideoSearch_Page;		//bit 3
														       	HddManage_Page;			//bit 4
														        AlarmSet_Page;			//bit 5
														        PTZSet_Page;			//bit 6
														        PTZControl_Page;		//bit 7
														        MobileSet_Page;			//bit 8
														        MotionSet_Page;			//bit 9
														        KBSet_Page;				//bit 10
														        TimeSet_Page;			//bit 11
														        Schedule_Page;			//bit 12
														        VideoSet_Page;			//bit 13
														        LanguageSet_Page;		//bit 14
														        SystemMaintain_Page;	//bit 15
														        */

	unsigned int UserPreviewRight; 		//
	unsigned int UserRecordPlayRight; 	//
	unsigned char reservel[3];	//
}__attribute__ ((__packed__)) UserInfo_t;

// 
typedef struct _UserManage_
{	// 4 + 36 + 104 + 104 * 6 + 8 
	unsigned int DeviceID;	//  
	char 		 DeviceName[36];/* */	//mai add 20100621

	UserInfo_t adminInfo;		// 

	UserInfo_t userinfo[MAX_USER_NUMBER - 1];	// 
	unsigned char reservel[8];
}__attribute__ ((__packed__)) UserManage_t;



//
//1,0
typedef struct _DEFAULT_PAGE_SELECT_{
	char DisplaySet_flag;
	char RecordSet_flag;
	char ScheduleSet_flag;
	char NetworkSet_flag;
	char DdnsSet_flag;
	char HDDManagement_flag;
	char AlarmSet_flag;
	char EmailSet_flag;
	char PTZSet_flag;
	char MobileSet_flag;
	char MotionSet_flag;
	char KBSet_flag;
	char TimeSet_flag;
	char PassWordSet_flag;
	char VideoSet_flag;
	char SystemMaintain_flag;

	unsigned char reservel[8];
}__attribute__ ((__packed__)) DefaultSelectFlag_t;





#define SETUP_PARAMETER_MAGIC 0x20091210
typedef struct _setupparameter_{
    unsigned long     magic; /**/
    keyparameter_t    KeyPara;
    recordmode_t      RecordMode;
    recordschedule_t  RecordSchedule[MAX_CHN_NUMPARA];
    decodechn_t       DecodeChn;
    hddmanage_t       HddManage;
    //userpassword_t    UserPassWord;	//mai hidden 20100622
    alarmsetting_t    AlarmSetting;
    showsetting_t     ShowSetting;
    colorsetting_t    ColorSetting;
    motiondetect_t    MotionDetect;
    Exception_t       ExceptionSetting;
    devicemaintain_t  DeviceMaintain;
    PTZsetting_t      PTZSetting;
    //systemlanguage_t  LanguageSetting;
    NetWorkSetting_t  NetworkSetting;
    DdnsSetting_t     DDNSSetting;
    systemsetting_t   SystemSetting;
    videosetting_t    VideoSetting;
    mobilesetting_t   mobile;
    langtaosetting_t  langtao;
    KBsetting_t		  KBSetting;
    videocover_t	  VideoCover;
    ipcamerasetting_t ipcamerasetting;
    ieclientsetting_t ieclient;
    emailsetting_t    email;
#if defined(MORE_USER)
	UserManage_t 	  UserManage;	//mai add 20100621
#endif
	DefaultSelectFlag_t defaultselectflag;	//mai add 20100622		
    networktimeprotocol_t NtpService;/* */
	encodemode_t      Encodemode;
    char              StringEnd[8];
}setupparameter_t;

typedef struct _stateparameter_{
    char DeviceType[24];
    char AppVer[24];/* */
    char UbootVer[24];
    char KernelVer[24];
    char RootfsVer[24];
    char SlaveUbootVer[24];
    char SlaveKernelVer[24];
    char SlaveRootfsVer[24];
    char LogoVer[24];
    char MacAddr[24];
    char SerialNum[24];

	char SWVertion[24];/*UI */
    char IEVertion[24];
    char HWVertion[24];
    char KBVertion[24];

    /*
    0:;1:;2:;3:*/
    unsigned long long HddState[8];
    unsigned long long HddTotalSize[8];/*:*/
    unsigned long long HddFreeSize[8];
    unsigned long long HddFreeRecord[8];
    unsigned long long ParaId;/*ID*/
} stateparameter_t;

/*
  */
typedef struct _systemparameter_{
    setupparameter_t setup;/**/
    stateparameter_t state;/**/
} systemparameter_t;

#endif
