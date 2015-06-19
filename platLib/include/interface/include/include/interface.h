/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Zhong caiwang <caiwang213@gmail.com>
 * Apr 2011-04-16
 *
 * Interfaces defines.
 */

#ifndef __INTERFACE_H__
#define __INTERFACE_H__


#include <stdlib.h>
#include <stdio.h>
#include "intf_base.h"
#include "mainthread.h"


class NetInterface : public DevInterface
{
	/*  */
	public:
		NetInterface(VOID *pMainThread);
		~NetInterface();

	public:
		/* () */
		virtual S32 CheckUser( S8 *pUserName, S8 *pPassword,CHECK_USER_TYPE_E enType = CHECK_USER_TYPE_IE );
		virtual S32 GetDevInfo( DEVINFO_S *pDevInfo, S32 s32LocalUserID = 1 );
		virtual S32 GetDevChnInfo( DEVCHN_INFO_S *pDevChnsInfo, S32 s32ChnNum );
		virtual U16 GetMediaPort();
		virtual U16 GetWebPort();
		virtual S32 GetNetworkState( S32 *s32Mode, U8 *pu8Ip, U8 *pu8NetMask );
		virtual S32 GetDnsAddrs( U8 (*pu8Dns)[4], S32 s32DnsNum );
		virtual S32 GetHostname( S8 *ps8Hostname );
		virtual S32 GetDscpLevel(S32* Level, S32 type, S32 s32ChnNO);
		virtual S32 GetSystemTime(DateTime *pSystemTime);
		virtual S32 SetSystemTime(DateTime *pSystemTime);
		virtual S32 GetDevChnNum();
		virtual S32 GetImageWidth(U32 u32Chn, U32 *pu32ImageWidth, U32 *pu32ImageHight);
		virtual S32 GetFrameRate(U32 u32Channel, STREAMTYPE_E enStreamType);
		virtual S32 GetDeviceName(S8 *ps8DeviceNameBuffer);
		virtual S32 SetIOAlarmStatus(BOOL bStatus, S32 s32IOChn, U32 u32DelayTime);
		virtual DEV_RES_E GetDevResolution();

        virtual BOOL RequestReader( U32 u32UserKey, S32 s32ChnNO, STREAMTYPE_E enStreamType, S32* s32ReaderID );
        virtual BOOL ReleaseReader(S32 s32ReaderID);
        virtual S32 GetLatestIFrame( S32 s32ReaderID, FRAMEINFO_S *pstFrameInfo );
        virtual S32 GetRealFrame(S32 s32ReaderID, FRAMEINFO_S *pstFrameInfo );
		virtual S32 RequestIFrame(S32 s32ChnNO,STREAMTYPE_E enStreamType);

        virtual S32 GetAlarmInfo(U32 u32ReaderID, INTF_ALARM_INFO_S  *pReturnAlarmInfo);
		virtual S32 WaitForGuiStart();
		
	    virtual S32 RequestAlarmListReaderID(U32 *pu32ReturnReaderID);
		virtual S32 ReleaseAlarmListReaderID(U32 u32ReaderID);
		
		virtual S32 GetStreamInfo(S32 s32ChnNO, STREAMTYPE_E enStreamType, STREAMINFO_S *si);
		virtual S32 SetIOMode(S32 s32ReaderID, S32 mode);

		//virtual S32 VideoActivation( U32 u32ChannelBitmap);
//		virtual S32 SetTime(const DateTime* time,const char* szTimeZone);
//		virtual S32 GetTime(DateTime* time,char* szTimeZone,S32& timeZoneLen);
		
		virtual S32 SnapIntoBuffer(S32 s32Chn, S8 *ps8FileName, S8 *ps8Buffer, LONG lBufferSize, LONG *plFileSize);

