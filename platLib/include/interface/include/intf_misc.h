/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Zhong caiwang <caiwang213@gmail.com>
 * Apr 2011-06-04
 *
 * 
 */
#ifndef __INTF_MISC_H__
#define __INTF_MISC_H__

typedef struct tag_EMAILTEST_INFO_S {
	U16 u16Ssl;
	U16 u16Port;
	S8 s8Server[MaxNameLen];
	S8 s8Sender[MaxNameLen];  
	S8 s8Receiver[MaxNameLen];
	S8 s8Password[MaxNameLen];
	S8 Reserved[16];
}EMAILTEST_INFO_S;

typedef struct tag_DDNSTEST_INFO_S {
	S8 s8Serveraddr;
	S8 s8Hostname[MaxNameLen];
	S8 s8Username[MaxNameLen];
	S8 s8Password[MaxNameLen];
	S8 Reserved[15];
}DDNSTEST_INFO_S;

//#define _FEATRUES_SUPPORT_PLATFORM  0x1  //
#define _FEATRUES_SUPPORT_ENCODE_    0x2  //
#define _FEATRUES_SUPPORT_DUALTALK_  0x3  //

class IMisc
{
	public:
		IMisc(){}
		virtual ~IMisc(){}
	public:
		/** 
		 * @remarks    
		 * @return     0, 0.
		 */
		virtual S32 EmailTest( EMAILTEST_INFO_S *pstTestReq, S8 *pRspMsg )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    DDNS
		 * @return     0, 0.
		 */
		virtual S32 DDNSTest( DDNSTEST_INFO_S *pstTestReq, S8 *pRspMsg )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    
		 * @return     0, 0.
		 */
		virtual S32 SetSysTime( const DateTime *pDateTime, const S8 *pTzone )
		{
			return 0;
			
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    
		 * @return     0, 0.
		 */
		virtual S32 GetSysTime( DateTime *pDateTime, S8 *pTzone, S32 s32ZoneLen )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks     
		 * @return     0, 0.
		 */
		virtual S32 ColorCtrl( S32 s32ChnNO, U32 u32Chnroma, U32 u32Bightness, U32 u32Contrast, U32 u32Saturation )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks     
		 * @return     TRUE, FALSE.
		 */

		virtual BOOL IsEncodeEnabled()
		{
			return 0; /*default FALSE*/;
			
			SHOW_NOT_IMPLE_FUNC;	
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks   (u32FeatruesMacro)
		 * @return     10-1
		 */

		virtual S32 FeaturesTest(U32 u32FeaturesMacro)
		{
			S32 ret = -1;
		 	switch(u32FeaturesMacro)
			{
				//case	_FEATRUES_SUPPORT_PLATFORM: break;//
	
				case _FEATRUES_SUPPORT_ENCODE_: //
					 ret = IsEncodeEnabled() ? 1 : 0;
					 break;
				case _FEATRUES_SUPPORT_DUALTALK_:
					 ret = 1;                  //
					 break;
				default:
					 ret = -1;
			}

			return ret; 
			
			SHOW_NOT_IMPLE_FUNC;	
			throw Exception(__FUNCTION__);

		}

		/** 
		 * @remarks    
		 * @return     0, 0.
		 */

		virtual S32 HddFormat( U32 u32HddBitmap, U32 u32ChannelBitmap )
		{
			SHOW_NOT_IMPLE_FUNC;	
			throw Exception(__FUNCTION__);
		}

		virtual S32 GetHddFormatState()
		{
			SHOW_NOT_IMPLE_FUNC;	
			throw Exception(__FUNCTION__);
		}

		virtual VOID SetPlaybackState(BOOL flag)
		{
			SHOW_NOT_IMPLE_FUNC;	
			throw Exception(__FUNCTION__);
		}

};

#endif
