/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Zhong caiwang <caiwang213@gmail.com>
 * Apr 2011-04-16
 *
 * Protocol layer. Responsible for communication and transmission.
 */

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__
#include "comm_def.h"
#include "proto_stddef.h"
#include "userlist.h"
#include "RS_UpgradeHelp.h"
#include "net_log.h"

#define  MAX_REQ_MSG_LEN    (20*1024)
#define  MAX_RSP_MSG_LEN    (1024)
#define  MAX_RECV_LEN       (1024)
#define  MAX_EVENTTYPE_NUM  (3)

#define TCP_PROTOCOL 0
#define UDP_PROTOCOL 1
#define TCP_DUALTALK 2
typedef struct _playback_flag_
{
	void *puser; /*if client user playback ,it will be not NULL,or NULL*/
	int  playbackcount;/*the client playback thread num*/
} playback_flag_t;
//VOID UpgradeWriteFlashProgressFunc(void *UGWriteFlashParam, unsigned char Progress);
class ParseElement;
class Protocol
{
	public:
		Protocol( DevInterface *pIntf );
		~Protocol();
		S32 ReqestProcess( S32 s32SockFd, S8 *ps8PeerIP, S8 *ps8PeerPort );
		S32 UdpReqestProcess( S32 s32SockFd );
		S32 SetStopPreviewFlag( BOOL bFlag );
		S32 DelAllUser();
        VOID RemoteUpgradeWriteFlashInfo(VOID * pUGAttr, unsigned char progress);

	protected:
		S32 RecvReqMsg();
		S32 RecvReqMsg( S32 s32SockFd, S8 *ps8Msg, U32 u32TimeOut );
		S32 RecvReqMsg( S32 s32SockFd, MsgHead *pMsgHeader, S8 *pMsgExt, S8 *pMsgBody, U32 u32BodySize, U32 u32TimeOut );
		S32 SendRspMsg( S32 s32SockFd, S8 *ps8Msg, U32 u32TimeOut, User *pUser);
		S32 TcpSendPackHeader( S32 s32SockFd, S32 s32Type );
		S32 TcpSendPackaged( S32 s32SockFd, S8 *ps8Buffer, S32 s32Size, S32 s32FrameNO );
		S32 TcpSendPackaged_2( S32 s32SockFd, S8 *ps8Buffer, S32 s32Size, S32 s32FrameNO );

		S32 UdpRecvReqMsg();
		S32 UdpSendRspMsg();
		S32 UdpSendPackaged( S32 s32SockFd,  S8 *ps8Buffer, S32 s32Size, S32 s32FrameNO, SA *pstCliSA, S32 s32SALen );

		/*  */
	    S32  SetLoginRsp( LoginRsp *pLoginRsp, User *pUser );
		S32  LoginProcess(S32 Version);
		VOID MainMsgLoop( User *pUser );
		VOID NetLogHandle(User *pUser,MsgHead *pMsg, S8 *ipaddr ,S32 ret, U32 userkey);

		/*  */
		// pUser = NULL ,Userkey = 11 
		S32  PreviewProcess( S32 s32SockFd = -1, S8 *ps8Msg = NULL, User *pUser = NULL);
		VOID PreviewLoop( PW_WORKTRD_S *pPwThread );

		/*  */
		S32 HeartBeatProcess( S32 s32SockFd, S8 *ps8Msg , User *pUser);

		/* UDP */
		S32 UdpPacketDetProcess();

		/*  */
		S32 QueryParamProcess( S32 s32SockFd, S8 *ps8Msg, User *pUser );
		S32 SetParamProcess( S32 s32SockFd, S8 *ps8Msg, User *pUser );

		/*  */
		S32 QueryRecordProcess( S32 s32SockFd, S8 *ps8Msg, User *pUser ); 
		
		/*  */
		S32 QueryLogProcess( S32 s32SockFd, S8 *ps8Msg, User *pUser ); 

