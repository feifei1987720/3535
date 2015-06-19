/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Zhong caiwang <caiwang213@gmail.com>
 * Apr 2011-04-16
 *
 * Paramenter config relevant interface defines.
 */

#ifndef __INTF_PARAMCONFIG_H__
#define __INTF_PARAMCONFIG_H__

#define MAX_USERRIGHT_PAGE   (128)
#define MAX_IPV4_LEN         (4)
#define MAX_IPV6_LEN         (16)

#ifdef MAX_PWD_LEN
#define MAX_PASSWORD_LEN     MAX_PWD_LEN
#else
#define MAX_PASSWORD_LEN    (50)
#endif

#include "RSNetParameter.h"

typedef struct tag_USER_RIGHT_S
{
	U32 u32UserSetRight;       /*  ,,,,,, */
	U32 u32PreviewChannel;     /*  */
	U32 u32PlayBackChannel;    /*  */
	U32 u32BackupChannel;      /*  */
	U32 u32PtzControlChannel;  /*  */
	U8  u8UserPreview;        /*  */
	U8  u8UserPlayBack;       /*  */
	U8  u8UserBackup;         /*  */
	U8  u8UserPtzControl;     /*  */
	U8  u8Reserved[16];
} USER_RIGHT_S;

typedef struct tag_DEVINFO_S
{
	U64 u64DevType;              /*  */
	S8 s8DeviceType[MaxNameLen]; /*  */
	S8 s8DevSwVer[MaxNameLen];   /*  */
	S8 s8DevName[MaxNameLen];    /* , */
	S8 s8MacAddr[MaxNameLen];    /* MAC */
	U32 u32DeviceID;  	         /*  */
	U32 u32TotalFPS_D1;			 /* 
						            TotalFPS=100, 4CIF 25f/s2HD1 25f/s1D1 25f/s */
	U8 u8ChannelNum;             /*  */
	U8 u8VideoFormat;            /*  */
	U8 u8AlarmInNum;             /*  */
	U8 u8AlarmOutNum;            /* () */

	U8 u8AIAnalyzeCap;           /*  */
	U8 u8AIAnalyzeChannels;      /*  */
	U8 u8SerialNum;	             /* 0() */
	U8 u8AudioNum;               /*  */

	U8 u8ResolutioFrameFix;      /* :0-,1-2- */
	U8 u8PreviewOff;             /* D90160-1- */
	U8 u8VideoCoverNum;          /* D9016 */
	U8 u8SupportSpot;            /* spot */

	U8 u8SubStreamMin;           /* */
	U8 u8SubStreamMax;           /*    :0    1    2    3    4    5     6     7    8       9       10     11     12 
									     :8    16  24    32   48   64    80    96  128      160     192    224    256 */
	U8 u8SubStreamRestrict;
	U8 u8RouterShowTag;          /*  */

	U32 u32PlatFormShowTag;      /* Value( Show Plat*(UINT*)"PLEn" Hide Plat0x00000000 ) */

	USER_RIGHT_S stUserRights;   /*  */

	U8 u8LanShowTag;			  /*Lan*/
	U8 u8VideoActivateSwitch;     //  	
	U8 u8MotionMode;              //01  44*36 	
	U8 u8ResolutionMode;      //0 D1,1D1960P 2CIF, 3CIF

    U8 WriteFlashFlag;            // 0 flash 1  flash
	U8 u8HidePhonePage;           //01
	U8 u8UpgradeType;
	U8 u8ElecLockFlag;            //0  1 

	U32 u32TotalFPS_960;          /*960TotalFPS=100, 4CIF 25f/s2HD1 25f/s1D1 25f/s */ 
		
	U8 u8WifiStatus;              //0 wifi, 1 
	U8 u8MaxPreviewNum;           //0
	U8 u8RtspPageEnable;          //RTSP 0, 1 
	U8 u8FtpPageFlag;             //Ftp1 0
	
	U8 c32PasswordFlag;           // 1320
	U8 u8UrmetDevStatusFlag;      // UEMET 	
	U8 u8FileSystemFlag;          // 0  1  	
	U8 u8KguardP2pUidFlag;        // 0 Uid, 1Uid	
	
	U8 u8MaxUserNum;              //0
	U8 u8EmailFlagSwitch;         // EmailScheduleFlag 10:.
	U8 u8EmailScheduleFlag;       // 0 1
	U8 u8PtzHiddenFlag;           // PTZ 0 (), 1 ()     
	
	S8 s8PlaybackMutex;           //  10
	S8 reserved2[3];

} DEVINFO_S;

