/******************************************************************************

 ******************************************************************************/

#ifndef __GS_RTP_H__
#define __GS_RTP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "hs_typeLib.h"
#include "hs_rtsp_commonLib.h"
#include "hs_miscLib.h"

#define 	DEFAULT_RTP_UDP_PORT		32767
#define 	RTP_DEFAULT_SSRC 			41030
#define 	RTP_MAX_SENDER				16
#define 	RTP_VERSION				2

#define RTP_HDR_SET_VERSION(pHDR, val)  ((pHDR)->version = val)
#define RTP_HDR_SET_P(pHDR, val)        ((pHDR)->p       = val)
#define RTP_HDR_SET_X(pHDR, val)        ((pHDR)->x       = val)
#define RTP_HDR_SET_CC(pHDR, val)       ((pHDR)->cc      = val)

#define RTP_HDR_SET_M(pHDR, val)        ((pHDR)->marker  = val)
#define RTP_HDR_SET_PT(pHDR, val)       ((pHDR)->pt      = val)

#define RTP_HDR_SET_SEQNO(pHDR, _sn)    ((pHDR)->seqno  = (_sn))
#define RTP_HDR_SET_TS(pHDR, _ts)       ((pHDR)->ts     = (_ts))

#define RTP_HDR_SET_SSRC(pHDR, _ssrc)    ((pHDR)->ssrc  = _ssrc)

#define RTP_HDR_LEN  					sizeof(RtpHdr_t)



typedef enum RtpPt_
{
    RTP_PT_ULAW             = 0,        /* mu-law */
    RTP_PT_GSM              = 3,        /* GSM */
    RTP_PT_G723             = 4,        /* G.723 */
    RTP_PT_ALAW             = 8,        /* a-law */
    RTP_PT_G722             = 9,        /* G.722 */
    RTP_PT_S16BE_STEREO     = 10,       /* linear 16, 44.1khz, 2 channel */
    RTP_PT_S16BE_MONO       = 11,       /* linear 16, 44.1khz, 1 channel */
    RTP_PT_MPEGAUDIO        = 14,       /* mpeg audio */
    RTP_PT_JPEG             = 26,       /* jpeg */
    RTP_PT_H261             = 31,       /* h.261 */
    RTP_PT_MPEGVIDEO        = 32,       /* mpeg video */
    RTP_PT_MPEG2TS          = 33,       /* mpeg2 TS stream */
    RTP_PT_H263             = 34,       /* old H263 encapsulation */
    //RTP_PT_PRIVATE          = 96,
    RTP_PT_H264             = 96,       /* hisilicon define as h.264 */
    RTP_PT_G726             = 97,       /* hisilicon define as G.726 */
    RTP_PT_ADPCM            = 98,       /* hisilicon define as ADPCM */
    RTP_PT_MPEG4		= 99,	/*xsf define as Mpeg4*/
    RTP_PT_INVALID          = 127
}RtpPt_e;


#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

/* total 12Bytes */
typedef struct RtpHdr_s
{

#if (BYTE_ORDER == LITTLE_ENDIAN)
    /* byte 0 */
    u16 cc      :4;   /* CSRC count */
    u16 x       :1;   /* header extension flag */
    u16 p       :1;   /* padding flag */
    u16 version :2;   /* protocol version */

    /* byte 1 */
    u16 pt      :7;   /* payload type */
    u16 marker  :1;   /* marker bit */
#elif (BYTE_ORDER == BIG_ENDIAN)
    /* byte 0 */
    u16 version :2;   /* protocol version */
    u16 p       :1;   /* padding flag */
    u16 x       :1;   /* header extension flag */
    u16 cc      :4;   /* CSRC count */
    /*byte 1*/
    u16 marker  :1;   /* marker bit */
    u16 pt      :7;   /* payload type */
#else
    #error YOU MUST DEFINE BYTE_ORDER == LITTLE_ENDIAN OR BIG_ENDIAN !
#endif


    /* bytes 2, 3 */
    u16 seqno  :16;   /* sequence number */

    /* bytes 4-7 */
    int ts;            /* timestamp in ms */

    /* bytes 8-11 */
    int ssrc;          /* synchronization source */
}RtpHdr_t;

