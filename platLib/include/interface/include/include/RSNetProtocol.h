/********************************************************************
		
	created by  2011.1.8
*********************************************************************/
#ifndef __RSNetProtocol_h__
#define __RSNetProtocol_h__

#pragma pack(4)

#define RS_DVR_D9104		0x5253000000010100LLU
#define RS_DVR_D9104B		0x5253000000010101LLU
#define RS_DVR_D9108		0x5253000000010200LLU
#define RS_DVR_D9108B		0x5253000000010201LLU
#define RS_DVR_D9016		0x5253000000010300LLU

#define RS_DVR_D9216H		0x5253000000020300LLU

#define RS_DVR_D9316H		0x5253000000020301LLU
#define RS_DVR_ID9316H		0x5253000001020300LLU

#define RS_DVR_D9604L		0x5253000100010100LLU
#define RS_DVR_D9608L		0x5253000100020200LLU
#define RS_DVR_D9616L		0x5253000100020300LLU

#define RS_DVR_D9608H		0x5253000100020201LLU
#define RS_DVR_D9616H		0x5253000100020301LLU

#define RS_NVR_N1004    	0x5253030200010100LLU
#define RS_HOME_H1002H  	0x5253020100010100LLU
#define RS_HOME_H1002M  	0x5253020100010101LLU
#define RS_DVR_D9608B		0x5253000100020202LLU
#define RS_DVR_D9616B		0x5253000100020302LLU

#define RS_IPC_TI       	0x5253040200030000LLU
#define RS_IPC_GM       	0x5253040200040000LLU

#define RS_MDVR_D5704 		0x5253010100010100LLU

#define RS_DECODER_A9001	0x5253050100010100LLU

#define MaxIpLen	16
#define MaxNameLen	36
#define MAX_NET_PARAMS_LEN  32


//////////////////////////////////////////////////////////////////////////
//MsgHead.Version
#define MsgHeadV1	0 
#define MsgHeadV2   2

//MsgHead.AckFlag
#define AckSuccess					0
#define AckFail						1
#define AckNoMsgType 				2
#define AckLoginUserLogined			10
#define AckLoginNoUserName			11
#define AckLoginPasswordError		12
#define AckLoginIPFilerError		13
#define AckDualtalkBusy				20
#define AckAuthorityFail			50
#define AckParamError				51
#define AckUpgrading				60
#define AckUpgradeVersionError		61
#define AckPreviewNoStream          62
#define AckUpgradeVersionSame       63
#define AckUpgradeLanguageError     64 
#define AckOverMaxPrevNumErr		65 
#define AckOverMaxUser              66 

//MsgHead.MsgType
#define MsgSearchDev			11
#define MsgDevNetCfg            12
#define MsgLogin				31
#define MsgHeartbeat			32
#define MsgTcpPreview			71
#define MsgGetUdpAddr			81
#define MsgStartUdpPreview		82
#define MsgStopUdpPreview		83
#define MsgReqIFrame		    87
#define MsgPTZControl			101
#define MsgQueryRecord			111
#define MsgRecordPlay			121
#define MsgRecordPlayFeedback	122
#define MsgReposRecordData		123
#define MsgDownloadRecordFile	131
#define MsgDualtalk				141
#define MsgQueryParam			211
#define MsgSetParam				216
#define MsgStatusRequest		890		//
#define MsgAlarmReport			891
#define MsgAlarmSubscribe		896
#define MsgRemoteUpgrade		5139
#define MsgEmailTest            301
#define MsgDDNSTest             302
#define MsgRemoteReboot         303
#define MsgSyncTimeCtl          304
#define MsgColorCtrl            305
#define MsgRemoteCapture        306
#define MsgGainSnaptReport      307
#define MsgQueryLog             308
#define MsgHddFormat            309
#define MsgSearchRemoteDev		310   //NVR
#define MsgSetRemoteDevNet		311   //NVR
#define MsgAddAllDevice         312   // 
#define MsgFtpUpgrade           320   //Ftp 
#define MsgActivateCloud        321   // 
#define MsgDevicePlayback       322   // 

//
#define MsgQueryKeyConf			400
#define MsgSetKeyConf			401

#define MsgReqVideoActivation   402

//MsgHead.MsgSubType
#define MsgNoSubType	0

//MsgPTZControl sub type
#define MsgPTZUp			1
#define MsgPTZDown			2
#define MsgPTZLeft			3
#define MsgPTZRight			4
#define MsgPTZUpLeft		5
#define MsgPTZUpRight		6
#define MsgPTZDownLeft		7
#define MsgPTZDownRight		8
#define MsgPTZZoomIn		11 //
#define MsgPTZZoomOut		12 //
#define MsgPTZFocusNear		13 //
#define MsgPTZFocusFar		14 //
#define MsgPTZIRISOpen		15 //
#define MsgPTZIRISClose		16 //
#define MsgPTZAutoScan		21
#define MsgPTZCruise		51
#define MsgPTZSetPreset		91
#define MsgPTZClearPreset	92
#define MsgPTZCallPreset	93
#define Open_Screen_PTZ     97
#define Close_Screen_PTZ    98
//MsgQueryRecord sub type
#define MsgRecordDetail		 101
#define MsgRecordDayOutline	 102
#define MsgRecordDayHalfHour 103

//MsgRecordPlayFeedback sub type
#define MsgRecordDataInsufficient		121
#define MsgRecordDataEnough				122

