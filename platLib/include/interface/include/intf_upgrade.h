
/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Zhong caiwang <caiwang213@gmail.com>
 * Apr 2011-06-04
 *
 * Paramenter config relevant interface defines.
 */

#ifndef __INTF_UPGRADE_H__
#define __INTF_UPGRADE_H__

#include "exception.h"
#include "RS_Upgrade.h"
typedef enum tag_StateBitmap_E
{
	SB_UPGRADE_DOING = 0,
	SB_UPGRADE_SUCC,
	SB_IN_USERINTERFACE

}StateBitmap_E;
typedef enum tag_UPVersion_E
{
	UPVer_UBoot = 0,
	UPVer_Kernel,
	UPVer_Rootfs,
	UPVer_App,
	UPVer_Logo,
	UPVer_SlaveBoot,
	UPVer_SlaveKernel,
	UPVer_SlaveRootfs,
	UPVer_WWW,
	UPVer_FPGA
}UPVersion_E;

typedef void (*pUpgradeWriteFlashFunc)(void * pUGAttr,unsigned char percent);

class IUpgrade
{
	public:
		IUpgrade() {}
		virtual ~IUpgrade() {}
	
	public:

		virtual S32 SetStateBitmap(StateBitmap_E state)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}	
		virtual S32 CheckStateBitmap(StateBitmap_E state)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		virtual S32 ClearStateBitmap(StateBitmap_E state)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		virtual S32 CheckDeviceType(char* szDeviceType)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		virtual S32 IsNewVersion(UPVersion_E upVersion,char* szNewVersion) //1 new
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		virtual S32 CheckCRC(unsigned long stdCRC,const char* pBuffer,unsigned long len)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		virtual S32 RemoteUpgradeHandle(RS_UpgradeFileHeader_t* pUpgradeHeader,
				char* pDataBuffer,unsigned long* pUpgradeBitmap, unsigned long upgradeFileSize,pUpgradeWriteFlashFunc UpgradeFunc ,VOID *UGWriteFlashParam)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		virtual U32 GetUpgradeMagic()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 GetGenType(S8 *ps8GenType)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual BOOL IsValidUpgradePacket(char *pPacketName)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 ComparePacketVersion(char *pPacketName1, char *pPacketName2)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 GetPacketUpgradeBitMap(S8 *pPacketName, unsigned long *pUpgradeBitmap)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		virtual S32 PlatformUpgradeHandle(S8 *ps8FileBuffer, U32 u32FileSize, unsigned long UpgradeBitmap)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		// @return 0  success   else  fail
		virtual S32 CheckFileHead(RS_UpgradeFileHeader_t* pUpgradeHeader, U32 FileHeadSize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		// @return 0  success   else  fail
		virtual S32 StartFtpUpgrade()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 GetDeviceSoftVersion(char* SoftVersion)                                                 
		{   
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);                                                                  
		}            
};

#endif
