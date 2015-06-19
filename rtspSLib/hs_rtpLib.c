/******************************************************************************

 ******************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "hs_rtsp_commonLib.h"
#include "hs_rtsp_base64Lib.h"
#include "hs_typeLib.h"
#include "hs_rtpLib.h"
#include "rtspSLib.h"
//#include "../ringbufLib/rs_ringbufLib.h"
//#include "CSVSDK.h"
//#include "TSmuxLib.h"

#include "global.h"
#include "networkLib.h"
#include "commonLib.h"
#include "streamLib.h"
#include "playbackLib.h"
#include "playbackFunc.h"
#include "diskLib.h"

//#include <avserver_ui.h>

#define MODLE_NAME		"RTP "

Base64Stat_t		gBase64Stat[MAX_ENC_NUM];
//extern RtspSvr_t	gRtspVodSvr[VOD_SVR_TYPE_MAX];
//extern StreamBuf_t gMpeg4Stream[2];

#define __one_copy__

int RTP_Base64Init(int chn)
{
	gBase64Stat[chn].isActive = False;
	gBase64Stat[chn].base64Stat = BASE64_STAT_READING;
	RTSP_Media_Paras_Init(chn);
	return GS_SUCCESS;
}


int RTP_CheckBase64Stat(int chn)
{
	if(gBase64Stat[chn].base64Stat == BASE64_STAT_READED){
		return True;
	}
	return False;
}


int RTP_GetJpegQTable(JpegQTable_t *pQtab, char *data, int len)
{
	int hLen = 0, n =0, i=0, id = 0;
	char *pSrc = data;

	while(pSrc < len+data){

		/* DQT(Difine Quantization Table)
		   0xFFDB+(2)+
		   */
		if(((*pSrc) == 0xff) && ((*(pSrc+1)) == 0xdb)){
			pSrc += 2;
			hLen = 0;
			hLen = (*pSrc)<< 8;	/* Highbyte */
			pSrc ++;
			hLen |= (*pSrc);
			pSrc ++;
			n = (hLen - 2)/(QUANT_TABLE_LEN+1);
			for(i=0; i<n; i++){
				id = *pSrc;
				pSrc ++;
				if(id >= 4){
					dbg(Err, DbgNoPerror, "Get qtable id error \n");
					return GS_FAIL;
				}
				pQtab->table[id].len = QUANT_TABLE_LEN;
				pQtab->table[id].src = pSrc;
				pSrc += QUANT_TABLE_LEN;
			}
		}

		/* DRI--Difine Restart Interval
		   0xFFDD+0x0004()+()
		   */
		if ((*pSrc == 0xFF) &&(*(pSrc+1) == 0xDD)){
			pSrc += 4;
			pQtab->interval[0]= *pSrc;
			pSrc++;
			pQtab->interval[1]= *pSrc;
		}

		/* SOS(Start of Scan)
		   0xFFDA+(2)++JPEG
		   */
		if ((*pSrc == 0xFF) &&(*(pSrc+1) == 0xDA)){
			pSrc += 2;
			hLen = 0;
			hLen =  *pSrc++ << 8;	/* Highbyte */
			hLen |= *pSrc++;		/* Lowbyte */
			pQtab->scan_data = pSrc + hLen -2;
			pQtab->scan_data_len = len - (pQtab->scan_data - data);
			break;
		}

		pSrc ++;
	}
	return GS_SUCCESS;
}


#if 0
int RTP_WaitIFrame(int chn, int nalType, VodSvrType_e svrType)
{
	RtpTargetHost_t		*pVTarget = NULL;
	RtpTargetHost_t		*pATarget = NULL;
	struct list_head 	*pos = NULL, *q = NULL;
	RtpUdpSender_t 		*pSender = VOD_GetSenderPtr(svrType, chn, RTP_STREAM_VIDEO);

	list_for_each_safe(pos, q, &pSender->targetHost.rtpTargetList){
		pVTarget = list_entry(pos, RtpTargetHost_t, rtpTargetList);
		if(pVTarget != NULL){
			if(pVTarget->isActive){
				if(pVTarget->hostState == RTP_TARGETHOST_STATE_REQ_IFRAME){
					if(nalType == NAL_TYPE_SPS || nalType == NAL_TYPE_IDR || nalType == NAL_TYPE_SEI){
						pVTarget->hostState = RTP_TARGETHOST_STATE_SENDING;
						pATarget = ((RtspSession_t *)(pVTarget->pRtspSess))->rtpTargets[RTP_STREAM_AUDIO];
						if(pATarget != NULL){
							pATarget->hostState = RTP_TARGETHOST_STATE_SENDING;
							dbg(Dbg, DbgNoPerror, "chn: %d, client: %s:%d"
									" start sending audio data \n", chn,
									pATarget->remoteIpAddr,
									pATarget->remotePort);
						}
					}
				}
			}
		}
	}
	return GS_SUCCESS;
}
#endif


int RTP_OpenUdpSocket(int port)
{
	int	sockFd;
	int	socketOptVal = 1;
	struct sockaddr_in addr;

	sockFd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockFd <= 0){
		dbg(Err, DbgPerror, "socket error \n");
		return GS_FAIL;
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	memset(&(addr.sin_zero), '\0', 8);
	if (-1 == setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR,
				&socketOptVal, sizeof(int))) {
		dbg(Err, DbgPerror,"setsockopt failed!\n");
		sock_close(sockFd);
		return GS_FAIL;
	}
	if (bind (sockFd, (struct sockaddr *)&addr, sizeof (addr)))
	{
		sock_close(sockFd);
		dbg(Err, DbgPerror, "UDP Socket Open error.\n");
		return GS_FAIL;
	}
	return sockFd;
}

int	RTP_CreateUdpSender(RtpUdpSender_t **ppRtpUdpSender, int channel, int	maxTarget, RtpPt_e pt)
{
	int sockFd ;
	int	socketOptVal = 1;
	static int	sPort = DEFAULT_RTP_UDP_PORT;
	RtpUdpSender_t *pSender = NULL;

	*ppRtpUdpSender = NULL;
	pSender = (RtpUdpSender_t *)malloc(sizeof(RtpUdpSender_t));
	if(pSender == NULL){
		dbg(Err, DbgPerror, "malloc psender error \n");
		return GS_FAIL;
	}
	memset(pSender, 0, sizeof(RtpUdpSender_t));
	INIT_LIST_HEAD(&(pSender->targetHost.rtpTargetList));

	sockFd = RTP_OpenUdpSocket(sPort + 2);
	if(sockFd < 0){
		dbg(Err, DbgNoPerror, "RTP_OpenUdpSocket error \n");
		free(pSender);
		pSender = NULL;
		return GS_FAIL;
	}
	if (-1 == setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &socketOptVal, sizeof(int))) {
		dbg(Err, DbgPerror, "setsockopt error \n");
		free(pSender);
		pSender = NULL;
		return GS_FAIL;
	}

	pSender->bActive 	= False;
	pSender->lastTs  	= 3600;
	pSender->pt  	 	= pt;
	/* fixed by ljh */
	if((RTP_PT_H264 == pt)||(RTP_PT_JPEG == pt)||(RTP_PT_MPEG4 == pt)){
		pSender->ssrc = RTP_DEFAULT_SSRC + channel;
		pSender->lastSn = 3600;
	}else{
		pSender->ssrc = RTP_DEFAULT_SSRC + channel + 128;
		pSender->lastSn = 1200;
	}
	pSender->rtpPort = sPort + 2;
	pSender->rtcpPort = 0;
	pSender->rtpSockFd = sockFd;
	pSender->channel = channel;
	pSender->pktType = PACKET_TYPE_RAW;
	if(0 == maxTarget){
		maxTarget = RTP_MAX_SENDER;
	}
	pSender->maxTargets = maxTarget;
	*ppRtpUdpSender = pSender;
	sPort += 2;
	return GS_SUCCESS;
}



RtpTargetHost_t *RTP_FindUdpSender(char *ip, int port, RtpUdpSender_t *pSender)
{
	struct list_head *pos;
	RtpTargetHost_t *pTarget = NULL;

	list_for_each(pos, &(pSender->targetHost.rtpTargetList)){
		pTarget = list_entry(pos, RtpTargetHost_t, rtpTargetList);
		if(pTarget != NULL){
			if((0 == strcmp(ip, pTarget->remoteIpAddr)) &&
					(port == pTarget->remotePort)){
				return pTarget;
			}
		}
	}
	return NULL;
}

int RTP_DelUdpSender(char *ip, int port, RtpUdpSender_t *pSender)
{
	RtpTargetHost_t *pTarget = NULL;

	pTarget = RTP_FindUdpSender(ip, port, pSender);
	if(pTarget != NULL){
		pTarget->isActive = False;
		list_del(&(pTarget->rtpTargetList));
		free(pTarget);
		pTarget = NULL;
		pSender->targetCnt --;
	}
	return GS_SUCCESS;
}

RtpTargetHost_t *RTP_AddUdpSender(char *ip, int port, RtpUdpSender_t *pSender)
{
	RtpTargetHost_t *pTarget = NULL;

	if(pSender == NULL){
		return NULL;
	}

	if(pSender->targetCnt >= pSender->maxTargets){
		dbg(Err, DbgNoPerror, "rtp chn = %d reached the max sender "
				"target host: %d\n", pSender->channel, pSender->maxTargets);
		return NULL;
	}

	pTarget = (RtpTargetHost_t *)malloc(sizeof(RtpTargetHost_t));
	if(pTarget == NULL){
		dbg(Err, DbgPerror, "malloc targethost error \n");
		return NULL;
	}
	memset(pTarget, 0, sizeof(RtpTargetHost_t));
	strcpy(pTarget->remoteIpAddr, ip);
	pTarget->remotePort = port;

	memset(&pTarget->remoteAddr, 0, sizeof(struct sockaddr_in));
	pTarget->remoteAddr.sin_family 		= AF_INET;
	pTarget->remoteAddr.sin_port 		= htons(pTarget->remotePort);
	pTarget->remoteAddr.sin_addr.s_addr 	= inet_addr(pTarget->remoteIpAddr);

	list_add(&pTarget->rtpTargetList, &pSender->targetHost.rtpTargetList);
	pSender->targetCnt ++;
	pTarget->hostState = RTP_TARGETHOST_STATE_REQ_IFRAME;
	pTarget->isActive = True;

	return pTarget;
}

int RTP_UdpPacket(RtpUdpSender_t *pSender, u32 pts, int marker,
		int len, char *data)
{
	RtpHdr_t *pRtpHdr = NULL;

	pSender->sendLen = 0;
	pRtpHdr = (RtpHdr_t *)pSender->sendBuf;

	RTP_HDR_SET_VERSION(pRtpHdr, RTP_VERSION);
	RTP_HDR_SET_P(pRtpHdr, 0);
	RTP_HDR_SET_X(pRtpHdr, 0);
	RTP_HDR_SET_CC(pRtpHdr, 0);

	RTP_HDR_SET_M(pRtpHdr, marker);
	RTP_HDR_SET_PT(pRtpHdr, pSender->pt);

	RTP_HDR_SET_SEQNO(pRtpHdr, htons(pSender->lastSn));

	RTP_HDR_SET_TS(pRtpHdr, htonl(pts));

	RTP_HDR_SET_SSRC(pRtpHdr, htonl(pSender->ssrc));

	pSender->lastSn ++;
	pSender->lastTs = pts;

	memcpy(pSender->sendBuf+ RTP_HDR_LEN, data, len);
	pSender->sendLen = RTP_HDR_LEN + len;

	return GS_SUCCESS;
}

int RTP_UdpSend(RtpUdpSender_t  *pSender)
{
	int ret;
	struct list_head *pos, *q;
	RtpTargetHost_t *pHost = NULL;

	list_for_each_safe(pos, q, &(pSender->targetHost.rtpTargetList) )
	{
		pHost= list_entry(pos, RtpTargetHost_t , rtpTargetList);
		if(True == RTSP_GetUdpSendStat(pSender->channel)){
			if ((pHost != NULL) && (pHost->isActive == True) && (pSender->sendLen > 0)
					&& ((pHost->hostState == RTP_TARGETHOST_STATE_SENDING)||(RTP_PT_H264 != pSender->pt)))
			{
				//printf("udp send ......\n");
				ret = sendto(pSender->rtpSockFd , pSender->sendBuf ,
						pSender->sendLen, 0, (struct sockaddr*)&pHost->remoteAddr,
						sizeof(struct sockaddr));

				if (ret != pSender->sendLen)
				{
					pSender->stats.sendErr ++;
				}
				else
				{
					pSender->stats.sendByte += pSender->sendLen;
					pSender->stats.sendPkt ++;
				}
			}
		}
	}
	return GS_SUCCESS;

}

#if 0
void RTP_UdpSendVideoPacket(int chn, u32 pts, int market,
		char *data, int len)
{
	RtpUdpSender_t *pSender = NULL;

	pSender = VOD_GetSenderPtr(VOD_SVR_TYPE_RTSP, chn, RTP_STREAM_VIDEO);
	if(pSender != NULL){
		RTP_UdpPacket(pSender, pts, market, len, data);
		RTP_UdpSend(pSender);
	}
}
#endif

#if 0
void RTP_UdpSendAudioPacket(int chn, u32 pts, int market,
		int len ,char *data )
{
	RtpUdpSender_t *pSender = NULL;
#if 1
	//SendAudioStreamToCSV(chn, len, (unsigned char *)data);
	/* DISABLE */
	//if( AUDIO_DISABLE != CFG_GetAudioEncType() ){
	pSender = VOD_GetSenderPtr(VOD_SVR_TYPE_RTSP, chn, RTP_STREAM_AUDIO);
	if(pSender != NULL){
		RTP_UdpPacket(pSender, pts, market, len, data);
		RTP_UdpSend(pSender);
	}
	//}
#endif
}
#endif

