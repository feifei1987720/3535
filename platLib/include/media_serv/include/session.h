/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Zhong caiwang <caiwang213@gmail.com>
 * Apr 2011-04-16
 *
 * Session define.
 */

#ifndef __SESSION_H__
#define __SESSION_H__

#include "comm_def.h"
#include "interface.h"
#if 0
#define MAKESESSION( readerid, chnID, streamtype, userkey )  \
	( ((U64)(readerid) << 32) | \
	  ((chnID) << 24) | ((streamtype) << 16) | (userkey) )
#endif

#define MAKESESSION( chnID, streamtype, userkey )  ( ((chnID) << 24) | ((streamtype) << 16) | (userkey) )
#define SESS_CHNNO( SessID )      ( SessID >> 24 )
#define SESS_STREAMTYPE( SessID )  ( (SessID >> 16) & 0xFF )
#define SESS_USERKEY( SessID )    ( SessID & 0xFFFF )
#define CHK_SESSID( SessID )    ((SessID >> 24) >= 0) && ((SessID >> 24) <= 16) && \
								((SessID >> 16) >= 0) && ((SessID >> 16) <= 2 ) && \
							    ((SessID & 0xFFFF) >= 0)

typedef struct tag_TCPTRANS
{
	BOOL bIsOn;
	S32  s32SockFd;
	S32  s32AVType;
	U32  u32FrameNO;
}TCPTRANS_S;

typedef struct tag_UDPTRANS
{
	BOOL         bIsOn;
	S32          s32SockFd;
	sockaddr_in  stCliSA;
	U32          u32FrameNO;
}UDPTRANS_S;

typedef struct tag_DUALTALKTRANS
{
	BOOL bIsOn;
	S32  s32SockFd;
	U32  u32FrameNO;
}DUALTALKTRANS_S;

class DevInterface;
class Session
{

	/*  */
	public:
		Session( S32 s32SessID );
		~Session();
		S32 Init( DevInterface *pIntf );
		S32 SetTcpTrans( BOOL bIsOn, S32 s32SockFd = -1, S32 s32AVType = -1 );
		S32 SetUdpTrans( BOOL bIsOn, S32 s32SockFd = -1, SA *pstSA = NULL );
		S32 SetDualTalkTrans( BOOL bIsOn, S32 s32SockFd);

		S32 GetLatestIFrame( FRAMEINFO_S *pstFrameInfo );
		S32 GetFrame( FRAMEINFO_S *pstFrameInfo );

		VOID EnableTrans( BOOL bIsTrans );
		BOOL IsEnableTrans();

//		BOOL IsEnableBuffer();
		
	/*  */
	private:
		S32 RequestBuffer();
		S32 ReleaseBuffer();

	private:
		S32  m_s32ReaderID; 
		BOOL m_bEnbaletTrans;

		STREAMTYPE_E  m_enStreamType;
		S32           m_s32ChnNO;
		U32           m_u32UserKey;
		BOOL          m_bIsJumpToIFrame;

		DevInterface *m_pIntf;
	public:
		S32             m_s32SessID;
		TCPTRANS_S      m_TcpTrans;
		UDPTRANS_S      m_UdpTrans;
		DUALTALKTRANS_S m_DualTalkTrans;
};

#endif
