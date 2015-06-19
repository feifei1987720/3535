/***********************************************************************************
 * Filename:	rtspLib.c

 * Description:	rtsp
 ************************************************************************************/

#include "rtspCLib.h"
#include "rtspFunc.h"
#include "commonLib.h"
#include "streamLib.h"

/* rtsp初始化 */
rtsp_info_t * rtspInit()
{
	return RTSP_Init();
}

/* rtsp销毁 */
void rtspUninit(rtsp_info_t *pRtspInfo)
{
	RTSP_Free(pRtspInfo);
}

/* rtsp请求视频 */
int rtspInviteStream(rtsp_info_t *pRtspInfo, int channelNo, int streamNo, char *pAddr, int port, char *pUserName, char *pPwd, int protocol, char *pFileName)
{
	int ret = 0;
	unsigned int tmpValue = 0;
	struct timeval tv, tv1;

	//	assert(pRtspInfo);

	RTSP_Set_Param(pRtspInfo, pAddr, port, pUserName, pPwd, protocol, pFileName);

	/* 根据IP、端口和通道，获取URL */
	RTSP_URL(pRtspInfo);

	/* 建立SOCKET */
	ret = RTSP_Init_Socket(pRtspInfo);
	if (ret == -1)
	{
		fprintf(stderr, "Create Socket Failed, error line: %d, error file: %s\n", __LINE__, __FILE__);
		return ret;
	}

	/* 建立RTSP连接 */
	ret = RTSP_Client_SubSvr(pRtspInfo);
	if (ret == -1)
	{
		//fprintf(stderr, "RTSP_Client_SubSvr Failed, error line: %d, error file: %s\n", __LINE__, __FILE__);
		return ret;
	}

	gettimeofday(&tv, NULL);
	while(pRtspInfo->pstRTSPCB->ucCBStatus < EN_RTSP_STATUS_AVILABLE)
	{
		gettimeofday(&tv1, NULL);
		/* 计算显示花费的时间 */
		tmpValue = tv1.tv_sec - tv.tv_sec;
		if(tmpValue >= 10)
		{//长时间未收到消息，则断开rtsp连接
			//Printf("wait rtsp respone time out, %d\r\n", tmpValue);
			RTSP_Exit(pRtspInfo);
			return -1;
		}
		/* 启动RTSP程序，与对方建立RTSP连接 */
		ret = RTSP_TaskProc(pRtspInfo,channelNo,streamNo);
		if (ret == -1)
		{
			fprintf(stderr, "RTSP_TaskProc Failed, error line: %d, error file: %s\n", __LINE__, __FILE__);
			RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_TEARDOWN);
		}
		usleep(10*1000);
	}

	/* Teardown状态表示RTSP互联时有错，重新建立连接 */
	if (pRtspInfo->pstRTSPCB->ucCBStatus == EN_RTSP_STATUS_TEARDOWN)
	{
		RTSP_Exit(pRtspInfo);
		return -1;
	}
	else if(pRtspInfo->pstRTSPCB->ucCBStatus == EN_RTSP_STATUS_AVILABLE)
	{/* Avilable状态表示RTSP互联成功，接收RTP数据 */
		return 0;
	}

	return -1;
}

char * my_strrstr(char const *s1, char const *s2)
{
	char *last;
	char *current;
	last = NULL;
	if (*s2 != '\0')
	{
		current = strstr(s1, s2);
		while (current != NULL) {
			last = current;
			current = strstr(last + 1, s2);
		}
	}
	return last;
}

