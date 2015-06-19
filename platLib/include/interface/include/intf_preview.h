/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Zhong caiwang <caiwang213@gmail.com>
 * Apr 2011-04-16
 *
 * Preview relevant interface defines.
 */

#ifndef __INTF_PREVIEW_H__
#define __INTF_PREVIEW_H__
#include "exception.h"

#define  MAX_FRAME_SIZE_64      (1024*64)
#define  MAX_FRAME_SIZE_128     (1024*128)
#define  MAX_FRAME_SIZE_256     (1024*256)
#define  MAX_FRAME_SIZE_512     (1024*512)
#define  MAX_FRAME_SIZE_640     (1024*640)
#define  MAX_FRAME_SIZE_896     (1024*896)

typedef enum 
{
	STREAM_TYPE_SUB  = 0,
	STREAM_TYPE_MAIN,
	STREAM_TYPE_MOBILE,
	STREAM_TYPE_DUALTALK
}STREAMTYPE_E;

#define MODE_PURE_FRAME  0x1
#define MODE_STORE_FRAME 0x2

typedef struct  tag_FRAMETYPE_S
{  
	FRAMETYPE_E enFrameType;
	U32         u32FrameLen;
	BOOL        bIsHasFrame;  /* if 1,has frame,else no */
	S8          *ps8Buffer;
}FRAMEINFO_S;

typedef enum
{
	MEDIA_TYPE_AUDIO = 0,
	MEDIA_TYPE_VIDEO,
	MEDIA_TYPE_COMPOUND
}MEDIATYPE_E;
typedef enum
{
	AENC_G711A = 0,
	AENC_TAIL
}AENCTYPE_E;
typedef enum
{
	VENC_H264 = 0,
	VENC_TAIL
}VENCTYPE_E;
typedef struct tag_AUDIOATTR_S
{
	AENCTYPE_E encType;
	BYTE       bBitsPerSample;
	U32        u32SampleRate;
	DOUBLE     fPackTime;
}AUDIOATTR_S;
typedef struct tag_VIDEOATTR_S
{
	VENCTYPE_E encType;
	BYTE     bBitsPerSample;
	S32      s32Width;
	S32      s32Height;
	U32      u32SampleRate;
	DOUBLE   fFrameRate;
}VIDEOATTR_S;
typedef struct tag_STREAMINFO_S
{
	VIDEOATTR_S videoAttr;
	AUDIOATTR_S audioAttr;
	MEDIATYPE_E mediaType;
}STREAMINFO_S;
class IPreview
{
	public:
		IPreview() { }
		virtual ~IPreview() { }

	public: 
		
		/** 
		 * @remarks      IDReleaseReader
		 * @u32UserKey   [in]  
		 * @s32ChnNO     [in]  ,0 
		 * @enStreamType [in]  
		 * @s32ReaderID  [in|out] ID
		 * @return       TRUE,FALSE.
		 */
		virtual BOOL  RequestReader( U32 u32UserKey, S32 s32ChnNO, STREAMTYPE_E enStreamType,S32* s32ReaderID )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks      
		 * @s32ReaderID [in] ID
		 * @return       TRUE,FALSE
		 */
		virtual BOOL ReleaseReader(S32 s32ReaderID)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		/** 
		 * @remarks      UserchannelI(:user+channelID)
		 * @s32ReaderID  [in]      ID
		 * @pstFrameInfo [in|out]  ,
		 * @return       0,.
		 */
		virtual S32 GetLatestIFrame(S32 s32ReaderID,FRAMEINFO_S *pstFrameInfo)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		/** 
		 * @remarks      Usercahnnel
		 * @s32ReaderID  [in]      ID
		 * @pstFrameInfo [in|out]  ,
		 * @return       0,.
		 */
		virtual S32 GetRealFrame(S32 s32ReaderID, FRAMEINFO_S *pstFrameInfo )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		/**
		 * @remarks  pts 
		 * @s32ReaderID [in] ID
		 * @pts         [in|out] pts
		 * @return      0 -1
		 */
		virtual S32 GetFramePts(S32 s32ReaderID,U64* pts)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		/**
		 * @remarks  ptspts
		 * @s32ReaderID [in] ID
		 * @return    0-1 
		 */
		virtual S32 AdjustFramePts(S32 s32ReaderID)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/**
		 * @remarks  
		 * @return    0-1 
		 */
		virtual BOOL IsEnableTranslate( S32 s32ChnNO, STREAMTYPE_E enStreamType )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		
		/**
		 * @remarks  I
		 * @return    0-1 
		 */
		virtual S32 RequestIFrame(S32 s32ChnNO,STREAMTYPE_E enStreamType)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/**
		 * @remarks ()
		 *            MODE_PURE_FRAME  
		 *            MODE_STORE_FRAME 
		 * @return    0-1 
		 */
		virtual S32 SetIOMode(S32 s32ReaderID, S32 mode)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}	
		/**
		 * @remarks 
		 * @return  0-1 
		 */

		virtual S32 GetStreamInfo(S32 s32ChnNO, STREAMTYPE_E enStreamType, STREAMINFO_S *si)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}		
};

#endif