typedef struct tag_DEVCHN_INFO_S
{
    U64 u64DevType;               /*  */
    S8  s8CurChnName[20];         /* ,NVR */

    U8  u8CurChnState;            /* ,NVR */
    U8  u8RecState;               /*  */
    U8  u8CurRecType;             /*  */
    U8  u8MDState;                /* Motion */

    U8  u8IOState;                /* IO */
    U8  u8VLossState;             /*  */
    U8  u8HddState;               /*  */
    U8  u8MDSchedule;             /* */

    U8  u8AlarmSchedule;          /* IO*/
    U8  u8ChannelNum;			  /* */

    S8 s8Resered[2];
} DEVCHN_INFO_S;

typedef struct tag_LANGTAO_INFO_S
{
	S8  s8Username[MaxNameLen];/* 16 */
	S8  s8Password[MaxNameLen];/* 16 */
	U32 u32Port;               /*  */
    U8  u8IPAddr[4];           /* IP */
	U8  u8NetType;             /* 0:2.5G1:2.75G2:3G 3:WIFI*/
	S8  s8Resered[7];
} LANGTAO_INFO_S;

typedef struct tag_TUOBAO_INFO_S
{
	S8  s8Puid[64];
	S8  s8Host[32];
	U16 u16Port;
	S8 s8Reserved[6];
}TUOBAO_INFO_S;

typedef enum tag_ESAVE_PARAM_E
{
	ESAVE_PARAM_ECC  = 1,
	ESAVE_PARAM_FLASH,
	ESAVE_PARAM_NULL,
} ESAVE_PARAM_E;


typedef enum tag_OPT_BIT_E
{
	OPT_BIT_STOP_ENC = 0,
	OPT_BIT_STOP_REC,
	OPT_BIT_UPDATE_OSD,
	OPT_BIT_REBOOT,
	OPT_BIT_RESTART_PLATFORM,
	OPT_BIT_RESTART_MEDIASERV,
	OPT_BIT_UPDATE_DISPMODE,
	OPT_BIT_MAX,
} OPT_BIT_E;

typedef enum tag_SAVE_FLASH_OPT_E
{
	SAVE_OPT_STOP_NONE = 0,
	SAVE_OPT_STOP_ENC,  
	SAVE_OPT_STOP_REC,
	SAVE_OPT_STOP_ERC,
} SAVE_FLASH_OPT_E;

typedef enum tag_DEV_RES_E
{
	DEV_RES_CIF = 0,
	DEV_RES_HD1,
	DEV_RES_D1,
	DEV_RES_720P,
	DEV_RES_960P,
	DEV_RES_1080I,
	DEV_RES_1080P,
	DEV_RES_NULL,
} DEV_RES_E;

typedef enum tag_CH_RES_E
{
	CH_RES_D1  = 0, 
	CH_RES_HD1, 
	CH_RES_CIF, 
	CH_RES_720P, 
	CH_RES_960P,
	CH_RES_1080I,
	CH_RES_1080P,
	CH_RES_NULL,
}CH_RES_E;

typedef enum tag_HTTP_AUTH_E
{
	HTTP_AUTH_NONE   = 0,
	HTTP_AUTH_BASIC  = 1,
	HTTP_AUTH_DIGEST = 2,
	HTTP_AUTH_NULL
} HTTP_AUTH_E;