/* socketbuffer */
int rtspRecvStreamToRingBuf(rtsp_info_t *pRtspInfo, stream_packet_queue_t * pPacketQueue, unsigned short *  length)
{
	int ret = 0;
	//int channel = 0;
	//int offset = 0;
	int  recvLen= 0;
	int len = 0;
	//unsigned int pts = 0;
	unsigned char *pBuf = NULL;
	//char tmpBuf[4] = {0};

#if 0
	if(pRtspInfo->pstRTSPDesc->protocol == TRANSPORT_TCP)
	{
		ret = netSocketSelectWait(pRtspInfo->pstRTSPCB->iRTSPTcpSock, 3000);
		if(ret == 1)
		{
			usleep(5*1000);
			recvLen = ring_free_size_ex(&pPacketQueue->ringBuf);
			pBuf = ring_get_start_buf(&pPacketQueue->ringBuf);
			if((recvLen > 0) && (pBuf != NULL))
			{
				recv(pRtspInfo->pstRTSPCB->iRTSPTcpSock, tmpBuf, 4, 0);
				rtp_para * Rtp_info = (rtp_para *)tmpBuf;
				int ret = 0;
				if(Rtp_info->magicnum !=0x24)
				{
					fprintf(stderr, "%s :%d: %x, Rtp_info->size =%d \r\n", __FILE__, __LINE__,tmpBuf[0] , ntohs(Rtp_info->size));
					return -1;
				}
				pPacketQueue->startFlag = 1;
				pRtspInfo->pstRtpParse->channel = Rtp_info->channel;
				*length = ntohs(Rtp_info->size) ;
				int Gotlen = 0;
				int leftLen = ntohs(Rtp_info->size) ;
				if(recvLen < leftLen)
				{
					ring_reset(&pPacketQueue->ringBuf);
					pBuf = ring_get_start_buf(&pPacketQueue->ringBuf);
				}

				//fprintf(stderr, "%s :%d: %x, Rtp_info->size =%d \r\n", __FILE__, __LINE__,tmpBuf[0] , leftLen);
				fprintf(stderr, "%s :%d: %x,channel =%x, Rtp_info->size =%d \r\n", __FILE__, __LINE__,tmpBuf[0] ,tmpBuf[1], leftLen);
				while ( leftLen > 0)
				{
					ret = recv(pRtspInfo->pstRTSPCB->iRTSPTcpSock, pBuf+Gotlen, leftLen, 0);
					fprintf(stderr, "%s :%d: recv %d  \r\n", __FILE__, __LINE__,ret);
					if(ret == -1)
					{
						if (EINTR == errno || EWOULDBLOCK == errno ||errno ==EAGAIN)//
						{
							fprintf(stderr,"socket=%d  ,  %s \n",pRtspInfo->pstRTSPCB->iRTSPTcpSock,strerror(errno));
							usleep(5*1000);
							continue;//接着读取数据
						}
						else
						{
							return -1;
						}
					}
					if(ret < leftLen )
					{
						usleep(2*1000);

					}
					// fprintf(stderr, "%s :%d: recv %d  \r\n", __FILE__, __LINE__,ret);
					Gotlen += ret;
					leftLen -= ret;

				}

				ring_set_data_end(&pPacketQueue->ringBuf,  Gotlen);
			}
		}
		else
		{//sock
			fprintf(stderr,"##################################%s:%d :%s",__func__,__LINE__,strerror(errno));
			len = -1;
		}
	}
	else if(pRtspInfo->pstRTSPDesc->protocol == TRANSPORT_UDP)
	{
		//              channel = RTSP_RTP_UdpRecv(pRtspInfo);
	}




#endif


	if(pRtspInfo->pstRTSPDesc->protocol == TRANSPORT_TCP)
	{
		ret = netSocketSelectWait(pRtspInfo->pstRTSPCB->iRTSPTcpSock, 1000);
		if(ret == 1)
		{
			recvLen = ring_free_size_ex(&pPacketQueue->ringBuf);
			pBuf = ring_get_start_buf(&pPacketQueue->ringBuf);
			if((recvLen > 0) && (pBuf != NULL))
			{
				len = recv(pRtspInfo->pstRTSPCB->iRTSPTcpSock, pBuf, recvLen, 0);
				if(len > 0)
				{
					if(pPacketQueue->startFlag == 0)
					{
						if((pBuf[0] & 0xff) != '$')
						{
							//Printf(" pBuf[0] != $  %c 1111111111111111 len=%d  recvLen =%d\n",pBuf[0],len,recvLen);
							//len = 1;
						}
						else
						{
							pPacketQueue->startFlag = 1;
							ring_set_data_end(&pPacketQueue->ringBuf, len);
						}
					}
					else
					{
						ring_set_data_end(&pPacketQueue->ringBuf, len);
					}
				}
				else
				{
					len = -1;
					 Printf("#########################recvLen =%d%s \r\n",recvLen,strerror(errno));
				}
			}
			else
			{
				len = -1;
				Printf("#########################recvLen =%d%s \r\n",recvLen,strerror(errno));
			}
		}
	}
	else if(pRtspInfo->pstRTSPDesc->protocol == TRANSPORT_UDP)
	{
		//channel = RTSP_RTP_UdpRecv(pRtspInfo);

	}
	return len;
}


