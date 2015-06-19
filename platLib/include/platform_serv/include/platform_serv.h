/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Mai guanwei <kawenmai@foxmail.com>
 *      2011-05-11
 *
 * 		Platform.
 */

#ifndef __PLATFORM_H__
#define __PLATFORM_H__


/*
#include "config.h"
#include "comm_def.h"
#include "comm_dbg.h"
#include "interface.h"
#include "rs_platform.h"
#include "parameterhi3511dvr.h"
#include "dvr_mavenc_ext.h"
*/

#include "interface.h"
#include "platform_def.h"

class RsPlatform;
class CTvWall;
class GpsBuffer;
class LTPlatform;

class PlatformServer 
{
	public:

		/***********************
		  *
		  *
		  **pIntf:
		  **bGpsFlag:GPS, trueGPSfalse:
		  *
		************************/
		PlatformServer(DevInterface *pIntf, bool bGpsFlag);


		/***********************
		  *
		  *
		************************/
		~PlatformServer();


		/***********************
		  *
		  *
		  **PlatformType:
		  * 	
		  **PlatformParam:
		  **
		  **
		  **  ==0 ==1
		  *
		************************/
		S32 StartPlatformService(PLATFORM_TYPE_E PlatformType, PLATFORM_PARAM_S *pPlatformParam);


		/***********************
		  *
		  *, , 1-2s
		  **PlatformType:
		  * 	
		  **ALL_PLATFORM
		  **
		  **  ==0 ==1
		  *
		************************/
		S32 StopPlatformService(PLATFORM_TYPE_E PlatformType);


		/***********************
		  *
		  *,
		  *
		************************/
		S32 StartTranslate();


		/***********************
		  *
		  *
		  *
		************************/
		S32 StopTranslate();

	private:
		BOOL m_bSearchRecordRunFlag;
		pthread_mutex_t m_SearchRecordFlag_mutex;
		S32 m_StreamState[MAX_SUPPORT_CHANNEL_NUMBER][STREAM_TYPE_MOBILE];

	private:
		U32 CurPlatform; //

		RsPlatform *m_pRsPlatform;
		CTvWall *m_pCTvWall;
        LTPlatform *m_pLTPlatform;
		
	private:
		BOOL m_bHuaWeiServInitFlag;
		BOOL m_bHuaWeiServRunFlag;

	public: //GPS
		BOOL m_bGpsFlag;
		pthread_t m_GpsThreadID;
		GpsBuffer *m_pGpsBuffer;

		S32 GpsBufferThread();

	public:
		DevInterface *m_pIntf;
		BOOL m_bPlatformServRunFlag;
		BOOL m_bTransferFlag;
		pthread_mutex_t m_Platform_mutex;

		//
	public:
		S32 CheckSearchRecordRunFlag();
		S32 CheckAndSetSearchRecordRunFlag();
		S32 ClearSearchRecordRunFlag();
};

#endif