//MsgAlarmReport sub type
#define MsgSnaptMotion                  188
#define MsgVLossAlarm                   189
#define MsgDevStatReport                190
#define MsgMotionAlarm			        191
#define MsgIOAlarm				        192
#define MsgAIAlarm				        193
#define MsgIntelligentAlarm		        194
#define MsgAvdAlarm				        195
#define MsgRecordAlarm          		201
#define MsgHddAlarm             		202
#define MsgFtpUpgradeAlarm              203   // Ftp 

//MsgAlarmSubscribe sub type
#define MsgAlarmDesired			        196
#define MsgAlarmRefused			        198
#define MsgAIData                       199
#define MsgTelModuleAlarm               210
#define MsgRemoteCHStatusReport         211
#define MsgRecordStatusReport			212
#define MsgHddStatusReport				213
#define MsgGpsInfoReport                214


//.SubType
#define MsgParamOsd				        501
#define MsgParamSystemBase		        502
#define MsgParamHdd				        503
#define MsgParamSerial			        504
#define MsgParamUser 			        505
#define MsgParamDst 			        506
#define MsgParamMainStream 	            507
#define MsgParamColor 			        508
#define MsgParamVideoCover 		        509
#define MsgParamPtz 			        510
#define MsgParamNetworkBase 	        511
#define MsgParamDDNS 			        512
#define MsgParamPPPOE 			        513
#define MsgParamEmail 			        514
#define MsgParamNtp 			        515
#define MsgParamMotion 			        516
#define MsgParamAbnormal 		        517
#define MsgParamIOAlarm 		        518
#define MsgParamRecord 			        519
#define MsgParamSchedule 		        520
#define MsgParamDefault 		        521
#define MsgParamIntelligent             522
#define MsgParamIntavd                  523 

#define MsgParamGeneral                 524
#define MsgParamMaintain                525
#define MsgParamSubStream               526
#define MsgParamMobile                  527
#define MsgParamEmailSchedule           528
#define MsgParamPlatform                529
#define MsgParamIpc                     530
#define MsgParamIpcIOAlarm              531
#define MsgParamIpcWifi                 532
#define MsgParam3G                      533
#define MsgParamRouterLan               534
#define MsgParamRouterWan               535
#define MsgParamTuobao                  537
#define MsgParamVehicleMng              538
#define MsgParamOutput			        539
#define MsgParamTelAlarmArea            540
#define MsgParamTelAlarmWireSensor      541
#define MsgParamTelAlarmWirelessSensor  542
#define MsgParamPhoneNumberSetting      543
#define MsgParamCameraSet               544
#define MsgParamMobileStream            545
#define MsgParamHuaweiPlatform          546
#define MsgParamIPFilter                547
#define MsgParamRtsp                    548
#define MsgParamFtp                     549
#define MsgParamElecLock             	550
#define MsgParamCloudStorage            551
#define MsgParamCloudEmail              552

//#define MsgParamP2pMobile               553
#define MsgParamP2pQrCodeID             553
#define MsgParamDevAllStatusSet         554   // URMET 
#define MsgParamLorexP2pParam           555   // Lorex
#define MsgParamFtpAutoUpgrade          556   // 
#define MsgParamManualRecord            557   // 


// sub type
#define MsgRemoteUpgradeData	761
#define MsgRemoteUpgradeEnd		762
#define MsgRemoteUpgradePartEnd 763
#define MsgRemoteUpgradeWriteFlashInProgress 764
#define MsgRemoteUpgradeWriteFlashEnd 765


#define MsgRSPlatform           800

//sync time sub type
#define MsgSyncSetTime          331
#define MsgSyncGetTime          332

//MsgStatusAlarmRequest	Sub Type 
#define MsgDevAllStatusReq			610
#define MsgRemoteCHStatusReq		611  //,NVRIPC


typedef struct
{
	BYTE Version : 2; //
	BYTE ExtSize : 6; // = ExtSize * 4
	BYTE Reserved;
	WORD AckFlag; //
	WORD MsgType; //
	WORD MsgSubType; //
	UINT MsgSize;  //
	UINT Seq;   //
}MsgHead; //sizeof(MsgHead) = 16


//////////////////////////////////////////////////////////////////////////
//
// MsgType=MsgSearchDev
// MsgSize=sizeof(SearchDevRsp)

#define SearchDevPort	9333

//SearchDevRsp.NetworkMode
#define NetworkModeDHCP		0
#define NetworkModePPPOE	1
#define NetworkModeManual	2

typedef struct
{
	char IP[MaxIpLen]; //IP
	char HostName[MaxNameLen]; //
	char NetMask[MaxIpLen]; //
	char GateWay[MaxIpLen]; //
	char DNS1[MaxIpLen]; //dns1
	char DNS2[MaxIpLen]; //dns2

	UINT Port; //
	UINT IEPort; //IE

	UINT NetworkMode; //

	char DevType[MaxNameLen - 8]; //
	unsigned long long DevTypeULL;
	char DevVer[MaxNameLen]; //
	UINT DeviceID;  //
   	char DeviceName[MaxNameLen]; //        
	char MacAddr[MaxNameLen]; //MAC

	UINT ChannelNum; //
	
	unsigned char Version;//
	char reserved[3];
}SearchDevRsp;


//////////////////////////////////////////////////////////////////////////
//
// MsgType=MsgLogin, MsgSize=sizeof(LoginReq)
// AckFlag=AckSuccess,MsgSize=sizeof(LoginRsp) AckFlag=AckLoginUserLogined,AckLoginNoUserName,AckLoginPasswordError,AckFail