int rtspPacketBuffToFrameQueue(rtsp_info_t *pRtspInfo, unsigned char  *pPacketBuff, stream_packet_head_t *pPacketHead, stream_frame_queue_t *pAudioFrameQueue, stream_frame_queue_t *pVideoFrameQueue,
		int *pVideoFrameNo, int *pAudioFrameNo,int Type,int forceRestartNum,int FrameLen,int channelNo,int streamNo,int* countStreamFrameRate)
{
	int frameOffset = 0;
	stream_frame_t *pVideoFrame = NULL, *pAudioFrame = NULL;
	if(Type == 2)
	{
#if 1
		streamFrameQueueLock(pAudioFrameQueue);
		pAudioFrame = streamGetFrameQueueBuffer(pAudioFrameQueue, FrameLen +12 + 4);//4
		if(pAudioFrame != NULL)
		{
			pAudioFrame->frameHead.type = FRAME_TYPE_AUDIO;
			pAudioFrame->frameHead.frameNo = ((*pVideoFrameNo)++)&0xFFFF;
			//			pAudioFrame->frameHead.pts = time(0);//pUsedPacket->packetHead.pts;
			//			pAudioFrame->frameHead.ipcPts = pPacket->packetHead.pts;
			gettimeofday(&pAudioFrame->frameHead.tv, NULL);

			pAudioFrame->frameHead.info.audio.format = pRtspInfo->pstRTSPGlobal->ucAudioMode;//FRAME_AUDIO_TYPE_G711;
			pAudioFrame->frameHead.info.audio.sampleRate = FRAME_AUDIO_SAMPLE_RATE_32K;
			pAudioFrame->frameHead.info.audio.channel = FRAME_AUDIO_CHANNEL_1;
			pAudioFrame->frameHead.info.audio.bitWide = FRAME_AUDIO_BIT_WIDE_16;
			pAudioFrame->frameHead.len = FrameLen  + 4;
			pAudioFrame->pFrameBuf[0] = 0x00;
			pAudioFrame->pFrameBuf[1] = 0x01;
			pAudioFrame->pFrameBuf[2] = 0xA0;
			pAudioFrame->pFrameBuf[3] = 0;

			//memcpy(pAudioFrame->pFrameBuf + 4, &pPacket->packetBuf[pPacket->packetHead.offset + sizeof(rtp_head_t)], audioFrameLen);
			memcpy(pAudioFrame->pFrameBuf +4 ,  pPacketBuff, FrameLen );
			//memcpy(pAudioFrame->pFrameBuf + 4, pPacket->pPacketBuf + pPacket->packetHead.offset + sizeof(rtp_head_t), audioFrameLen);

			streamInsertFrameToFrameQueue(pAudioFrameQueue, pAudioFrame);
		}
		steramFrameQueueUnlock(pAudioFrameQueue);
#endif

	}
	else
	{
		streamFrameQueueLock(pVideoFrameQueue);
		//分配帧队列的地址
		pVideoFrame = streamGetFrameQueueBuffer(pVideoFrameQueue, FrameLen +4);
		if(pVideoFrame != NULL)
		{
			pVideoFrame->frameHead.type = FRAME_TYPE_VIDEO;
			pVideoFrame->frameHead.frameNo = ((*pVideoFrameNo)++)&0xFFFF;
			pVideoFrame->frameHead.source = FRAME_SOURCE_IPC;
			pVideoFrame->frameHead.info.video.format = FRAME_VIDEO_TYPE_H264;
			pVideoFrame->frameHead.info.video.width = pRtspInfo->pstRTSPGlobal->nVideoWidth;
			pVideoFrame->frameHead.info.video.height = pRtspInfo->pstRTSPGlobal->nVideoHeight;
			//pVideoFrame->frameHead.info.video.frameRate = pRtspInfo->pstRTSPGlobal->nFrameRate;
			pVideoFrame->frameHead.info.video.frameRate = networkGetCountStreamFrameRate(channelNo,streamNo); //xg 20150309
			pVideoFrame->frameHead.info.video.bitRate = pRtspInfo->pstRTSPGlobal->nBitRate/1024;
			pVideoFrame->frameHead.info.video.forceRestartNum = forceRestartNum;
			pVideoFrame->frameHead.len = FrameLen + 4;
			frameOffset = 4;
			pVideoFrame->pFrameBuf[0] = 0x00;
			pVideoFrame->pFrameBuf[1] = 0x00;
			pVideoFrame->pFrameBuf[2] = 0x00;
			pVideoFrame->pFrameBuf[3] = 0x01;
			//Printf("pVideoFrame->frameHead.info.video.frameRate =%d\n",pVideoFrame->frameHead.info.video.frameRate);

			if(streamNo==0)
			{
				*countStreamFrameRate = *countStreamFrameRate +1;
				//if(channelNo ==18)
				//Printf("*countStreamFrameRate =%d  channelNo =%d \n",*countStreamFrameRate,channelNo);
			}

			memcpy(pVideoFrame->pFrameBuf+frameOffset, pPacketBuff,FrameLen );
#define SAVE 0

#if SAVE
			static int fd = 0;
			if(fd == 0)
			{
				fd = open("/mnt/h2641.h264", O_CREAT | O_WRONLY);
				if(fd <= 0)
				{
					//  Exit();
				}
			}

			write(fd, pVideoFrame->pFrameBuf, FrameLen+4);
#endif



#if 0
			int i ;
			for(i =0 ;i!=20 ;++i)
			{
				fprintf(stderr,"%02x ",*(pPacketBuff->packetBuf.BufferAddr+i));

			}
			fprintf(stderr,"\r\n");
#endif

			//   Printf("come here,video frame, FrameLen =%d ,%d ,%d ,%d  %d , %d ,%d\r\n",FrameLen,pRtspInfo->pstRTSPGlobal->nVideoWidth
			//          ,pRtspInfo->pstRTSPGlobal->nVideoHeight,pRtspInfo->pstRTSPGlobal->nFrameRate,(pRtspInfo->pstRTSPGlobal->nBitRate/1024)
			//          ,pVideoFrame->frameHead.frameNo,pPacketHead->pts);
			gettimeofday(&pVideoFrame->frameHead.tv, NULL);
			//pVideoFrame->frameHead.tv.tv_sec = pPacketHead->pts/90;
			pVideoFrame->frameHead.pts = pPacketHead->pts; //time(0);
			streamInsertFrameToFrameQueue(pVideoFrameQueue, pVideoFrame);

		}
		steramFrameQueueUnlock(pVideoFrameQueue);

	}
	return 0;
}