typedef enum tag_CHECK_USER_TYPE
{
	CHECK_USER_TYPE_IE  = 0,
	CHECK_USER_TYPE_MOBILE  ,
} CHECK_USER_TYPE_E;

typedef struct tag_SEARCH_REMOTE_DEV_INFO_S
{
	U32 Port;
	U32 WebPort;
	U8 IPAddr[4];
	U8 Netmask[4];
	U8 GateWay[4];
	U8 Dns1[4];
	U8 Dns2[4];
	U8 ChannelNum;/**/
	
	S8 DevType[MaxNameLen - 8]; //
	unsigned long long DevTypeULL;
	char DevVer[MaxNameLen]; //
	UINT DeviceID;  //
   	char DeviceName[MaxNameLen]; //        
	char MacAddr[MaxNameLen]; //MAC
	
	U8 Version ;
	U8 reserved[3];
	
	tag_SEARCH_REMOTE_DEV_INFO_S *pNext;
} SEARCH_REMOTE_DEV_INFO_S;

typedef struct tag_SET_REMOTE_DEVNET_INFO_S
{
	U8 TargetIPAddr[4];			/*IP*/
	U32 TargetPort;				/**/    
	
    /*IP*/
    U8 IPAddr[4];
    /**/
    U8 NetMask[4];
    /**/
    U8 GateWay[4];
    /*DNS1*/
    U8 Dns1[4];
    /*DNS2*/
    U8 Dns2[4];
    /**/
    U32 Port;
    /*Web*/
    U32 WebPort;
	
	S8 UserName[36];		/**/    
	S8 UserPW[36];		/**/    

	U8 MacAddr[36];
	unsigned long long DevTypeULL;
	U8 Version ;
	U8 reserved[3];
} SET_REMOTE_DEVNET_INFO_S;

typedef struct tag_MOBILE_ID_S
{
	int flag;
	S8 asid[36];
}MOBILE_ID_S;

typedef struct tag_MOBILE_MSG_PUSH_ID_S
{
	S8 boxid[36];
	int sidNum;
	MOBILE_ID_S sid[32];
}MOBILE_MSG_PUSH_ID_S;


class IParamConfig
{
	public:
		IParamConfig() {}
		virtual ~IParamConfig() {}
	
	public:
		/** 
		 * @remarks    
		 * @return     , -1.
		 */
		virtual S32 GetDevChnNum()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    
		 * @return     , -1.
		 */
		virtual S32 GetMaxUserNum()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    
		 * @pUserName  
		 * @pPassword  
		 * @enType     
		 * @return     ID, .
		 */
#define EUSER_NOUSER      (-1)
#define EUSER_AUTHFAILD   (-2)
#define EUSER_PERMDENY    (-3)
#define EUSER_EXIST       (-4)
		virtual S32 CheckUser( S8 *pUserName, S8 *pPassword , CHECK_USER_TYPE_E enType = CHECK_USER_TYPE_IE)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		
		/** 
		 * @remarks    
		 * @pDevInfo   , 
		 * @return     0, -1.
		 */
		virtual S32 GetDevInfo( DEVINFO_S *pDevInfo, S32 s32LocalUserID = 1 )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    
		 * @return     , NULL.
		 */
		virtual VOID *GetSysParam()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		/** 
		 * @remarks    ()
		 * @return     , NULL.
		 */