//VideoFormat
#define RS_PAL  0
#define RS_NTSC 1

typedef struct
{
	char UserName[MaxNameLen];
	char Password[MaxNameLen];
	UINT UserKey; //0UserKey
	UINT AlarmReportDesired : 1;
	UINT Reserved : 31;
}LoginReq;


//
typedef struct
{
	UINT UserSetRight;// ,,,,,,
//	UINT ManualRecordChannel;  // 
	UINT PreviewChannel;     //
	UINT PlayBackChannel;       //
	UINT BackupChannel;        //
	UINT PtzControlChannel;     //
	BYTE UserPreview;        //
//	BYTE UserManualRecord;   //
	BYTE UserPlayBack;        //
	BYTE UserBackup;          //
	BYTE UserPtzControl;       //
	BYTE Reserved[16];
}UserRightWrap;
C_ASSERT(sizeof(UserRightWrap) == 40);

typedef enum
{
	PLATF_SWITCH_RS_PLATFORM = 0,
	PLATF_SWITCH_HUAWEI_PLATFORM,
}PlatformSwitch_E;

typedef struct
{
	unsigned long long DevType; //
	char DevVer[MaxNameLen]; //
	char DeviceName[MaxNameLen]; //        
	char MacAddr[MaxNameLen]; //MAC
	UINT DeviceID;  //
 
 	UserRightWrap UserRight; //

	UINT UserKey; //
 	WORD TotalFPS_D1; //D1TotalFPS=100, 4CIF 25f/s2HD1 25f/s1D1 25f/s
 	BYTE ChannelNum; //
 	BYTE VideoFormat; //

 	BYTE AlarmInNum;  //
 	BYTE AlarmOutNum; //()
	BYTE AIAnalyzeCap; //
	BYTE AIAnalyzeChannels; //

	BYTE SerialNum;	//0()
	BYTE ISAdmin;   //
	BYTE AudioNum;         //
	BYTE ResolutioFrameFix;//:0-,1-2-

	BYTE PreviewOff;       //D90160-1-
	BYTE VideoCoverNum;    //D9016
	BYTE SupportSpot;      //spot
	BYTE SubStreamMin;     //

	BYTE SubStreamMax;     //:0     1     2     3     4    5     6     7    8       9       10      11     12
	BYTE PlatformSupport; //0() 1()
	
    WORD ChnInfoSize;		//DevChnInfo
							//MsgRemoteCHStatusReq

	BYTE SubStreamRestrict; // 0() 1 ()
	BYTE DualtalkShowTag;  //0(:) 1()
	BYTE RouterShowTag;    //
	BYTE LanShowTag;		//Lan

	UINT PlatFormShowTag;   //Value Show Plat*(UINT*)"PLEn" Hide Plat0x00000000 
	char ProductName[24];
	UINT PlatFormSwitch;   //PlatformSupport = 1PlatFormShowTag = "PLEn" ,  PlatformSwitch_E , 0RS_PLATFORM
	
	BYTE VideoActivateSwitch; //  
	BYTE MotionMode;          //01  44*36 
	BYTE ResolutionMode;      //0 D1,1D1960P
	BYTE HidePhonePage;        //01
	
	BYTE UpgradeType;         //0  1 
	BYTE ElecLockFlag;         //0  1 
 	WORD TotalFPS_960;			//960TotalFPS=100, 4CIF 25f/s2HD1 25f/s1D1 25f/s

	// one byte
	BYTE WifiStatus:1;           //0 wifi, 1 
	BYTE RtspPageEnable:1;           //RTSP 0, 1 
	BYTE FtpPageFlag:1;          //Ftp1 0
	BYTE c32PasswordFlag:1;     // 1320
	BYTE UrmetDevStatusFlag:1;   // ;UEMET 	
	BYTE FileSystemFlag:1;       // ;0  1 
	BYTE KguardP2pUidFlag:1;      // 0 Uid, 1Uid
	BYTE EmailFlagSwitch:1;     // EmailScheduleFlag 10:.
	
	BYTE EmailScheduleFlag:1;     // 0 1
	BYTE PtzHiddenFlag:1;	     // PTZ 0 (), 1 ()
	BYTE Reserved1:6;	
	BYTE Reserved2[2]; 
}LoginRsp;
C_ASSERT(sizeof(LoginRsp) == 232);




//////////////////////////////////////////////////////////////////////////
//
// MsgType=MsgHeartbeat
// 


//////////////////////////////////////////////////////////////////////////
//

//PackHead.Version
#define PackHeadV1		0
#define PackHeadV2      1

#define MaxPackSize		    1400
#define MaxPackSize_2       (8*1024)

//PackHead.Type
#define FramePack				0 //
#define ReposPack				1 //
#define NoDataPack				2 //
#define FileEndPack				3 //

typedef struct
{
	BYTE Version : 2; //
	BYTE ExtSize : 6; // = ExtSize * 4
	BYTE Type;
	WORD PackNum;
	UINT FrameNo;
	WORD PackNo; //0,1,2,3...
	WORD PackSize;
}PackHead; //, sizeof(PackHead) = 12


//////////////////////////////////////////////////////////////////////////
//

//Type
#define PreviewMain	0
#define PreviewSub	1

//Content
#define PreviewComposite	0
#define PreviewVideoOnly	1

typedef struct
{
	UINT Channel; // 0,1,2,3...
	BYTE Type; //
	BYTE Content; //
	WORD Reserved;
}PreviewParam;