int RtspParseRtpPacketInfo(rtsp_info_t *pRtspInfo, stream_packet_queue_t * pPacketBuff,stream_packet_head_t *pPacketHead,stream_frame_queue_t *pAudioFrameQueue,stream_frame_queue_t *pVideoFrameQueue,
		int *pVideoFrameNo, int *pAudioFrameNo,int forceRestartNum,int channelNo,int streamNo,int* countStreamFrameRate)
{
	unsigned char rtspHeadBuf[4]={0};
	unsigned char rtpFU[2] = {0};
	unsigned char RtpHead[12]={0};
	int DataLen =0;
	int Channel =0;
	char FU_indicator = 0;
	char FU_header = 0;
	unsigned char * PacketBuffAddr = NULL;
	unsigned char AudiotempBuf[4096];
	unsigned char UndefineBuf[4096];
	int content_length = 0;
	rtp_head_t * RtpHeadInfo = NULL;
	char * isContent = NULL;
	char * p = NULL;
	int FrameLen = 0;
	char rtspHeadTmpBuf[256] = {0};
	do
	{
		ring_pop_n(&pPacketBuff->ringBuf, rtspHeadBuf, 4,4);
		//  Printf("%02x,channel=%d,datalen =%d \r\n",rtspHeadBuf[0]&0xff,(rtspHeadBuf[1]&0xff),((rtspHeadBuf[2] & 0xff) << 8) + (rtspHeadBuf[3] & 0xff));
		switch(rtspHeadBuf[0]&0xff)
		{
			//rtp packet's header
			case '$':
				Channel = (rtspHeadBuf[1]&0xff);
				DataLen = ((rtspHeadBuf[2] & 0xff) << 8) + (rtspHeadBuf[3] & 0xff);
				pPacketHead->len = DataLen;
				// Printf("DataLen =%d \r\n",DataLen);
				if(DataLen <= 0)
				{
					return -1;
				}
				
				//rtp's channel
				if(0==(Channel%2))
				{
					ring_pop_n(&pPacketBuff->ringBuf,RtpHead, RTP_HEAD_LEN,12);
					RtpHeadInfo = (rtp_head_t *)RtpHead;
					if((RtpHeadInfo->pt == EN_RTP_PT_H264))
					{
						//  Printf("come here\r\n");
						ring_pop_temp(&pPacketBuff->ringBuf,rtpFU,2);
						FU_indicator = rtpFU[0] & 0xff;
						FU_header = rtpFU[1] & 0xff;

						//  Printf("come here indicator =%d   header =%d,fu_a=%d\r\n",FU_indicator,FU_header,(FU_indicator & 0x1F));
						pPacketHead->pts = htonl(RtpHeadInfo->ts);
						if( (pPacketHead->seq + 1) != htons(RtpHeadInfo->seqno))
						{
							//Printf("%s:lost packet , seqNo = %d ~ %d\r\n", pRtspInfo->pstRTSPDesc->hostAddress ,pPacketHead->seq, htons(RtpHeadInfo->seqno));
						}
						pPacketHead->seq = htons(RtpHeadInfo->seqno);
						if(RTP_NAL_FU_A == (FU_indicator & 0x1F))
						{
							ring_pop_n(&pPacketBuff->ringBuf,rtpFU,2,2);
							//Frame start
							if(1== (FU_header >> 7))
							{
								int  Nal_unit_type  =  ((FU_header & 0x1F) | (FU_indicator & 0xE0));
								if((Nal_unit_type &31) ==NAL_TYPE_IDR )
								{
									pPacketHead->isKey=1;
								}
								PacketBuffAddr = GetPacketBufferAddr(&pPacketBuff->packetBuf,FRAME_START);
								PacketBuffAddr[0] = Nal_unit_type ;
								ring_pop_n(&pPacketBuff->ringBuf,PacketBuffAddr+1,DataLen-RTP_HEAD_LEN-2,MAX_FRAME_BUFFER_LEN);
								SetPacketBufferEnd(&pPacketBuff->packetBuf,DataLen- RTP_HEAD_LEN -1);

							}
							// Frame end
							else if(1== ((rtpFU[1]&0x40) >> 6)&0xff)
							{
								PacketBuffAddr = GetPacketBufferAddr(&pPacketBuff->packetBuf,FRAME_NOT_START);
								ring_pop_n(&pPacketBuff->ringBuf,PacketBuffAddr,DataLen- RTP_HEAD_LEN -2,MAX_FRAME_BUFFER_LEN);
								SetPacketBufferEnd(&pPacketBuff->packetBuf,DataLen- RTP_HEAD_LEN -2 );
								FrameLen = GetPacketBufferEnd(&pPacketBuff->packetBuf);
								
								rtspPacketBuffToFrameQueue(pRtspInfo,pPacketBuff->packetBuf.BufferAddr,pPacketHead,pAudioFrameQueue,pVideoFrameQueue,pVideoFrameNo,pAudioFrameNo,0,forceRestartNum\
									,FrameLen,channelNo,streamNo,countStreamFrameRate);
							}
							//the middle of frame
							else
							{
								PacketBuffAddr = GetPacketBufferAddr(&pPacketBuff->packetBuf,FRAME_NOT_START);
								ring_pop_n(&pPacketBuff->ringBuf,PacketBuffAddr,DataLen- RTP_HEAD_LEN -2,MAX_FRAME_BUFFER_LEN);
								SetPacketBufferEnd(&pPacketBuff->packetBuf,DataLen- RTP_HEAD_LEN -2 );

							}
						}
						//sps or pps or other type frame
						else if((1 <= (FU_indicator & 0x1F)) && (23 >= (FU_indicator & 0x1F)))
						{
							if((FU_indicator & 0x1F) == NAL_TYPE_SPS)
							{
								//pPacketHead->isKey=1;

							}
							PacketBuffAddr = GetPacketBufferAddr(&pPacketBuff->packetBuf,FRAME_START);
							ring_pop_n(&pPacketBuff->ringBuf,PacketBuffAddr,DataLen- RTP_HEAD_LEN,MAX_FRAME_BUFFER_LEN);
							SetPacketBufferEnd(&pPacketBuff->packetBuf,DataLen- RTP_HEAD_LEN );
							FrameLen = GetPacketBufferEnd(&pPacketBuff->packetBuf);
							//  Printf("come here, FrameLen=%d \r\n", FrameLen);
							rtspPacketBuffToFrameQueue(pRtspInfo,pPacketBuff->packetBuf.BufferAddr,pPacketHead,pAudioFrameQueue,pVideoFrameQueue,pVideoFrameNo,pAudioFrameNo,0,forceRestartNum\
							,FrameLen,channelNo,streamNo,countStreamFrameRate);
						}
						//undefined NALU type
						else
						{
							Printf(" %s  :invalid NAL type, ucnaltype=[%02x],   %d start=%d \n",pRtspInfo->pstRTSPDesc->hostAddress, FU_indicator ,DataLen,pPacketBuff->ringBuf.dataStart);
							return -1;
						}
					}
					else if(RtpHeadInfo->pt == EN_RTP_PT_H264 +1)
					{
						//if a audio frame is coming  while receiving i or p frame, which will disorder the buffer
						// PacketBuffAddr = GetPacketBufferAddr(&pPacketBuff->packetBuf,FRAME_START);
						ring_pop_n(&pPacketBuff->ringBuf,AudiotempBuf,DataLen- RTP_HEAD_LEN,4096);
						// SetPacketBufferEnd(&pPacketBuff->packetBuf,DataLen- RTP_HEAD_LEN );
						// FrameLen = GetPacketBufferEnd(&pPacketBuff->packetBuf);
						 //Printf(" %s : come here, audio FrameLen=%d \r\n",pRtspInfo->pstRTSPDesc->hostAddress, FrameLen);
						 rtspPacketBuffToFrameQueue(pRtspInfo,AudiotempBuf,pPacketHead,pAudioFrameQueue,pVideoFrameQueue,pVideoFrameNo,pAudioFrameNo,2,forceRestartNum,DataLen- RTP_HEAD_LEN,
						 	channelNo,streamNo,countStreamFrameRate);

					}
					else if(RtpHeadInfo->pt == EN_RTP_PT_ULAW)
					{
						//if a audio frame is coming  while receiving i or p frame, which will disorder the buffer
						// PacketBuffAddr = GetPacketBufferAddr(&pPacketBuff->packetBuf,FRAME_START);
						ring_pop_n(&pPacketBuff->ringBuf,AudiotempBuf,DataLen- RTP_HEAD_LEN,4096);
						// SetPacketBufferEnd(&pPacketBuff->packetBuf,DataLen- RTP_HEAD_LEN );
						// FrameLen = GetPacketBufferEnd(&pPacketBuff->packetBuf);
						// Printf(" %s : come here, G711  audio FrameLen=%d \r\n",pRtspInfo->pstRTSPDesc->hostAddress, DataLen- RTP_HEAD_LEN);
						 rtspPacketBuffToFrameQueue(pRtspInfo,AudiotempBuf,pPacketHead,pAudioFrameQueue,pVideoFrameQueue,pVideoFrameNo,pAudioFrameNo,2,forceRestartNum,DataLen- RTP_HEAD_LEN,
						 	channelNo,streamNo,countStreamFrameRate);

					}
					else
					{
						//undefined payload                      
					//	 Printf(" %s : undefined payload ,come here, FrameLen=%d  ,payload=%d\r\n", pRtspInfo->pstRTSPDesc->hostAddress ,DataLen,RtpHeadInfo->pt);
						//PacketBuffAddr = GetPacketBufferAddr(&pPacketBuff->packetBuf,FRAME_START);
						ring_pop_n(&pPacketBuff->ringBuf,UndefineBuf,DataLen -RTP_HEAD_LEN,4096);
						//SetPacketBufferEnd(&pPacketBuff->packetBuf,DataLen- RTP_HEAD_LEN );
					//	 FrameLen = GetPacketBufferEnd(&pPacketBuff->packetBuf);
					// rtspPacketBuffToFrameQueue(pRtspInfo,pPacketBuff,pPacketHead,pAudioFrameQueue,pVideoFrameQueue,pVideoFrameNo,pAudioFrameNo,2,forceRestartNum,FrameLen);

					}
				}
				//rtcp's channel
				else
				{
					//  Printf("%s : come here rtcp's channel =%d \r\n", pRtspInfo->pstRTSPDesc->hostAddress,Channel);
					// to make sure the rtcp response packet got out of the packet buffer completely.
					ring_set_data_start(&pPacketBuff->ringBuf, DataLen);
					continue;
				}
				break;


				//get_parameter command's response
			case 'R':
				ring_pop_temp(&pPacketBuff->ringBuf, rtspHeadTmpBuf, sizeof(rtspHeadTmpBuf));
				if((isContent= my_strrstr(rtspHeadTmpBuf, "Content-Length:"))!=NULL)
				{
					content_length =  atoi(isContent+15);
					//  Printf("@@@@@@@@@@@@@@@@@@@@@ content_length =%d\n",content_length);
				}
				p= my_strrstr(rtspHeadTmpBuf, "\r\n");
				if(p==NULL)
				{
					// Printf("@@@@@@@@@@@@@@@@@@@@@ not found the bound\n");
				}
				int len_temp = p - rtspHeadTmpBuf;
				ring_set_data_start(&pPacketBuff->ringBuf, len_temp+content_length+2);
				 //Printf("%s : Replay,isContent=%d,len_temp=%d !\r\n", pRtspInfo->pstRTSPDesc->hostAddress,content_length,len_temp);
				continue;

			default:
				 Printf("%s : #################################err!\r\n", pRtspInfo->pstRTSPDesc->hostAddress);
				 Printf("%02x,%02x,%02x,%02x, start=%d DataLen=%d \r\n",rtspHeadBuf[0],rtspHeadBuf[1],rtspHeadBuf[2]  ,rtspHeadBuf[3],pPacketBuff->ringBuf.dataStart,DataLen);
				return -1;
		}

	}while(0);


	return 0;


}



