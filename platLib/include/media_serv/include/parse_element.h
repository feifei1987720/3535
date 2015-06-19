/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Zhong caiwang <caiwang213@gmail.com>
 * Apr 2011-06-03
 *
 * Element prase define
 */

#ifndef __PARSE_ELEMENT_H__
#define __PARSE_ELEMENT_H__
#include "comm_def.h"

#define TOP_ELEMENT 1
#define LIST_ELEMENT 2

typedef enum tag_PARAM_PAGE_E
{
	PARAM_PAGE_REQ = 0,
	PARAM_PAGE_OSD,
	PARAM_PAGE_SHELTER,
	PARAM_PAGE_RECORD,
	PARAM_PAGE_SCHEDULE,
	PARAM_PAGE_MAINSTREAM,
	PARAM_PAGE_NETBASIC,
	PARAM_PAGE_SUBSTREAM,
	PARAM_PAGE_EMAIL,
	PARAM_PAGE_MOBILE,
	PARAM_PAGE_DDNS,
	PARAM_PAGE_PPPOE,
	PARAM_PAGE_NTP,
	PARAM_PAGE_MOTION,
	PARAM_PAGE_IOALARM,
	PARAM_PAGE_HDD,
	PARAM_PAGE_PTZ,
	PARAM_PAGE_SERIAL,
	PARAM_PAGE_GENERAL,
	PARAM_PAGE_USERINFO,
	PARAM_PAGE_BASEINFO,
	PARAM_PAGE_DST,
	PARAM_PAGE_MAINTAIN,
	PARAM_PAGE_DEFAULT,
	PARAM_PAGE_ABNORMAL,
	PARAM_PAGE_COLOR,
	PARAM_PAGE_INTELLIGENT,
	PARAM_PAGE_INTAVD,
	PARAM_PAGE_EMAILSCHEDULE,
	PARAM_PAGE_PLATFORM,
	PARAM_PAGE_IPC,
	PARAM_PAGE_3G,
	PARAM_PAGE_IPCIOALARM,
	PARAM_PAGE_ROUTER_LAN,
	PARAM_PAGE_ROUTER_WAN,
	PARAM_PAGE_IPCWIFI,
	PARAM_PAGE_TUOBAO,
	PARAM_PAGE_VEHICLEMNG,
	PARAM_PAGE_OUTPUT,
	PARAM_PAGE_TELALARM_AREA,
	PARAM_PAGE_TELALARM_WIRESENSOR,
	PARAM_PAGE_TELALARM_WIRELESSSENSOR,
	PARAM_PAGE_PHONENUMBERSETTING,
	PARAM_PAGE_CAMERASET,
	PARAM_PAGE_MOBILESTREAM,
	PARAM_PAGE_HUAWEIPLATFORM,
	PARAM_PAGE_KEYCONF,
	PARAM_PAGE_IPFILTER,
	PARAM_PAGE_RTSP,
	PARAM_PAGE_FTP,
	PARAM_PAGE_ELECLOCK,
	PARAM_PAGE_CLOUDSTORAGE,
	PARAM_PAGE_CLOUDEMAIL,
	PARAM_PAGE_P2PQRCODEID,
	PARAM_PAGE_DEVALLSTATUS,
	PARAM_PAGE_LOREXP2PPARAM,
	PARAM_PAGE_FTPAUTOUPGRADE,
	PARAM_PAGE_MANUALRECORD,
	PARAM_PAGE_END,
} PARAM_PAGE_E;

typedef S32 (*FormatCallBack)( VOID *pstSet, VOID *pstSoleSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize );
typedef S32 (*ParseCallBack)( S8 *pDataBuff, VOID *pstReq );

class DevInterface;
class ParseElement 
{
	// 
	public:
		ParseElement(DevInterface *pIntf);
		~ParseElement();
		S32 LocalParamFormat( PARAM_PAGE_E enPage, U32 u32ChnMask, S8 *pDataBuff );
		S32 NetParamParse( PARAM_PAGE_E enPage, S8 *pDataBuff, VOID *pstReq, S32 s32ParseSize );	
		
	// 
	private:
		S32 Formation(FormatCallBack Func, S8 *pDataBuff, U32 u32ChnMask, U32 u32ElemSize, U32 u32ListSize, U32 u32Level = TOP_ELEMENT);
		S32 Parse( ParseCallBack ElemFunc, ParseCallBack ItemFunc, ParseCallBack ItemlistFunc, 
				S8 *pDataBuff, VOID *pstReq, U32 u32TotalSize);
		static S32 GetElementData( VOID *pstElem, S32 s32ElemSize, S8 *pDataBuff );