//////////////////////////////////////////////////////////////////////////
//TCP
// MsgType=MsgTcpPreview, MsgSize=sizeof(TcpPreviewReq)
// AckFlag=AckSuccess AckFlag=AckFail

typedef struct
{
	PreviewParam Param;
	UINT UserKey;
}TcpPreviewReq;


//////////////////////////////////////////////////////////////////////////
//UDP
// MsgType=MsgGetUdpAddr
// MsgSize=size(GetUdpAddrRsp)

typedef struct
{
	struct sockaddr_in ClientAddr;
	UINT socketfd;
	BYTE Addr[44];
}GetUdpAddrRsp;


//////////////////////////////////////////////////////////////////////////
//UDP
// MsgType=MsgStartUdpPreview, MsgSize=sizeof(UdpStartPreviewReq)
// AckFlag=AckSuccess, MsgSize=sizeof(UdpStartPreviewRsp) AckFlag=AckFail

typedef struct
{
	GetUdpAddrRsp Rsp;
	PreviewParam Param;
}UdpStartPreviewReq;

typedef struct
{
	UINT UdpPreviewKey;
}UdpStartPreviewRsp;


//////////////////////////////////////////////////////////////////////////
//UDP
// MsgType=MsgStopUdpPreview, MsgSize=sizeof(UdpStopPreviewReq)
// 

typedef struct
{
	UINT UdpPreviewKey;
}UdpStopPreviewReq;


//////////////////////////////////////////////////////////////////////////
//I
// MsgType=MsgForceIFrame, MsgSize=sizeof(PreviewParam)
//
typedef struct 
{       
	UINT Channel; // 0,1,2,3...
	BYTE Type;    //,
	BYTE Reserved[7];
} IFrameReq;

//////////////////////////////////////////////////////////////////////////
//
// MsgType=MsgPTZControl, MsgSubtype=MsgPTZUp..., MsgSize=sizeof(PTZControlReq)
// AckFlag=AckSuccess, AckFlag=AckFail

#define PTZMinSpeed			0
#define PTZMaxSpeed			100
#define PTZMinCruiseNo		0
#define PTZMaxCruiseNo		63
#define PTZMinPresetIndex	1
#define PTZMaxPresetIndex	255

//Flag
#define PTZControlStart	0
#define PTZControlStop	1

typedef struct
{
	UINT Flag;
	UINT Speed;
}PTZControlNormal;

typedef struct
{
	UINT Flag;
	UINT No;
}PTZControlCruise;

typedef struct
{
	UINT Index;
}PTZControlPreset;

typedef struct
{
	UINT Channel;
	union
	{
		PTZControlNormal Normal; //MsgPTZUp...MsgPTZAutoScan
		PTZControlCruise Cruise; //MsgPTZCruise
		PTZControlPreset Preset; //MsgPTZSetPreset...MsgPTZCallPreset
	};
}PTZControlReq;

//////////////////////////////////////////////////////////////////////////
//
typedef struct
{
	UINT Channel;
	BYTE Chroma;       /* , <0 - 63> */
	BYTE Brightness;   /* , <0 - 63> */
	BYTE Contrast;     /* , <0 - 63> */
	BYTE Saturation;   /* , <0 - 63> */
	BYTE reservel[20];
}ColorCtrlReq;

//////////////////////////////////////////////////////////////////////////
//

//typedef struct
//{
//	WORD Year; //2011
//	BYTE Month; //1 ~ 12
//	BYTE Day; //1 ~ 31
//	BYTE Hour; //0 ~ 23
//	BYTE Minute; //0 ~ 59
//	BYTE Second; //0 ~ 59
//	BYTE Reserved;
//}DateTime;


//////////////////////////////////////////////////////////////////////////
//

//RecordType
#define NormalRecord	0x1
#define AlarmRecord		0x2
#define AllRecord		0xFFFFFFFF //Only for req, equal to NormalRecord|AlarmRecord...

typedef struct
{
	UINT Channel;
	DateTime Begin;
	DateTime End;
	UINT Type; //RecordType
}QueryRecordReq;

//,,,AckFlag=AckSuccess, MsgSize=0

// MsgType=MsgQueryRecord, MsgSubtype=MsgRecordDetail, MsgSize=sizeof(QueryRecordReq)
// AckFlag=AckSuccess, MsgSize=n*sizeof(RecordDetailRsp) AckFlag=AckFail

typedef struct
{
	UINT StructSize; //sizeof(RecordDetailRsp)
	DateTime Begin;
	DateTime End;
	UINT Type; //RecordType
	UINT Size; //
}RecordDetailRsp;

// MsgType=MsgQueryRecord, MsgSubtype=MsgRecordDayOutline, MsgSize=sizeof(QueryRecordReq)
// AckFlag=AckSuccess, MsgSize=n*sizeof(RecordDayOutlineRsp) AckFlag=AckFail

typedef struct
{
	UINT StructSize; //sizeof(RecordDayOutlineRsp)
	WORD Year; //2011
	BYTE Month; //1 ~ 12
	BYTE Day; //1 ~ 31
	BYTE Hour; //0 ~ 23
	BYTE Minute; //0 ~ 59
	BYTE Second; //0 ~ 59
	BYTE Type;
}RecordDayOutlineRsp;

typedef struct
{
	UINT StructSize; //sizeof(RecordDayHalfHourRsp)
	WORD Year;       //2011
	BYTE Month;      //1 ~ 12
	BYTE Day;        //1 ~ 31
	BYTE HarfHour;   //0 ~ 47
	BYTE Minute;     //0 ~ 59
	BYTE Second;     //0 ~ 59
	BYTE Type;
}RecordDayHalfHourRsp;