#if 0


/* bufferrtsp */
int rtspParseStreamFromRingBuf(rtsp_info_t *pRtspInfo, stream_packet_queue_t * pPacketQueue, stream_packet_t *pPacket, int packetBufLen, stream_packet_head_t *pPacketHead, int *pVideoFrameLen )
{
	int ret = 1;
	int channel = 0;
	int offset = 0;
	int len = 0;
	int  mallocLen = 0;
	unsigned int pts = 0;
	char rtspHeadBuf[4] = {0};
	char rtspHeadTmpBuf[256] = {0};
	char * p=NULL;
	char * isContent = NULL;
	int isKeyFrame=0;
	ring_pop_n(&pPacketQueue->ringBuf, rtspHeadBuf, 4);
	// Printf("##########################  %s: 0x%02x.0x%02x.0x%02x.0x%02x\r\n  ",pRtspInfo->pstRTSPDesc->hostAddress, rtspHeadBuf[0], rtspHeadBuf[1], rtspHeadBuf[2], rtspHeadBuf[3]);

	ret =getRtspBufInfo(rtspHeadBuf, &channel, &len);
	int content_length = 0;
	if(1==ret)
	{
		if((channel%2)==0)
		{
			// mallocLen = ((len / 512) + 1) * 512;
			//  pPacket->pPacketBuf = malloc(mallocLen);
			//ring_pop_n(&pPacketQueue->ringBuf, pPacket->pPacketBuf, len);
			ring_pop_n(&pPacketQueue->ringBuf, pPacketQueue->pPacketBuf[pPacketQueue->usedBufNum].pPacketBuf, len);
			ret = RTSP_GetStreamFrameInfo(pRtspInfo, pPacketQueue->pPacketBuf[pPacketQueue->usedBufNum].pPacketBuf, len, &offset, pVideoFrameLen, &pts, &channel,&isKeyFrame);
			if(channel < 0)
			{
				Printf("###########channel < 0 !\r\n");
				ret = -1;
			}
			pPacketHead->channel = channel;
			pPacketHead->offset = offset;
			pPacketHead->pts = pts;
			pPacketHead->len = len - offset;
			pPacketHead->isKey = isKeyFrame;
		}
		else
		{
			ring_set_data_start(&pPacketQueue->ringBuf, len);
			ring_pop_n(&pPacketQueue->ringBuf, rtspHeadBuf, 4);
			ret =getRtspBufInfo(rtspHeadBuf, &channel, &len);
			if(1 == ret)
			{
				// mallocLen = ((len / 512) + 1) * 512;
				//pPacket->pPacketBuf = malloc(mallocLen);
				//ring_pop_n(&pPacketQueue->ringBuf, pPacket->pPacketBuf, len);
				ring_pop_n(&pPacketQueue->ringBuf, pPacketQueue->pPacketBuf[pPacketQueue->usedBufNum].pPacketBuf, len);
				ret = RTSP_GetStreamFrameInfo(pRtspInfo, pPacketQueue->pPacketBuf[pPacketQueue->usedBufNum].pPacketBuf, len, &offset, pVideoFrameLen, &pts, &channel,&isKeyFrame);
				if(channel < 0)
				{
					Printf("###########channel < 0 !\r\n");
					ret = -1;
				}
				pPacketHead->channel = channel;
				pPacketHead->offset = offset;
				pPacketHead->pts = pts;
				pPacketHead->len = len - offset;
				pPacketHead->isKey = isKeyFrame;
			}
			else if(0==ret)
			{
				ring_pop_temp(&pPacketQueue->ringBuf, rtspHeadTmpBuf, 256);
				if((isContent= my_strrstr(rtspHeadTmpBuf, "Content-Length:"))!=NULL)
				{
					content_length =  atoi(isContent+15);
					//   Printf("@@@@@@@@@@@@@@@@@@@@@ content_length =%d\n",content_length);

				}
				p= my_strrstr(rtspHeadTmpBuf, "\r\n");
				if(p==NULL)
				{
					ret =-1;
					Printf("@@@@@@@@@@@@@@@@@@@@@ not found the bound\n");
				}

				int len_temp = p - rtspHeadTmpBuf;
				ring_set_data_start(&pPacketQueue->ringBuf, len_temp+content_length+2);
				ring_pop_n(&pPacketQueue->ringBuf, rtspHeadBuf, 4);
				ret =getRtspBufInfo(rtspHeadBuf, &channel, &len);
				if(ret == 1)
				{
					//mallocLen = ((len / 512) + 1) * 512;
					// pPacket->pPacketBuf = malloc(mallocLen);
					// ring_pop_n(&pPacketQueue->ringBuf, pPacket->pPacketBuf, len);
					ring_pop_n(&pPacketQueue->ringBuf, pPacketQueue->pPacketBuf[pPacketQueue->usedBufNum].pPacketBuf, len);
					ret = RTSP_GetStreamFrameInfo(pRtspInfo, pPacketQueue->pPacketBuf[pPacketQueue->usedBufNum].pPacketBuf, len, &offset, pVideoFrameLen, &pts, &channel,&isKeyFrame);
					if(channel < 0)
					{
						Printf("###########channel < 0 !\r\n");
						ret = -1;
					}
					pPacketHead->channel = channel;
					pPacketHead->offset = offset;
					pPacketHead->pts = pts;
					pPacketHead->len = len - offset;
					pPacketHead->isKey = isKeyFrame;

				}
				else
				{
					Printf("##########################%s:%d   %d, offset = %d, len = %d, channel = %d, 0x%02x.0x%02x.0x%02x.0x%02x\r\n",__func__,__LINE__, len, offset, pPacketHead->len, channel, rtspHeadBuf[0], rtspHeadBuf[1], rtspHeadBuf[2], rtspHeadBuf[3]);
					ret = -1;
				}
			}
		}
	}
	else if(0==ret)
	{
		ring_pop_temp(&pPacketQueue->ringBuf, rtspHeadTmpBuf, 256);
		if((isContent= my_strrstr(rtspHeadTmpBuf, "Content-Length:"))!=NULL)
		{
			content_length =  atoi(isContent+15);
			//   Printf("@@@@@@@@@@@@@@@@@@@@@ content_length =%d\n",content_length);

		}
		p= my_strrstr(rtspHeadTmpBuf, "\r\n");
		if(p==NULL)
		{
			ret =-1;
			Printf("@@@@@@@@@@@@@@@@@@@@@ not found the bound\n");
		}

		int len_temp = p - rtspHeadTmpBuf;
		ring_set_data_start(&pPacketQueue->ringBuf, len_temp+content_length+2);
		ring_pop_n(&pPacketQueue->ringBuf, rtspHeadBuf, 4);
		ret =getRtspBufInfo(rtspHeadBuf, &channel, &len);
		if(ret == 1)
		{
			//mallocLen = ((len / 512) + 1) * 512;
			// pPacket->pPacketBuf = malloc(mallocLen);
			//ring_pop_n(&pPacketQueue->ringBuf, pPacket->pPacketBuf, len);
			ring_pop_n(&pPacketQueue->ringBuf, pPacketQueue->pPacketBuf[pPacketQueue->usedBufNum].pPacketBuf, len);
			ret = RTSP_GetStreamFrameInfo(pRtspInfo, pPacketQueue->pPacketBuf[pPacketQueue->usedBufNum].pPacketBuf, len, &offset, pVideoFrameLen, &pts, &channel,&isKeyFrame);
			if(channel < 0)
			{
				Printf("###########channel < 0 !\r\n");
				ret = -1;
			}
			pPacketHead->channel = channel;
			pPacketHead->offset = offset;
			pPacketHead->pts = pts;
			pPacketHead->len = len - offset;
			pPacketHead->isKey = isKeyFrame;

		}

		else
		{
			//Printf("##########################%s:%d   %d, offset = %d, len = %d, channel = %d, 0x%02x.0x%02x.0x%02x.0x%02x\r\n",__func__,__LINE__, len, offset, pPacketHead->len, channel, rtspHeadBuf[0], rtspHeadBuf[1], rtspHeadBuf[2], rtspHeadBuf[3]);
			ret = -1;
		}

	}
	else
	{

		//Printf("##########################%s:%d   %d, offset = %d, len = %d, channel = %d, 0x%02x.0x%02x.0x%02x.0x%02x\r\n",__func__,__LINE__, len, offset, pPacketHead->len, channel, rtspHeadBuf[0], rtspHeadBuf[1], rtspHeadBuf[2], rtspHeadBuf[3]);
		ret = -1;
	}
	return ret;
}