		/*  */
		S32 SetAlarmSubscribeProcess( S32 s32SockFd, S8 *ps8Msg, User *pUser );
		VOID AlarmStatusUpToReportLoop(User *pUser );

		/**/
		S32 SnaptReportProcess( S32 s32SockFd, S8 *ps8Msg, U32 u32UserKey = 11);
		VOID SnaptReportLoop(SNPT_WORKTRD_S *pSnaptThread);

		/*  */
		S32 PtzCtrlProcess( S32 s32SockFd, S8 *ps8Msg, User *pUser );

		/*  */
		S32 RecDownloadProcess( S32 s32SockFd = -1, S8 *ps8Msg = NULL, U32 u32UserKey = 11 );
		VOID RecDownloadLoop( DL_WORKTRD_S *pThread );

		/*  */
		S32 RemoteUpgradeProcess( S32 s32SockFd = -1, S8 *ps8Msg = NULL, U32 u32UserKey = 11 );
		VOID UpgradeRun( UG_WORKTRD_S* pUgThread );

		S32 CheckFileToUpgrade(char* pUpgradeHeaderData,unsigned long* UpgradeBitmap);
		
		int RepectSwitcheUpgradeFile( S32 s32SockFd,MsgHead head,char* pUpgradeHeaderData,
				unsigned long* UpgradeBitmap,unsigned long* UpgradeFileSize, unsigned int userkey);

		int RecieveUpgradeFile(int clientfd, char *pBuffer, RS_UpgradeFileHeader_t *pUpgradeHeader, 
				unsigned long *UpgradeBitmap,unsigned long UpgradeFileSize, 
				unsigned int HeadSeq,unsigned long& stdCRC,MsgHead& recvMsgHead);

		int RecievePartOfUpgradeFile(int clientfd, char *pBuffer, unsigned long partSize, unsigned int *HeadSeq);
		
		/* **************************************************** */
		//pUser   pUser = NULL ,Userkey = 11 
		/* **************************************************** */
		/*  */
		S32 EmailTestProcess( S32 s32SockFd = -1, S8 *ps8Msg = NULL, User *pUser = NULL);
		VOID EmailTestRun( S32 s32SockFd , User *pUser);

		/* DDNS */
		S32 DDNSTestProcess( S32 s32SockFd = -1, S8 *ps8Msg = NULL, User *pUser = NULL);
		VOID DDNSTestRun( S32 s32SockFd,User *pUser );

		/*  */
		S32 HddFormatProcess( S32 s32SockFd = -1, S8 *ps8Msg = NULL, User *pUser = NULL);
		VOID HddFormatRun( S32 s32SockFd , User *pUser);
		VOID GetHddFormatInfo( S32 s32SockFd , User *pUser);

		/*  */
		S32 SyncTimeCtlProcess( S32 s32SockFd = -1, S8 *ps8Msg = NULL, User *pUser = NULL );

		/* remote reboot  */
		S32 RemoteRebootProcess( S32 s32SockFd = -1, S8 *ps8Msg = NULL, User *pUser = NULL );

		/* color control */
		S32 ColorCtrlProcess( S32 s32SockFd = -1, S8 *ps8Msg = NULL, User *pUser = NULL );

		/* Reqest I frame*/
		S32 ReqIFrameProcess( S32 s32SockFd = -1, S8 *ps8Msg = NULL, User *pUser = NULL);

		/* dualtalk */
	    S32 DualTalkProcess( S32 s32SockFd , S8 *ps8Msg );	
		VOID DualTalkRecvLoop( DTR_WORKTRD_S* pDTRThread );
		VOID DualTalkDecLoop();

		S32 RemoteSnapProcess( S32 s32SockFd = -1, S8 *ps8Msg = NULL, U32 u32UserKey = 11 );
		/*  */
		S32 ExportUserInfo();
		
		/*  */
		S32 SearchRemoteDevProcess( S32 s32SockFd, S8 *ps8Msg, User *pUser );
		VOID SearchRemoteRun( S32 s32SockFd , User *pUser);
		
