/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Zhong caiwang <caiwang213@gmail.com>
 * Apr 2011-04-16
 *
 * Media server define.
 */

#ifndef __MEDIA_SERV_H__
#define __MEDIA_SERV_H__
#include "comm_def.h"
#include "net_log.h"

class DevInterface;
class Protocol;

class MediaServer 
{
	// 
	public:
		MediaServer(DevInterface *pIntf);
		MediaServer(DevInterface *pIntf, NET_LOG_S *pNetLog);
		~MediaServer();
		S32 StopTranslate();
		S32 StartTranslate();
		S32 DeleteAllUser();
		//S32 ResetMediaServer();
		S32 SuspendMediaServer(BOOL bSuspendFlag); //true==suspend, false==not!
		
	// 
	private:
		S32 TCPServStart();
		S32 UDPServStart();
		S32 TCPListen();
		S32 UDPListen();
		S32 AcceptProcess();
		S32 GetPeerIpAddr( struct sockaddr *pstPeerAddr, S32 s32SALen, S8 *PeerIP, S8 *PeerPort);

		static VOID *ThreadEntry(VOID *pArg);

	public:
		DevInterface *pIntf;

	private:
		S32      m_s32ListenFd;
		Protocol *m_pProto;
		BOOL	m_bTcpServRunFlag;
		BOOL	m_bUdpServRunFlag;
};
#endif