/* rtsp 组帧 */
int rtspPacketToFrameQueue(rtsp_info_t *pRtspInfo, stream_packet_queue_t *pPacketQueue, stream_packet_t *pPacket, stream_frame_queue_t *pAudioFrameQueue, stream_frame_queue_t *pVideoFrameQueue,
		int forceRestartNum, int packetFlg, int *pVideoFrameLen, int *pVideoFrameNo, int *pAudioFrameNo)
{
	int videoFrameLen = 0, audioFrameLen = 0;
//	int displayNum = 0, displayChannel = 0;//需要显示的通道的数目
//	int decodeWidth = 0, decodeHeight = 0;//需要解码的宽高
	int frameOffset = 0;//组帧时，已拷贝的帧数据的长度
	stream_packet_t * pUsedPacket = NULL;
	stream_frame_t *pVideoFrame = NULL, *pAudioFrame = NULL;

	/* 此处逻辑，在增加音频时，需要修改 */
	if(pPacket->packetHead.channel == 2)
	{//
#if 1
		audioFrameLen = pPacket->packetHead.len;
		streamFrameQueueLock(pAudioFrameQueue);
		pAudioFrame = streamGetFrameQueueBuffer(pAudioFrameQueue, audioFrameLen + 4);//海思需要4字节的头
		if(pAudioFrame != NULL)
		{
			pAudioFrame->frameHead.type = FRAME_TYPE_AUDIO;
			pAudioFrame->frameHead.frameNo = ((*pVideoFrameNo)++)&0xFFFF;
			//			pAudioFrame->frameHead.pts = time(0);//pUsedPacket->packetHead.pts;
			//			pAudioFrame->frameHead.ipcPts = pPacket->packetHead.pts;
			gettimeofday(&pAudioFrame->frameHead.tv, NULL);
			pAudioFrame->frameHead.info.audio.format = pRtspInfo->pstRTSPGlobal->ucAudioMode;//FRAME_AUDIO_TYPE_G711;
			pAudioFrame->frameHead.info.audio.sampleRate = FRAME_AUDIO_SAMPLE_RATE_32K;
			pAudioFrame->frameHead.info.audio.channel = FRAME_AUDIO_CHANNEL_1;
			pAudioFrame->frameHead.info.audio.bitWide = FRAME_AUDIO_BIT_WIDE_16;
			pAudioFrame->frameHead.len = audioFrameLen - sizeof(rtp_head_t) + 4;
			pAudioFrame->pFrameBuf[0] = 0x00;
			pAudioFrame->pFrameBuf[1] = 0x01;
			pAudioFrame->pFrameBuf[2] = 0xA0;
			pAudioFrame->pFrameBuf[3] = 0;
			//memcpy(pAudioFrame->pFrameBuf + 4, &pPacket->packetBuf[pPacket->packetHead.offset + sizeof(rtp_head_t)], audioFrameLen);
			memcpy(pAudioFrame->pFrameBuf + 4, pPacketQueue->pPacketBuf[pPacketQueue->copyBufNum].pPacketBuf + pPacket->packetHead.offset + sizeof(rtp_head_t), audioFrameLen);
			//memcpy(pAudioFrame->pFrameBuf + 4, pPacket->pPacketBuf + pPacket->packetHead.offset + sizeof(rtp_head_t), audioFrameLen);

			streamInsertFrameToFrameQueue(pAudioFrameQueue, pAudioFrame);
		}
		steramFrameQueueUnlock(pAudioFrameQueue);
		AudiostreamFreeUsedBuffer(pPacketQueue, pPacket);
#endif
	}
	else
	{

		videoFrameLen = *pVideoFrameLen;
		//把packet放在链表的末端，形成完整的一帧
		streamInsertQueue(pPacketQueue, pPacket);
		//当出现一帧的结束标志时
		if(packetFlg == 3)
		{//一帧的结束, 则将数据插入帧队列中
			streamFrameQueueLock(pVideoFrameQueue);
			//分配帧队列的地址
			pVideoFrame = streamGetFrameQueueBuffer(pVideoFrameQueue, videoFrameLen);
			if(pVideoFrame != NULL)
			{
				pVideoFrame->frameHead.type = FRAME_TYPE_VIDEO;
				pVideoFrame->frameHead.frameNo = ((*pVideoFrameNo)++)&0xFFFF;
				pVideoFrame->frameHead.source = FRAME_SOURCE_IPC;
				pVideoFrame->frameHead.info.video.format = FRAME_VIDEO_TYPE_H264;
				pVideoFrame->frameHead.info.video.width = pRtspInfo->pstRTSPGlobal->nVideoWidth;
				pVideoFrame->frameHead.info.video.height = pRtspInfo->pstRTSPGlobal->nVideoHeight;
				pVideoFrame->frameHead.info.video.frameRate = pRtspInfo->pstRTSPGlobal->nFrameRate;
				pVideoFrame->frameHead.info.video.bitRate = pRtspInfo->pstRTSPGlobal->nBitRate/1024;
				pVideoFrame->frameHead.info.video.forceRestartNum = forceRestartNum;
				pVideoFrame->frameHead.len = videoFrameLen;
				frameOffset = 4;
				pVideoFrame->pFrameBuf[0] = 0x00;
				pVideoFrame->pFrameBuf[1] = 0x00;
				pVideoFrame->pFrameBuf[2] = 0x00;
				pVideoFrame->pFrameBuf[3] = 0x01;
				//循环将packet里一帧的码流数据拷贝到frame缓冲里
				while(1)
				{
					pUsedPacket = streamGetPacketUsedBuffer(pPacketQueue);
					if(pUsedPacket == NULL)
					{
						break;
					}
					if(pUsedPacket->packetHead.channel != 0)
					{//不拷贝音频帧
						streamFreeUsedBuffer(pPacketQueue, pUsedPacket);
					}
					else
					{//视频
						if(frameOffset + pUsedPacket->packetHead.len <= videoFrameLen)
						{////总长度在分配的缓冲长度之内

							//memcpy(pVideoFrame->pFrameBuf+frameOffset, &pUsedPacket->packetBuf[pUsedPacket->packetHead.offset], pUsedPacket->packetHead.len);
							memcpy(pVideoFrame->pFrameBuf+frameOffset, pPacketQueue->pPacketBuf[pPacketQueue->copyBufNum].pPacketBuf + pUsedPacket->packetHead.offset, pUsedPacket->packetHead.len);
							//memcpy(pVideoFrame->pFrameBuf+frameOffset, pUsedPacket->pPacketBuf + pUsedPacket->packetHead.offset, pUsedPacket->packetHead.len);
							frameOffset += pUsedPacket->packetHead.len;
							//							pVideoFrame->frameHead.pts = time(0);//pUsedPacket->packetHead.pts;
							//							pVideoFrame->frameHead.ipcPts = pUsedPacket->packetHead.pts;
							gettimeofday(&pVideoFrame->frameHead.tv, NULL);
							pVideoFrame->frameHead.pts = pUsedPacket->packetHead.pts; //time(0);
							streamFreeUsedBuffer(pPacketQueue, pUsedPacket);

						}
						else
						{
							streamFreeUsedBuffer(pPacketQueue, pUsedPacket);
							break;
						}
					}
				}
				if(frameOffset != videoFrameLen)
				{//数据有问题，则不插入帧队列
					Printf("frame error, frameOffset = %d, videoFrameLen = %d\r\n", frameOffset, videoFrameLen);
					streamFreeFrameBuffer(pVideoFrameQueue, pVideoFrame);
				}
				else
				{//插入帧队列
#if 0 //打印网络接收包，组帧的耗时时长
					static struct timeval tv, tv1;
					gettimeofday(&tv, NULL);
					unsigned int tmpValue = (tv.tv_sec - tv1.tv_sec)*1000 + (tv.tv_usec - tv1.tv_usec)/1000;
					if(tmpValue >= 0)
					{
						Printf("frame type 0x%x, interval time %d ms\r\n", pVideoFrame->pFrameBuf[4], tmpValue);
					}
					gettimeofday(&tv1, NULL);
#endif
					streamInsertFrameToFrameQueue(pVideoFrameQueue, pVideoFrame);
				}
			}
			steramFrameQueueUnlock(pVideoFrameQueue);
			videoFrameLen = 0;
			*pVideoFrameLen = 0;
		}
	}

	return 0;
}

