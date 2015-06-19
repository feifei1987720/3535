#ifndef __RTSP_C_LIB_H__
#define __RTSP_C_LIB_H__

#include "global.h"
#include "streamLib.h"


#define FRAME_SOURCE_IPC        0//该帧是从IPC获得的
#define FRAME_SOURCE_PLAT       1//该帧是从平台获得的

typedef enum
{               
	EN_RTP_PT_ULAW             = 0,        /* mu-law */
	EN_RTP_PT_GSM              = 3,        /* GSM */
	EN_RTP_PT_G723             = 4,        /* G.723 */
	EN_RTP_PT_ALAW             = 8,        /* a-law */
	EN_RTP_PT_G722             = 9,        /* G.722 */
	EN_RTP_PT_S16BE_STEREO     = 10,       /* linear 16, 44.1khz, 2 channel */
	EN_RTP_PT_S16BE_MONO       = 11,       /* linear 16, 44.1khz, 1 channel */
	EN_RTP_PT_MPEGAUDIO        = 14,       /* mpeg audio */
	EN_RTP_PT_JPEG             = 26,       /* jpeg */
	EN_RTP_PT_MPEG_TS          = 33,        /* mpeg ts */
	EN_RTP_PT_H264             = 96,       /* hisilicon define as h.264 */
	EN_RTP_PT_G726             = 97,       /* hisilicon define as G.726 */
	EN_RTP_PT_ADPCM            = 98,       /* hisilicon define as ADPCM */
	EN_RTP_PT_MPEG4                = 99,       /*xsf define as Mpeg4*/
	EN_RTP_PT_INVALID          = 127
}EN_RTP_PT;


#define RTP_HEAD_LEN 12//rtp头的长度



#define RTSP_DATA_BUFER_SIZE       (4*1024)
#define RTSP_DATA_MAIN_BUFER_SIZE  (8*1024)  /*for rtsp main recv buffer size*/
#define RTSP_DATA_SUB_BUFER_SIZE   (4*1024)  /*for rtsp sub recv buffer size*/
#define RTSP_MAX_SESSION_LEN       64
#define RTSP_MAX_TRACKID_LEN       256 //256
#define RTSP_MAX_URL_LEN           1024   //1024
#define RTSP_MAX_ATH_LEN           256   //1024
#define RTSP_MAX_REALM_LEN         128   //256
#define RTSP_TICK_BASE             1                  /* 定时器消息的基本单位是1个tick */
#define RTSP_TICK_VALUE            10                 /* 系统以10ms为基本单位 */
#define NULL_DWORD                 0xffffffff
#define NULL_WORD                  0xffff
#define NULL_BYTE                  0xff
#define RTSP_DEFAULT_PORT          554
#define RTP_VEDIO_PORT             1600
#define RTP_AUDIO_PORT             1800
#define RTSP_CLIENT_LOC_PORT       16000
#define USER_AGENT "RTSP 1.0"
#define G711_A                        0x01  /* 64kbps G.711 A, see RFC3551.txt  4.5.14 PCMA */
#define G711_U                        0x02  /* 64kbps G.711 U, see RFC3551.txt  4.5.14 PCMU */
#define ADPCM_DVI4                    0x03  /* 32kbps ADPCM(DVI4) for RTP; see DVI4 diiffers in three 
                                               respects from the IMA ADPCM at RFC3551.txt 4.5.1 DVI4 */
#define G726_16KBPS                   0x04  /* 16kbps G.726, see RFC3551.txt  4.5.4 G726-16 */
#define G726_24KBPS                   0x05  /* 24kbps G.726, see RFC3551.txt  4.5.4 G726-24 */
#define G726_32KBPS                   0x06  /* 32kbps G.726, see RFC3551.txt  4.5.4 G726-32 */
#define G726_40KBPS                   0x07  /* 40kbps G.726, see RFC3551.txt  4.5.4 G726-40 */
#define RTSP_VER                "RTSP/1.0"

#define         GS_SEND                 (1)
#define         GS_SET_SEND             (0)

#define RTP_NAL_FU_A               28
#define RTP_NAL_FU_B               29

