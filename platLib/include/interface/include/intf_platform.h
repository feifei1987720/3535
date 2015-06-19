

#ifndef __INTF_PLATFORM_H__
#define __INTF_PLATFORM_H__

#include "RSPLParameter.h"

#define NO_PLATFORM -1

//32
#define MAX_SUPPORT_CHANNEL_NUMBER 32

//PUMessage PurposeType 
#define PURPOSE_BY_SCREEN 1 //
#define PURPOSE_BY_LED    2 //
#define PURPOSE_BY_VOICE  4 //

#define DATETIME_TO_TM( DateTime, tmTime ) \
	do \
	{ \
		tmTime.tm_year = DateTime.Year;\
		tmTime.tm_mon  = DateTime.Month;\
		tmTime.tm_mday = DateTime.Day;\
		tmTime.tm_hour = DateTime.Hour;\
		tmTime.tm_min  = DateTime.Minute;\
		tmTime.tm_sec  = DateTime.Second;\
}while(0)


class IPlatform
{
	public:
		IPlatform() {}
		virtual ~IPlatform() {}

	public:
		virtual U32 GetPlatformEnable()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		virtual S32 GetPlatformMduConType()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		virtual S32 GetPlatformService()
		{
			return NO_PLATFORM;
//			SHOW_NOT_IMPLE_FUNC;
//			throw Exception(__FUNCTION__);
		}
		virtual S8 *GetPlatformHost()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		virtual U32 GetPlatformPort()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		virtual S8 *GetPlatformPuId()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		virtual S8 *GetPlatformPuName()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		virtual S8 *GetPlatformPuPassword()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		virtual S32 GetPlatformResetFlag()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		virtual S32 GetPlatformModifyFlag()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		virtual S32 ClearPlatformModifyFlag()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		virtual S32 GetPlatformIp(S8 *ps8Ip)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		virtual S8 *GetIpByHost(const S8 *ps8Host, U32 u32Port, S8 *ps8Ip)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		virtual S32 GetPlatformPuIP(S8 *ps8Ip)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		virtual U32 GetPlatformPuPort()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 RsPlatParam_SystemBaseInfoAdapter(VOID *pvBaseInfo, VOID *pvSoleBaseInfo, S32 s32Chn, S32 s32AdapterOpt)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 RsPlatParam_OSDParameterAdapter(VOID *pvOsdSet, VOID *pvSoleOsdSet, S32 s32Chn, S32 s32AdapterOpt)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 RsPlatParam_HddParameterAdapter(VOID *pvHddInfo, VOID *pvSoleHddInfo, S32 s32Chn, S32 s32AdapterOpt)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 RsPlatParam_ColorParameterAdapter(VOID *pvColorInfo, VOID *pvSoleColorSet, S32 s32Chn, S32 s32AdapterOpt)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 RsPlatParam_VideoCoverParameterAdapter(VOID *pvVideoCover, VOID *pvSoleVideoCover, S32 s32Chn, S32 s32AdapterOpt)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 RsPlatParam_PTZParameterAdapter(VOID *pvPTZSet, VOID *pvSolePTZSet, S32 s32Chn, S32 s32AdapterOpt)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 RsPlatParam_NetworkParameterAdapter(VOID *pvNetworkSet, VOID *pvSoleNetworkSet, S32 s32Chn, S32 s32AdapterOpt)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 RsPlatParam_SubStreamParameterAdapter(VOID *pvSubStreamSet, VOID *pvSoleSubStreamSet, S32 s32Chn, S32 s32AdapterOpt)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 RsPlatParam_EmailParameterAdapter(VOID *pvEmailSet, VOID *pvSoleEmailSet, S32 s32Chn, S32 s32AdapterOpt)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 RsPlatParam_MobileParameterAdapter(VOID *pvMobileSet, VOID *pvSoleMobileSet, S32 s32Chn, S32 s32AdapterOpt)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 RsPlatParam_MotionParameterAdapter(VOID *pvMotionSet, VOID *pvSoleMotionSet, S32 s32Chn, S32 s32AdapterOpt)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 RsPlatParam_AbnormalParameterAdapter(VOID *pvAbnormalSet, VOID *pvSoleAbnormalSet, S32 s32Chn, S32 s32AdapterOpt)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 RsPlatParam_IOAlarmParameterAdapter(VOID *pvIOAlarmSet, VOID *pvSoleIOAlarmSet, S32 s32Chn, S32 s32AdapterOpt)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 RsPlatParam_MainStreamParameterAdapter(VOID *pvMainStreamSet, VOID *pvSoleMainStreamSet, S32 s32Chn, S32 s32AdapterOpt)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 RsPlatParam_RecordParameterAdapter(VOID *pvRecordSet, VOID *pvSoleRecordSet, S32 s32Chn, S32 s32AdapterOpt)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 RsPlatParam_ScheduleParameterAdapter(VOID *pvScheduleSet, VOID *pvSoleScheduleSet, S32 s32Chn, S32 s32AdapterOpt)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 RsPlatParam_MaintainParameterAdapter(VOID *pvMaintainSet, VOID *pvSoleMaintainSet, S32 s32Chn, S32 s32AdapterOpt)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 RsPlatParam_DefaultParameterAdapter(VOID *pvDefaultSet, VOID *pvSoleDefault, S32 s32Chn, S32 s32AdapterOpt)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 RsPlatParam_PlatformParameterAdapter(VOID *pvPlatformSet, VOID *pvSolePlatformSet, S32 s32Chn, S32 s32AdapterOpt)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 RsPlatParam_CarFuntionParameterAdapter(VOID *pvCarFunSet, VOID *pvSoleCarFunSet, S32 s32Chn, S32 s32AdapterOpt)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 PlatSaveFlash( SAVE_FLASH_OPT_E enOpt )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 UpdatePlatParam()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 PlatServRestart(U8 u8Source)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S8 CheckPlatformUpgradeTag()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S8 ClearPlatformUpgradeTag()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S8 PlatformMessage(S8 *ps8Message, U32 u32MessageBufferSize, S32 s32Purpose)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		
		virtual S32 RsPlatParam_AIParameterAdapter(VOID *pvAISet, VOID *pvSoleAISet, S32 s32Chn, S32 s32AdapterOpt)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		
		/** 
		 * @remarks    	 
		 * @return     	 true false.
		 */
		virtual S32 GetMdvrPlatfParamEnable()
		{
			return false;
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		
		virtual S32 RsPlatParam_ELockParameterAdapter(VOID *pvELockSet, VOID *pvSoleELockSet, S32 s32Chn, S32 s32AdapterOpt)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		
		virtual S32 RsPlatParam_ICCardParameterAdapter(VOID *pvICCardSet, VOID *pvSoleICCardSet, S32 s32Chn, S32 s32AdapterOpt)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		
		virtual S32 RsPlatParam_DDNSParameterAdapter(VOID *pvDDNSSet, VOID *pvSoleDDNSSet, S32 s32Chn, S32 s32AdapterOpt)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
};
#endif