		static S32 Base3GInfoParamAdapter( VOID *p3GInfoSet, VOID *pSole3GInfoSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize);
		static S32 BaseInfoParamAdapter( VOID *pOsdSet, VOID *pSoleOsdSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize );
		static S32 HddParamAdapter( VOID *pOsdSet, VOID *pSoleOsdSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize );
		static S32 SerialParamAdapter( VOID *pSerialSet, VOID *pSoleSerialSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		static S32 GeneralParamAdapter( VOID *pGeneralSet, VOID *pSoleGeneralSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		static S32 UserInfoParamAdapter( VOID *pUserInfoSet, VOID *pSoleUserInfoSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		static S32 DstParamAdapter( VOID *pDstSet, VOID *pSoleDstSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		static S32 MainStreamParamAdapter( VOID *pMainStreamSet,VOID *pSoleMainStreamSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		static S32 SubStreamParamAdapter( VOID *pSubStreamSet, VOID *pSoleSubStreamSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		static S32 ShelterParamAdapter( VOID *pShelterSet, VOID *pSoleShelterSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		static S32 PtzParamAdapter( VOID *pPtzSet, VOID *pSolePtzSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		static S32 OsdParamAdapter( VOID *pOsdSet, VOID *pSoleOsdSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize );
		static S32 NetBasicParamAdapter( VOID *pNetBasicSet, VOID *pSoleNetBasicSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		static S32 DdnsParamAdapter( VOID *pDdnsSet, VOID *pSoleDdnsSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		static S32 MobileParamAdapter( VOID *pMobileSet, VOID *pSoleMobileSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		static S32 PPPoEParamAdapter( VOID *pPPPoESet, VOID *pSolePPPoESet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		static S32 EmailParamAdapter( VOID *pEmailSet, VOID *pSoleEmailSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		static S32 NtpParamAdapter( VOID *pNtpSet, VOID *pSoleNtpSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		static S32 MotionParamAdapter( VOID *pMotionSet, VOID *pSoleMotionSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		static S32 AbnormalParamAdapter( VOID *pAbnormalSet, VOID *pSoleAbnormalSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		static S32 IOAlarmParamAdapter( VOID *pIOSet, VOID *pSoleIOSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		static S32 RecordParamAdapter( VOID *pRecSchedSet, VOID *pSoleRecSchedSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		static S32 ScheduleParamAdapter( VOID *pTimeSchedSet, VOID *pSoleTimeSchedSet,S32 s32ChnNO,BOOL bDirection, U32 u32Parsesize);
		static S32 ColorParamAdapter( VOID *pTimeSchedSet, VOID *pSoleTimeSchedSet,S32 s32ChnNO,BOOL bDirection, U32 u32Parsesize);
		static S32 DefaultParamAdapter( VOID *pDefaultSet, VOID *pSoleDefaultSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		static S32 MaintainParamAdapter( VOID *pMaintainSet, VOID *pSoleMaintainSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		static S32 IntelligentParamAdapter( VOID *pIntelligentSet, VOID *pSoleIntelligentSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		static S32 IntAvdParamAdapter( VOID *pIntAvdSet, VOID *pSoleIntAvdSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		static S32 EmailScheduleParamAdapter( VOID *pEmailScheduledSet, VOID *pSoleEmailScheduledSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize );
		static S32 PlatformParamAdapter( VOID *pPlatformdSet, VOID *pSolePlatformdSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize );
		static S32 IPCParamAdapter( VOID *pTimeSchedSet, VOID *pSoleTimeSchedSet,S32 s32ChnNO,BOOL bDirection, U32 u32Parsesize);
		static S32 IpcIOAlarmParamAdapter( VOID *pTimeSchedSet, VOID *pSoleTimeSchedSet,S32 s32ChnNO,BOOL bDirection, U32 u32Parsesize);
		static S32 RouterLanParamAdapter( VOID *pRouterLandSet, VOID *pSoleRouterLandSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize );
		static S32 RouterWanParamAdapter( VOID *pRouterWandSet, VOID *pSoleRouterWandSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize );
		static S32 WifiParamAdapter( VOID *pWifidSet, VOID *pSoleWifidSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize );
		static S32 VehicleMngParamAdapter( VOID *pVehicleMngdSet, VOID *pSoleVehicleMngdSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize );
        static S32 TuobaoParamAdapter( VOID *pRouterWandSet, VOID *pSoleRouterWandSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize ); 
		static S32 OutputSetParamAdapter( VOID *pvOutputSet, VOID *pvSoleOutputSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize );
		static S32 TelAlarmAreaParamAdapter( VOID *pvTelAlarmAreaSet, VOID *pvSoleTelAlarmAreaSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize );
		static S32 TelAlarmWireSensorParamAdapter( VOID *pvTelAlarmWireSensorSet, VOID *pvSoleTelAlarmWireSensorSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize ); 
		static S32 TelAlarmWirelessSensorParamAdapter( VOID *pvTelAlarmWirelessSensorSet, VOID *pvSoleTelAlarmWirelessSensorSet, S32 s32ChnNO, BOOL bDirection,U32 u32ParseSize );
		static S32 PhoneNumberSettingParamAdapter( VOID *pPhoneNumberSetting, VOID *pSolePhoneNumberSetting, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize );
		static S32 CameraSetParamAdapter( VOID *pCameraSet, VOID *pSoleCameraSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize );
		static S32 MobileStreamSetParamAdapter( VOID *pMobileStream, VOID *pSoleMobileStream, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize );
		static S32 HuaweiPlatformParamAdapter( VOID *pHuaweiPlatform, VOID *pSoleHuaweiPlatform, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize);

		static S32 KeyConfParamAdapter( VOID *pKeyConf, VOID *pSoleKeyConf, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize);
		static S32 IPFilterParamAdapter( VOID *pIPFilter, VOID *pSoleIPFilter, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize);	
		static S32 RtspParamAdapter( VOID *pRtspSet, VOID *pSoleRtspSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize);
		static S32 FtpParamAdapter( VOID *pFtpSet, VOID *pSoleFtpSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize);
		static S32 ElecLockParamAdapter( VOID *pElecLockSet, VOID *pSoleElecLockSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize);
		static S32 CloudStorageParamAdapter( VOID *pCloudStSet, VOID *pSoleCloudStkSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize);
		static S32 CloudEmailParamAdapter( VOID *pCloudEmailSet, VOID *pSoleCloudEmail, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize);
		static S32 P2pQrCodeIDParamAdapter( VOID *pSet, VOID *pSoleSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize);
		static S32 DevAllStatusParamAdapter( VOID *pDevAllStatusSet, VOID *pSoleDevAllStatusSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize);
		static S32 LorexP2pParamAdapter ( VOID *pLorexP2pParam, VOID *pSoleLorexP2pParam,S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize );
		static S32 FtpAutoUpgradeParamAdapter( VOID *pFtpAutoUpgrade, VOID *pSoleFtpAutoAdapter, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize);
		static S32 ManualRecordParamAdapter( VOID *pManualRecordSet, VOID *pSoleManualRecordSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize);


		static S32 ParseQueryReq( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseBaseInfoSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseHddSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSerialSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseGeneralSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSoleUserSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseDstSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseMaintainSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseMainStreamSet(S8 *pDataBuff, VOID *pstReq);
		static S32 ParseSoleMainStreamSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSoleSubStreamSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSoleVideoCoverSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSolePtzSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseOsdSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSoleOsdSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseNetBasicSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseDdnsSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseMobileSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParsePPPoESet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseEmailSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseNtpSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSoleMotionSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSoleAbnormalSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSoleIOAlarmSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseRecordSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSoleRecordSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSoleSchedule( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSoleColorSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseDefaultSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseIntelligentSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSoleIntelligentSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSoleIntAvdSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseEmailScheduleSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParsePlatformSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSoleIPCSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSoleIpcIOAlarmSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseRouterLanSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseRouterWanSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseWifiSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSoleWifiSet( S8 *pDataBuff, VOID *pstReq);
		static S32 ParseVehicleMngSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseOutputSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseTelAlarmArea( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSoleTelAlarmArea( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSoleTelAlarmWireSensor( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSoleTelAlarmWirelessSensor( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSolePhoneNumberSetting( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSoleCameraSet( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSoleMobileStreamSet(S8 *pDataBuff, VOID *pstReq);
		static S32 ParseHuaweiPlatform( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSoleHuaweiPlatform( S8 *pDataBuff, VOID *pstReq );

		static S32 ParseKeyConf( S8 *pDataBuff, VOID *pstReq );
		
		static S32 ParseIPFilter( S8 *pDataBuff, VOID *pstReq);
		static S32 ParseSoleIPFilter( S8 *pDataBuff, VOID *pstReq);
		static S32 ParseRtsp( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseFtp( S8 *pDataBuff, VOID *FtpReq );
		static S32 ParseSoleFtp( S8 *pDataBuff, VOID *pstReq);
		static S32 ParseSoleElecLock( S8 *pDataBuff, VOID *pstReq);
		static S32 ParseCloudStorage( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseCloudeEmail( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseP2PQrCodeId( S8 *pDataBuff, VOID *pstReq );
		static S32 ParseSoleDevAllStatus( S8 *pDataBuff, VOID *pstReq);
		static S32 ParseLorexP2pParam( S8 *pDataBuff, VOID *pstReq); 
		static S32 ParseFtpAutoUpgrade( S8 *pDataBuff, VOID *pstReq); 
		static S32 ParseManualRecord( S8 *pDataBuff, VOID *pstReq);



		static DevInterface *m_pIntf;
	public:

	private:
//		S8 m_s8ParseStr[10*1024];
};
#endif