/* RTP解包*/
typedef struct
{
	unsigned int uiTimestamp;
	unsigned int uiNetTimestamp;
	unsigned char ucPayloadType;
	unsigned char unused[3];
	unsigned short usmarker;
	unsigned short seqNo;

}ST_RTP_HEAD;


/* RTSP控制块状态 */
typedef enum
{
	EN_RTSP_STATUS_CONNET,
	EN_RTSP_STATUS_OPTION,
	EN_RTSP_STATUS_DESCRIBE,
	EN_RTSP_STATUS_SETUPONE,
	EN_RTSP_STATUS_SETUPTWO,
	EN_RTSP_STATUS_PLAY,
	EN_RTSP_STATUS_AVILABLE,
	EN_RTSP_STATUS_TEARDOWN,

	EN_RTSP_STATUS_BUTT
}EN_RTSP_CB_STATUS;

/* HTTP消息接收状态类型 */
typedef enum
{
	EN_RTSP_RECV_CONTINUE,
	EN_RTSP_RECV_FINISHED,
	EN_RTSP_RECV_ERROR,

	EN_RTSP_RECV_TYPE_NULL
}EN_RTSP_RECV_TYPE;

/* RTSP命令 */
typedef struct
{
	char *accept;
	char *authorization;
	char *range;
	char *session;
	char *transport;
	int scale;
	int speed;
}ST_RTSP_COMMAND_T;

enum
{
	TRANSPORT_UDP = 0,
	TRANSPORT_TCP = 1,
};

enum
{
	PROTOCOL_RTSP = 0,
	PROTOCOL_PLAT = 1,
};


/* RTSP控制块 */
typedef struct
{
	unsigned char ucCBStatus;
	unsigned char unused[3];
	unsigned long ulCBTimer;                    /* 状态定时器 */
	int iRTSPTcpSock;                               /* RTSP client套接字 */
	int iRTPUdpVedioSock;                        /* Vedio套接字 */
	int iRTPUdpAudioSock;                        /* Audio套接字 */
	unsigned short usRTPVideoPort;
	unsigned short usRTPAudioPort;
	unsigned short usRTSPPort;
	unsigned short ucRetryNum;                /* 重试次数 */
	int iCSeq;                                              /* RTSP请求-响应对的序列号 */

	/* http 接收数据结构 */
	char acRtspData[RTSP_DATA_BUFER_SIZE];  /* HTTP数据缓冲区, 包括http头 */
	unsigned int usRtspDataLen;          /* Rtsp数据缓冲区长度 */
	//char IPNC_acRtspData[RTSP_DATA_BUFER_SIZE];
}rtsp_control_block_t;

/* 设备数据结构 */
typedef struct
{
	int id; /* 记录第几个设备 */
	char user[32];
	char password[32];
	char hostAddress[32];
	unsigned short rtspPort;
	char fileName[64];
	unsigned char protocol;

	int nRtspSeq;

	unsigned char ucauthflag;
	unsigned char unused[3];

	char realm[RTSP_MAX_REALM_LEN];
	char nonce[RTSP_MAX_REALM_LEN];
	unsigned char trackID1;
	unsigned char trackID2;
	unsigned char audioCoder;
	unsigned char videoCoder;
	char actrackID1[RTSP_MAX_TRACKID_LEN];
	char actrackID2[RTSP_MAX_TRACKID_LEN];
	char sessionID[RTSP_MAX_SESSION_LEN];

	unsigned short serverVideoPort;
	unsigned short serverAudioPort;

	unsigned long  ulLocationIP;
	unsigned int usLocationPort;

	char acAuthorization[RTSP_MAX_ATH_LEN];        /* Authorization */
	char acLocationURL[RTSP_MAX_URL_LEN];          /* Location URL地址 */
	char acContenBase[RTSP_MAX_ATH_LEN];
}rtsp_igd_desc_t;

/* RTSP全局变量 */
typedef struct
{
	unsigned long ulTicks;
	int nVideoWidth;
	int nVideoHeight;
	int nFrameRate;
	int nBitRate;
	unsigned char ucAudioMode;
	unsigned char unused[3];
}rtsp_global_t;