		virtual VOID UpdateSysParam()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}


		/** 
		 * @remarks    
		 * @return     , -1.
		 */
		virtual U16 GetMediaPort()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    Web
		 * @return     , -1.
		 */
		virtual U16 GetWebPort()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    IP
		 * @return     0, -1.
		 */
		virtual S32 GetNetworkState( S32 *s32Mode, U8 *pu8Ip, U8 *pu8NetMask )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    DNS
		 * @return     0, -1.
		 */
		virtual S32 GetDnsAddrs( U8 (*pu8Dns)[4], S32 s32DnsNum )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    
		 * @return     0, -1.
		 */
		virtual S32 GetHostname( S8 *ps8Hostname )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    Flash
		 * @return     0, -1.
		 */
		virtual S32 SaveFlash( SAVE_FLASH_OPT_E enOpt )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    
		 * @return     0, -1.
		 */
		virtual S32 RestartMachine()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    System base info
		 * @return     0, -1, : bit0:,bit1:,bit2:OSD,bit3:
		 */
		virtual S32 BaseInfoParamAdapter( VOID *pBaseInfoSet, VOID *pSoleBaseInfoSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    Hdd
		 * @return     0, -1.
		 */
		virtual S32 HddParamAdapter( VOID *pHddSet, VOID *pSoleHddSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    Serial
		 * @return     0, -1.
		 */
		virtual S32 SerialParamAdapter( VOID *pSerialSet, VOID *pSoleSerialSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    General
		 * @return     0, -1.
		 */
		virtual S32 GeneralParamAdapter( VOID *pGeneralSet, VOID *pSoleGeneralSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    UserInfo
		 * @return     0, -1.
		 */
		virtual S32 UserInfoParamAdapter( VOID *pUserInfoSet, VOID *pSoleUserInfoSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    Dst
		 * @return     0, -1.
		 */
		virtual S32 DstParamAdapter( VOID *pDstSet, VOID *pSoleDstSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    MainStream
		 * @return     0, -1, : bit0:,bit1:,bit2:OSD,bit3:
		 */
		virtual S32 MainStreamParamAdapter( VOID *pMainStreamSet, VOID *pSoleMainStreamSet,S32 s32ChnNO,BOOL bDirection,U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    SubStream
		 * @return     0, -1, : bit0:,bit1:,bit2:OSD,bit3:
		 */
		virtual S32 SubStreamParamAdapter( VOID *pSubStreamSet, VOID *pSoleSubStreamSet,S32 s32ChnNO,BOOL bDirection,U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    Shelter
		 * @return     0, -1.
		 */
		virtual S32 ShelterParamAdapter( VOID *pShelterSet, VOID *pSoleShelterSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    Ptz
		 * @return     0, -1.
		 */
		virtual S32 PtzParamAdapter( VOID *pPtzSet, VOID *pSolePtzSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    Osd
		 * @return     0, -1, : bit0:,bit1:,bit2:OSD,bit3:
		 */
		virtual S32 OsdParamAdapter( VOID *pOsdSet, VOID *pSoleOsdSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    NetBasic
		 * @return     0, -1, : bit0:,bit1:,bit2:OSD,bit3:
		 */
		virtual S32 NetBasicParamAdapter( VOID *pNetBasicSet, VOID *pSoleNetBasicSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    Mobile
		 * @return     0, -1.
		 */
		virtual S32 MobileParamAdapter( VOID *pMobileSet, VOID *pSoleMobileSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    Ddns
		 * @return     0, -1.
		 */
		virtual S32 DdnsParamAdapter( VOID *pDdnsSet, VOID *pSoleDdnsSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    PPPoE
		 * @return     0, -1.
		 */
		virtual S32 PPPoEParamAdapter( VOID *pPPPoESet, VOID *pSolePPPoESet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    Email
		 * @return     0, -1.
		 */
		virtual S32 EmailParamAdapter( VOID *pEmailSet, VOID *pSoleEmailSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    Ntp
		 * @return     0, -1.
		 */
		virtual S32 NtpParamAdapter( VOID *pNtpSet, VOID *pSoleNtpSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    Motion
		 * @return     0, -1.
		 */
		virtual S32 MotionParamAdapter( VOID *pMotionSet, VOID *pSoleMotionSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    Abnormal
		 * @return     0, -1.
		 */
		virtual S32 AbnormalParamAdapter( VOID *pAbnormalSet, VOID *pSoleAbnormalSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    IO
		 * @return     0, -1.
		 */
		virtual S32 IOAlarmParamAdapter( VOID *pIOSet, VOID *pSoleIOSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    Record
		 * @return     0, -1, : bit0:,bit1:,bit2:OSD,bit3:
		 */
		virtual S32 RecordParamAdapter( VOID *pRecSchedSet, VOID *pSoleRecSchedSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    Schedule
		 * @return     0, -1.
		 */
		virtual S32 ScheduleParamAdapter( VOID *pScheduledSet, VOID *pSoleScheduledSet,S32 s32ChnNO,BOOL bDirection, U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    Maintain
		 * @return     0, -1, : bit0:,bit1:,bit2:OSD,bit3:
		 */
		virtual S32 MaintainParamAdapter( VOID *pMaintainSet, VOID *pSoleMaintainSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    Color
		 * @return     0, -1.
		 */
		virtual S32 ColorParamAdapter( VOID *pColorSet, VOID *pSoleColorSet,S32 s32ChnNO,BOOL bDirection, U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    Default
		 * @return     0, -1, : bit0:,bit1:,bit2:OSD,bit3:
		 */
		virtual S32 DefaultParamAdapter( VOID *pDefaultSet, VOID *pSoleDefaultSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    Pea
		 * @return     0, -1.
		 */
		virtual S32 IntelligentParamAdapter( VOID *pIntelligentSet, VOID *pSoleIntelligentSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    Avd
		 * @return     0, -1.
		 */
		virtual S32 IntAvdParamAdapter( VOID *pAvdSet, VOID *pSoleAvdSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    EmailSchedule
		 * @return     0, -1.
		 */
		virtual S32 EmailScheduleParamAdapter( VOID *pEmailScheduledSet, VOID *pSoleEmailScheduledSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    
		 * @pDevInfo   , 
		 * @return     0, -1.
		 */
		virtual S32 GetDevChnInfo( DEVCHN_INFO_S *pDevChnsInfo, S32 s32ChnNum )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks  
		 * @return   0
		 */
		virtual S32 SetIOAlarmStatus(BOOL bStatus, S32 s32IOChn, U32 u32DelayTime)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);

		}
		
		/** 
		 * @remarks    
		 * @pu32ImageWidth, pu32ImageHight    
		 * @return     0, -1.
		 */
		virtual S32 GetImageWidth(U32 u32Chn, U32 *pu32ImageWidth, U32 *pu32ImageHight)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    
		 * @return     , -1.
		 */
		virtual S32 GetFrameRate(U32 u32Channel, STREAMTYPE_E enStreamType)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    
		 * @return     0, -1.
		 */
		virtual S32 GetSystemTime(DateTime *pSystemTime)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    
		 * @return     0, -1.
		 */
		virtual S32 SetSystemTime(DateTime *pSystemTime)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    Platform
		 * @return     0, -1.
		 */
		virtual S32 PlatformParamAdapter( VOID *pPlatformdSet, VOID *pSolePlatformdSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
				
		/** 
		 * @remarks    
		 * @return     , -1.
		 */
		virtual S32 GetMainthreadRunFlag()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    
		 * @return     , -1.
		 */
		virtual U64 GetDevType()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    
		 * @return     , -1.
		 */
		virtual U32 GetLangtaoInfo( LANGTAO_INFO_S *stInfo )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    
		 * @return     , DEV_RES_NULL.
		 */
		virtual DEV_RES_E GetDevResolution()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    
		 * @return     , -1.
		 */
		virtual S32 GetDevHddNum()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/**Yiguang 2011.12.19 
		 * @remarks    IPC
		 * @return     0, -1, : bit0:,bit1:,bit2:OSD,bit3:
		 */
        virtual S32 IPCParamAdapter( VOID *pIPCSet, VOID *pSoleIPCSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize )
        {
            SHOW_NOT_IMPLE_FUNC;
            throw Exception(__FUNCTION__);
        }

		virtual S32 Base3GInfoParamAdapter( VOID *p3GInfoSet, VOID *pSole3GInfoSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize)
		{
			SHOW_NOT_IMPLE_FUNC;
            throw Exception(__FUNCTION__);

		}
		/**maiguanwei 2012.01.12 
		 * @remarks    , ftp
		 * @return     0, -1
		 */
		virtual S32 GetDeviceName(S8 *ps8DeviceNameBuffer)
		{
            SHOW_NOT_IMPLE_FUNC;
            throw Exception(__FUNCTION__);
        }

		/** 
		 * @remarks    
		 * @return     , -1.
		 */
		virtual U16 GetMobilePort()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/**
		 * @remarks    IpcIOAlarm
		 * @return     0, -1, : bit0:,bit1:,bit2:OSD,bit3:
		 */
        virtual S32 IpcIOAlarmParamAdapter( VOID *pIpcIOAlarmSet, VOID *pSoleIpcIOAlarmSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize )
        {
            SHOW_NOT_IMPLE_FUNC;
            throw Exception(__FUNCTION__);
        }

		/** 
		 * @remarks    RouterLan
		 * @return     0, -1.
		 */
		virtual S32 RouterLanParamAdapter( VOID *pRouterLandSet, VOID *pSoleRouterLandSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    RouterWan
		 * @return     0, -1.
		 */
		virtual S32 RouterWanParamAdapter( VOID *pRouterWandSet, VOID *pSoleRouterWandSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		
		/** 
		 * @remarks    RTSP
		 * @return     , -1.
		 */

		virtual U16 GetRTSPPort()
		{
			return 554;
			
		   	SHOW_NOT_IMPLE_FUNC;
            throw Exception(__FUNCTION__);
		}
		
		
		/**
		 * @remarks    
		 * @return     0,pPasswordpUserName,-1
		 * pPassworkd  MAX_PWD_LEN
		 */

		virtual S32 GetUserPasswd(const S8 *pUserName, S8 *pPassword)
		{
			SHOW_NOT_IMPLE_FUNC;
            throw Exception(__FUNCTION__);
		}
		
		/**
		 * @remarks    (basic)
		 * @return     0,-1
		 */
		virtual S32 CheckUserByHttpBasic(const S8 *credentials)
		{
			SHOW_NOT_IMPLE_FUNC;
            throw Exception(__FUNCTION__);
		}
		
		/**
		 * @remarks    
		 * @return     CH_RES_NULL
		 */

		virtual CH_RES_E GetChnResolution(S32 s32ChnNo,  STREAMTYPE_E enStreamType)
		{
			return CH_RES_D1;
			
			SHOW_NOT_IMPLE_FUNC;
            throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    Wifi
		 * @return     0, -1.
		 */
		virtual S32 WifiParamAdapter( VOID *pWifidSet, VOID *pSoleWifidSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		/** 
		 * @remarks    RTSP
		 * @return     RTSP
		 */

	    virtual HTTP_AUTH_E  GetHttpAuthType()
		{
			return HTTP_AUTH_NONE;
			
			SHOW_NOT_IMPLE_FUNC;
            throw Exception(__FUNCTION__);
		}
        	virtual S32 TuobaoParamAdapter( VOID *pTuobaoSet, VOID *pSoleTuobaoSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		
		virtual S32 GetTuobaoInfo(TUOBAO_INFO_S *tbi)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 SetTuobaoInfo(TUOBAO_INFO_S *tbi)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    VehicleMng
		 * @return     0, -1.
		 */
		virtual S32 VehicleMngParamAdapter( VOID *pVehicleMngdSet, VOID *pSoleVehicleMngdSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		
		virtual S32 SearchRemoteDev(SEARCH_REMOTE_DEV_INFO_S *&pSearchResult)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 SearchRemoteDevRelease(SEARCH_REMOTE_DEV_INFO_S *&pSearchResult)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 SetRemoteDevNet(SET_REMOTE_DEVNET_INFO_S *pSetRemDevNetInfo)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		
		virtual S32 OutputSetParamAdapter( VOID *pvOutputSet, VOID *pvSoleOutputSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		
		virtual VOID ResetNetServer()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 TelAlarmAreaParamAdapter( VOID *pvTelAlarmArea, VOID *pvSoleTelAlarmArea, S32 s32ChnNO,BOOL bDirection, U32 u32ParseSize )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		
		virtual S32 TelAlarmWireSensorParamAdapter( VOID *pvTelAlarmWireSensor, VOID *pvSoleTelAlarmWireSensor, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 TelAlarmWirelessSensorParamAdapter( VOID *pvTelAlarmWirelessSensor, VOID *pvSoleTelAlarmWirelessSensor, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 PhoneNumberSettingParamAdapter( VOID *pvPhoneNumberSetting, VOID *pvSolePhoneNumberSetting, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		virtual S32 CameraSetParamAdapter( VOID *pvCameraSet, VOID *pvSoleCameraSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		virtual S32 MobileStreamSetParamAdapter( VOID *pvMobileStream, VOID *pvSoleMobileStream, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 HuaweiPlatformParamAdapter( VOID *pvHuaweiPlatform, VOID *pvSoleHuaweiPlatform, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		virtual S32 KeyConfParamAdapter( VOID *pvKeyConf, VOID *pvSoleKeyConf, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		
		/* 
		 *@param[in] s32ChnNO 
		 *@param[out] level
		 *@param[in] type type = 0 command type = 1  
		 * */
		virtual S32 GetDscpLevel(S32* Level, S32 type, S32 s32ChnNO)
        {
			SHOW_NOT_IMPLE_FUNC;
            throw Exception(__FUNCTION__);
        }

		/** 
		 * @remarks     
		 * @return     0, -1.
		 */
		virtual S32 VideoActivation( U32 u32ChannelBitmap)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
        /**
		 * IP 
		 * @result = 0 else 
		 * **/
		virtual S32  CheckIP(U8 *paddr)
        {
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
            
        }
		
		virtual S32 IPFilterParamAdapter( VOID *pvIPFilter, VOID *pvSoleIPFilter, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 RtspParamAdapter( VOID *pvRtspSet, VOID *pvSoleRtspSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual STREAMTYPE_E GetMobileStreamType()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 AddAllDeviceRun()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 StartRemoteUpgrade(S8 **pBuf, ULONG u32BufSize)
		{
			return 0;
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		
		virtual	S32 FtpParamAdapter( VOID *pFtpSet, VOID *pSoleFtpSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		
		virtual	S32 ElecLockParamAdapter( VOID *pElecLockSet, VOID *pSoleElecLockSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 CloudStorageParamAdapter( VOID *pCloudStSet, VOID *pSoleCloudStkSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 CloudEmailParamAdapter( VOID *pCloudEmailSet, VOID *pSoleCloudEmail, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		virtual S32 P2pQrCodeIDParamAdapter( VOID *pSet, VOID *pSoleSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual VOID EndRemoteUpgrade(S8 **ppBuf)
		{
			return ;
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		
		virtual S32 DevAllStatusParamAdapter( VOID *pDevAllStatusSet, VOID *pSoleDevAllStatusSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		
		virtual	S32 LorexP2pParamAdapter ( VOID *pLorexP2pParam, VOID *pSoleLorexP2pParam,S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 FtpAutoUpgradeParamAdapter( VOID *pFtpAutoUpgrade, VOID *pSoleFtpAutoAdapter, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    P2pkey Flash 
		 * @return     0, -1.
		 */
		virtual S32 SaveP2pKeyFlash()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 ManualRecordParamAdapter ( VOID *pManualRecordSet, VOID *pSoleManualRecordSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		
		/** 
		 * @remarks       
		 * @MsgInfoFlag   [out] pMsgInfo 1: ,0 
		 * @pMsgInfo      [out]  
		 * @return        0-1 
		 */
		virtual S32 ActivateCloudInfo(S32* MsgInfoFlag, VOID *pMsgInfo )		
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}


		virtual S32 MobileMsgPushParaHandler(void* pMsgPush, bool flag)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

};

#endif