typedef enum RtpSenderType
{
	RTP_SENDER_UNICAST		= 0, 	/*  */
	RTP_SENDER_MULTICAST	= 1,	/*  */
	RTP_SENDER_BROADCAST	= 3,	/*  */
}RtpSenderType_e;

typedef enum PacketType
{
	PACKET_TYPE_RAW 	= 0,	/* 8,TCP */
	PACKET_TYPE_RTP,			/* RTP12 */
	PACKET_TYPE_RTP_STAP,		/* STAP-A315 */
	PACKET_TYPE_BUTT,
}PacketType_e;

typedef enum AVType
{
	AV_TYPE_VIDEO 		= 0,
	AV_TYPE_AUDIO,
	AV_TYPE_AV,
	AV_TYPE_BUTT,
}AVType_e;

typedef enum TcpSyncState
{
	SYNC_WAIT = 2,
	SYNC_OK,
}TcpSyncStat_e;


typedef struct RtpStat_s
{
	u64	sendPkt;		/*  */
	u64	sendByte;		/*  */
	u64	sendErr;		/*  */
	u64	recvPkt;		/*  */
	u64	recvByte;		/*  */
	u64	unavailable;	/*  */
	u64	bad;			/* packets that did not appear to be RTP */
	u64	discarded;		/*  */
	u64	timeoutCnt;		/*  */
}RtpStat_t;


typedef struct RtpUdpSender_s
{
	int 			maxTargets;
	int 			targetCnt;
	RtpTargetHost_t		targetHost;
	RtpPt_e			pt;
	u32			ssrc;
	u32			lastSn;
	u32			resv;
	u32			lastTs;
	RtpStat_t		stats;
	int			bActive;
	int			rtpPort;
	int			rtcpPort;
	int			channel;
	int			rtpSockFd;
	RtpSenderType_e		senderType;
	RtpTransportType_e	transportType;
	PacketType_e		pktType;
	AVType_e		avType;
	u8			sendBuf[MAX_RTP_LEN];
	u32			sendLen;
}RtpUdpSender_t;

#define QUANT_TABLE_LEN		64
#define MJPEG_RTP_LEN		888//(892-4)   //ljh 4

typedef struct JpegHdr_s
{
	u32			tspec:8;		/*  */
	u32			off:24;			/*  */
	u8			type;			/*  */
	u8			q;			/* Q */
	u8			width;			/*  */
	u8			height;			/*  */
}JpegHdr_t;

typedef struct JpegHdrQTable_s
{
	u8		mbz;
	u8		precision;
	u8		length[2];
}JpegHdrQTable_t;

typedef struct QTable_s
{
	int	len;
	char 	*src;
}QTalbe_t;

typedef struct JpegQTable_s
{
	QTalbe_t	table[4];
	char        	*scan_data;         /* JPEG */
	int         	scan_data_len;      /* JPEG */
	u8          	interval[2];        /*  */
	u8          	unused[2];
}JpegQTable_t;

typedef struct JpegRestart_s{
	u8			interval[2];        /*  */
	u16			f:1;                /* F */
	u16			l:1;                /* I */
	u16			count:14;           /*  */
}JpegRestart_t;



int  RTP_DelUdpSender(char *ip, int port, RtpUdpSender_t *pSender);
RtpTargetHost_t *RTP_AddUdpSender(char *ip, int port, RtpUdpSender_t *pSender);
int  RTP_CreateUdpSender(RtpUdpSender_t **ppRtpUdpSender, int channel, int	maxTarget, RtpPt_e pt);
void RTP_UdpSendVideoPacket(int chn, u32 pts, int market, char *data, int len);
void RTP_UdpSendAudioPacket(int chn, u32 pts, int market, int len ,char *data );
int RTP_TcpStartSendMediaData(RtspSession_t *pSess);
int RTP_CheckBase64Stat(int chn);
int RTP_Base64Init(int chn);
void RTP_UdpSendMediaPkt(int type, int chn, u32 pts, int frameType, time_t wTime, int len, char *data);

int ES_TS_Initialize();
int ES_TS_RTP_UdpSend(int type, int chn, u32 pts, int frameType, time_t wTime, int len, char *data);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __GS_RTP_H__ */

