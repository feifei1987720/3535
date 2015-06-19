#ifndef __RTSP_FUNC_H__
#define __RTSP_FUNC_H__

#include "rtspCLib.h"
#include "rtspFunc.h"

#define RTSP_MAX_CHANNEL_NUM  16
#define  RTSP_MAX_STREAM_NUM    3

typedef struct rtsp_describe_Info_t
{
	unsigned char channelNo;
	unsigned char streamNo;
	unsigned short videoHeigth;
	unsigned short videoWeith;
	unsigned short videoFreamRate;
	unsigned short videoBitRate;
	char Describebuf[2048];
}rtsp_describe_Info_t;

typedef struct dev_channel_rtsp_Info_t
{
	rtsp_describe_Info_t   *rtspDescribeInfo;
}dev_channel_rtsp_Info_t;



/* total 12Bytes */
typedef struct rtp_head_t
{
#if (BYTE_ORDER == LITTLE_ENDIAN) 
	/* byte 0 */
	unsigned short cc      :4;   /* CSRC count */
	unsigned short x       :1;   /* header extension flag */
	unsigned short p       :1;   /* padding flag */
	unsigned short version :2;   /* protocol version */

	/* byte 1 */
	unsigned short pt      :7;   /* payload type */
	unsigned short marker  :1;   /* marker bit */
#elif (BYTE_ORDER == BIG_ENDIAN)
	/* byte 0 */
	unsigned short version :2;   /* protocol version */
	unsigned short p       :1;   /* padding flag */
	unsigned short x       :1;   /* header extension flag */
	unsigned short cc      :4;   /* CSRC count */
	/*byte 1*/
	unsigned short marker  :1;   /* marker bit */
	unsigned short pt      :7;   /* payload type */
#else           
#error YOU MUST DEFINE BYTE_ORDER == LITTLE_ENDIAN OR BIG_ENDIAN !
#endif  


	/* bytes 2, 3 */
	unsigned short seqno  :16;   /* sequence number */

	/* bytes 4-7 */
	unsigned int ts;            /* timestamp in ms */

	/* bytes 8-11 */
	unsigned int ssrc;          /* synchronization source */
}rtp_head_t;


/* RTSP */
rtsp_info_t * RTSP_Init();

/* RTSP */
void RTSP_Set_Param(rtsp_info_t *pRtspInfo, char *pAddr, int port, char *pUserName, char *pPwd, int protocol, char *pFileName);

/* RTSP */
int RTSP_Init_Socket(rtsp_info_t *pRtspInfo);

/* RTSP */
int RTSP_Exit(rtsp_info_t *pRtspInfo);

/* RTSP */
void RTSP_SetDefaultTimerLength(rtsp_info_t *pRtspInfo);

/* RTSPTCP */
int RTSP_Client_SubSvr(rtsp_info_t *pRtspInfo);

/* RTSP */
int RTSP_ResetGlob(rtsp_info_t *pRtspInfo);

/* RTSP */
int RTSP_Free(rtsp_info_t *pRtspInfo);

/* RTSP */
int RTSP_TaskProc(rtsp_info_t *pRtspInfo,int channelNo,int streamNo);

/* RTSP */
int RTSP_TimerProc(rtsp_info_t *pRtspInfo);

/*  */
int RTSP_TcpSocketRecv(rtsp_info_t *pRtspInfo,int channlNo,int streamNo);

/* Option */
int RTSP_OptionRtspRespMsg(rtsp_info_t *pRtspInfo);

/* Describe */
int RTSP_DescribeRtspRespMsg(rtsp_info_t *pRtspInfo,int channlNo,int streamNo);

/* Setupone */
int RTSP_SetuponeRtspRespMsg(rtsp_info_t *pRtspInfo);

/* Setuptwo */
int RTSP_SetuptwoRtspRespMsg(rtsp_info_t *pRtspInfo);

/* Play */
int RTSP_PlayRtspRespMsg(rtsp_info_t *pRtspInfo);

/* TeardownRTSP */
int RTSP_SendTeardownMsg(rtsp_info_t *pRtspInfo);

/* tcp */
int RTSP_TcpRecvRtspMsg(rtsp_info_t *pRtspInfo);

/* Describe401 */
int RTSP_ParseDescribe401Resp(rtsp_info_t *pRtspInfo);

/* Describe200okDescribe */
int RTSP_ParseDescribe200Resp(rtsp_info_t *pRtspInfo,int channlNo,int streamNo);

/* RTSP */
void RTSP_Build_Common (char *buffer, int *at, const int iCSeq, ST_RTSP_COMMAND_T *cmd);

/* RTSP */
int RTSP_Send_Cmd (rtsp_info_t *pRtspInfo, const char *cmd_name, ST_RTSP_COMMAND_T *cmd, const char* track_name);

/* RTPVedio UDP  */
int RTSP_CreateRTPVedioUDPSoc(rtsp_info_t *pRtspInfo);

/* RTPAudio UDP  */
int RTSP_CreateRTPAudioUDPSoc(rtsp_info_t *pRtspInfo);

/*  */
int RTSP_SockConnectable(int sock_fd);

/*  */
int RTSP_SockReadable(int sock_fd);

/*  */
int RTSP_SockWriteable(int sock_fd);

/* RTSP */
int RTSP_SetCBState(rtsp_info_t *pRtspInfo, int ucState);

/*  */
int RTSP_GetTick(rtsp_info_t *pRtspInfo);

/* RTSP URL */
int RTSP_URL(rtsp_info_t *pRtspInfo);

ssize_t Recv(int s, void *buf, size_t len, int flags,struct timeval timeout);

int  RecvFull(int sock, char* buffer, int len);

int Recv_TCP(int sock, stream_packet_queue_t *pPacketQueue, stream_packet_t *pPacket, int packetLen, int *recvLen);

int RTSP_RTP_TcpRecv(rtsp_info_t *pRtspInfo, stream_packet_queue_t *pPacketQueue, stream_packet_t *pPacket, int packetBufLen, int *pChannel, int *pRecvLen);

/*  */
/*    -1  0  1  2 */
int RTSP_GetStreamFrameInfo(rtsp_info_t *pRtspInfo, char *pPacketBuf, int dataLen, int *pOffset, int *pFrameLen, unsigned int *pPts, int *pChannel,int *isKeyFrame);

/* RTPmarker */
void RTSP_RTP_Parse_RTPHead(char * RawData, ST_RTP_HEAD *pstRtpHead);

/* H264 */
int RTSP_RTP_Parse_H264(rtsp_info_t *pRtspInfo, char * RawData, int RawDataLen);

int rtspGetDescribeInfo(int channelNo, int streamNo, rtsp_describe_Info_t *pRtspDesInfo);
int rtspSetDescribeInfo(rtsp_describe_Info_t *pRtspDesInfo);


int RTSP_KeepAlive(rtsp_info_t *pRtspInfo, const char *cmd_name);

#endif//__RTSP_FUNC_H__