//////////////////////////////////////////////////////////////////////////
//
// MsgType=MsgDownloadRecordFile, MsgSize=sizeof(DownloadRecordFileReq)+n*sizeof(RecordDetailRsp)
// AckFlag=AckSuccess AckFlag=AckFail
//: file1,FileEndPack; file2,FileEndPack; ... filen,FileEndPack
typedef struct
{
	UINT StructSize; //sizeof(DownloadRecordFileReq)
	UINT UserKey;
	UINT Channel;
}DownloadRecordFileReq;


//////////////////////////////////////////////////////////////////////////
//

//
// MsgType=MsgRecordPlay, MsgSize=sizeof(RecordPlayReq)
// AckFlag=AckSuccess, AckFlag=AckFail, ,MsgRecordDataInsufficient

typedef struct
{
	UINT UserKey;
	UINT Channel;
	UINT Type; //RecordType
	DateTime Begin;
	DateTime End;
	BYTE ExactFrame[40]; //40 == sizeof(RSFrameHead)
}RecordPlayReq;

//
// MsgType=MsgRecordPlayFeedback, MsgSubType=MsgRecordDataInsufficient,MsgRecordDataEnough
//

//
// MsgType=MsgReposRecordData, MsgSize=sizeof(ReposRecordDataReq)
//,,(),ReposPack,
typedef struct
{
	DateTime Begin;
	DateTime End;
}ReposRecordDataReq;

// ,
// MsgType=MsgHeartbeat
//


//////////////////////////////////////////////////////////////////////////
//
// MsgType=MsgDualtalk, MsgSize=sizeof(DualtalkReq)
// AckFlag=AckSuccess AckFlag=AckFail
typedef struct
{
	UINT UserKey;
}DualtalkReq;


//////////////////////////////////////////////////////////////////////////
//

//LogType
#define LOG_TYPE_IO         0x0
#define LOG_TYPE_MD         0x1
#define LOG_TYPE_HDD        0x2
#define LOG_TYPE_VLOSS      0x3
#define LOG_TYPE_OPERA      0x4  // 
#define LOG_TYPE_ALARM      0x5  // , iomd     
#define LOG_TYPE_ALL        0x6  // , 

//Operation type
#define LOG_OPT_FIRST  0x0
#define LOG_OPT_NEXT   0x1
#define LOG_OPT_PREV   0x2 
#define LOG_OPT_LAST   0x3
typedef struct
{   
    DateTime Begin;
    DateTime End;
    UINT Type; 
    UINT Operation;  // :;;;
    UINT ExpectNum;  // 
}QueryLogReq;
    
typedef struct
{
    UINT StructSize; //sizeof(QueryLogRsp)
    UINT Channel;
    UINT Type;
    DateTime Point;
    BYTE Reserved[4];
}QueryLogRsp;

//////////////////////////////////////////////////////////////////////////
//

//Element : ElementSize + ExtensibleStruct + zero or more List
//List : ListSize + one or more Element
//ExtensibleStruct : StructSize + other member define
//: ElementExtensibleStructStructSize=4)ListElement
//		ListElement

// AckFlag=AckSuccess AckFlag=AckFail AckFlag=AckParamError, AckFlag=AckAuthorityFail



typedef struct
{
	UINT StructSize;
	UINT ChannelMask;
}ChannelParamReq;

//////////////////////////////////////////////////////////////////////////
//
//
//
//MsgType=MsgAlarmReport
//: MsgSubType = MsgMotionAlarm, MsgSize = sizeof(MotionAlarmRpt)
//IO: 	MsgSubType = MsgIOAlarm, MsgSize = sizeof(MotionIORpt)
//: MsgSubType = MsgAIAlarm, MsgSize = sizeof(MotionAIRpt)

//Status
#define AlarmOn  1 
#define AlarmOff 0 

typedef struct
{
	UINT Channel;
	UINT Status;
	DateTime AlarmTime;
    char RecState;               /*  */
    char CurRecType;             /*  */
    char MDSchedule;             /* */
	char MDRecEnable;           /* MD */
}MotionAlarmRpt;


typedef struct
{
	UINT Channel;
	UINT Status;
	DateTime AlarmTime;
    char RecState;               /*  */
    char CurRecType;             /*  */
    char AlarmSchedule;          /* IO*/
	char Reserved[1];
}IOAlarmRpt;


typedef struct
{
	UINT Channel;
	UINT Status;
	DateTime AlarmTime;
}IntelligentAlarmRpt;
typedef struct
{
	UINT Channel;
	UINT Status;
	DateTime AlarmTime;
}AvdAlarmRpt;

typedef struct
{
	UINT Channel;
	UINT Status;
	DateTime AlarmTime;
}VLossAlarmRpt;


//MsgDevStatReport
//MsgNormalStatusReq
typedef struct
{
	UINT Channel;
	UINT Status;
	DateTime AlarmTime;
}DevStatRpt;

typedef struct
{
	UINT Channel;
	UINT Status;
	UINT AreaNum;
	UINT SensorNum; 
	DateTime AlarmTime;
}TelModuleAlarmRpt;


//AIType
#define AIPerimeterAlarm 0 //
#define AITripwireAlarm 1 //

typedef struct
{
	UINT Channel;
	UINT Status;
	UINT AIType;
	DateTime AlarmTime;
}AIAlarmRpt;

