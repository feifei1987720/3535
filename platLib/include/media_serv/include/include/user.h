/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Zhong caiwang <caiwang213@gmail.com>
 * Apr 2011-04-16
 *
 * User defines.
 */
#ifndef __USER_H__
#define __USER_H__
#include "sessionlist.h"

//#define MAX_CHANNEL_NUM   16  
#define MAX_CHANNEL_NUM   32     // 32  

//#define MAX_THREADCHN_NUM  4    /*  */
//#define MAX_THREADCHN_NUM  2    /*  */
#define MAX_THREADCHN_NUM  1    /*  */
#define MAX_DOWNLOAD_CHN   4*2  /* ,  */

class FsVisitor;
typedef struct tag_PW_WORKTRD_S
{
	pthread_t   ThreadID;      /* ID */
	BOOL        bIsStart;      /*  */
	S8          *ps8Buffer;
	SessionList SessList;
}PW_WORKTRD_S;

typedef struct tag_PB_CONTEXT_S
{
	pthread_t ThreadID;      /* ID */
	BOOL      bIsStart;      /*  */
	S32       s32SockFd;
	S8        s8MsgVersion;  /*  */
	U32       u32MsgSeq;     /*  */
	S8        *ps8Msg;
	FsVisitor *pFsVisitor;
	VOID      *UserH;
}DL_WORKTRD_S;

typedef struct tag_DTR_WORKTRD_S
{
	S32       s32SockFd;
}DTR_WORKTRD_S;
typedef struct tag_SNPT_WORKTRD_S
{
	pthread_t    snaptReportID;
	BOOL         bIsStart;
	S32       	 s32SockFd;
	UINT         uSnaptReportKey;
	LONG         lCallbackCount;
}SNPT_WORKTRD_S;
class User 
{

	public:
		User( U32 u32Key );
		~User();
		S32  Init( S32 s32SockFd, S32 s32UserID, BOOL bIsAlarmReport, S8 *ps8PeerIP, S8 *ps8PeerPort ,S32 Version);
		BOOL IsLive();
		S32  SetToDead();
		S32  GetPeerInfo();

	protected:
		S32 CloseAllSession();
		S32 CloseAllThread();
	public:
		
		BOOL m_bIsAlarmReport;
		U32  m_u32MsgHeadSeq;
		S8   m_s8PeerIP[NI_MAXHOST];                    /* IP */
		S8   m_s8PeerPort[NI_MAXSERV];                  /*  */
	private:
		BOOL m_bIsLive;

	public:
		U32  m_u32UserKey;
		S32  m_s32LocalUserID;
		S32  m_s32SockFd;
		BOOL m_bEncrpty;

		PW_WORKTRD_S m_PwThread[MAX_CHANNEL_NUM/MAX_THREADCHN_NUM];
		DL_WORKTRD_S m_DlThread[MAX_DOWNLOAD_CHN];        /*  */
		SNPT_WORKTRD_S m_SNPThread;
		pthread_t    m_MsgThreadID;                     /* ID */  
	
};
#endif