/* RTP解包*/
typedef struct
{
	unsigned char ucsendflag;
	unsigned char ucPayloadType;
	unsigned short seqNo;
	unsigned int uiTimestamp;
	unsigned int uiNetTimestamp;
	int iRecvDataLen;
    int channel;
}rtsp_rtp_parse_t;

typedef struct packet_info_t
{//包信息
	unsigned short seq;//
	char channel;//
	char unused[1];//
	unsigned int pts;
	unsigned short offset;//
	unsigned short len;//
}packet_info_t;
#define MAX_PACKET_NUM		1000//最大包数目
typedef struct rtsp_info_t
{//rtsp信息
	unsigned char runFlag;//运行标记  0：停止  1：开始
	rtsp_control_block_t *pstRTSPCB;
	rtsp_igd_desc_t *pstRTSPDesc;
	rtsp_global_t *pstRTSPGlobal;
	rtsp_rtp_parse_t *pstRtpParse;
	unsigned int statusTimeout[EN_RTSP_STATUS_BUTT];//rtsp交互各个状态的超时时间

#if 0
	char *pRecvBuf;//接收数据缓冲
	int bufStartPos;//起始位置
	int bufEndPos;//结束位置
	int curRecvPos;//当前接收数据的长度
	int curParsePos;//接收数据完整的位置
	int frameLen;//帧长度

	packet_info_t packetInfo[MAX_PACKET_NUM];
	int startPacketNo;//包起始位置
	int endPacketNo;//包结束位置
	int curPacketNo;//当前包序号
#endif
}rtsp_info_t;


typedef struct rtp_para
{
    char magicnum;
    char channel;
    unsigned short size;
}rtp_para;



/* rtsp初始化 */
rtsp_info_t * rtspInit();
/* rtsp销毁 */
void rtspUninit(rtsp_info_t *pRtspInfo);
/* rtsp请求视频 */
int rtspInviteStream(rtsp_info_t *pRtspInfo, int channelNo, int streamNo, char *pAddr, int port, char *pUserName, char *pPwd, int protocol, char *pFileName);
/****************************************************
 * 接收视频流
 *
 * pRtspInfo:		rtsp的所有信息
 * pPacketBuf:		当前接收的数据包buf
 * packetBufLen:	包的总长度
 *
 * pChannel:		当前数据包的数据的通道  0：视频  2：音频
 * pRecvLen:		当前数据的长度
 * pOffset:		rtp头信息的长度
 * pVideoFrameLen:	帧长度
 ****************************************************/
int rtspRecvStream(rtsp_info_t *pRtspInfo, stream_packet_queue_t * pPacketQueue, stream_packet_t *pPacket, int packetBufLen, stream_packet_head_t *pPacketHead, int *pVideoFrameLen);
/* rtsp 组帧 */
int rtspPacketToFrameQueue(rtsp_info_t *pRtspInfo, stream_packet_queue_t *pPacketQueue, stream_packet_t *pPacket, stream_frame_queue_t *pAudioFrameQueue, stream_frame_queue_t *pVideoFrameQueue,
		int forceRestartNum, int packetFlg, int *pVideoFrameLen, int *pVideoFrameNo, int *pAudioFrameNo);
/*  关闭流 */
void rtspCloseStream(rtsp_info_t *pRtspInfo);

int rtspParseDetailURL(const char * url, char* address,int* portNum,char * filename);



int RtspParseRtpPacketInfo(rtsp_info_t *pRtspInfo, stream_packet_queue_t * pPacketBuff,stream_packet_head_t *pPacketHead,stream_frame_queue_t *pAudioFrameQueue,stream_frame_queue_t *pVideoFrameQueue,
                           int *pVideoFrameNo, int *pAudioFrameNo,int forceRestartNum,int channelNo,int streamNo,int* countStreamFrameRate);


int rtspPacketBuffToFrameQueue(rtsp_info_t *pRtspInfo, unsigned char *pPacketBuff, stream_packet_head_t *pPacketHead, stream_frame_queue_t *pAudioFrameQueue, stream_frame_queue_t *pVideoFrameQueue,
                           int *pVideoFrameNo, int *pAudioFrameNo,int Type,int forceRestartNum,int FrameLen,int channelNo,int streamNo,int* countStreamFrameRate);

#endif//__RTSP_C_LIB_H__