/**/
#define MAX_AI_POS_COUNT  16
typedef struct  
{
	/**/
	BYTE Channel;
	/**/
	BYTE RuleNo;
	/**/
	BYTE RuleEnable;
	/*0-1-*/
	BYTE RuleType;
	/**/
	BYTE Direction;
	/**/
	BYTE reservel[3];
	/*xy*/
	unsigned short aiPoints[MAX_AI_POS_COUNT]; 
} AIDataRpt;

//MsgRecordStatusReport
//
#define RECORDSTATUSTYPE_NORMAL      0 
#define RECORDSTATUSTYPE_MANUAL      1 
#define RECORDSTATUSTYPE_MOTION      2 
#define RECORDSTATUSTYPE_IO          3 
#define RECORDSTATUSTYPE_MOTION_IO   4 
typedef struct
{
	UINT Type;  //
	DateTime AlarmTime;//
	
	UINT ChannelNum;     // 0xffffffff
	BYTE RecordStatus;   //  1  0 
	BYTE reservel[3];
    UINT AllChanRecState;    //	
}RecordStatusRpt;

typedef enum{
	RSNET_HDDSTATUS_NULL		= 0,
	RSNET_HDDSTATUS_OK			= 1,
	RSNET_HDDSTATUS_UNFORMAT	= 2,
	RSNET_HDDSTATUS_FERROR		= 3,
	RSNET_HDDSTATUS_FULL		= 4,
}RS_HDDSTATUS_E;

//MsgHddStatusReport
typedef struct
{
	
	BYTE HddStatus;	//RS_HDDSTATUS_E
	BYTE reserve[3];
	char HddSN[32];		//
	DateTime AlarmTime;	//
}HddStatusRpt;



//////////////////////////////////////////////////////////////////////////
//Gps
//PuInfoHead_tGpsINfo_tRSFrame.h 
//RSInfo
#define RSPuInfoTag *(unsigned int*)"RSI"
#define RSPuInfoTypeGPS 'G'
#define RSPuInfoTypeRepos 'R'
#define RSPuInfoTypeEnd 'E'

typedef struct __PuInfoHeadRpt_t
{
	unsigned int Tag : 24;
	unsigned int Type : 8;
	unsigned int InfoSize;
	unsigned long long Time; //(microseconds) Time/1000,000 = the number of seconds after January 1, 1970 UTC
}PuInfoHeadRpt_t;

//pu_info_head_t.type = RSPuInfoTypeGPS, pu_info_head_t.info_size = sizeof(gps_info_t)
typedef struct __GpsInfoRpt_t
{
	double Longitude;
	double Latitude;
	float Speed;
	float SensorSpeed;          //
	float Azimuth;    			// 0 0~360
	int Signal3G;               //3G
	unsigned int TotalMileage;  //  
	int Temperature;
	char LatitudeDir;       	//  'N' or 'S'
	char LongitudeDir;      	//  'E' or 'W'
	char Rectified;             // 
	char Status;   				//, 'A' , 'V'3
	unsigned char IgnitionFlag; //1:0
	char Reserved[3];
	double LongitudeOffset;
	double LatitudeOffset;
}GpsInfoRpt_t;

//GPS
//MsgGpsInfoReport
typedef struct 
{
	PuInfoHeadRpt_t stHeadRpt;
	GpsInfoRpt_t stBodyRpt;
	
	DateTime AlarmTime;
}GpsInfoPacketRpt;


//////////////////////////////////////////////////////////////////////////
//

// MsgType=MsgStatusRequest, MsgSubtype	= MsgDevAllStatusReq; MsgSize=0
// AckFlag=AckSuccess, MsgSize=sizeof(DevAllStatusRpt) AckFlag=AckFail

typedef enum {
	RECORD_STATUS_STOP_E = 0,
	RECORD_STATUS_NORMAL_E,
	RECORD_STATUS_MANUAL_E,   
	RECORD_STATUS_MOTION_E,
	RECORD_STATUS_IOALARM_E,
	RECORD_STATUS_MOTION_IO_E,
	RECORD_STATUS_MAX_E,
}RS_RECORD_STATUS_E;
typedef struct
{
	DateTime AlarmTime;
	
	UINT RecordEnable;	//, 0
	UINT RecordStatus;	//, 0

	UINT MotionEnable;	//, 0
	UINT MotionStatus;	//, 0
	
	UINT IOEnable;		//IO, 0
	UINT IOStatus;		//IO, 0
	
	UINT VLossStatus;	//, 0
	
	BYTE HddStatus[8];	//RS_HDDSTATUS_E
	BYTE HddSN[8][32];	//
	
	//32,  RS_RECORD_STATUS_E 
	BYTE RecordAllStatus[32]; 
	BYTE FtpNewVersFlag;    //:1 ;0 
	BYTE reserve[7];
}DevAllStatusRpt;

// MsgType=MsgStatusRequest, MsgSubtype	= MsgRemoteCHStatusReq; MsgSize=0
// AckFlag=AckSuccess, MsgSize=MAX_CHN_NUM * sizeof(RemoteChnStatusRpt) AckFlag=AckFail

//MsgRemoteCHStatusReport
//MsgRemoteCHStatusReq
typedef struct
{
	unsigned long long DevType;  /*  */
	char CurChnName[20];         /*  */

	char CurChnState;            /*  */
    char RecState;               /*  */
    char CurRecType;             /*  */
    char MDState;                /* Motion */

    char IOState;                /* IO */
    char VLossState;             /*  */
	char HddState;               /*  */
    char MDSchedule;             /* */

    char AlarmSchedule;          /* */
	
    unsigned char ChannelNum;		/**/
    char Resered[2];
}DevChnInfo;
C_ASSERT(sizeof(DevChnInfo) == 40);