#if 0
int firstFrame = 0;
int RTP_UdpSendH264Pkt(int chn, u32 pts, int frameType, time_t wTime,int len, char *data)
{
	char *pvide_data = NULL;
	char fua_buf[MAX_RTP_LEN];
	u8 nal_type;
	u8 s_token;
	int data_len, left_len, pos, iFlag;
	int spsLen = 0, ppsLen = 0, seiLen;
	//int productId = CFG_GetProductId();
	int send_len[5] = {0};
	char * psend_data[5] = {NULL};
	u8 send_count = 0;
	u8 uloop=0;
	if(chn != 0 && chn != 1 && chn != 2){
		return 0;
	}
	pvide_data = data + 4;
	data_len   = len - 4;
	if(data == NULL){
		return GS_SUCCESS;
	}

#if 0
	if( productId == ID_GXV3500 || productId == ID_CL3500 ||
			productId == ID_IP5150 ){
		if(len <= 0 || len > ENC_SIZE_SD){
			dbg(Err, DbgNoPerror, "Len = %d \n", len);
			return GS_SUCCESS;
		}
	}else{
		if(len <= 0 || len > ENC_OUTBUFF_SIZE){
			dbg(Err, DbgNoPerror, "Len = %d \n", len);
			return GS_SUCCESS;
		}
	}
#endif

#if 1 //
	if(gBase64Stat[chn].base64Stat == BASE64_STAT_READING){
		nal_type = H264_Get_NalType(*pvide_data);
		if(NAL_TYPE_SPS == nal_type){
			seiLen = RINGBUF_GetSpsPpsSeiLen(&spsLen, &ppsLen, len, data);
			RTSP_Media_Para_SetSeqBase64( chn, pvide_data, spsLen-4);

			pvide_data = data + 4 + spsLen;
			data_len   = len - 4 - spsLen;
			nal_type = H264_Get_NalType(*pvide_data);
			if(nal_type == NAL_TYPE_PPS){
				RTSP_Media_Para_SetPictBase64( chn, pvide_data, ppsLen-4);
			}
#if 0
			if(GS_SUCCESS == RTSP_Media_Para_SetBase64( chn)){
				gBase64Stat[chn].base64Stat = BASE64_STAT_READED;
			}else{
				dbg(Err, DbgNoPerror, "RTSP_Media_Para_SetBase64"" chn = %d error \n", chn);
			}
#endif
			gBase64Stat[chn].base64Stat = BASE64_STAT_READED;
		}
		return GS_SUCCESS;
	}
#endif
	nal_type = H264_Get_NalType(*pvide_data);

	if(frameType == VENC_I_FRAME ){
		iFlag = 1;
	}else{
		iFlag = 0;
	}
	//SendVideoStreamToCSV(chn, len, iFlag, (unsigned char *)data);

	RTP_WaitIFrame(chn, nal_type, VOD_SVR_TYPE_RTSP);
	if (frameType == VENC_I_FRAME)
	{
		send_count = 0;
		pvide_data = data + 4;
		seiLen = RINGBUF_GetSpsPpsSeiLen(&spsLen, &ppsLen, len, data);
		send_len[0] = spsLen - 4;
		psend_data[0] = pvide_data;
		send_count++;
		pvide_data = pvide_data + spsLen;
		send_len[1] = ppsLen - 4;
		psend_data[1] = pvide_data;
		send_count++;
		pvide_data = pvide_data + ppsLen;
		if (seiLen > 0)
		{
			send_len[send_count] = seiLen - 4;
			psend_data[send_count] = pvide_data;
			send_count++;
			pvide_data = pvide_data + seiLen;
			send_len[send_count] = len - spsLen - ppsLen - seiLen - 4;
		}
		else
		{
			send_len[send_count] = len - spsLen - ppsLen - 4;
		}
		psend_data[send_count] = pvide_data;
		send_count++;
	}
	else
	{
		send_count = 1;
		psend_data[0] = data + 4;
		send_len[0] = len - 4;
	}
	for(uloop=0; uloop<send_count; uloop++)
	{
		/* 1024 */
		if(send_len[uloop] <= NAL_FRAGMENTATION_SIZE){
			RTP_UdpSendVideoPacket(chn, pts, 1, psend_data[uloop], send_len[uloop]);
		}else{
			/* 1024 */
			nal_type = H264_Get_NalType(*psend_data[uloop]);
			//printf("nal_type = %d \n", nal_type);
			/*RFC3984  FU-ARTP*/
			/*+---------------+
			  |0|1|2|3|4|5|6|7|
			  +-+-+-+-+-+-+-+-+
			  |F|NRI|Type = 1c|
			  +---------------+*/
			fua_buf[0] = 0x1c | (*psend_data[uloop] & ~0x1F); /*fua_buf[0] filled the FU indicator*/
			s_token = 1;
			left_len = send_len[uloop];
			pos = 0;
			while(left_len > NAL_FRAGMENTATION_SIZE)
			{
				/*+---------------+
				  |0|1|2|3|4|5|6|7|
				  +-+-+-+-+-+-+-+-+
				  |S|E|R| NalType |
				  +---------------+*/
				fua_buf[1] = (s_token<<7) | nal_type; /*fua_buf[0] filled the FU header*/
				/**/
				memcpy(fua_buf+2, psend_data[uloop] + pos + s_token, NAL_FRAGMENTATION_SIZE-s_token);
				RTP_UdpSendVideoPacket(chn, pts, 0, fua_buf, NAL_FRAGMENTATION_SIZE+2-s_token);
				s_token = 0;
				left_len  -= NAL_FRAGMENTATION_SIZE;
				pos  += NAL_FRAGMENTATION_SIZE;
			}
			if (s_token)
			{
				nal_type |= 128;
			}
			fua_buf[1] = 64 | nal_type; /*fua_buf[0] filled the FU header*/
			memcpy(fua_buf+2, psend_data[uloop] + pos + s_token, left_len-s_token);
			RTP_UdpSendVideoPacket(chn, pts, 1, fua_buf, left_len+2-s_token);
		}
	}
	return GS_SUCCESS;
}
#endif

#if 0
int RTP_UdpSendH264Pkt(int chn, u32 pts, int frameType, time_t wTime,
		int len, char *data)
{
	char *pvide_data = NULL;
	char fua_buf[MAX_RTP_LEN];
	u8 nal_type;
	u8 s_token;
	int data_len, left_len, pos;
	int	spsLen = 0, ppsLen = 0, sei;
	int	productId = CFG_GetProductId();

	if(chn != 0 && chn != 1){
		return 0;
	}

	pvide_data = data + 4;
	data_len   = len - 4;

	if(data == NULL){
		return GS_SUCCESS;
	}

	if( productId == ID_GXV3500 || productId == ID_CL3500 ||
			productId == ID_IP5150 ){
		if(len <= 0 || len > ENC_SIZE_SD){
			dbg(Err, DbgNoPerror, "Len = %d \n", len);
			return GS_SUCCESS;
		}

	}else{
		if(len <= 0 || len > ENC_OUTBUFF_SIZE){
			dbg(Err, DbgNoPerror, "Len = %d \n", len);
			return GS_SUCCESS;
		}
	}
#if 1	//
	if(gBase64Stat[chn].base64Stat == BASE64_STAT_READING){
		nal_type = H264_Get_NalType(*pvide_data);
		if(NAL_TYPE_SPS == nal_type){
			sei = RINGBUF_GetSpsPpsSeiLen(&spsLen, &ppsLen, len, data);
			RTSP_Media_Para_SetSeqBase64( chn, pvide_data, spsLen-4);
			pvide_data = data + 4 + spsLen;
			data_len   = len - 4 - spsLen;
			nal_type = H264_Get_NalType(*pvide_data);
			if(nal_type == NAL_TYPE_PPS){
				RTSP_Media_Para_SetPictBase64( chn, pvide_data, ppsLen-4);
			}
			if(GS_SUCCESS == RTSP_Media_Para_SetBase64( chn)){
				gBase64Stat[chn].base64Stat = BASE64_STAT_READED;
			}else{
				dbg(Err, DbgNoPerror, "RTSP_Media_Para_SetBase64"
						" chn = %d error \n", chn);
			}
		}
		return GS_SUCCESS;
	}
#endif
	nal_type = H264_Get_NalType(*pvide_data);
	RTP_WaitIFrame(chn, nal_type, VOD_SVR_TYPE_RTSP);

	pvide_data = data + 4;
	data_len   = len - 4;

	/* 1024 */
	if(data_len <= NAL_FRAGMENTATION_SIZE){
		RTP_UdpSendVideoPacket(chn, pts, 1, pvide_data, data_len);
	}else{
		/* 1024 */
		nal_type = H264_Get_NalType(*pvide_data);
		//printf("nal_type = %d \n", nal_type);
		/*RFC3984  FU-ARTP*/
		/*+---------------+
		  |0|1|2|3|4|5|6|7|
		  +-+-+-+-+-+-+-+-+
		  |F|NRI|Type = 1c|
		  +---------------+*/
		fua_buf[0] = 0x1c | (*pvide_data & ~0x1F);			/*fua_buf[0] filled the FU indicator*/
		s_token = 1;
		left_len = data_len;
		pos = 0;

		while(left_len > NAL_FRAGMENTATION_SIZE)
		{
			/*+---------------+
			  |0|1|2|3|4|5|6|7|
			  +-+-+-+-+-+-+-+-+
			  |S|E|R| NalType |
			  +---------------+*/
			fua_buf[1] = (s_token<<7) | nal_type;			/*fua_buf[0] filled the FU header*/

			/**/
			memcpy(fua_buf+2, pvide_data + pos + s_token, NAL_FRAGMENTATION_SIZE-s_token);
			RTP_UdpSendVideoPacket(chn, pts, 0, fua_buf, NAL_FRAGMENTATION_SIZE+2-s_token);
			s_token = 0;
			left_len 	-= NAL_FRAGMENTATION_SIZE;
			pos 		+= NAL_FRAGMENTATION_SIZE;
		}

		if (s_token)
		{
			nal_type |= 128;
		}
		fua_buf[1] = 64 | nal_type;							/*fua_buf[0] filled the FU header*/
		memcpy(fua_buf+2, pvide_data + pos + s_token, left_len-s_token);
		RTP_UdpSendVideoPacket(chn, pts, 1, fua_buf, left_len+2-s_token);
	}
	return GS_SUCCESS;
}
#endif



void RTP_UdpSendVideo(int type, int chn, u32 pts, int frameType,
		time_t wTime, int len, char *data)
{
	switch(type){
		case VENC_TYPE_H264:
			//RTP_UdpSendH264Pkt(chn, pts, frameType, wTime, len, data);
			break;

		case VENC_TYPE_JPEG:
			//RTP_UdpSendMJpegPkt(chn, pts, frameType, wTime, len, data);
			break;

		case VENC_TYPE_MPEG4:
			//RTP_UdpSendMpeg4Pkt(chn, pts, frameType, wTime, len, data);
			break;

		default:
			dbg(Err, DbgNoPerror, "venc type[%d] error  !!!\n", type);
			break;
	}
}

void RTP_UdpSendMediaPkt(int type, int chn, u32 pts, int frameType,
		time_t wTime, int len, char *data)
{
	if(frameType == AUDIO_TYPE){
		//RTP_UdpSendAudioPacket(chn, pts, 1, len, data );
	}else{
		//RTP_UdpSendVideo(type, chn, pts, frameType, wTime, len, data);
	}
}

int RTP_TcpPacket_Jpeg(RtpTcpSender_t *pSender, u8 *pSendbuff, RtpPt_e payloadType, u32 pts, u8 marker)
{
	RtpHdr_t *pRtpHdr = NULL;
	//u8 *psendbuf = NULL;
	pRtpHdr = (RtpHdr_t *)(pSendbuff);

	RTP_HDR_SET_VERSION(pRtpHdr, RTP_VERSION);
	RTP_HDR_SET_P(pRtpHdr, 0);
	RTP_HDR_SET_X(pRtpHdr, 0);
	RTP_HDR_SET_CC(pRtpHdr, 0);
	RTP_HDR_SET_M(pRtpHdr, marker);
	RTP_HDR_SET_PT(pRtpHdr, payloadType);
	RTP_HDR_SET_SEQNO(pRtpHdr, htons(pSender->lastSn));
	pSender->lastSn ++;
	RTP_HDR_SET_TS(pRtpHdr, htonl(pts));
	RTP_HDR_SET_SSRC(pRtpHdr,htonl(pSender->videoH264Ssrc));
	pSender->lastTs = pts;

	return GS_SUCCESS;

}


int RTP_TcpPacket(RtpTcpSender_t *pSender, RtpPt_e payloadType, u32 pts, int marker, int len, char *data)
{
	RtpHdr_t *pRtpHdr = NULL;
	unsigned short *intlvd_ch = (unsigned short *)(pSender->sendBuf+2);

	pSender->sendLen = 0;
	pRtpHdr = (RtpHdr_t *)(pSender->sendBuf + 4);
	RTP_HDR_SET_VERSION(pRtpHdr, RTP_VERSION);
	RTP_HDR_SET_P(pRtpHdr, 0);
	RTP_HDR_SET_X(pRtpHdr, 0);
	RTP_HDR_SET_CC(pRtpHdr, 0);
	RTP_HDR_SET_M(pRtpHdr, marker);
	RTP_HDR_SET_PT(pRtpHdr, payloadType);
	if( payloadType == RTP_PT_ALAW || RTP_PT_ULAW == payloadType || RTP_PT_G726 == payloadType)
	{
		RTP_HDR_SET_SEQNO(pRtpHdr, htons(pSender->AudioSeq));
		RTP_HDR_SET_SSRC(pRtpHdr, htonl(pSender->audioG711Ssrc));
		pSender->AudioSeq ++;
	}
	else
	{
		RTP_HDR_SET_SEQNO(pRtpHdr, htons(pSender->lastSn));
		RTP_HDR_SET_SSRC(pRtpHdr, htonl(pSender->videoH264Ssrc));
		pSender->lastSn ++;
	}
	RTP_HDR_SET_TS(pRtpHdr, htonl(pts));
	(payloadType == RTP_PT_G726 )? RTP_HDR_SET_SSRC(pRtpHdr, htonl(pSender->audioG711Ssrc)):RTP_HDR_SET_SSRC(pRtpHdr, htonl(pSender->videoH264Ssrc));


	pSender->lastTs = pts;
#ifndef __one_copy__
	memcpy(pSender->sendBuf + RTP_HDR_LEN + 4 , data, len);
#endif 
	pSender->sendLen = RTP_HDR_LEN + len;

	pSender->sendBuf[0] = '$';
	if((payloadType == RTP_PT_H264) || (payloadType == RTP_PT_MPEG4) || (payloadType == RTP_PT_JPEG))
	{//fixed by xsf
		pSender->sendBuf[1] = pSender->interleaved[RTP_STREAM_VIDEO].rtp;
	}
	else
	{
		pSender->sendBuf[1] = pSender->interleaved[RTP_STREAM_AUDIO].rtp;
	}
	*intlvd_ch = htons((unsigned short) pSender->sendLen);

	return GS_SUCCESS;
}

