#ifndef __RS_UPGRADE_HELP_H__
#define __RS_UPGRADE_HELP_H__

#include "comm_def.h"
#include "proto_stddef.h"
#include "user.h"
#define MaxUpgradeModules 32

typedef struct 
{
	int socketfd;
	MsgHead  msgHead;
	unsigned long UpgradeFileSize;
	unsigned long UpgradeBitmap[MaxUpgradeModules];
	union
	{
		RS_UpgradeFileHeader_t RS_UpgradeHead;
		RS_NewUpgradeFileHeader_t RS_NewUpgradeHead;
		RS_FpgaNewUpgradeFileHeader_t RS_FpgaNewUpgradeHead;
	};
	User *pUser;
}UG_WORKTRD_S;

typedef struct
{
    VOID* pClass;
    UG_WORKTRD_S* pUGAttr;
} Rs_UpgradeParam_s;

typedef struct
{
	upgradefileheaderentry_t NeedUpgrade[MaxUpgradeModules];
}RS_UpgradeRsp;

//int RepectSwitcheUpgradeFile( S32 s32SockFd,MsgHead head,char* pUpgradeHeaderData,
//	unsigned long* UpgradeBitmap,unsigned long* UpgradeFileSize);
//
//int RecieveUpgradeFile(int clientfd, char *pBuffer, RS_UpgradeFileHeader_t *pUpgradeHeader, 
//	unsigned long *UpgradeBitmap,unsigned long UpgradeFileSize, 
//	unsigned int HeadSeq,unsigned long& stdCRC,MsgHead& recvMsgHead);
//
//int RecievePartOfUpgradeFile(int clientfd, char *pBuffer, unsigned long partSize, unsigned int *HeadSeq);


#endif