		/*  */
		S32 SetRemoteDevNetProcess( S32 s32SockFd, S8 *ps8Msg, User *pUser);
		VOID SetRemoteDevNetRun( S32 s32SockFd , User *pUser);

		S32 StartMainMsgLoop( User *pUser );
		S32 StartPreviewLoop( User *pUser, S32 s32ThreadIndex );
		S32 StartRecDownloadLoop( User *pUser, S32 s32ThreadIndex );
		S32 StartUpgradeThread( UG_WORKTRD_S* pUGAttr ,User *pUser);
		S32 StartDualTalkRecvLoop( DTR_WORKTRD_S* pDTRAttr );
		S32 StartDualTalkDecLoop();
		S32 StartAlarmStatusUpToReportLoop(User *pUser);
		S32 StartEmailTest( S32 s32SockFd , User *pUser);
		S32 StartDDNSTest( S32 s32SockFd , User *pUser);
		S32 StartHddFormat( S32 s32SockFd , User *pUser);
		S32 StartSnaptReportLoop(User *pUser);
		S32 StartSearchRemote( S32 s32SockFd,User *pUser );
		S32 StartSetRemoteDevNet( S32 s32SockFd , User *pUser);
		
		S32 SetSockOption( S32 s32SockFd );
		/*  */
		static VOID *ThreadEntry(VOID *pArg);
		inline VOID Lock()   { pthread_mutex_lock( &m_Mutex ); }
		inline VOID UnLock() { pthread_mutex_unlock( &m_Mutex ); }

		/**/
		S32 QueryKeyConfProcess( S32 s32SockFd, S8 *ps8Msg, User *pUser);
		S32 SetKeyConfProcess( S32 s32SockFd, S8 *ps8Msg, User *pUser );

		S32 ReqVideoActivationProcess( S32 s32SockFd, S8 *ps8Msg, User *pUser);
		S32 AddAllDeviceProcess( S32 s32SockFd, S8 *ps8Msg, User *pUser );
		
		S32 StatusAlarmRequestProcess(S32 s32SockFd, S8 *ps8Msg, User *pUser);
	    // Ftp 
		S32 FtpUpgradeProcess(S32 s32SockFd, S8 *ps8Msg, User *pUser);      
		//
		S32 ActivateCloudProcess( S32 s32SockFd, S8 *ps8Msg, User *pUser );
		S32 StartActivateCloud( S32 s32SockFd, User *pUser);
		S32 ActivateCloudRun( S32 s32SockFd, User *pUser);

	private:

		BOOL       m_bStopPreview;
		/* TCP */
		S32        m_s32ConnSockFd;
		S8         m_s8Msg[MAX_REQ_MSG_LEN];
		S8         m_s8PeerIP[NI_MAXHOST];   /* IP */
		S8         m_s8PeerPort[NI_MAXSERV]; /*  */

		/* UDP */
		S32        m_s32UdpSockFd;
		S8         m_s8UdpMsg[MAX_RECV_LEN];
		S32        m_s32CliAddrLen;
		SA_STORAGE m_stCliAddr;
		
		UserList   m_UserList;
		pthread_mutex_t m_Mutex;

		/*  */
		ParseElement *m_pParseElem;
		
		/* alarm subscibe */
		BOOL      m_bAlarmReportRunFlag;

		/* upgrade */
		
		/*  dualtalk */
		BOOL       m_bDualTalkRunFlag;
		//BOOL       m_bDualTalkEnable;
		pthread_t  m_decThreadId;
		S32        m_s32DualTalkFd;
		S32        m_s32DualTalkChn;

		/* Email adn ddns test */
		S8         m_s8TestMsg[MAX_REQ_MSG_LEN];
	public:
		DevInterface *m_pIntf;
		BOOL       m_bTcpReset; //
		BOOL       m_bUdpReset; //
		S8		   m_s8HddFormatStatue;  
		NET_LOG_S  m_NetLog;
		playback_flag_t m_PlaybackFlag;
};

#endif