		//Media Param
		virtual S32 BaseInfoParamAdapter( VOID *pBaseInfoSet, VOID *pSoleBaseInfoSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize);
		virtual S32 HddParamAdapter( VOID *pHddSet, VOID *pSoleHddSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		virtual S32 SerialParamAdapter( VOID *pSerialSet, VOID *pSoleSerialSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		virtual S32 GeneralParamAdapter( VOID *pGeneralSet, VOID *pSoleGeneralSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		virtual S32 UserInfoParamAdapter( VOID *pUserInfoSet, VOID *pSoleUserInfoSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		virtual S32 DstParamAdapter( VOID *pDstSet, VOID *pSoleDstSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		virtual S32 MainStreamParamAdapter( VOID *pMainStreamSet, VOID *pSoleMainStreamSet,S32 s32ChnNO,BOOL bDirection,U32 u32Parsesize);
		virtual S32 SubStreamParamAdapter( VOID *pSubStreamSet, VOID *pSoleSubStreamSet,S32 s32ChnNO,BOOL bDirection,U32 u32Parsesize);
		virtual S32 ShelterParamAdapter( VOID *pShelterSet, VOID *pSoleShelterSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		virtual S32 PtzParamAdapter( VOID *pPtzSet, VOID *pSolePtzSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		virtual S32 OsdParamAdapter( VOID *pOsdSet, VOID *pSoleOsdSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize);
		virtual S32 NetBasicParamAdapter( VOID *pNetBasicSet, VOID *pSoleNetBasicSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		virtual S32 MobileParamAdapter( VOID *pMobileSet, VOID *pSoleMobileSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		virtual S32 DdnsParamAdapter( VOID *pDdnsSet, VOID *pSoleDdnsSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		virtual S32 PPPoEParamAdapter( VOID *pPPPoESet, VOID *pSolePPPoESet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		virtual S32 EmailParamAdapter( VOID *pEmailSet, VOID *pSoleEmailSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		virtual S32 NtpParamAdapter( VOID *pNtpSet, VOID *pSoleNtpSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		virtual S32 MotionParamAdapter( VOID *pMotionSet, VOID *pSoleMotionSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		virtual S32 AbnormalParamAdapter( VOID *pAbnormalSet, VOID *pSoleAbnormalSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		virtual S32 IOAlarmParamAdapter( VOID *pIOSet, VOID *pSoleIOSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		virtual S32 RecordParamAdapter( VOID *pRecSchedSet, VOID *pSoleRecSchedSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		virtual S32 ScheduleParamAdapter( VOID *pScheduledSet, VOID *pSoleScheduledSet,S32 s32ChnNO,BOOL bDirection, U32 u32Parsesize);
		virtual S32 MaintainParamAdapter( VOID *pMaintainSet, VOID *pSoleMaintainSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		virtual S32 ColorParamAdapter( VOID *pColorSet, VOID *pSoleColorSet,S32 s32ChnNO,BOOL bDirection, U32 u32Parsesize);
		virtual S32 DefaultParamAdapter( VOID *pDefaultSet, VOID *pSoleDefaultSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		virtual S32 IntelligentParamAdapter( VOID *pIntelligentSet, VOID *pSoleIntelligentSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		virtual S32 IntAvdParamAdapter( VOID *pAvdSet, VOID *pSoleAvdSet, S32 s32ChnNO, BOOL bDirection, U32 u32Parsesize);
		virtual S32 EmailScheduleParamAdapter( VOID *pEmailScheduledSet, VOID *pSoleEmailScheduledSet, S32 s32ChnNO, BOOL bDirection, U32 u32ParseSize );
		
		//Platform
		virtual S32 GetPlatformPuIP(S8 *ps8Ip);
		virtual U32 GetPlatformPuPort();
		virtual S8 *GetPlatformPuName();
		virtual S8 *GetPlatformPuPassword();
		virtual S8 CheckPlatformUpgradeTag();
		virtual S32 RsPlatParam_CarFuntionParameterAdapter(VOID *pvCarFunSet, VOID *pvSoleCarFunSet, S32 s32Chn, S32 s32AdapterOpt);
		virtual S32 RsPlatParam_SystemBaseInfoAdapter(VOID *pvBaseInfo, VOID *pvSoleBaseInfo, S32 s32Chn, S32 s32AdapterOpt);
		virtual S32 RsPlatParam_OSDParameterAdapter(VOID *pvOsdSet, VOID *pvSoleOsdSet, S32 s32Chn, S32 s32AdapterOpt);
		virtual S32 RsPlatParam_HddParameterAdapter(VOID *pvHddInfo, VOID *pvSoleHddInfo, S32 s32Chn, S32 s32AdapterOpt);
		virtual S32 RsPlatParam_ColorParameterAdapter(VOID *pvColorInfo, VOID *pvSoleColorSet, S32 s32Chn, S32 s32AdapterOpt);
		virtual S32 RsPlatParam_VideoCoverParameterAdapter(VOID *pvVideoCover, VOID *pvSoleVideoCover, S32 s32Chn, S32 s32AdapterOpt);
		virtual S32 RsPlatParam_PTZParameterAdapter(VOID *pvPTZSet, VOID *pvSolePTZSet, S32 s32Chn, S32 s32AdapterOpt);
		virtual S32 RsPlatParam_NetworkParameterAdapter(VOID *pvNetworkSet, VOID *pvSoleNetworkSet, S32 s32Chn, S32 s32AdapterOpt);
		virtual S32 RsPlatParam_SubStreamParameterAdapter(VOID *pvSubStreamSet, VOID *pvSoleSubStreamSet, S32 s32Chn, S32 s32AdapterOpt);
		virtual S32 RsPlatParam_EmailParameterAdapter(VOID *pvEmailSet, VOID *pvSoleEmailSet, S32 s32Chn, S32 s32AdapterOpt);
		virtual S32 RsPlatParam_MobileParameterAdapter(VOID *pvMobileSet, VOID *pvSoleMobileSet, S32 s32Chn, S32 s32AdapterOpt);
		virtual S32 RsPlatParam_MotionParameterAdapter(VOID *pvMotionSet, VOID *pvSoleMotionSet, S32 s32Chn, S32 s32AdapterOpt);
		virtual S32 RsPlatParam_AbnormalParameterAdapter(VOID *pvAbnormalSet, VOID *pvSoleAbnormalSet, S32 s32Chn, S32 s32AdapterOpt);
		virtual S32 RsPlatParam_IOAlarmParameterAdapter(VOID *pvIOAlarmSet, VOID *pvSoleIOAlarmSet, S32 s32Chn, S32 s32AdapterOpt);
		virtual S32 RsPlatParam_MainStreamParameterAdapter(VOID *pvMainStreamSet, VOID *pvSoleMainStreamSet, S32 s32Chn, S32 s32AdapterOpt);
		virtual S32 RsPlatParam_RecordParameterAdapter(VOID *pvRecordSet, VOID *pvSoleRecordSet, S32 s32Chn, S32 s32AdapterOpt);
		virtual S32 RsPlatParam_ScheduleParameterAdapter(VOID *pvScheduleSet, VOID *pvSoleScheduleSet, S32 s32Chn, S32 s32AdapterOpt);
		virtual S32 RsPlatParam_MaintainParameterAdapter(VOID *pvMaintainSet, VOID *pvSoleMaintainSet, S32 s32Chn, S32 s32AdapterOpt);
		virtual S32 RsPlatParam_DefaultParameterAdapter(VOID *pvDefaultSet, VOID *pvSoleDefault, S32 s32Chn, S32 s32AdapterOpt);
		virtual S32 RsPlatParam_PlatformParameterAdapter(VOID *pvPlatformSet, VOID *pvSolePlatformSet, S32 s32Chn, S32 s32AdapterOpt);
		virtual S32 PlatSaveFlash( SAVE_FLASH_OPT_E enOpt );
		virtual S32 UpdatePlatParam();
		virtual S8  PlatformMessage(S8 *ps8Message, U32 u32MessageBufferSize, S32 s32Purpose);
		virtual S32 RsPlatParam_DDNSParameterAdapter(VOID *pvDDNSSet, VOID *pvSoleDDNSSet, S32 s32Chn, S32 s32AdapterOpt);
			
		//ELock
		virtual S32 RegisterELockRequestCallBack(ELockRequestCallBack ELockRequestFuncs, INTF_ELOCK_USERH *pELUserH);
		virtual S32 ELockCtrl(INTF_ELOCK_CTRL_TYPE_E ELockCtrlType, INTF_ELOCK_CTRL_PARAM_IN_S *pParamIn, INTF_ELOCK_CTRL_PARAM_OUT_S *pParamOut);
		virtual S32 ELockReportFrequency();
		virtual S32 RsPlatParam_ELockParameterAdapter(VOID *pvELockSet, VOID *pvSoleELockSet, S32 s32Chn, S32 s32AdapterOpt);
		virtual S32 RsPlatParam_ICCardParameterAdapter(VOID *pvICCardSet, VOID *pvSoleICCardSet, S32 s32Chn, S32 s32AdapterOpt);
		
		//GPS
		virtual S32 GetGpsReportFrequence();
		virtual S32 GetCurGpsInfo(RSGpsInfoPacket_t *pGpsInfoPacket);

		//Upgrade
		virtual S32 SetStateBitmap(StateBitmap_E state);
		virtual S32 CheckStateBitmap(StateBitmap_E state);
		virtual S32 ClearStateBitmap(StateBitmap_E state);
		virtual S32 CheckDeviceType(char* szDeviceType);
		virtual S32 GetGenType(S8 *ps8GenType);
		virtual S32 IsNewVersion(UPVersion_E upversion,char* szNewVersion);
		virtual S32 CheckCRC(unsigned long stdCRC,const char* pBuffer,unsigned long len);
		virtual S32 RemoteUpgradeHandle(RS_UpgradeFileHeader_t* pUpgradeHeader,
				char* pDataBuffer,unsigned long* pUpgradeBitmap, unsigned long upgradeFileSize,pUpgradeWriteFlashFunc UpgradeFunc ,VOID *UGWriteFlashParam);
		virtual U32 GetUpgradeMagic();
		virtual BOOL IsValidUpgradePacket(char *pPacketName);
		virtual S32 ComparePacketVersion(char *pPacketName1, char *pPacketName2);
		virtual S32 GetPacketUpgradeBitMap(S8 *pPacketName, unsigned long *pUpgradeBitmap);
		virtual S32 PlatformUpgradeHandle(S8 *ps8FileBuffer, U32 u32FileSize, unsigned long UpgradeBitmap);
		
		virtual S32 CheckFileHead(RS_UpgradeFileHeader_t* pUpgradeHeader, U32 FileHeadSize);
		virtual S32 StartFtpUpgrade();
		//ptz
		virtual S32 PtzCtrl( S32 s32Cmd, S32 s32ChnNO, S32 s32Param1, S32 s32Param2 );
		virtual BOOL IsGuiPtzAutoScan();
		virtual S32 PtzBindProtocol();
		

		virtual S32 RestartMachine();
		/*  */
	protected:

	private:
		CMainThread     *m_pMainThread;
		SYSPARAMEMTER_S *m_pSysParam;
		CMAVEnc   *m_pEncoder;
		CFsManage *m_pFileManager;

};
#endif