#endif

/*   */
void rtspCloseStream(rtsp_info_t *pRtspInfo)
{
	if(pRtspInfo->pstRTSPCB->iRTSPTcpSock >0)
	{
		RTSP_SendTeardownMsg(pRtspInfo);
	}
	RTSP_Exit(pRtspInfo);
}


int rtspParseDetailURL(const char * url, char* address,int* portNum,char * filename)
{
	// fprintf(stderr,"########### %s\n",url);
	do {
		char const* prefix = "rtsp://";
		unsigned const prefixLength = 7;
		if (strncasecmp(url, prefix, prefixLength) != 0)
		{
			break;
		}
		unsigned const parseBufferSize = 100;
		char const* from = &url[prefixLength];
		char const* colonPasswordStart = NULL;
		char const* p;
		for (p = from; *p != '\0' && *p != '/'; ++p)
		{
			if (*p == ':' && colonPasswordStart == NULL)
			{
				colonPasswordStart = p;
			}
			else if (*p == '@')
			{
				if (colonPasswordStart == NULL) colonPasswordStart = p;

				char const* usernameStart = from;
				unsigned usernameLen = colonPasswordStart - usernameStart;
				unsigned i;
				for ( i = 0; i < usernameLen; ++i)
					;
				char const* passwordStart = colonPasswordStart;
				if (passwordStart < p) ++passwordStart;

				unsigned passwordLen = p - passwordStart;
				unsigned j;
				for ( j= 0; j < passwordLen; ++j)
					;
				from = p + 1;
				break;
			}
		}

		char *to = &address[0];
		unsigned i;
		for (i = 0; i < parseBufferSize; ++i)
		{
			if (*from == '\0' || *from == ':' || *from == '/')
			{

				*to = '\0';
				break;
			}
			*to++ = *from++;
		}
		if (i == parseBufferSize)
		{
			Printf("URL is too long");
			break;
		}

		*portNum = 554;
		char nextChar = *from;
		if (nextChar == ':')
		{
			int portNumInt;
			if (sscanf(++from, "%d", &portNumInt) != 1)
			{
				Printf("No port number follows ':'");
				break;
			}
			if (portNumInt < 1 || portNumInt > 65535)
			{
				Printf("Bad port number");
				break;
			}
			*portNum = portNumInt;
			while (*from >= '0' && *from <= '9') ++from;
		}
		strcpy(filename,from+1);
		return 1;

	} while (0);

	return -1;
}