int RTP_TcpSend(RtpTcpSender_t *pSender, int type)
{
	int 	ret = 0;

	/* DISABLE  */
	if( type == RTP_STREAM_AUDIO )
	{
		//if( AUDIO_DISABLE == CFG_GetAudioEncType() ){
		//	return GS_SUCCESS;
		//}
	}

	if(pSender->tcpSockFd > 0)
	{
		ret = Net_TcpSendN(pSender->tcpSockFd, (char *)pSender->sendBuf, pSender->sendLen +4);
		/*sendBuf[0] = '$'
		  sendBuf[1] = interleaved
		  sendBuf[2~3] = sendLen
		  sendBuf[4~] = data  */
		if(ret == pSender->sendLen + 4 )
		{
			return GS_SUCCESS;
		}
	}
	dbg(Err, DbgPerror, "send %d, sendLen = %d , sockfd = %d\n", ret, pSender->sendLen, pSender->tcpSockFd);
	return GS_FAIL;
}

#if 1
void *RTP_TcpSendH264ThrFxn(void *pArgs)
{
	int audioEncType ;
	int nalType, pos, leftLen, dataLen;
	int pktLen = 0;//,syncStat = SYNC_WAIT;
	u32  index, pts;
	u8  sToken;
//	char sBuf[MAX_RTP_LEN];
	struct timeval timeout;
	char	*pBuf = NULL; 
	char 	*pData = NULL;
	char 	*pSend = NULL;
	RtspSession_t *pSess = (RtspSession_t *)pArgs;
//	struct timeval tv;
	unsigned int audio_pts = 0;


	//int rtspfd = open("/opt/ipnc/rtsp2.h264",O_CREAT |O_RDWR | O_APPEND);	// lyh

	if(pSess == NULL ){
		dbg(Err, DbgNoPerror, "Input params error \n");
		goto __RtpTcpThrFxnExit;
	}

	if(pSess->pRtpTcpSender == NULL){
		dbg(Dbg, DbgNoPerror, "tcpSender is NULL \n");
		goto __RtpTcpThrFxnExit;
	}
#if 0
	type = CFG_GetAudioEncType();
	if( type == AUDIO_ENC_FMT_ALAW){
		audioEncType = RTP_PT_ALAW;
	}else if (type == AUDIO_ENC_FMT_ULAW){
		audioEncType = RTP_PT_ULAW;
	}else{
		audioEncType = -1;
	}
#endif

	int channelNo = pSess->channelNo;
	int streamNo = pSess->streamNo;
	stream_frame_queue_t *pFrameQueue = NULL;
	stream_frame_t * pFrame = NULL;
	int readBegin = -1;
	int readEnd = -1;
	stream_frame_head_t frameHead;
//	int channel = 0;
	int waitIFrame = 0;
	int frameNo = 0;
	int noFrame = 0;
	int sendSize =128*1024; //512 * 1024;
	int recvSize = 8*1024;
	//int frameType =0;
	//#define H264_Get_NalType(c)  ( (c) & 0x1F )  
	//frameType=H264_Get_NalType(*(pFrame->pFrameBuf+4));

	memset(&frameHead, 0, sizeof(stream_frame_head_t));

	audioEncType = RTP_PT_ULAW; //RTP_PT_ULAW;

	timeout.tv_sec  = 3;
	timeout.tv_usec = 0;

	setsockopt(pSess->rtspSockFd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
	setsockopt(pSess->rtspSockFd, SOL_SOCKET, SO_SNDBUF, (char *)&sendSize, sizeof(sendSize));
	setsockopt(pSess->rtspSockFd, SOL_SOCKET, SO_RCVBUF, (char *)&recvSize, sizeof(recvSize));
	index = 0;

	__printf("start send data\n");
	while(RTSP_STATE_PLAY == pSess->sessStat)
	{
//		__printf("send status\n");
		pFrameQueue = (stream_frame_queue_t *)networkGetStreamFrameQueue(channelNo, streamNo);
		pFrame = streamGetFrameFromNetPool(pFrameQueue, &readBegin, &readEnd);
		if(pFrame == NULL)
		{
			usleep(10 * 1000);
			if(noFrame >= 100)
			{
				goto __RtpTcpThrFxnExit;
			}
			noFrame++;
			continue;
		}
		noFrame = 0;

		if(waitIFrame == 0)
		{
			if((pFrame->pFrameBuf[4] & 0x1F) == NAL_TYPE_SPS)
			{
				waitIFrame = 1;
				frameNo = pFrame->frameHead.frameNo;
			}
			else
			{
				streamFreeFrameBuffer(pFrameQueue, pFrame);
				continue;
			}
		}
		else
		{
			if(((frameNo + 1)&0xFFFF) != pFrame->frameHead.frameNo)
			{//I
				Printf("dev %d, channel %d, lost frame %d ~ %d\r\n", channelNo, streamNo, frameNo, pFrame->frameHead.frameNo);
				waitIFrame = 0;
				streamFreeFrameBuffer(pFrameQueue, pFrame);
				continue;
			}
		}
		frameNo = pFrame->frameHead.frameNo;

		pBuf 	= (char *)(pSess->pRtpTcpSender->sendBuf + RTP_HDR_LEN + 4); 
#if 0
		pts 	= pFrame->frameHead.pts;
#else
//		gettimeofday(&tv,NULL);
//		pts = (unsigned int)(((tv.tv_sec * 1000) + (tv.tv_usec /1000))*90);
		pts = (unsigned int)(((pFrame->frameHead.tv.tv_sec * 1000) + (pFrame->frameHead.tv.tv_usec /1000))*90);
		audio_pts = pts*8/90;
#endif
		//__printf("pts=%u\n", pts);

		if(pFrame->frameHead.type == FRAME_TYPE_AUDIO)
		{//
			if(pSess->reqStreamFlag[RTP_STREAM_AUDIO])
			{
				if(audioEncType != -1)
				{	//* lwx, 2010.11.19
					memcpy(pBuf, pFrame->pFrameBuf, pFrame->frameHead.len);
					//__printf("send audio frame, pktlen=%d\n", pktLen);
					//RTP_TcpPacket(pSess->pRtpTcpSender, audioEncType, pts, 1, pktLen, pData);
					RTP_TcpPacket(pSess->pRtpTcpSender, audioEncType, audio_pts, 1, pktLen, pData);
					if(GS_SUCCESS != RTP_TcpSend(pSess->pRtpTcpSender, RTP_STREAM_AUDIO))
					{
						dbg(Err, DbgNoPerror, "RTP_TcpSend error, sockfd = %d\n",pSess->rtspSockFd);
						streamFreeFrameBuffer(pFrameQueue, pFrame);
						goto __RtpTcpThrFxnExit;
					}
				}
			}
		}
		else
		{
			if(pSess->reqStreamFlag[RTP_STREAM_VIDEO])
			{
				pSend  	= pFrame->pFrameBuf + H264_STARTCODE_LEN;
				dataLen = pFrame->frameHead.len - H264_STARTCODE_LEN;
				nalType = H264_Get_NalType(*(pFrame->pFrameBuf+4));
				/* 1024 */
				if(dataLen <= NAL_FRAGMENTATION_SIZE)
				{
					memcpy(pBuf, pFrame->pFrameBuf+4, pFrame->frameHead.len-4);
					RTP_TcpPacket(pSess->pRtpTcpSender, RTP_PT_H264, pts, 1, dataLen, pSend);
					if(GS_SUCCESS != RTP_TcpSend(pSess->pRtpTcpSender, RTP_STREAM_VIDEO))
					{
						dbg(Err, DbgNoPerror, "RTP_TcpSend error, sockfd = %d\n", pSess->rtspSockFd);
						streamFreeFrameBuffer(pFrameQueue, pFrame);
						goto __RtpTcpThrFxnExit;
					}
				}
				else
				{
					/* 1024 FU-A  */
					pBuf[0] = 0x1c | (*(pFrame->pFrameBuf+4) & (~0x1f));
					leftLen = dataLen;
					sToken  = 1;
					pos     = 0;
					while(leftLen > NAL_FRAGMENTATION_SIZE)
					{
//		printf("0cccccccccccccccccccccccccccccccccccc leftLen = %d, pts = %lu\r\n", leftLen, pts);
						if(RTSP_STATE_PLAY != pSess->sessStat)
						{
							dbg(Err, DbgNoPerror, "pSess stat is not play \n");
							streamFreeFrameBuffer(pFrameQueue, pFrame);
							goto __RtpTcpThrFxnExit;
						}
						pBuf[1] = (sToken << 7) | nalType;
						memcpy(pBuf+2, pFrame->pFrameBuf + 4 + pos + sToken, NAL_FRAGMENTATION_SIZE - sToken);
						RTP_TcpPacket(pSess->pRtpTcpSender, RTP_PT_H264, pts, 0, NAL_FRAGMENTATION_SIZE + 2 - sToken, pBuf);
						if(GS_SUCCESS != RTP_TcpSend(pSess->pRtpTcpSender, RTP_STREAM_VIDEO))
						{
							dbg(Err, DbgNoPerror, "RTP_TcpSend error, sockfd = %d\n", pSess->rtspSockFd);
							streamFreeFrameBuffer(pFrameQueue, pFrame);
							goto __RtpTcpThrFxnExit;
						}
						sToken 	= 0;
						leftLen -= NAL_FRAGMENTATION_SIZE;
						pos 	+= NAL_FRAGMENTATION_SIZE;
					}
					if(sToken)
					{
						nalType |= 128;
					}
					pBuf[1] = 64 | nalType;
					memcpy(pBuf+2, pFrame->pFrameBuf+4+pos+sToken, leftLen-sToken);
					RTP_TcpPacket(pSess->pRtpTcpSender, RTP_PT_H264, pts, 1, leftLen+2-sToken, pBuf );
					if(GS_SUCCESS != RTP_TcpSend(pSess->pRtpTcpSender,RTP_STREAM_VIDEO))
					{
						dbg(Err, DbgNoPerror, "RTP_TcpSend error, sockfd = %d\n",pSess->rtspSockFd);
						streamFreeFrameBuffer(pFrameQueue, pFrame);
						goto __RtpTcpThrFxnExit;
					}
				}
			}
		}
		streamFreeFrameBuffer(pFrameQueue, pFrame);
	}

__RtpTcpThrFxnExit:
	__printf("[rtp] tcp send h264 thread exit , sock = %d ... \n", pSess->rtspSockFd);
	pSess->sessStat = RTSP_STATE_STOP;

	dbg(Dbg, DbgNoPerror, "__RtpTcpThrFxnExit !!! \n");
	__printf("[rtp] tcp send h264 thread ok \n");
	return NULL;
}

#endif

#if  0

#define MICROSEC   0.000001

int RTP_TcpSend2(RtpTcpSender_t *pSender, char* AVData, int AVDataLen, int type)
{
	int 	ret = 0;
	if( type == RTP_STREAM_AUDIO )
	{
		//if( AUDIO_DISABLE == CFG_GetAudioEncType() ){
		//	return GS_SUCCESS;
		//}
	}

	if(pSender == NULL)
	{
		return GS_FAIL;
	}

	if(pSender->tcpSockFd > 0)
	{
		ret = Net_TcpSendN(pSender->tcpSockFd, (char *)AVData, AVDataLen);

		if(ret == AVDataLen)
		{
			return GS_SUCCESS;
		}
	}

#if 0
	time_t ttt = time(NULL);		
	struct tm *ptm = localtime(&ttt);			
	fprintf(stderr, "time %04d-%02d-%02dT%02d:%02d:%02dZ [File: %s, %s, Line %d] : send %d sendLen = %d , sockfd = %d errno %d %s\n", 
		ptm->tm_year+1900, ptm->tm_mon +1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, 
		__FILE__, __FUNCTION__, __LINE__,  ret, pSender->sendLen, pSender->tcpSockFd, errno, strerror(errno)); 
	perror(" send ");
#endif

	return GS_FAIL;
}

void *RTP_TcpSendH264ThrFxn(void *pArgs)
{
	int audioEncType ;
	int nalType, pos, leftLen, dataLen;
	int pktLen = 0;//,syncStat = SYNC_WAIT;
	u32  index, pts;
	u8  sToken;
//	char sBuf[MAX_RTP_LEN];
	struct timeval timeout;
	char	*pBuf = NULL; 
	char 	*pData = NULL;
	char 	*pSend = NULL;
	RtspSession_t *pSess = (RtspSession_t *)pArgs;
//	struct timeval tv;
	unsigned int audio_pts = 0;

      char sBuf[MAX_RTP_LEN];
	//int rtspfd = open("/opt/ipnc/rtsp2.h264",O_CREAT |O_RDWR | O_APPEND);	// lyh

	if(pSess == NULL ){
		dbg(Err, DbgNoPerror, "Input params error \n");
		goto __RtpTcpThrFxnExit;
	}

	if(pSess->pRtpTcpSender == NULL){
		dbg(Dbg, DbgNoPerror, "tcpSender is NULL \n");
		goto __RtpTcpThrFxnExit;
	}
#if 0
	type = CFG_GetAudioEncType();
	if( type == AUDIO_ENC_FMT_ALAW){
		audioEncType = RTP_PT_ALAW;
	}else if (type == AUDIO_ENC_FMT_ULAW){
		audioEncType = RTP_PT_ULAW;
	}else{
		audioEncType = -1;
	}
#endif

	int channelNo = pSess->channelNo;
	int streamNo = pSess->streamNo;
	stream_frame_queue_t *pFrameQueue = NULL;
	stream_frame_t * pFrame = NULL;
	int readBegin = -2;
	int readEnd = -2;
	stream_frame_head_t frameHead;
//	int channel = 0;
	int waitIFrame = 0;
	int frameNo = 0;
	int nodelay = 0;
	int noFrame = 0;
	int sendSize =64*1024; //512 * 1024;
	int recvSize = 8*1024;
      int data_pos = 0;
	int framerate = 0;
	//int frameType =0;
	//#define H264_Get_NalType(c)  ( (c) & 0x1F )  
	//frameType=H264_Get_NalType(*(pFrame->pFrameBuf+4));

	memset(&frameHead, 0, sizeof(stream_frame_head_t));

	audioEncType = RTP_PT_ULAW; //RTP_PT_ULAW;

	timeout.tv_sec  = 3;
	timeout.tv_usec = 0;

	setsockopt(pSess->rtspSockFd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
	setsockopt(pSess->rtspSockFd, SOL_SOCKET, SO_SNDBUF, (char *)&sendSize, sizeof(sendSize));
	setsockopt(pSess->rtspSockFd, SOL_SOCKET, SO_RCVBUF, (char *)&recvSize, sizeof(recvSize));
	index = 0;

	__printf("start send data\n");
	 struct timeval timestart,timeend;
	 timeend.tv_usec  =  0;
	 timeend.tv_sec = 0;
       // gettimeofday(&time1, NULL);  	
	while(RTSP_STATE_PLAY == pSess->sessStat)
	{
//		__printf("send status\n");
             
		pFrameQueue = (stream_frame_queue_t *)networkGetStreamFrameQueue(channelNo, streamNo);
		pFrame = streamGetFrameFromNetPool(pFrameQueue, &readBegin, &readEnd);
		
		if(pFrame == NULL)
		{
			usleep(3 * 1000);
			if(noFrame >= 100)
			{
				goto __RtpTcpThrFxnExit;
			}
			noFrame++;
			continue;
		}
		noFrame = 0;
              

		if(waitIFrame == 0)
		{
			if((pFrame->pFrameBuf[4] & 0x1F) == NAL_TYPE_SPS)
			{
				waitIFrame = 1;
				frameNo = pFrame->frameHead.frameNo;
			}
			else
			{
				streamFreeFrameBuffer(pFrameQueue, pFrame);
				continue;
			}
		}
		else
		{
		      if((pFrame->pFrameBuf[4] & 0x1F) == NAL_TYPE_SPS ||(pFrame->pFrameBuf[4] & 0x1F) ==NAL_TYPE_PPS)
		      	{
                            nodelay = 1;
			}
			if(((frameNo + 1)&0xFFFF) != pFrame->frameHead.frameNo)
			{//I
			      #if 1
				Printf("dev %d, channel %d, lost frame %d ~ %d\r\n", channelNo, streamNo, frameNo, pFrame->frameHead.frameNo);
				waitIFrame = 0;
				streamFreeFrameBuffer(pFrameQueue, pFrame);
				continue;
				#endif
			}
		}
		frameNo = pFrame->frameHead.frameNo;
              framerate =  pFrame->frameHead.info.video.frameRate;
		pBuf 	= (char *)(pSess->pRtpTcpSender->sendBuf + RTP_HDR_LEN + 4); 
#if 0
		pts 	= pFrame->frameHead.pts;
#else
//		gettimeofday(&tv,NULL);
//		pts = (unsigned int)(((tv.tv_sec * 1000) + (tv.tv_usec /1000))*90);
		pts = (unsigned int)(((pFrame->frameHead.tv.tv_sec * 1000) + (pFrame->frameHead.tv.tv_usec /1000))*90);
		audio_pts = pts*8/90;
#endif
		//__printf("pts=%u\n", pts);

		if(pFrame->frameHead.type == FRAME_TYPE_AUDIO)
		{//
			if(pSess->reqStreamFlag[RTP_STREAM_AUDIO])
			{
				if(audioEncType != -1)
				{	//* lwx, 2010.11.19
					memcpy(pBuf, pFrame->pFrameBuf, pFrame->frameHead.len);
					RTP_TcpPacket(pSess->pRtpTcpSender, audioEncType, audio_pts, 1, pktLen, pData);
					if(GS_SUCCESS != RTP_TcpSend(pSess->pRtpTcpSender, RTP_STREAM_AUDIO))
					{
						dbg(Err, DbgNoPerror, "RTP_TcpSend error, sockfd = %d\n",pSess->rtspSockFd);
						streamFreeFrameBuffer(pFrameQueue, pFrame);
						goto __RtpTcpThrFxnExit;
					}
				}
			}
		}
		else
		{
			if(pSess->reqStreamFlag[RTP_STREAM_VIDEO])
			{
				pSend  	= pFrame->pFrameBuf + H264_STARTCODE_LEN;
				dataLen = pFrame->frameHead.len - H264_STARTCODE_LEN;
				nalType = H264_Get_NalType(*(pFrame->pFrameBuf+4));

			      u8 *SendData = (u8 *)malloc(1024*1024);
				int SendDataPos = 0;
				if(SendData == NULL)
				{
					continue;
				}

                         pSess->pRtpTcpSender->sendBuf = &SendData[SendDataPos];

#ifdef __one_copy__
					pBuf = (char *)(pSess->pRtpTcpSender->sendBuf+RTP_HDR_LEN+4); 
#endif 

				/* 1024 */
				if(dataLen <= NAL_FRAGMENTATION_SIZE)
				{

#ifdef __one_copy__

                            
				  memcpy(pBuf, pSend, dataLen);
#endif 		

                             if(GS_SUCCESS != RTP_TcpPacket(pSess->pRtpTcpSender, RTP_PT_H264, pts, 1, dataLen, pSend))
				      {
							continue;
				      }
					if(GS_SUCCESS != RTP_TcpSend(pSess->pRtpTcpSender, RTP_STREAM_VIDEO))
					{
						dbg(Err, DbgNoPerror, "RTP_TcpSend error, sockfd = %d\n", pSess->rtspSockFd);
						streamFreeFrameBuffer(pFrameQueue, pFrame);
						goto __RtpTcpThrFxnExit;
					}
					streamFreeFrameBuffer(pFrameQueue, pFrame);
 		                      gettimeofday(&timestart, NULL);  	
					 double  timediff	=	( timestart.tv_sec+timestart.tv_usec*MICROSEC) - (timeend.tv_sec+timeend.tv_usec*MICROSEC); 
					
 					timeend.tv_usec  =  timestart.tv_usec;
					timeend.tv_sec = timestart.tv_sec;
					
				      if (0==nodelay)
				     	{
				           if(timediff *1000 < 20)
				          {
                                          usleep((20- timediff *1000)*1000);
					   }    
				     	}
					 else
					 {
                                           nodelay  = 0;
					 }
					
				}
				else
				{
					/* 1024 FU-A  */
					pBuf[0] = 0x1c | (*(pFrame->pFrameBuf+4) & (~0x1f));
					leftLen = dataLen;
					sToken  = 1;
					data_pos     = 0;
					pos     = 0;
					while(leftLen > NAL_FRAGMENTATION_SIZE)
					{
					if(RTSP_STATE_PLAY != pSess->sessStat)
					{
						dbg(Err, DbgNoPerror, "pSess stat is not play \n");
						streamFreeFrameBuffer(pFrameQueue, pFrame);
						goto __RtpTcpThrFxnExit;
					}

                                   unsigned char temp = pBuf[0];
					pSess->pRtpTcpSender->sendBuf = &SendData[SendDataPos];
					pBuf = (char *)(pSess->pRtpTcpSender->sendBuf+RTP_HDR_LEN+4); 
					pBuf[0] = temp;
					pBuf[1] = (sToken << 7) | nalType ;											
					memcpy(pBuf + 2, pSend + data_pos + sToken, NAL_FRAGMENTATION_SIZE - sToken);
							
					RTP_TcpPacket(pSess->pRtpTcpSender, RTP_PT_H264, pts, 0,  NAL_FRAGMENTATION_SIZE + 2 - sToken, sBuf);
						
							
				        //memcpy(SendData + SendDataPos, (char *)pSess->pRtpTcpSender->sendBuf, pSess->pRtpTcpSender->sendLen +4);
					 SendDataPos += pSess->pRtpTcpSender->sendLen +4;

					sToken 	= 0;
					leftLen -= NAL_FRAGMENTATION_SIZE;
					data_pos 	+= NAL_FRAGMENTATION_SIZE;
					}
					
					if(sToken)
					{
						nalType |= 128;
					}


					u8 temp = pBuf[0];
					pSess->pRtpTcpSender->sendBuf = &SendData[SendDataPos];
					pBuf = (char *)(pSess->pRtpTcpSender->sendBuf+RTP_HDR_LEN+4); 
					pBuf[0] = temp;


					pBuf[1] = 64 | nalType ;					

					memcpy(pBuf+2, pSend+data_pos+sToken, leftLen-sToken);

					RTP_TcpPacket(pSess->pRtpTcpSender, RTP_PT_H264, pts, 1, leftLen+2-sToken, sBuf );

	                            
					//memcpy(SendData + SendDataPos, (char *)pSess->pRtpTcpSender->sendBuf, pSess->pRtpTcpSender->sendLen +4);
					SendDataPos += pSess->pRtpTcpSender->sendLen +4;
						
					   
					if(GS_SUCCESS != RTP_TcpSend2(pSess->pRtpTcpSender, SendData, SendDataPos, RTP_STREAM_VIDEO))
					{
						 
							dbg(Err, DbgNoPerror, "RTP_TcpSend error, sockfd = %d\n",pSess->rtspSockFd);
							streamFreeFrameBuffer(pFrameQueue, pFrame);
							goto __RtpTcpThrFxnExit;	
					}
					 streamFreeFrameBuffer(pFrameQueue, pFrame);
					 gettimeofday(&timestart, NULL);  	
					 double  timediff2	=	( timestart.tv_sec+timestart.tv_usec*MICROSEC) - (timeend.tv_sec+timeend.tv_usec*MICROSEC);

					 timeend.tv_usec  =  timestart.tv_usec;
					 timeend.tv_sec = timestart.tv_sec;
					
				       if(timediff2 *1000 < (32))
				       {
				      //  fprintf(stderr,"big frame time diff =%lf  frame =%d \r\n",timediff2 *1000,framerate);
                                      usleep((32 - timediff2 *1000)*1000);
					}
					
 	                  	}
				if(SendData != NULL)
				{
					free(SendData);
					SendData = NULL;
				}

		      }
		}
	}

__RtpTcpThrFxnExit:
	__printf("[rtp] tcp send h264 thread exit , sock = %d ... \n", pSess->rtspSockFd);
	pSess->sessStat = RTSP_STATE_STOP;

	dbg(Dbg, DbgNoPerror, "__RtpTcpThrFxnExit !!! \n");
	__printf("[rtp] tcp send h264 thread ok \n");
	return NULL;
}

#endif

void *RTP_TcpSendRecordH264ThrFxn(void *pArgs)
{
	int audioEncType ;
	int nalType, pos, leftLen, dataLen;
	int pktLen = 0;//,syncStat = SYNC_WAIT;
	u32  index, pts;
	u8  sToken;
//	char sBuf[MAX_RTP_LEN];
	struct timeval timeout;
	char	*pBuf = NULL; 
	char 	*pData = NULL;
	char 	*pSend = NULL;
	RtspSession_t *pSess = (RtspSession_t *)pArgs;
	struct timeval tv;
	unsigned int audio_pts = 0;
	int ret = 0;

	playback_file_info_t fileInfo;
	char diskPath[128] = {0};
	char recordFilePath[128] = {0};

	//int rtspfd = open("/opt/ipnc/rtsp2.h264",O_CREAT |O_RDWR | O_APPEND);	// lyh
	if(pSess == NULL ){
		dbg(Err, DbgNoPerror, "Input params error \n");
		goto __RtpTcpThrFxnExit;
	}

	if(pSess->pRtpTcpSender == NULL){
		dbg(Dbg, DbgNoPerror, "tcpSender is NULL \n");
		goto __RtpTcpThrFxnExit;
	}
#if 0
	type = CFG_GetAudioEncType();
	if( type == AUDIO_ENC_FMT_ALAW){
		audioEncType = RTP_PT_ALAW;
	}else if (type == AUDIO_ENC_FMT_ULAW){
		audioEncType = RTP_PT_ULAW;
	}else{
		audioEncType = -1;
	}
#endif

	int channelNo = pSess->channelNo;
	int streamNo = pSess->streamNo;
//	stream_frame_queue_t *pFrameQueue = NULL;
	stream_frame_t * pFrame = NULL;
//	int readBegin = -1;
//	int readEnd = -1;
	stream_frame_head_t frameHead;
//	int channel = 0;
	int waitIFrame = 0;
	int frameNo = 0;
	//int frameType =0;
	//#define H264_Get_NalType(c)  ( (c) & 0x1F )  
	//frameType=H264_Get_NalType(*(pFrame->pFrameBuf+4));

	memset(diskPath, 0, sizeof(diskPath));
	sprintf(diskPath, "%s/disk%04X", DISK_PATH, pSess->diskNo);
	memset(recordFilePath, 0, sizeof(recordFilePath));
	sprintf(recordFilePath, "%s/%04X", diskPath, pSess->recordNo);

	if(playbackRecordOpen(&fileInfo, recordFilePath, pSess->startAddr, pSess->endAddr, 0) != 0)
	{
		playbackRecordClose(&fileInfo);
		Printf("open record file error\r\n");
		goto __RtpTcpThrFxnExit;
	}

	pFrame = (stream_frame_t *)malloc(MAX_FRAME_BUFFER_LEN + sizeof(stream_frame_head_t));
	if(pFrame == NULL)
	{
		perror("rtsp backup pFrame malloc");
		goto __RtpTcpThrFxnExit;
	}

	memset(&frameHead, 0, sizeof(stream_frame_head_t));

	audioEncType = RTP_PT_ULAW; //RTP_PT_ULAW;

	timeout.tv_sec  = 3;
	timeout.tv_usec = 0;

	setsockopt(pSess->rtspSockFd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
	index = 0;

	__printf("start send data\n");
	while(RTSP_STATE_PLAY == pSess->sessStat)
	{
//		__printf("send status\n");
		ret = playbackRecordRead(&fileInfo, 1, pFrame);
		if(ret == 0)
		{
			if(waitIFrame == 0)
			{
				if((pFrame->pFrameBuf[4] & 0x1F) == NAL_TYPE_SPS)
				{
					waitIFrame = 1;
					frameNo = pFrame->frameHead.frameNo;
				}
				else
				{
					continue;
				}
			}
			else
			{
				if(((frameNo + 1)&0xFFFF) != pFrame->frameHead.frameNo)
				{//I
					Printf("dev %d, channel %d, lost frame %d ~ %d\r\n", channelNo, streamNo, frameNo, pFrame->frameHead.frameNo);
					waitIFrame = 0;
					continue;
				}
			}
			frameNo = pFrame->frameHead.frameNo;

			pBuf 	= (char *)(pSess->pRtpTcpSender->sendBuf + RTP_HDR_LEN + 4); 
//			printf("time =========== %d, %d, len  ========== %d\r\n", pFrame->frameHead.tv.tv_sec, pFrame->frameHead.tv.tv_usec, pFrame->frameHead.len);
#if 0
			pts 	= pFrame->frameHead.ipcPts;
#else
			gettimeofday(&tv, NULL);
			pts = (unsigned int)(((tv.tv_sec * 1000) + (tv.tv_usec /1000))*90);
			//pts = (unsigned int)(((pFrame->frameHead.tv.tv_sec * 1000) + (pFrame->frameHead.tv.tv_usec /1000))*90);
			audio_pts = pts*8/90;
#endif
			//__printf("pts=%u\n", pts);

			if(pFrame->frameHead.type == FRAME_TYPE_AUDIO)
			{//
#if 1
				if(pSess->reqStreamFlag[RTP_STREAM_AUDIO])
				{
					if(audioEncType != -1)
					{	//* lwx, 2010.11.19
						memcpy(pBuf, pFrame->pFrameBuf, pFrame->frameHead.len);
						//__printf("send audio frame, pktlen=%d\n", pktLen);
						//RTP_TcpPacket(pSess->pRtpTcpSender, audioEncType, pts, 1, pktLen, pData);
						RTP_TcpPacket(pSess->pRtpTcpSender, audioEncType, audio_pts, 1, pktLen, pData);
						if(GS_SUCCESS != RTP_TcpSend(pSess->pRtpTcpSender, RTP_STREAM_AUDIO))
						{
							dbg(Err, DbgNoPerror, "RTP_TcpSend error, sockfd = %d\n",pSess->rtspSockFd);
							goto __RtpTcpThrFxnExit;
						}
					}
				}
#endif
			}
			else
			{
				if(pSess->reqStreamFlag[RTP_STREAM_VIDEO])
				{
					pSend  	= pFrame->pFrameBuf + H264_STARTCODE_LEN;
					dataLen = pFrame->frameHead.len - H264_STARTCODE_LEN;
					nalType = H264_Get_NalType(*(pFrame->pFrameBuf+4));
					/* 1024 */
					if(dataLen <= NAL_FRAGMENTATION_SIZE)
					{
						memcpy(pBuf, pFrame->pFrameBuf+4, pFrame->frameHead.len-4);
						RTP_TcpPacket(pSess->pRtpTcpSender, RTP_PT_H264, pts, 1, dataLen, pSend);
						if(GS_SUCCESS != RTP_TcpSend(pSess->pRtpTcpSender, RTP_STREAM_VIDEO))
						{
							dbg(Err, DbgNoPerror, "RTP_TcpSend error, sockfd = %d\n", pSess->rtspSockFd);
							goto __RtpTcpThrFxnExit;
						}
					}
					else
					{
						/* 1024 FU-A  */
						pBuf[0] = 0x1c | (*(pFrame->pFrameBuf+4) & (~0x1f));
						leftLen = dataLen;
						sToken  = 1;
						pos     = 0;
						while(leftLen > NAL_FRAGMENTATION_SIZE)
						{
							//		printf("0cccccccccccccccccccccccccccccccccccc leftLen = %d, pts = %lu\r\n", leftLen, pts);
							if(RTSP_STATE_PLAY != pSess->sessStat)
							{
								dbg(Err, DbgNoPerror, "pSess stat is not play \n");
								goto __RtpTcpThrFxnExit;
							}
							pBuf[1] = (sToken << 7) | nalType;
							memcpy(pBuf+2, pFrame->pFrameBuf + 4 + pos + sToken, NAL_FRAGMENTATION_SIZE - sToken);
							RTP_TcpPacket(pSess->pRtpTcpSender, RTP_PT_H264, pts, 0, NAL_FRAGMENTATION_SIZE + 2 - sToken, pBuf);
							if(GS_SUCCESS != RTP_TcpSend(pSess->pRtpTcpSender, RTP_STREAM_VIDEO))
							{
								dbg(Err, DbgNoPerror, "RTP_TcpSend error, sockfd = %d\n", pSess->rtspSockFd);
								goto __RtpTcpThrFxnExit;
							}
							sToken 	= 0;
							leftLen -= NAL_FRAGMENTATION_SIZE;
							pos 	+= NAL_FRAGMENTATION_SIZE;
						}
						if(sToken)
						{
							nalType |= 128;
						}
						pBuf[1] = 64 | nalType;
						memcpy(pBuf+2, pFrame->pFrameBuf+4+pos+sToken, leftLen-sToken);
						RTP_TcpPacket(pSess->pRtpTcpSender, RTP_PT_H264, pts, 1, leftLen+2-sToken, pBuf );
						if(GS_SUCCESS != RTP_TcpSend(pSess->pRtpTcpSender,RTP_STREAM_VIDEO))
						{
							dbg(Err, DbgNoPerror, "RTP_TcpSend error, sockfd = %d\n",pSess->rtspSockFd);
							goto __RtpTcpThrFxnExit;
						}
					}
					usleep(20 * 1000);
				}
			}
		}
		else if(ret == 1)
		{
			Printf("rtsp backup record success\r\n");
			goto __RtpTcpThrFxnExit;
		}
		else
		{
			Printf("rtsp backup record error\r\n");
			goto __RtpTcpThrFxnExit;
		}
	}

__RtpTcpThrFxnExit:
	if(pFrame != NULL)
	{
		free(pFrame);
		pFrame = NULL;
	}
	playbackRecordClose(&fileInfo);

	__printf("[rtp] tcp send h264 thread exit ... \n");
	pSess->sessStat = RTSP_STATE_STOP;
	dbg(Dbg, DbgNoPerror, "__RtpTcpThrFxnExit !!! \n");
	__printf("[rtp] tcp send h264 thread ok \n");
	return NULL;
}

#if 0
/*added by xsf for Mpeg4*/
void *RTP_TcpSendMpeg4ThrFxn(void *pArgs)
{

	int type;
	int audioEncType=0 ;
	int  pos, leftLen, dataLen;
	int pktLen,syncStat = SYNC_WAIT;
	u32  index, syncIndex, pts;
	char sBuf[MAX_RTP_LEN];
	struct timeval timeout;
	char 	*pData = NULL;
	char 	*pSend = NULL;
	RtspSession_t *pSess = (RtspSession_t *)pArgs;

	if(pSess == NULL ){
		dbg(Err, DbgNoPerror, "Input params error \n");
		goto __RtpTcpThrFxnExit;
	}

	if(pSess->pRtpTcpSender == NULL){
		dbg(Dbg, DbgNoPerror, "tcpSender is NULL \n");
		goto __RtpTcpThrFxnExit;
	}
#if 0
	type = CFG_GetAudioEncType();
	if( type == AUDIO_ENC_FMT_ALAW){
		audioEncType = RTP_PT_ALAW;
	}else if( type == AUDIO_ENC_FMT_ULAW){
		audioEncType = RTP_PT_ULAW;
	}else{
		audioEncType = -1;
	}
#endif
	timeout.tv_sec  = 3;
	timeout.tv_usec = 0;

	setsockopt(pSess->rtspSockFd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
	index = 0;
	while(RTSP_STATE_PLAY == pSess->sessStat){
		if(SYNC_WAIT == syncStat){
			//index = RINGBUF_GetNewIndex(pSess->channel);
			index = RINGBUF_GetIFrameIndex(pSess->channel);
			while(syncStat == SYNC_WAIT){
				if(RTSP_STATE_PLAY != pSess->sessStat){
					printf("###xsf:pSess->sessStat = %d\n",pSess->sessStat);
					dbg(Err, DbgNoPerror, "pSess stat is not play \n");
					goto __RtpTcpThrFxnExit;
				}

				pktLen = RINGBUF_GetCurPacketLen(pSess->channel, index);
				if(pktLen <= 0){
					usleep(1000);
					//printf("find sps , index = %d, len = %d\n", index, pktLen);
					continue;
				}else if (MPEG4_VO == RINGBUF_GetCurNalType(pSess->channel,index)){
					syncStat = SYNC_OK;
				}else{
					RINGBUF_IndexCount(pSess->channel, &index);
					continue;
				}
			}
		}

		pktLen 	= RINGBUF_GetOnePacket(pSess->channel, index, &pData);
		/*  */
		while(pktLen <= 0){
			if(RTSP_STATE_PLAY != pSess->sessStat){
				dbg(Err, DbgNoPerror, "pSess stat is not play \n");
				goto __RtpTcpThrFxnExit;
			}
			pktLen 	= RINGBUF_GetOnePacket(pSess->channel, index, &pData);
			dbg(Dbg, DbgNoPerror, "too fast , len = %d, index = %d,"" newIndex = %d\n", pktLen, index,RINGBUF_GetNewIndex(pSess->channel));
			usleep(1000);
		}

		syncIndex = RINGBUF_GetNewIndex(pSess->channel);
		/*  */
		if(True == RINGBUF_CheckTimeout(pSess->channel, index, syncIndex)){
			syncStat = SYNC_WAIT;
			dbg(Warn, DbgNoPerror, " too slow :syncIndex = %d, index = %d,"
					"newIndex = %d,  len = %d\n",syncIndex, index,
					RINGBUF_GetNewIndex(pSess->channel), pktLen);
			continue;
		}

		type = RINGBUF_GetCurNalType(pSess->channel, index);
		pts 	= RINGBUF_GetCurPktPts(pSess->channel, index);
		if(type == AUDIO_TYPE){
			if( audioEncType != -1 ){	//* lwx, 2010.11.19
				RTP_TcpPacket(pSess->pRtpTcpSender, audioEncType, pts, 1, pktLen, pData);
				if(GS_SUCCESS != RTP_TcpSend(pSess->pRtpTcpSender, RTP_STREAM_AUDIO)){
					dbg(Err, DbgNoPerror, "RTP_TcpSend error, sockfd = %d\n", pSess->rtspSockFd);
					goto __RtpTcpThrFxnExit;
				}
			}
		}else{
			//printf("###xsf:type = %d\n", type);
			pSend  = pData ;
			dataLen = pktLen;
			/* 1024 */
			if(dataLen <= NAL_FRAGMENTATION_SIZE){
				RTP_TcpPacket(pSess->pRtpTcpSender, RTP_PT_MPEG4, pts, 1, dataLen, pSend);
				if(GS_SUCCESS != RTP_TcpSend(pSess->pRtpTcpSender, RTP_STREAM_VIDEO)){
					dbg(Err, DbgNoPerror, "RTP_TcpSend error, sockfd = %d\n", pSess->rtspSockFd);
					goto __RtpTcpThrFxnExit;
				}
			}else{
				/* 1024  */
				leftLen = dataLen;
				pos     = 0;
				while(leftLen > NAL_FRAGMENTATION_SIZE){
					if(RTSP_STATE_PLAY != pSess->sessStat){
						dbg(Err, DbgNoPerror, "pSess stat is not play \n");
						goto __RtpTcpThrFxnExit;
					}
					/*mpeg4H264 TCP*/
					memcpy(sBuf, pSend+pos, NAL_FRAGMENTATION_SIZE);

					RTP_TcpPacket(pSess->pRtpTcpSender, RTP_PT_MPEG4, pts, 0, NAL_FRAGMENTATION_SIZE , sBuf);
					if(GS_SUCCESS != RTP_TcpSend(pSess->pRtpTcpSender, RTP_STREAM_VIDEO)){
						dbg(Err, DbgNoPerror, "RTP_TcpSend error, sockfd = %d\n",
								pSess->rtspSockFd);
						goto __RtpTcpThrFxnExit;
					}
					leftLen -= NAL_FRAGMENTATION_SIZE;
					pos 	+= NAL_FRAGMENTATION_SIZE;
				}
				memcpy(sBuf, pSend+pos, leftLen);
				RTP_TcpPacket(pSess->pRtpTcpSender, RTP_PT_MPEG4, pts, 1, leftLen, sBuf );
				if(GS_SUCCESS != RTP_TcpSend(pSess->pRtpTcpSender, RTP_STREAM_VIDEO)){
					dbg(Err, DbgNoPerror, "RTP_TcpSend error, sockfd = %d\n",
							pSess->rtspSockFd);
					goto __RtpTcpThrFxnExit;
				}
			}
		}
		RINGBUF_IndexCount(pSess->channel, &index);
	}


__RtpTcpThrFxnExit:
	__printf("[rtp] tcp send mpeg4 thread exit now ... \n");
	pSess->sessStat = RTSP_STATE_STOP;
	dbg(Dbg, DbgNoPerror, "__RtpTcpThrFxnExit !!! \n");
	__printf("[rtp] tcp send mpeg4 thread exit ok \n");
	return NULL;
}

//#define USE_VLC

void *RTP_TcpSendJpegThrFxn(void *pArgs)
{
	int iIsGsClient = 0;
	int chn = 0;
	int type, audioEncType, pktLen, leftLen;
	u32 index, syncIndex,  pts, dataLen;
	//JpegHdrQTable_t	tabHdr;
	JpegQTable_t	qTable;
	//JpegRestart_t   restart;        /*  */
	struct timeval 	timeout;
	//char			sendBuf[MAX_RTP_LEN];
	char			*pBuf = NULL; 
	char 			*pData = NULL;
	u8 *pSend = NULL, *ptempsend = NULL;
	JpegHdr_t		*pJpgHdr = NULL;
	JpegRestart_t   *prestart = NULL;
	JpegHdrQTable_t	*ptabHdr = NULL;
	u8 loop;
	u8 market=0;

	RtspSession_t 	*pSess = (RtspSession_t *)pArgs;
	if(pSess == NULL){
		dbg(Err, DbgNoPerror, "pSess is NULL \n");
		goto __RtpTcpSendJpegThrFxnExit;
	}

	if(pSess->pRtpTcpSender == NULL){
		dbg(Dbg, DbgNoPerror, "tcpSender is NULL \n");
		goto __RtpTcpSendJpegThrFxnExit;
	}

	if(pSess->channel == 0){
		chn = 0;
	}else if(pSess->channel == 1){
		chn = 1;
	}else{
		chn = 2;
	}
#if 0
	type = CFG_GetAudioEncType();
	if( type == AUDIO_ENC_FMT_ALAW){
		audioEncType = RTP_PT_ALAW;
	}else if(type == AUDIO_ENC_FMT_ULAW){
		audioEncType = RTP_PT_ULAW;
	}else {
		audioEncType = -1;
	}
#endif
	audioEncType = RTP_PT_ULAW;

	timeout.tv_sec  = 3;
	timeout.tv_usec = 0;

	setsockopt(pSess->rtspSockFd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
#if 0
	memset(sendBuf, 0, MAX_RTP_LEN);
	pJpgHdr = (JpegHdr_t *)sendBuf;
	pJpgHdr->tspec		= 0;
	pJpgHdr->off		= 0;
	pJpgHdr->type		= 65;	/* (:64 -- yuv4:2:2, 65 -- yuv4:2:0)(:0 -- yuv4:2:2, 1 -- yuv4:2:0) */
	pJpgHdr->q			= 255;	/* 255 -- dynamic quant tables */
	pJpgHdr->width		= CFG_GetVideoWidth(chn)/8;
	pJpgHdr->height		= CFG_GetVideoHeight(chn)/8;

	tabHdr.mbz			= 0;
	tabHdr.precision	= 0;
	tabHdr.length[0]	= 0x00;
	tabHdr.length[1]	= 0x80;

	restart.count       = 0x3fff;  /* 0x3fff */
	restart.f           = 1;
	restart.l           = 1;

	//printf("============= UseAgent: %s =============\n", pSess->userAgent);

	pSign = strstr(pSess->userAgent, "Grandstream");
	if( pSign == NULL ){
		//printf("Is Not Grandstream Client \n");
		iIsGsClient = 0;
	}else{
		//printf("Is Grandstream Client \n");
		iIsGsClient = 1;
	}
#endif
	iIsGsClient = 0;
	index = RINGBUF_GetNewIndex(chn);
	//__printf("index=%d, chn=%d\n", index, chn);
	while(RTSP_STATE_PLAY == pSess->sessStat){
		pktLen 	= RINGBUF_GetOnePacket(chn, index, &pData);
		//printf("len = %d \n", pktLen);
		/*  */
		while(pktLen <= 0){
			if(RTSP_STATE_PLAY != pSess->sessStat){
				dbg(Err, DbgNoPerror, "pSess stat is not play \n");
				goto __RtpTcpSendJpegThrFxnExit;
			}
			pktLen 	= RINGBUF_GetOnePacket(chn, index, &pData);
			dbg(Dbg, DbgNoPerror, "too fast , len = %d, index = %d,"" newIndex = %d\n", pktLen, index,
					RINGBUF_GetNewIndex(chn));
			//index = RINGBUF_GetNewIndex(chn);
			usleep(1000);
		}

		syncIndex = RINGBUF_GetNewIndex(chn);
		/*  */
		if(True == RINGBUF_CheckTimeout(chn, index, syncIndex)){
			dbg(Warn, DbgNoPerror, " too slow :syncIndex = %d, index = %d,"
					"newIndex = %d,  len = %d\n",syncIndex, index,
					RINGBUF_GetNewIndex(chn), pktLen);
			index = RINGBUF_GetNewIndex(chn);
			continue;
		}

#ifdef __one_copy__
		pBuf = (char *)(pSess->pRtpTcpSender->sendBuf+RTP_HDR_LEN+4);
#endif
		type  = RINGBUF_GetCurNalType(chn, index);
		pts 	= RINGBUF_GetCurPktPts(chn, index);
		//__printf("index=%d, chn=%d, pts=%u\n", index, chn, pts);
		if(type == AUDIO_TYPE){
			if( audioEncType != -1 ){
#ifdef __one_copy__
				memcpy(pBuf, pData, pktLen);
#endif
				RTP_TcpPacket(pSess->pRtpTcpSender, audioEncType, pts, 1, pktLen, pData);
				if(GS_SUCCESS != RTP_TcpSend(pSess->pRtpTcpSender, RTP_STREAM_AUDIO)){
					dbg(Err, DbgNoPerror, "RTP_TcpSend error, sockfd = %d\n",
							pSess->rtspSockFd);
					goto __RtpTcpSendJpegThrFxnExit;
				}
			}
		}else{
			/* $+video/audio+(RTP)1+1+2=4 */
			pSess->pRtpTcpSender->sendBuf[0] = '$';
			pSess->pRtpTcpSender->sendBuf[1] = pSess->pRtpTcpSender->interleaved[RTP_STREAM_VIDEO].rtp;
			pSend = pSess->pRtpTcpSender->sendBuf + 4;

			/* RTP12 */
			pSend += 12;

			/* JPEG8 */
			pJpgHdr = (JpegHdr_t *)pSend;
			pJpgHdr->tspec      = 0;
			pJpgHdr->off        = 0;
			if(iIsGsClient == 1 ){
				pJpgHdr->type = 1;              /* :0 -- yuv4:2:2, 1 -- yuv4:2:0 */
			}else{
				pJpgHdr->type = 65;             /* :64 -- yuv4:2:2, 65 -- yuv4:2:0 */
			}
			pJpgHdr->q          = 255;          /* 255 -- dynamic quant tables */
			pJpgHdr->width      =0; //gAVSERVER_UI_ctrl.avserverConfig.encodeConfig[chn].cropWidth/8;//CFG_GetVideoWidth(chn)/8;       //<>
			pJpgHdr->height     =0;// gAVSERVER_UI_ctrl.avserverConfig.encodeConfig[chn].cropHeight/8;//CFG_GetVideoHeight(chn)/8;
			pSend += 8;

			if( GS_SUCCESS != RTP_GetJpegQTable(&qTable, pData, pktLen)){
				dbg(Err, DbgNoPerror, "RTP_GetJpegQTable error \n");
				goto __NextFrame;
			}


			/* If is Grandstream Client, do not send restart interval */
			if(iIsGsClient == 1 ){
				leftLen = pktLen;
			}else{
				leftLen = qTable.scan_data_len;                /* JPEG */
				//restart.interval[0] = qTable.interval[0];      /*  */
				//restart.interval[1] = qTable.interval[1];

			}
#if 0
#ifdef USE_VLC
			leftLen = qTable.scan_data_len;                /* JPEG */
			restart.interval[0] = qTable.interval[0];      /*  */
			restart.interval[1] = qTable.interval[1];
#else
			leftLen = pktLen;
#endif 
#endif 
			/* 4 */
			if( iIsGsClient != 1){

				prestart = (JpegRestart_t *)pSend;
				prestart->count       = 0x3fff;                 /* 0x3fff */
				prestart->f           = 1;
				prestart->l           = 1;
				prestart->interval[0] = qTable.interval[0];
				prestart->interval[1] = qTable.interval[1];     /*  */
				pSend += 4;
			}

			if( iIsGsClient == 1){

				pSend = pSess->pRtpTcpSender->sendBuf + 24;
				pSess->pRtpTcpSender->sendLen = 20;
			}else{

				pSend = pSess->pRtpTcpSender->sendBuf + 28;
				pSess->pRtpTcpSender->sendLen = 24;
			}

			/* 4+64+64=132 */
			pJpgHdr->off = 0;
			if(pJpgHdr->off == 0){

				ptabHdr = (JpegHdrQTable_t *)pSend;
				ptabHdr->mbz         = 0;
				ptabHdr->precision  = 0;
				ptabHdr->length[0]  = 0x00;
				ptabHdr->length[1]  = 0x80;
				pSend += 4;
				for(loop=0; loop<QUANT_TABLE_LEN;loop++)
				{
					*(pSend+loop) = *(qTable.table[0].src+loop);
				}
				pSend += QUANT_TABLE_LEN;
				for(loop=0; loop<QUANT_TABLE_LEN;loop++)
				{
					*(pSend+loop) = *(qTable.table[1].src+loop);
				}
				pSend += QUANT_TABLE_LEN;
				pSess->pRtpTcpSender->sendLen += 132;
			}

			while(leftLen > 0){
				/* RTPmarket=1market=0 */
				if(leftLen > MJPEG_RTP_LEN){

					market  = 0;
					dataLen = MJPEG_RTP_LEN;
				}else{

					market  = 1;
					dataLen = leftLen;
				}

				/* RTP */
				ptempsend = pSess->pRtpTcpSender->sendBuf + 4;
				//pSend = sendBuf + sizeof(JpegHdr_t);
#if 0
				if( iIsGsClient != 1){
					memcpy(pSend, &restart, sizeof(JpegRestart_t));   /* JPEG */
					pSend += sizeof(JpegRestart_t);
				}
#if 0
#ifdef USE_VLC
				memcpy(pSend, &restart, sizeof(JpegRestart_t));   /* JPEG */
				pSend += sizeof(JpegRestart_t);
#endif 
#endif 

				if(pJpgHdr->off == 0){
					memcpy(pSend, &tabHdr, sizeof(JpegHdrQTable_t));
					pSend += sizeof(JpegHdrQTable_t);
					memcpy(pSend, qTable.table[0].src, QUANT_TABLE_LEN);
					pSend += QUANT_TABLE_LEN;
					memcpy(pSend, qTable.table[1].src, QUANT_TABLE_LEN);
					pSend += QUANT_TABLE_LEN;
				}

				if(leftLen > MJPEG_RTP_LEN){
					market  = 0;
					dataLen = MJPEG_RTP_LEN;
				}else{
					market  = 1;
					dataLen = leftLen;
				}


				if( iIsGsClient == 1 ){
					memcpy(pSend, pData+pJpgHdr->off, dataLen);              /* APPO+JPEG */
				}else{
					memcpy(pSend, qTable.scan_data + pJpgHdr->off, dataLen); /* JPEG */
				}

#if 0
#ifdef USE_VLC
				memcpy(pSend, qTable.scan_data + pJpgHdr->off, dataLen); /* JPEG */
#else
				memcpy(pSend, pData+pJpgHdr->off, dataLen);              /* APPO+JPEG */
#endif 
#endif 
#endif
				//RTP_TcpPacket(pSess->pRtpTcpSender, RTP_PT_JPEG, pts, market, pSend + dataLen - sendBuf, sendBuf);
				RTP_TcpPacket_Jpeg(pSess->pRtpTcpSender, ptempsend,RTP_PT_JPEG, pts, market);

				/* (RTP) */
				pSess->pRtpTcpSender->sendLen += dataLen;
				*(unsigned short *)(pSess->pRtpTcpSender->sendBuf+2)
					= htons((unsigned short) pSess->pRtpTcpSender->sendLen);

				if( iIsGsClient == 1 ){

					memcpy(pSend, pData+pJpgHdr->off, dataLen);              /* ?APPO??+JPEG??y?Y */
				}else{

					memcpy(pSend, qTable.scan_data + pJpgHdr->off, dataLen); /* ?JPEG??y?Y */
				}
				if(GS_SUCCESS != RTP_TcpSend(pSess->pRtpTcpSender, RTP_STREAM_VIDEO)){
					dbg(Err, DbgNoPerror, "RTP_TcpSend error, sockfd = %d\n",
							pSess->rtspSockFd);
					goto __RtpTcpSendJpegThrFxnExit;
				}
				pJpgHdr->off += dataLen;
				leftLen 	 -= dataLen;
				if( iIsGsClient == 1){
					pSend = pSess->pRtpTcpSender->sendBuf + 24;
					pSess->pRtpTcpSender->sendLen = 20;
				}else{
					pSend = pSess->pRtpTcpSender->sendBuf + 28;
					pSess->pRtpTcpSender->sendLen = 24;
				}
			}
		}
__NextFrame:
		RINGBUF_IndexCount(chn, &index);
	}


__RtpTcpSendJpegThrFxnExit:
	__printf("[rtp] tcp send jpeg thread exit now ... \n");
	pSess->sessStat = RTSP_STATE_STOP;
	dbg(Dbg, DbgNoPerror, "__RtpTcpSendJpegThrFxnExit !!! \n");
	__printf("[rtp] tcp send jpeg thread exit ok \n");
	return NULL;
}
#endif

int RTP_TcpSendH264VideoData(RtspSession_t *pSess)
{
	if(pSess->isPlayBack == 0)
	{//
		if( 0 != pthread_create(&pSess->tcpThrId, NULL, RTP_TcpSendH264ThrFxn, pSess)){
			dbg(Err, DbgPerror, "pthread_create RTP_TcpSendH264ThrFxn error \n");
			return GS_FAIL;
		}
	}
	else
	{//rtsp
		if( 0 != pthread_create(&pSess->tcpThrId, NULL, RTP_TcpSendRecordH264ThrFxn, pSess)){
			dbg(Err, DbgPerror, "pthread_create RTP_TcpSendRecordH264ThrFxn error \n");
			return GS_FAIL;
		}
	}
	return GS_SUCCESS;
}

#if 0
/*added by xsf for Mpeg4*/
int RTP_TcpSendMpeg4VideoData(RtspSession_t *pSess)
{
	if( 0 != pthread_create(&pSess->tcpThrId, NULL, RTP_TcpSendMpeg4ThrFxn, pSess)){
		dbg(Err, DbgPerror, "pthread_create RTP_TcpSendH264ThrFxn error \n");
		return GS_FAIL;
	}
	return GS_SUCCESS;
}

int RTP_TcpSendJpegVideoData(RtspSession_t *pSess)
{
	if( 0 != pthread_create(&pSess->tcpThrId, NULL, RTP_TcpSendJpegThrFxn, pSess)){
		dbg(Err, DbgPerror, "pthread_create RTP_TcpSendH264ThrFxn error \n");
		return GS_FAIL;
	}
	return GS_SUCCESS;
}
#endif

int RTP_TcpStartSendMediaData(RtspSession_t *pSess)
{
	int ret = GS_FAIL;
	int type = VENC_TYPE_JPEG;//CFG_GetVideoEncType(pSess->channel);
	//if(gAVSERVER_config.encodeConfig[pSess->channel].codecType == ALG_VID_CODEC_H264)
	type = VENC_TYPE_H264;
	//	else	
	//	type = VENC_TYPE_JPEG;
	dbg(Dbg, DbgNoPerror, "RTP_TcpStartSendMediaData \n");
	switch(type){
		case VENC_TYPE_H264:
			ret = RTP_TcpSendH264VideoData(pSess);
			break;

		case VENC_TYPE_JPEG:
			//ret = RTP_TcpSendJpegVideoData(pSess);
			break;

		case VENC_TYPE_MPEG4:
			//ret = RTP_TcpSendMpeg4VideoData(pSess);
			break;

		default:
			dbg(Err, DbgNoPerror, "GetVideoEncType error \n");
			break;
	}
	return ret;
}


#ifdef TS_STREAM_UDP
extern unsigned long  UnicastIP1, UnicastIP2, UnicastIP3;
extern unsigned short UnicastPort1, UnicastPort2, UnicastPort3;
extern unsigned char  UnicastOn1, UnicastOn2, UnicastOn3;

int es_videofd;
int es_audiofd;
int ts_videofd;
int ts_audiofd;

Ts_Muxer_T tsMuxer;

u8 *ts_sendbuf;

#define ES_UDP_FRAGMENTATION_SIZE		1024      //16384

#define ES_FU_A

static int dealTsStream(Ts_Callback_Param_T param);

int ES_TS_Initialize()
{
	es_videofd = RTP_OpenUdpSocket(6200);
	es_audiofd = RTP_OpenUdpSocket(6201);
	ts_videofd = RTP_OpenUdpSocket(6202);
	ts_audiofd = RTP_OpenUdpSocket(6203);

	ts_sendbuf = (u8 *)malloc(409600);
	if(ts_sendbuf == NULL){
		printf("ES_TS_Initialize malloc ts_sendbuf failed !!!!!!!!!!~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	}
	ts_sendbuf = ((unsigned long) ts_sendbuf + 3) & 0xFFFFFFFC;

	TsMuxerInit(&tsMuxer);
	Initilize(&tsMuxer, 25, 0x1fff, 0x1fff, 0x1fff, 0x1fff);
	SetPID(&tsMuxer, TS_PAT_E, 0x10);
	SetPID(&tsMuxer, TS_PMT_E, 0x42);
	SetPID(&tsMuxer, TS_SIT_E, 0xf00);
	SetPID(&tsMuxer, TS_PCR_E, 0x43);
	SetPID(&tsMuxer, TS_VIDEO_E, 0x44);
	SetPID(&tsMuxer, TS_AUDIO_E, 0x50);
	setStreamType(&tsMuxer, STREAM_TYPE_VIDEO_H264, STREAM_TYPE_AUDIO_AAC);
	initTsCallBack(&tsMuxer, NULL, ts_sendbuf + RTP_HDR_LEN, 7, dealTsStream);

	return GS_SUCCESS;	
}

int ES_TS_RTP_UdpWrite(char *packet, int len, int is_es, int is_video)
{
	int ret;
	struct sockaddr_in  remoteAddr;
	int send_fd;

	RtpHdr_t *pRtpHdr = NULL;
	pRtpHdr = (RtpHdr_t *)packet;

#if 0
	static FILE *fp=NULL;
	static int i=0;
	if(fp==NULL && i==0){
		fp = fopen("/opt/ipnc/test.h264", "wb");
	}

	i++;
	if(fp != NULL){
		if(i > 60)
			fwrite(packet+RTP_HDR_LEN, 1, len-RTP_HDR_LEN, fp);

		if(i > 1000){
			fclose(fp);
			fp = NULL;
		}			
	}
#endif

	if(UnicastOn1){
		memset(&remoteAddr, 0, sizeof(struct sockaddr_in));
		remoteAddr.sin_family = AF_INET;	
		if(is_es){	
			remoteAddr.sin_port = is_video ? htons(UnicastPort1) : htons(UnicastPort1+1);
			send_fd = is_video ? es_videofd : es_audiofd;
		}
		else{
			remoteAddr.sin_port = is_video ? htons(UnicastPort1+2) : htons(UnicastPort1+3);
			send_fd = is_video ? ts_videofd : ts_audiofd;
		}
		remoteAddr.sin_addr.s_addr 	= UnicastIP1;

		ret = sendto(send_fd , packet, len, 0, (struct sockaddr*)&remoteAddr, sizeof(struct sockaddr));
		if (ret != len)
		{
			printf("send err 1 \n");
		}
		else
		{
			//printf("send %s %s ok1: %d,  data len: %d!\n", is_es?"es":"ts", is_video?"video":"audio", ntohs(pRtpHdr->seqno), ret - 12);	
			//printf("send %s %s ok1: %d,  data len: %d!\n", is_es?"chn0":"chn1", is_video?"video":"audio", ntohs(pRtpHdr->seqno), ret - 12);		
		}
	}

	if(UnicastOn2){
		memset(&remoteAddr, 0, sizeof(struct sockaddr_in));
		remoteAddr.sin_family 		= AF_INET;
		if(is_es){
			remoteAddr.sin_port 	= is_video ? htons(UnicastPort2) : htons(UnicastPort2+1);
			send_fd = is_video ? es_videofd : es_audiofd;
		}
		else{
			remoteAddr.sin_port 	= is_video ? htons(UnicastPort2+2) : htons(UnicastPort2+3);
			send_fd = is_video ? ts_videofd : ts_audiofd;
		}
		remoteAddr.sin_addr.s_addr 	= UnicastIP2;

		ret = sendto(send_fd, packet, len, 0, (struct sockaddr*)&remoteAddr, sizeof(struct sockaddr));
		if (ret != len)
		{
			printf("send err 2 \n");
		}
		else
		{
			//printf("send ok2 %d \n", ntohs(pRtpHdr->seqno));
		}
	}

	if(UnicastOn3){
		memset(&remoteAddr, 0, sizeof(struct sockaddr_in));
		remoteAddr.sin_family 		= AF_INET;
		if(is_es){
			remoteAddr.sin_port 	= is_video ? htons(UnicastPort3) : htons(UnicastPort3+1);
			send_fd = is_video ? es_videofd : es_audiofd;
		}
		else{
			remoteAddr.sin_port 	= is_video ? htons(UnicastPort3+2) : htons(UnicastPort3+3);
			send_fd = is_video ? ts_videofd : ts_audiofd;
		}
		remoteAddr.sin_addr.s_addr 	= UnicastIP3;

		ret = sendto(send_fd, packet, len, 0, (struct sockaddr*)&remoteAddr, sizeof(struct sockaddr));
		if (ret != len)
		{
			printf("send err 3 \n");
		}
		else
		{
			//printf("send ok3 %d \n", ntohs(pRtpHdr->seqno));
		}
	}


	return GS_SUCCESS;	
}

int ES_TS_RTP_UdpPacket(char *packet, u32 pts, int marker, u16 sn, int len, char *data, int is_es, int is_video)
{   
	static u32 ssrc = RTP_DEFAULT_SSRC;

	RtpHdr_t *pRtpHdr = NULL;
	pRtpHdr = (RtpHdr_t *)packet;

	RTP_HDR_SET_VERSION(pRtpHdr, RTP_VERSION);
	RTP_HDR_SET_P(pRtpHdr, 0);
	RTP_HDR_SET_X(pRtpHdr, 0);
	RTP_HDR_SET_CC(pRtpHdr, 0);

	RTP_HDR_SET_M(pRtpHdr, marker);
	if(is_es)
		RTP_HDR_SET_PT(pRtpHdr, is_video ? RTP_PT_H264 : RTP_PT_ULAW);
	else
		RTP_HDR_SET_PT(pRtpHdr, is_video ? RTP_PT_MPEG2TS : RTP_PT_ULAW);

	RTP_HDR_SET_SEQNO(pRtpHdr, htons(sn));

	RTP_HDR_SET_TS(pRtpHdr, htonl(pts));

	RTP_HDR_SET_SSRC(pRtpHdr, htonl(ssrc));

#ifndef ES_FU_A
	if(is_video == 0)
		memcpy(packet+ RTP_HDR_LEN, data, len);
#endif

	return GS_SUCCESS;
}

int ES_RTP_UdpSendVideoPacket(int chn, u32 pts, int market, char *data, int len)
{
	char packet[ES_UDP_FRAGMENTATION_SIZE + 128];
	static u16 lastSnChn0 = 3600;
	static u16 lastSnChn1 = 3200;

#ifndef ES_FU_A
	ES_TS_RTP_UdpPacket(packet, pts, market, lastSn, len, data, 1, 1);
	ES_TS_RTP_UdpWrite(packet,  RTP_HDR_LEN + len, 1, 1);
#else
	if(chn==0){
		ES_TS_RTP_UdpPacket(data, pts, market, lastSnChn0, len, data+RTP_HDR_LEN, 1, 1);
		ES_TS_RTP_UdpWrite(data,  RTP_HDR_LEN + len, 1, 1);
		lastSnChn0++;
	}
	else{
		ES_TS_RTP_UdpPacket(data, pts, market, lastSnChn1, len, data+RTP_HDR_LEN, 1, 1);
		ES_TS_RTP_UdpWrite(data,  RTP_HDR_LEN + len, 0, 1);
		lastSnChn1++;
	}
#endif

	return GS_SUCCESS;
}

#ifndef ES_FU_A
int ES_RTP_UdpSendVideo(type, chn, pts, frameType, wTime, len, data)
{
	char *pvide_data = NULL;
	int fua_buf[MAX_RTP_LEN];
	u8 nal_type;
	u8 s_token;
	int data_len, left_len, pos, iFlag;
	int spsLen = 0, ppsLen = 0, seiLen;
	int send_len[5] = {0};
	char * psend_data[5] = {NULL};
	u8 send_count = 0;
	u8 uloop=0;
	RtpHdr_t *pRtpHdr = NULL;

	if(chn != 0 && chn != 1 && chn != 2){
		return 0;
	}

	pvide_data = data + 4;
	data_len   = len - 4;
	if(data == NULL){
		return GS_SUCCESS;
	}

	nal_type = H264_Get_NalType(*pvide_data);

	if(frameType == VENC_I_FRAME ){
		iFlag = 1;
	}else{
		iFlag = 0;
	}
#if 1
	if (frameType == VENC_I_FRAME)
	{
		send_count = 0;
		pvide_data = data + 4;
		seiLen = RINGBUF_GetSpsPpsSeiLen(&spsLen, &ppsLen, len, data);
		send_len[0] = spsLen - 4;
		psend_data[0] = pvide_data;
		send_count++;
		pvide_data = pvide_data + spsLen;
		send_len[1] = ppsLen - 4;
		psend_data[1] = pvide_data;
		send_count++;
		pvide_data = pvide_data + ppsLen;
		if (seiLen > 0)
		{
			//printf("seiLen: %d\n", seiLen);
			send_len[send_count] = seiLen - 4;
			psend_data[send_count] = pvide_data;
			send_count++;
			pvide_data = pvide_data + seiLen;
			send_len[send_count] = len - spsLen - ppsLen - seiLen - 4;
		}
		else
		{
			send_len[send_count] = len - spsLen - ppsLen - 4;
		}
		psend_data[send_count] = pvide_data;
		send_count++;
	}
	else
	{
		send_count = 1;
		psend_data[0] = data + 4;
		send_len[0] = len - 4;
	}
#else // Don't remove 00 00 00 01
	if (frameType == VENC_I_FRAME)
	{
		send_count = 0;
		pvide_data = data;
		seiLen = RINGBUF_GetSpsPpsSeiLen(&spsLen, &ppsLen, len, data);
		send_len[0] = spsLen;
		psend_data[0] = pvide_data;
		send_count++;
		pvide_data = pvide_data + spsLen;
		send_len[1] = ppsLen;
		psend_data[1] = pvide_data;
		send_count++;
		pvide_data = pvide_data + ppsLen;
		if (seiLen > 0)
		{
			send_len[send_count] = seiLen;
			psend_data[send_count] = pvide_data;
			send_count++;
			pvide_data = pvide_data + seiLen;
			send_len[send_count] = len - spsLen - ppsLen - seiLen;
		}
		else
		{
			send_len[send_count] = len - spsLen - ppsLen;
		}
		psend_data[send_count] = pvide_data;
		send_count++;
	}
	else
	{
		send_count = 1;
		psend_data[0] = data;
		send_len[0] = len;
	}
#endif

	for(uloop=0; uloop<send_count; uloop++)
	{
		if(send_len[uloop] <= ES_UDP_FRAGMENTATION_SIZE){
			ES_RTP_UdpSendVideoPacket(chn, pts, 1, psend_data[uloop], send_len[uloop]);
		}else{
			nal_type = H264_Get_NalType(*psend_data[uloop]);
			left_len = send_len[uloop];
			pos = 0;
			while(left_len > ES_UDP_FRAGMENTATION_SIZE)
			{				
				ES_RTP_UdpSendVideoPacket(chn, pts, 0, psend_data[uloop] + pos, ES_UDP_FRAGMENTATION_SIZE);
				left_len -= ES_UDP_FRAGMENTATION_SIZE;
				pos += ES_UDP_FRAGMENTATION_SIZE;
			}

			ES_RTP_UdpSendVideoPacket(chn, pts, 1, psend_data[uloop] + pos, left_len);
		}
	}
	return GS_SUCCESS;
}
#else
int ES_RTP_UdpSendVideo(type, chn, pts, frameType, wTime, len, data)
{
	char *pvide_data = NULL;
	char buffer[ES_UDP_FRAGMENTATION_SIZE+64];
	char *fua_buf=buffer;
	fua_buf = (((unsigned long)(fua_buf+31)) & 0xffffffe0);
	u8 nal_type;
	u8 s_token;
	int data_len, left_len, pos, iFlag;
	int spsLen = 0, ppsLen = 0, seiLen;
	int send_len[5] = {0};
	char * psend_data[5] = {NULL};
	u8 send_count = 0;
	u8 uloop=0;
	RtpHdr_t *pRtpHdr = NULL;

	if(chn != 0 && chn != 1 && chn != 2){
		return 0;
	}

	pvide_data = data + 4;
	data_len   = len - 4;
	if(data == NULL){
		return GS_SUCCESS;
	}

	nal_type = H264_Get_NalType(*pvide_data);

	if(frameType == VENC_I_FRAME ){
		iFlag = 1;
	}else{
		iFlag = 0;
	}
#if 1
	if (frameType == VENC_I_FRAME)
	{
		send_count = 0;
		pvide_data = data + 4;
		seiLen = RINGBUF_GetSpsPpsSeiLen(&spsLen, &ppsLen, len, data);
		send_len[0] = spsLen - 4;
		psend_data[0] = pvide_data;
		send_count++;
		pvide_data = pvide_data + spsLen;
		send_len[1] = ppsLen - 4;
		psend_data[1] = pvide_data;
		send_count++;
		pvide_data = pvide_data + ppsLen;
		if (seiLen > 0)
		{
			//printf("seiLen: %d\n", seiLen);
			send_len[send_count] = seiLen - 4;
			psend_data[send_count] = pvide_data;
			send_count++;
			pvide_data = pvide_data + seiLen;
			send_len[send_count] = len - spsLen - ppsLen - seiLen - 4;
		}
		else
		{
			send_len[send_count] = len - spsLen - ppsLen - 4;
		}
		psend_data[send_count] = pvide_data;
		send_count++;
	}
	else
	{
		send_count = 1;
		psend_data[0] = data + 4;
		send_len[0] = len - 4;
	}
#else // Don't remove 00 00 00 01
	if (frameType == VENC_I_FRAME)
	{
		send_count = 0;
		pvide_data = data;
		seiLen = RINGBUF_GetSpsPpsSeiLen(&spsLen, &ppsLen, len, data);
		send_len[0] = spsLen;
		psend_data[0] = pvide_data;
		send_count++;
		pvide_data = pvide_data + spsLen;
		send_len[1] = ppsLen;
		psend_data[1] = pvide_data;
		send_count++;
		pvide_data = pvide_data + ppsLen;
		if (seiLen > 0)
		{
			send_len[send_count] = seiLen;
			psend_data[send_count] = pvide_data;
			send_count++;
			pvide_data = pvide_data + seiLen;
			send_len[send_count] = len - spsLen - ppsLen - seiLen;
		}
		else
		{
			send_len[send_count] = len - spsLen - ppsLen;
		}
		psend_data[send_count] = pvide_data;
		send_count++;
	}
	else
	{
		send_count = 1;
		psend_data[0] = data;
		send_len[0] = len;
	}
#endif

	for(uloop=0; uloop<send_count; uloop++)
	{
		if(send_len[uloop] <= ES_UDP_FRAGMENTATION_SIZE){
			memcpy(fua_buf+RTP_HDR_LEN, psend_data[uloop], send_len[uloop]);
			ES_RTP_UdpSendVideoPacket(chn, pts, 1, fua_buf, send_len[uloop]);
		}else{
			nal_type = H264_Get_NalType(*psend_data[uloop]);
			/********** RFC3984 FU-A **********/
			/*+---------------+
			  |0|1|2|3|4|5|6|7|
			  +-+-+-+-+-+-+-+-+
			  |F|NRI|Type = 1c|
			  +---------------+*/
			fua_buf[RTP_HDR_LEN+0] = 0x1c | (*psend_data[uloop] & ~0x1F); /*fua_buf[0] filled the FU indicator*/
			s_token = 1;
			left_len = send_len[uloop];
			pos = 0;
			while(left_len > ES_UDP_FRAGMENTATION_SIZE)
			{
				/*+---------------+
				  |0|1|2|3|4|5|6|7|
				  +-+-+-+-+-+-+-+-+
				  |S|E|R| NalType |
				  +---------------+*/
				fua_buf[RTP_HDR_LEN+1] = (s_token<<7) | nal_type; /*fua_buf[0] filled the FU header*/
				/**/
				memcpy(fua_buf+RTP_HDR_LEN+2, psend_data[uloop] + pos + s_token, ES_UDP_FRAGMENTATION_SIZE-s_token);
				ES_RTP_UdpSendVideoPacket(chn, pts, 0, fua_buf, ES_UDP_FRAGMENTATION_SIZE+2-s_token);
				s_token = 0;
				left_len  -= ES_UDP_FRAGMENTATION_SIZE;
				pos  += ES_UDP_FRAGMENTATION_SIZE;
			}

			if (s_token)
			{
				nal_type |= 128;
			}
			fua_buf[RTP_HDR_LEN+1] = 64 | nal_type; /*fua_buf[0] filled the FU header*/
			memcpy(fua_buf+RTP_HDR_LEN+2, psend_data[uloop] + pos + s_token, left_len-s_token);
			ES_RTP_UdpSendVideoPacket(chn, pts, 1, fua_buf, left_len+2-s_token);
		}
	}
	return GS_SUCCESS;
}
#endif


void ES_RTP_UdpSendAudio(int chn, u32 pts, int market, int len, char *data)
{
	char packet[2048 + 128];
	static u16 lastSnChn0 = 1200;
	static u16 lastSnChn1 = 1000;

	if(chn==0){
		ES_TS_RTP_UdpPacket(packet, pts, market, lastSnChn0, len, data, 1, 0);
		ES_TS_RTP_UdpWrite(packet,  RTP_HDR_LEN + len, 1, 0);
		lastSnChn0++;
	}
	else{
		ES_TS_RTP_UdpPacket(packet, pts, market, lastSnChn1, len, data, 1, 0);
		ES_TS_RTP_UdpWrite(packet,  RTP_HDR_LEN + len, 0, 0);
		lastSnChn1++;
	}

	return GS_SUCCESS;
}


int TS_RTP_UdpSendVideo(type, chn, pts, frameType, wTime, len, data)
{

#if 0
	char *pTempBuf = NULL;
	int tempBufLen = 0;
	unsigned char *pTsBuf = NULL;
	int tsBufLen = 0;
	int tsBufCount = 0;
	struct timeval ts_pts;

	gettimeofday(&ts_pts, NULL);
	if(GS_SUCCESS != WriteTs(&tsMuxer, (unsigned char *)data, len, (type == AUDIO_TYPE) ? A_FRAME : V_FRAME, &ts_pts))
	{
		printf("WriteTs failed\n");
	}

	return GS_SUCCESS;	
#endif

	char *pvide_data = NULL;
	u8 nal_type;
	u8 s_token;
	int data_len, left_len, pos, iFlag;
	int spsLen = 0, ppsLen = 0, seiLen;
	int send_len[5] = {0};
	char * psend_data[5] = {NULL};
	u8 send_count = 0;
	u8 uloop=0;
	RtpHdr_t *pRtpHdr = NULL;
	struct timeval ts_pts;

	gettimeofday(&ts_pts, NULL);

	if(chn != 0 && chn != 1 && chn != 2){
		return 0;
	}

	pvide_data = data;
	data_len   = len;
	if(data == NULL){
		return GS_SUCCESS;
	}

	nal_type = H264_Get_NalType(*pvide_data);

	if(frameType == VENC_I_FRAME ){
		iFlag = 1;
	}else{
		iFlag = 0;
	}
#if 1
	if (frameType == VENC_I_FRAME)
	{
		send_count = 0;
		pvide_data = data;
		seiLen = RINGBUF_GetSpsPpsSeiLen(&spsLen, &ppsLen, len, data);
		send_len[0] = spsLen;
		psend_data[0] = pvide_data;
		send_count++;
		pvide_data = pvide_data + spsLen;
		send_len[1] = ppsLen;
		psend_data[1] = pvide_data;
		send_count++;
		pvide_data = pvide_data + ppsLen;
		if (seiLen > 0)
		{
			//printf("seiLen: %d\n", seiLen);
			send_len[send_count] = seiLen;
			psend_data[send_count] = pvide_data;
			send_count++;
			pvide_data = pvide_data + seiLen;
			send_len[send_count] = len - spsLen - ppsLen - seiLen;
		}
		else
		{
			send_len[send_count] = len - spsLen - ppsLen;
		}
		psend_data[send_count] = pvide_data;
		send_count++;
	}
	else
	{
		send_count = 1;
		psend_data[0] = data;
		send_len[0] = len;
	}
#else // Don't remove 00 00 00 01
	if (frameType == VENC_I_FRAME)
	{
		send_count = 0;
		pvide_data = data;
		seiLen = RINGBUF_GetSpsPpsSeiLen(&spsLen, &ppsLen, len, data);
		send_len[0] = spsLen;
		psend_data[0] = pvide_data;
		send_count++;
		pvide_data = pvide_data + spsLen;
		send_len[1] = ppsLen;
		psend_data[1] = pvide_data;
		send_count++;
		pvide_data = pvide_data + ppsLen;
		if (seiLen > 0)
		{
			send_len[send_count] = seiLen;
			psend_data[send_count] = pvide_data;
			send_count++;
			pvide_data = pvide_data + seiLen;
			send_len[send_count] = len - spsLen - ppsLen - seiLen;
		}
		else
		{
			send_len[send_count] = len - spsLen - ppsLen;
		}
		psend_data[send_count] = pvide_data;
		send_count++;
	}
	else
	{
		send_count = 1;
		psend_data[0] = data;
		send_len[0] = len;
	}
#endif

	for(uloop=0; uloop<send_count; uloop++)
	{
		if(GS_SUCCESS != WriteTs(&tsMuxer, (unsigned char *)psend_data[uloop], send_len[uloop], (type == AUDIO_TYPE) ? A_FRAME : V_FRAME, &ts_pts))
		{
			printf("WriteTs failed\n");
		}
	}

	return GS_SUCCESS;
}

static int dealTsStream(Ts_Callback_Param_T param)
{
	static u16 lastSn = 3600;

	//printf("dealTsStream\n");

#if 0
	ES_TS_RTP_UdpPacket(ts_sendbuf, time(0), 0, lastSn, param.curTsPackNum * TS_PACKET_LEN, param.m_pForTsPacked, 0, 1);
	ES_TS_RTP_UdpWrite(ts_sendbuf,  RTP_HDR_LEN + param.curTsPackNum * TS_PACKET_LEN, 0, 1);
#else
	ES_TS_RTP_UdpWrite(param.m_pForTsPacked, param.curTsPackNum * TS_PACKET_LEN, 0, 1);
#endif

	lastSn ++;	

	return GS_SUCCESS;
}

/*
   int ES_TS_RTP_UdpSend(int type, int chn, u32 pts, int frameType, time_t wTime, int len, char *data)
   {
   if(frameType == AUDIO_TYPE){		
   ES_RTP_UdpSendAudio(chn, pts, 1, len, data );
//TS_RTP_UdpSendAudio(chn, pts, 1, len, data );
}else{
if(chn == 0)
ES_RTP_UdpSendVideo(type, chn, pts, frameType, wTime, len, data);
else if(chn == 1)
TS_RTP_UdpSendVideo(type, chn, pts, frameType, wTime, len, data);
}
}
*/

int ES_TS_RTP_UdpSend(int type, int chn, u32 pts, int frameType, time_t wTime, int len, char *data)
{
	if(frameType == AUDIO_TYPE){		
		ES_RTP_UdpSendAudio(chn, pts, 1, len, data );
	}else{
		ES_RTP_UdpSendVideo(type, chn, pts, frameType, wTime, len, data);
	}
}

#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


