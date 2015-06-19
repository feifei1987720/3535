
#ifndef __INTF_TVWALL_H_
#define __INTF_TVWALL_H_

#define WINDOW_ENABLE  *(U32 *)"WnEn"
#define WINDOW_DISABLE *(U32 *)"WnUn"

#define WINDOW_AUDIO_ENABLE  *(U32 *)"WAEn"
#define WINDOW_AUDIO_DISABLE *(U32 *)"WAUn"

#define WINDOW_LOCK *(U32 *)"WnEL"
#define WINDOW_UNLOCK *(U32 *)"WnUL"

typedef enum{
	MAIN_STREAM_TYPE = 0,
	SUB_STREAM_TYPE,
	MAX_STREAM_TYPE,
}INTF_STREAM_TYPE_E;

typedef struct {
	INTF_STREAM_TYPE_E enStreamType;
	U32 u32FrameHight;
	U32 u32FrameWidth;
}INTF_STREAM_INFO_S;

typedef struct _tag_GUINFO_PARAM_S{
	U32 EnableTag;
	U32 AudioEnableTag;
	U32 WinLockTag;
	S8 s8Name[32];
	S8 s8DmID[32];
	S8 s8GuID[32];
	S8 s8MduID[32];
	S8 s8MduIP[32];
	U32 u32MduPort;
	S8 s8MduID_1[32];
	S8 s8MduIP_1[32];
	U32 u32MduPort_1;
	INTF_STREAM_INFO_S stStreamInfo;
}GUINFO_PARAM_S;

#define MAX_DISP_NUM 16

typedef struct _tag_TVWALL_PARAM_S{
	U32 u32Step;
	U32 u32WinMode;
	U32 u32WinCount;
	GUINFO_PARAM_S stGuInfo[MAX_DISP_NUM];
}TVWALL_PARAM_S;

typedef enum tag_DISPLAY_MODE_E
{
    DISPLAY_ONE = 1,
    DISPLAY_FOUR,
    DISPLAY_NINE,
    DISPLAY_SIXTEEN,
    DISPLAY_1PLUS5, //1+5
    DISPLAY_1PLUS7, //1+7
    DISPLAY_PIP,/*1+1*/
    DISPLAY_PIP2,/*1+2*/
    DISPLAY_ZOOMIN,/**/
    DISPLAY_INVALID
}DISPLAY_MODE_E;

typedef enum tag_OUTPUT_DEV_E{
    OUTPUT_DEV_VOUT  =0,	
    OUTPUT_DEV_VSPOT =1,
    OUTPUT_DEV_VGA   =2,
    OUTPUT_DEV_MAX   =3
}OUTPUT_DEV_E;

typedef enum tag_DEV_MEDIA_WND_E
{
    DEV_MEDIA_WND_VVGA = 0,  //VGA      
    DEV_MEDIA_WND_VOUT, 
    DEV_MEDIA_WND_VSPOT,
    DEV_MEDIA_WND_HDMI,
    DEV_MEDIA_WND_BUTT
}DEV_MEDIA_WND_E;

typedef struct
{
	S32 s32X;
	S32 s32Y;
	S32 s32W;
	S32 s32H;
}INTF_RECT_S;



class ITvWall
{
	public:
		ITvWall(){}
		virtual ~ITvWall(){}

	public:
		virtual S32 GetCurOutput(OUTPUT_DEV_E *pOutputDev, U32 *pu32VgaType)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 ChangeDeviceDisplayMode(OUTPUT_DEV_E OutputDev, DISPLAY_MODE_E DisplayMode, U32 u32ChnList[MAX_DISP_NUM], BOOL bStopDecoder)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual DEV_MEDIA_WND_E GetOutPutDevType()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual VOID *CreatDecoder()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 DestoryDecoder(VOID *pvBufferHandle)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 EnableDecoder(U32 u32WindowNo, INTF_RECT_S stRect, DEV_MEDIA_WND_E enWinDev, VOID *pvBufferHandle, INTF_STREAM_INFO_S *pstStreamInfo, BOOL bAudioEnable)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 DisableDecoder(U32 u32WindowNo, DEV_MEDIA_WND_E enWinDev)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		//virtual S32 TvWallParamAdapter(TVWALL_PARAM_S *pTvWallParam, BOOL bDirection)
		//{
		//	SHOW_NOT_IMPLE_FUNC;
		//	throw Exception(__FUNCTION__);
		//}

		virtual S32 LoadTvWallParam(TVWALL_PARAM_S *pTvWallParam)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 SaveTvWallParam(TVWALL_PARAM_S *pTvWallParam)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 GetWindowRect(U32 u32WindowNo, U32 u32WinCount, INTF_RECT_S *pstRect)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 DecVideoFrame(U8 *pu8FrameData, U32 u32FrameSize, U64 u64Pts, U32 u32WindowNo)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 DecAudioFrame(U8 *pu8FrameData, U32 u32FrameSize, U64 u64Pts, U32 u32WindowNo)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 SetAudioOutputStatus(OUTPUT_DEV_E enOutputDev, U32 u32Chn, BOOL bStatus)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
};
#endif
