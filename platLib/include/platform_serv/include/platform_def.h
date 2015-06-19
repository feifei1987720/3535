
#ifndef __PLATFORM_DEF_H__
#define __PLATFORM_DEF_H__

#include "type.h"

typedef enum
{
	RS_PLATFORM = 0,
	RS_TVWALL,
	RS_LT_PLATFORM,
	RS_HUAWEI_PLATFORM,
	ALL_PLATFORM = 99,
}PLATFORM_TYPE_E;

typedef enum
{
	PLATFORM_UDP_PROTOCOL = 0,
	PLATFORM_TCP_PROTOCOL = 1,
}PLATFORM_TRANSMIT_PROTOCOL_E;

typedef enum
{
	PLATFORM_MODULE_STATUS_CONNECTTING,
	PLATFORM_MODULE_STATUS_CONNECT_FAIL,
	PLATFORM_MODULE_STATUS_REGISTER_SUCCESS,
	PLATFORM_MODULE_STATUS_REGISTER_FAIL,
	PLATFORM_MODULE_STATUS_PLAYBACK_RUN,
	PLATFORM_MODULE_STATUS_PLAYBACK_OVER,
}PLATFORM_MODULE_STATUS_E;



/*****************RS PLATFORM PARAM****************/
typedef struct
{
	S8 s8PuID[32];
	S8 s8PlatformIP[32];
	U32 u32PlatformPort;
	PLATFORM_TRANSMIT_PROTOCOL_E TramsmitProtocol;
	BOOL bCompressFlag; //
}RS_PLATFORM_PARAM_S;
/*****************RS PLATFORM PARAM****************/


/***************HUAWEI PLATFORM PARAM**************/
#define HUAWEI_MAX_LENS_NUMBER 16

	typedef enum{
		HUAWEI_CENTER_STATUS = 0,
		HUAWEI_LENS_STATUS,
		HUAWEI_PLAYBACK_STATUS,
	}HUAWEI_PLATFORM_STATUS_TYPE_S;

typedef struct
{
	HUAWEI_PLATFORM_STATUS_TYPE_S StatusType;
	
	U32 u32ChannelNo;

	PLATFORM_MODULE_STATUS_E Status;
}HUAWEI_PLATFORM_STATUS_S;

typedef void HUAWEI_H;

typedef int (* HuaweiPlatfStatusCallBack)(HUAWEI_PLATFORM_STATUS_S *pstHuaweiStatus, HUAWEI_H *pHuaweiHandle);

typedef enum{
	HUAWEI_LENS_TYPE, //
	MAX_HUAWEI_TYPE,
}HUAWEI_SUBDEV_TYPE_E;

typedef struct{
}HUAWEI_SUBDEV_LENS_PARAM_S;

typedef struct _tag_HUAWEI_PLATFORM_SUBDEV_PARAM_S
{
	U32 u32Enable;
	S8 s8SubDevID[32];
	S8 s8SubDevName[48];
}HUAWEI_PLATFORM_LENS_PARAM_S;

typedef struct
{
	S8 s8MainDevID[32];
	S8 s8CenterIP[32];
	U32 u32CenterPort;
	U8  u8ProtocolType; /* 0:TCP,1:UDP. */
	U8  u8MaxChannelNum; 
	U8  u8RegistDelayTime;
	U8  resever;
	
	HuaweiPlatfStatusCallBack StatusCBFunc;
	HUAWEI_H *pHuaweiHandle;
	
	U32  u32LensBitmap;
	HUAWEI_PLATFORM_LENS_PARAM_S LensParam[HUAWEI_MAX_LENS_NUMBER];
	
	U32  u32AlarmInNum;
	U32  u32AlarmOutNum;
}HUAWEI_PLATFORM_PARAM_S;
/***************HUAWEI PLATFORM PARAM**************/


typedef struct
{
	S8 s8PuID[32];
	S8 s8PlatformIP[32];
	U32 u32PlatformPort;
	PLATFORM_TRANSMIT_PROTOCOL_E TramsmitProtocol;
}Lt_PLATFORM_PARAM_S;

typedef struct
{
	union{
		RS_PLATFORM_PARAM_S RsPlatformParam;
		RS_PLATFORM_PARAM_S RsTvWallParam;
		HUAWEI_PLATFORM_PARAM_S HuaWeiParam;
		Lt_PLATFORM_PARAM_S LtPlatformParam;
	}Param;
}PLATFORM_PARAM_S;

#endif