//MsgRemoteCHStatusReport
//MsgRemoteCHStatusReq
typedef struct
{
	int StructSize;
	DevChnInfo ChnInfo;
}RemoteChnStatusRpt;

// MsgFtpUpgradeAlarm 
#define FTPUPGRADE_SUCCESS    0
#define FTPUPGRADE_UPGRADEING 1
#define FTPUPGRADE_PRE_REBOOT 2 
#define FTPUPGRADE_ERROR      3 

typedef struct {
	BYTE UpgradeProcess;  //  100 	
	BYTE UpgradeStatus;    // 
	BYTE reserved[2];	
}FtpUpgradeRpt;



//////////////////////////////////////////////////////////////////////////
//
// MsgType=MsgAlarmSubscribe, MsgSubType=MsgAlarmDesired, MsgAlarmRefused
// AckFlag=AckSuccess AckFlag=AckFail


//////////////////////////////////////////////////////////////////////////
//

// MsgType=MsgRemoteUpgrade, MsgSize=sizeof(RS_UpgradeReq)
// AckFlag=AckSuccess, MsgSize=RS_UpgradeRsp AckFlag=AckUpgrading,AckUpgradeVersionError,AckFail

#define MaxUpgradeFileHead 1024

typedef struct
{
	UINT UserKey;
	BYTE FileHead[MaxUpgradeFileHead];
}RS_UpgradeReq;

#define MaxUpgradeModules  32
/*
typedef struct
{
	upgradefileheaderentry_t NeedUpgrade[MaxUpgradeModules];
}RS_UpgradeRsp;
*/

// 
//MsgType=MsgRemoteUpgrade, MsgSubType=MsgRemoteUpgradeData,MsgSize=MaxUpgradeFileHead
#define MaxUpgradeDataSize	4096 //MaxUpgradeDataSize

//
// MsgType=MsgRemoteUpgrade, MsgSubType=MsgRemoteUpgradePartEnd
//

//
// MsgType=MsgRemoteUpgrade,MsgSubType=MsgRemoteUpgradeEnd, MsgSize=sizeof(RemoteUpgradeEndCheck)
// AckFlag=AckSuccess, AckFlag=AckFail ,MsgSize=sizeof(RemoteUpgradeEndWriteFlash)

typedef struct
{
	ULONG Crc;
}RemoteUpgradeEndCheck;

typedef struct
{
    BYTE WriteFlashFlag; //flash  1:   0: 
    BYTE reserved[3];
}RemoteUpgradeEndWriteFlash;

// MsgType=MsgRemoteUpgrade,MsgSubType=MsgRemoteUpgradeWriteFlashInProgress, MsgSize=sizeof(WriteFlashProgress)  (0 < progress <= 100)
// MsgType=MsgRemoteUpgrade,MsgSubType=MsgRemoteUpgradeWriteFlashEnd 
typedef struct 
{
    unsigned char Progress;//
    char reserved[3];
}WriteFlashProgress;

//Email
typedef struct
{
	WORD Ssl;
	WORD Port;
	BYTE Server[MaxNameLen];
	BYTE Sender[MaxNameLen];  
	BYTE Receiver[MaxNameLen];
	BYTE Password[MaxNameLen];
	BYTE Reserved[16];
} EmailTestReq;

//DDNS
typedef struct
{
	BYTE Serveraddr;
	BYTE Hostname[MaxNameLen];
	BYTE Username[MaxNameLen];
	BYTE Password[MaxNameLen];
	BYTE Reserved[15];
} DDNSTestReq;

typedef struct
{
	int  RetVal;
	BYTE RetMsg[500];
} TestRsp;

//
#define HDDFORMAT_SUCCESS            0
#define HDDFORMAT_FORMATTING         1 
#define HDDFORMAT_FAIL               2 
#define CANNOT_HDDFORMAT_RECDOWNLOAD 3
typedef struct
{
	UINT HddBitmap;
	UINT ChannelBitmap;	//
		
	BYTE Reserved[12];
} HddFormatReq;

typedef struct 
{
	BYTE HddNO;     //  
	BYTE HddFormatProcess; //    10
	BYTE HddFormatStatue;  //  0- 1 2 

	 BYTE Reserved[13];
}HddFormatRsp;
//////////////////////////////////////////////////////////////////////////
//
#define MaxTimeZoneSize  10
typedef struct 
{   
    DateTime Time;
    char     TimeZone[MaxTimeZoneSize];
    char     Reserved[42];
}TimeSyncReq;

//////////////////////////////////////////////////////////////////////////
//
typedef struct 
{
	/*VGA0=CIF, 1=QCIF, 2=D1
	3=UXGA(1600x1200), 4=SVGA(800x600), 5=HD720p(1280x720),6=VGA,7=XVGA, 8=HD900p
	*/
	UINT    StructSize;
	WORD	wPicSize;				/* 0=CIF, 1=QCIF, 2=D1 3=UXGA(1600x1200), 4=SVGA(800x600), 5=HD720p(1280x720),6=VGA*/
	WORD	wPicQuality;			/*  0- 1- 2- */
} NET_DVR_JPEGPARA, *LPNET_DVR_JPEGPARA;

