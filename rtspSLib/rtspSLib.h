/******************************************************************************

 ******************************************************************************/

#ifndef __GS_RTSP_H__
#define __GS_RTSP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#include "hs_rtpLib.h"
#include "hs_rtsp_commonLib.h"

#define UCTRL_HEAD 		"MCTP/1.0"
#define SOMAXCONN		128
//#define AF_INET		2	/* Internet IP Protocol 	*/
//#define PF_INET		AF_INET
#define SOL_SOCKET		1

#define SO_DEBUG		1
#define SO_REUSEADDR		2

/* Address to accept any incoming messages. */
//#define	INADDR_ANY		((unsigned long int) 0x00000000)

//struct sockaddr {
//	sa_family_t	sa_family;	/* address family, AF_xxx	*/
//	char		sa_data[14];	/* 14 bytes of protocol address	*/
//};

//enum sock_type {
	//SOCK_STREAM	= 1,
	//SOCK_DGRAM	= 2,
	//SOCK_RAW	= 3,
	//SOCK_RDM	= 4,
	//SOCK_SEQPACKET	= 5,
	//SOCK_DCCP	= 6,
//	SOCK_PACKET	= 10,
//};

typedef struct RtspSvr_s
{
	//int			port;
	int 			rtspSvrSockFd;
	int 			rtspSvrPort;
	RtspSession_t		rtspSessWorkList;
	VodSvrState_e		vodSvrState;
	RtpUdpSender_t		*pRtpUdpSender[MAX_VOD_CHN][RTP_STREAM_MAX];
	int			clientCnt[MAX_VOD_CHN];
	VodSvrType_e		svrType;
	int 			udpSendActive;
	int                     pos;//
	//Pause_Handle		hPause;
	//Rendezvous_Handle 	rvInit;
	//Rendezvous_Handle 	rvExit;
	pthread_mutex_t		rtspListMutex;
}RtspSvr_t;

/* rtsp url */
int RTSP_ParseUrl(int *port, char *server, char *fileName, const char *url);




int  RTSP_GetClientNum(int chn);
int  RTSP_CheckChn(int chn);
//void RTSP_ParamsInit(Cfg_t *pCfg);
void RTSP_ParamsInit();
int  RTSP_ThrStart(pthread_t *pid);
int  RTSP_GetUdpSendStat(int chn);
void RTSP_SetUdpSendActive(int chn, int isActive);
int  RTSP_ThrStop(void);
void RTSP_SessListLock(void );
void RTSP_SessListUnlock(void );
void RTSP_CloseAllClient(RtspSvr_t *pRtspSvr);
RtspSession_t *RTSP_GetSessPtrBySessId(char *id);
void closeRemoteStream(void);
int close_all(void);


void rtspServerUpdateParam();
int rtspServerStart();
int rtspServerStop();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __GS_RTSP_H__ */