typedef struct 
{
	UINT StructSize; //sizeof(RemoteCaptureReq)
	UINT UserKey;  //User key
	UINT Channel;  //Channel No.
	NET_DVR_JPEGPARA jpgPara;//Picture quality parameters
} RemoteCaptureReq;

typedef struct
{
	UINT picSize;
	BYTE Reserved[16];
} RemoteCaptureRsp;

///////////////////////////////////////////////////////////////////////////
//Snapt Report
//Request
typedef struct 
{
	UINT UserKey;
	WORD reserved1;
	WORD reserved2;
} SnaptReportReq;

typedef struct 
{
	DateTime snaptTime; 
	UINT     picSize;
}SnaptCommInfo;
//Motion Report
typedef struct
{
	SnaptCommInfo cmInfo;
	WORD          ch; //0,1,2
	WORD          reserved;
} SnaptMotionReport;

//////////////////////////////////////////////////////////////////////////
//
//NVR

// MsgType=MsgDevNetCfg, MsgSubType=MsgNoSubType, MsgSize=sizeof(NetMDevInfoReq)
// AckFlag=AckSuccess, MsgSize=sizeof(NetMDevInfoRsp) AckFlag=AckFail, MsgSize=sizeof(NetMDevInfoRsp);
#define MAX_UNAME_LEN 10
#define MAX_PWD_LEN   14

struct NetMDevInfoReq //
{
   BYTE msgid[16];  //message id
   BYTE user[MAX_UNAME_LEN]; //user name (for device's authority)
   BYTE pwd[MAX_PWD_LEN];    //password  (for device's authority)
   BYTE cip[4];     //current device ip
   
   BYTE ip[4];     //the device ip what the client wants to change into
   BYTE mask[4];   //the device net mask
   BYTE gw[4];      //the device default gateway
   BYTE dns1[4];    //DNS1
   BYTE dns2[4];    //DNS2
   WORD mport;      //media port   
   WORD wport;      //web port
   BYTE reserved[4];
};

// New NetMDevInfoReq,  
struct NewNetMDevInfoReq //
{
	NetMDevInfoReq DevInfoReq;//  
	U64  devTypeULL;            
	BYTE macAddr[36];   //MAC      
};

struct NetMDevInfoRsp //
{
	BYTE msgid[16];  //msgid,
};


//////////////////////////////////////////////////////////////////////////
//
//NVR

// MsgType=MsgSearchRemoteDev, MsgSubType=MsgNoSubType, MsgSize=sizeof(SearchRemDevReq)
// AckFlag=AckSuccess, MsgSize=n*sizeof(SearchRemDevRe) AckFlag=AckFail, MsgSize=0;

typedef struct
{
	UINT UserKey;
}SearchRemDevReq;

typedef struct
{
	UINT StructSize;		//sizeof(SearchRemDevRe)
	BYTE IPAddr[4];			/*IP*/
	BYTE Netmask[4];
	BYTE GateWay[4];
	BYTE Dns1[4];
	BYTE Dns2[4];

	UINT Port;				/**/    
	UINT WebPort;
	
	BYTE DevType[8];
	char UserName[36];		/**/    
	char UserPW[36];		/**/    
   	char DeviceName[36];	/**/
	
	BYTE Protocol;			/*0:raysharp,1:hikvison,2:sony*/	
	BYTE ChnMax;			/*ip*/
	BYTE Version ;
	BYTE Reserved;
	
	BYTE MacAddr[36];
	unsigned long long DevTypeULL;
}SearchRemDevRep;


//////////////////////////////////////////////////////////////////////////
//
//NVR

// MsgType=MsgSetRemoteDevNet, MsgSubType=MsgNoSubType, MsgSize=n*sizeof(SetRemDevNetReq)
// AckFlag=AckSuccess, MsgSize=n*sizeof(SetRemDevNetRep) AckFlag=AckFail, MsgSize=0;
typedef struct
{
	UINT StructSize;		//sizeof(SetRemDevNetRe)

	UINT UserKey;
	
	BYTE TargetIPAddr[4];			/*IP*/
	UINT TargetPort;				/**/    
	
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
    /*DNS1*/
    BYTE Dns1[4];
    /*DNS2*/
    BYTE Dns2[4];
	
	char UserName[36];		/**/    
	char UserPW[36];		/**/    
	
	BYTE MacAddr[36];
	unsigned long long DevTypeULL;
	BYTE Version ;
	BYTE reserved[3];
}SetRemDevNetReq;

typedef struct
{
	UINT StructSize;		//sizeof(SetRemDevNetRep)
	UINT ErrCode;			//0sdk
}SetRemDevNetRep;


//////////////////////////////////////////////////////////////////////////
//
// MsgType=MsgReqVideoActivation;MsgSize=sizeof(VideoActivationReq);
// AckFlag=AckSuccess,MsgSize= 0   AckFlag=AckFail;
typedef struct
{
	UINT StructSize;
	UINT ChannelBitmap; 
}VideoActivationReq;

// ErrCode 
#define E_SUCCESS    0        /** < succeed > **/
#define E_FAILED     -1       /** < failed  > **/
#define E_CONTENT    -101     /** < content failed > **/
#define E_CONNECT    -102     /** < connect failed > **/
#define E_SEND       -103     /** < send faile > **/
#define E_RECV       -104     /** < recv faile > **/
typedef struct 
{
	char InfoFlag;  // :0  ,1 EmaliTest
	char ErrCode;   //  
	char reserve[2];
	char RetMsgInfo[500];
}ActivateCloudRsp;

#pragma pack()

#endif
