/***********************************************************************************
 * Filename:	networkLib.c

 * Description:
 ***********************************************************************************/

#include <assert.h>

#include "networkLib.h"
#include "streamLib.h"
#include "rtspCLib.h"
#include "rtspSLib.h"
#include "commonLib.h"
#include "global.h"
#include "configLib.h"
#include "myThreadLib.h"
#include "onvif_client.h"
#include "privateLib.h"
#include "pteClient.h"
#include "ntp.h"
#include "sdk_struct.h"


#define USED_LIB_EVENT			0
#define USED_THREAD_POOL		1//是否使用线程池
#define MAX_NETWORK_SUPPORT_NUM		1024//最多允许的网络请求的数目
#define MAX_STREAM_NO 2
static network_dev_channel_info_t g_networkDevChannel[MAX_SUPPORT_DEV_NUM][MAX_SUPPORT_DEV_CHANNEL_NUM];
static network_dev_channel_info_t *gp_networkDevChannel[MAX_NETWORK_SUPPORT_NUM];
static unsigned int g_networkStreamFrameRate[MAX_DISPLAY_NUM] ={25};
static  sdk_stream_info_t g_networkRecvBitrate[MAX_DISPLAY_NUM];
static  sdk_stream_info_t g_tmpNetworkRecvBitrate[MAX_DISPLAY_NUM];
static pthread_mutex_t g_networkMutex;
static pthread_mutex_t g_msgMutex;
static pthread_mutex_t g_streamMutex;
static pthread_mutex_t g_streamMutex;
pthread_mutex_t g_loginstatus;
static pthread_t g_threadId;

static int getNetworkInfoByOnvif(network_info_t *pNetworkInfo, int streamNo);
static int networkOnvifEventSubscribe(network_info_t *pNetworkInfo);

int setNetworkChannelRecvBitrate(unsigned int channelNo,unsigned int streamNo,unsigned int bitrate)
{
	if((channelNo >= 0) && (channelNo <= MAX_SUPPORT_DEV_NUM) && (streamNo >= 0) && (streamNo <= MAX_SUPPORT_DEV_CHANNEL_NUM))
	{
	    if(streamNo == 0)
	    	{
	    	  
		      g_networkRecvBitrate[channelNo].MainRecvBitRate= bitrate;
			 // Printf("mainrecvrate =%d \r\n",g_networkRecvBitrate[channelNo].MainRecvBitRate *8/1024);
	    	}
		else if(streamNo == 1)
			{
			
               g_networkRecvBitrate[channelNo].SubRecvBitRate= bitrate;
		 //Printf("subrecvrate =%d \r\n",g_networkRecvBitrate[channelNo].SubRecvBitRate *8/1024);
		    }
	}
	return 0;
}



int SetRecvBit(int * key, int channelNo, int streamNo,int len , int *FrameNO ,int *pVideoFrameNo)
{
       int framerate = 0;
		if((*key == 1))
		{
			framerate =   *pVideoFrameNo - *FrameNO ;
			if(framerate >=0 && framerate < 33)
			{
			  if(streamNo==0)
			    	{
			    	  g_networkRecvBitrate[channelNo].MainRecvBitRate= g_tmpNetworkRecvBitrate[channelNo].MainRecvBitRate;
			    	}
				else if(streamNo==1)
					{
					  g_networkRecvBitrate[channelNo].SubRecvBitRate= g_tmpNetworkRecvBitrate[channelNo].SubRecvBitRate;
			    	}
			}
			else if(framerate >= 33 &&  framerate < 55)
			{
			     if(streamNo==0)
			    	{
			    	  g_networkRecvBitrate[channelNo].MainRecvBitRate= g_tmpNetworkRecvBitrate[channelNo].MainRecvBitRate/2;
			    	}
				else if(streamNo==1)
					{
					  g_networkRecvBitrate[channelNo].SubRecvBitRate= g_tmpNetworkRecvBitrate[channelNo].SubRecvBitRate/2;
			    	}	 

			}
			else if(framerate >= 55 && framerate <82)
			{
			    if(streamNo==0)
			    	{
			    	  g_networkRecvBitrate[channelNo].MainRecvBitRate= g_tmpNetworkRecvBitrate[channelNo].MainRecvBitRate/3;
			    	}
				else if(streamNo==1)
					{
					  g_networkRecvBitrate[channelNo].SubRecvBitRate= g_tmpNetworkRecvBitrate[channelNo].SubRecvBitRate/3;
			    	}	    
			}
			else if( framerate >= 82)
			{

              if(streamNo==0)
			    	{
			    	  g_networkRecvBitrate[channelNo].MainRecvBitRate= g_tmpNetworkRecvBitrate[channelNo].MainRecvBitRate/4;
			    	}
				else if(streamNo==1)
					{
					  g_networkRecvBitrate[channelNo].SubRecvBitRate= g_tmpNetworkRecvBitrate[channelNo].SubRecvBitRate/4;
			    	}	
				
			}
					
			 if(streamNo == 0)
            {
			      g_tmpNetworkRecvBitrate[channelNo].MainRecvBitRate= len;
            }
			else if(streamNo ==1)
			{
                 g_tmpNetworkRecvBitrate[channelNo].SubRecvBitRate= len;
			 }
			
			*key =0;
			networkSetCountStreamFrameRate(channelNo,streamNo,framerate);
			framerate =0;
			*FrameNO = *pVideoFrameNo;
		}
		else 
		{
			 if(streamNo == 0)
            	{
			      g_tmpNetworkRecvBitrate[channelNo].MainRecvBitRate += len;
            	}
			else if(streamNo ==1)
				{
                 g_tmpNetworkRecvBitrate[channelNo].SubRecvBitRate += len;
			    }
		}
}

int getNetworkChannelRecvBitrate(unsigned int channelNo,unsigned int streamNo,void *pRecvData, int* recvDataLen ,int* exten)
{
	//Printf("4444444444444444444444444 channelNo=%d \n ",channelNo);
	if((channelNo >= 0) && (channelNo <= MAX_DISPLAY_NUM) && (streamNo >= 0) && (streamNo <= MAX_SUPPORT_DEV_CHANNEL_NUM))
	{
	
		memcpy(pRecvData,&g_networkRecvBitrate[channelNo],sizeof(g_networkRecvBitrate[channelNo]));
		*recvDataLen =sizeof(g_networkRecvBitrate[channelNo]);
		
	}
	if(channelNo ==0xFA)
	{
		memcpy(pRecvData,g_networkRecvBitrate,sizeof(g_networkRecvBitrate));
		*recvDataLen = sizeof(g_networkRecvBitrate);
		
	}
	return 0;
}

int GetMessageFromQueue(int channelNo , int streamNo )
{
	int runFlag = -1;
	int ret = 0 ;
	int queueNum = 0 ;
	link_queue_data_t networkMsg = {0};
	// memset(&networkMsg, 0, sizeof(link_queue_data_t));
	pthread_mutex_lock(&g_msgMutex);
	ret = queueLinkOut(&g_networkDevChannel[channelNo][streamNo].msgQueue, &networkMsg);
	queueNum = queueLinkLength(g_networkDevChannel[channelNo][streamNo].msgQueue);
	if(queueNum > 0)
	{
		Printf("networkThreadInfo channelNo %d, streamNo %d, msg left msg num %d\r\n", channelNo, streamNo, queueNum);
	}
	pthread_mutex_unlock(&g_msgMutex);

	if(ret == 0)
	{
		switch(networkMsg.type)
		{
			case NETWORK_MSG_QUEUE_TYPE_PASUE:
				{
					runFlag = FLAG_PAUSE;
					break;
				}
			case NETWORK_MSG_QUEUE_TYPE_RUN:
				{
					runFlag = FLAG_RUN;
					break;
				}
			case NETWORK_MSG_QUEUE_TYPE_EXIT:
				{
					runFlag = FLAG_EXIT;
					break;
				}
			case NETWORK_MSG_QUEUE_TYPE_RESTART:
				{
					runFlag = FLAG_RESTART;
					break;
				}
			case NETWORK_MSG_QUEUE_TYPE_STOP:
				{
					runFlag = FLAG_STOP;
					break;
				}
			case NETWORK_MSG_QUEUE_TYPE_NEXT:
				{
					runFlag = FLAG_PATROL_NEXT_CHANNEL;
					break;
				}
			case NETWORK_MSG_QUEUE_TYPE_WAIT:
				{
					runFlag = FLAG_WAIT;
					break;
				}
			default:
				break;
		}
		networkSetRunFlag(channelNo, streamNo, runFlag);
	}
	return runFlag;

}





int RTSP_GetStream(rtsp_info_t *pRtspInfo, network_info_t networkInfo, stream_packet_queue_t *pPacketQueue, stream_packet_head_t *pPacketHead, stream_frame_queue_t *pAudioFrameQueue,stream_frame_queue_t*pVideoFrameQueue,
		int forceRestartNum,  int* pVideoFrameNo, int* pAudioFrameNo,int channelNo, int streamNo,int * connectFlag,int * FrameNO)
{
	int ret = 0;
	int runFlag = -1;
	int KeepAlive = 1;
	int length = 0;
	int packetBufLen = MAX_STREAM_PACKET_SIZE;
	int timeoutNum = 0;
	int BufferSize = 0;
	int countStreamFrameRate =0;
	int framerate = 0;
	ret = rtspInviteStream(pRtspInfo, channelNo, streamNo, networkInfo.addr, networkInfo.port, networkInfo.userName, networkInfo.userPwd, TRANSPORT_TCP /*networkInfo.transport*/, networkInfo.fileName);
	if(ret != 0)
	{
		//Printf("invite channelNo %d, streamNo %d, stream ip (%s), port %d, name (%s), pwd (%s), fileName %s, error\r\n",
		//		channelNo, streamNo, networkInfo.addr, networkInfo.port, networkInfo.userName, networkInfo.userPwd, networkInfo.fileName);
#if 0
		if(patrolFlag == 1)
		{//如果是轮询状态，则请求失败后，主动切换到下一个轮询通道
			if(networkGetRunFlag(channelNo, streamNo) == FLAG_RUN)
			{//在运行状态下，才切换
				networkSetRunFlag(channelNo, streamNo, FLAG_PATROL_NEXT_CHANNEL);
			}
		}
#endif
		rtspCloseStream(pRtspInfo);
		sleep(1);
		return 0;
	}
	else
	{//连接成功后，强制重启编码器
		Printf("invite channelNo %d, streamNo %d, stream ip (%s), port %d, name (%s), pwd (%s), fileName %s, success!, socket = %d\r\n",
				channelNo, streamNo, networkInfo.addr, networkInfo.port, networkInfo.userName, networkInfo.userPwd, networkInfo.fileName, pRtspInfo->pstRTSPCB->iRTSPTcpSock);
		*connectFlag = 1;
		if(pRtspInfo->pstRTSPCB->iRTSPTcpSock >= MAX_NETWORK_SUPPORT_NUM)
		{
			Printf("can not support , socket %d is too big\r\n", pRtspInfo->pstRTSPCB->iRTSPTcpSock);
			rtspCloseStream(pRtspInfo);
			streamPushPacketBufferToFrameBuffer(pPacketQueue);
			sleep(1);
			return 0;
		}
		if(streamNo == 0)
			{
				g_networkRecvBitrate[channelNo].Height = pRtspInfo->pstRTSPGlobal->nVideoHeight;
				g_networkRecvBitrate[channelNo].Width = pRtspInfo->pstRTSPGlobal->nVideoWidth;
			}
		
		gp_networkDevChannel[pRtspInfo->pstRTSPCB->iRTSPTcpSock] = &g_networkDevChannel[channelNo][streamNo];
		g_networkDevChannel[channelNo][streamNo].isConnect = 1;
		forceRestartNum++;
	}
	ring_reset(&pPacketQueue->ringBuf);
	netSocketSetNonBlock(pRtspInfo->pstRTSPCB->iRTSPTcpSock);
	netSocketSetBufferRecvSize(pRtspInfo->pstRTSPCB->iRTSPTcpSock, 64*1024);  //512
	netSocketSetBufferSendSize(pRtspInfo->pstRTSPCB->iRTSPTcpSock,8*1024);
	pPacketQueue->startFlag = 0;
	if(0==streamNo )
	{
		BufferSize = RTSP_DATA_MAIN_BUFER_SIZE;
	}
	else  if(1==streamNo)
	{
		BufferSize = RTSP_DATA_SUB_BUFER_SIZE ;
	}

	while(1)
	{
		//gpioSetNetworkPower(!checkNetworkStatus("eth0"));
		runFlag =  GetMessageFromQueue( channelNo ,streamNo );
		if(runFlag == FLAG_EXIT)
		{
			break;
		}
		else if(runFlag == FLAG_PAUSE)
		{
			myMsleep(40);
			continue;
		}
		else if(runFlag == FLAG_STOP)
		{
			break;
		}
		else if(runFlag == FLAG_RESTART)
		{
			break;
		}
#if 0
		else if(runFlag == FLAG_PATROL_NEXT_CHANNEL)
		{
			if(memcmp(&networkInfoOld, &networkInfo, sizeof(network_info_t)) != 0)
			{//
				memcpy(&networkInfoOld, &networkInfo, sizeof(network_info_t));
				break;
			}
			memcpy(&networkInfoOld, &networkInfo, sizeof(network_info_t));
		}
#endif



#if 1
		if((KeepAlive%1000) == 0)
		{
			ret=  RTSP_KeepAlive(pRtspInfo,"GET_PARAMETER");
			// fprintf(stderr,"&&&&&&&&&&&&&&&&&keepalive  %d\n",ret);
			KeepAlive =1;
		}
		else
		{
			KeepAlive++;
		}

#endif
		int len = rtspRecvStreamToRingBuf(pRtspInfo, pPacketQueue,&length);
		if(len >= 0)
		{
			while(1)
			{
				if(ring_used_size(&pPacketQueue->ringBuf) > BufferSize)
				{
					ret= RtspParseRtpPacketInfo(pRtspInfo, pPacketQueue,pPacketHead,pAudioFrameQueue,pVideoFrameQueue,
							pVideoFrameNo, pAudioFrameNo,forceRestartNum,channelNo,streamNo,&countStreamFrameRate);
					if(ret >=0)
					{
					   SetRecvBit(&pPacketHead->isKey,  channelNo,  streamNo, pPacketHead->len , FrameNO ,pVideoFrameNo);
					}
					else
					{
						setNetworkChannelRecvBitrate(channelNo, streamNo,0);
						ret = -1;
						break;
					}

				}
				else
				{
					myMsleep(5);
					break;
				}
			}
		}
		if(len > 0)
		{
			timeoutNum = 0;
		}
		if(len == 0)
		{
			timeoutNum++;
			if(timeoutNum >= 2)
			{
				Printf("come here\r\n");
				ret = -1;
			}
		}
		if(ret == -1 || len < 0 )
		{
			Printf("come here\r\n");	
			myMsleep(40);
			break;
		}
		if(streamNo == 0)
		{
			usleep(500);
		}
		else
		{
			myMsleep(5);
		}
	}
	myMsleep(10);
	rtspCloseStream(pRtspInfo);
	setNetworkChannelRecvBitrate(channelNo, streamNo,0);
	return 0;
}






int ONVIF_GetStream(rtsp_info_t *pRtspInfo, network_info_t networkInfo, stream_packet_queue_t *pPacketQueue, stream_packet_head_t *pPacketHead, stream_frame_queue_t *pAudioFrameQueue,stream_frame_queue_t*pVideoFrameQueue,
		int forceRestartNum,  int* pVideoFrameNo, int* pAudioFrameNo,int channelNo, int streamNo,int * connectFlag ,int * FrameNO)
{
	int ret = 0;
	int runFlag = -1;
	int KeepAlive = 1;
	int length = 0;
	int packetBufLen = MAX_STREAM_PACKET_SIZE;
	int timeoutNum = 0;
	int  BufferSize = 0;
	int countStreamFrameRate =0;
	int framerate = 0;
	ret = rtspInviteStream(pRtspInfo, channelNo, streamNo, networkInfo.addr, networkInfo.port, networkInfo.userName, networkInfo.userPwd, TRANSPORT_TCP /*networkInfo.transport*/, networkInfo.fileName);
	if(ret != 0)
	{
		// Printf("invite channelNo %d, streamNo %d, stream ip (%s), port %d, name (%s), pwd (%s), fileName %s, error\r\n",
		//       channelNo, streamNo, networkInfo.addr, networkInfo.port, networkInfo.userName, networkInfo.userPwd, networkInfo.fileName);
#if 0
		if(patrolFlag == 1)
		{
			if(networkGetRunFlag(channelNo, streamNo) == FLAG_RUN)
			{
				networkSetRunFlag(channelNo, streamNo, FLAG_PATROL_NEXT_CHANNEL);
			}
		}
#endif
		rtspCloseStream(pRtspInfo);
		myMsleep(30);
		return 0;
	}
	else
	{//
		//Printf("invite channelNo %d, streamNo %d, stream ip (%s), port %d, name (%s), pwd (%s), fileName %s, success!, socket = %d\r\n",
		//     channelNo, streamNo, networkInfo.addr, networkInfo.port, networkInfo.userName, networkInfo.userPwd, networkInfo.fileName, pRtspInfo->pstRTSPCB->iRTSPTcpSock);
		*connectFlag = 1;
		if(pRtspInfo->pstRTSPCB->iRTSPTcpSock >= MAX_NETWORK_SUPPORT_NUM)
		{
			Printf("can not support , socket %d is too big\r\n", pRtspInfo->pstRTSPCB->iRTSPTcpSock);
			rtspCloseStream(pRtspInfo);
			streamPushPacketBufferToFrameBuffer(pPacketQueue);
			myMsleep(30);
			return 0;
		}
		if(streamNo == 0)
		{
				g_networkRecvBitrate[channelNo].Height = pRtspInfo->pstRTSPGlobal->nVideoHeight;
				g_networkRecvBitrate[channelNo].Width = pRtspInfo->pstRTSPGlobal->nVideoWidth;
		}
		gp_networkDevChannel[pRtspInfo->pstRTSPCB->iRTSPTcpSock] = &g_networkDevChannel[channelNo][streamNo];
		g_networkDevChannel[channelNo][streamNo].isConnect = 1;
		forceRestartNum++;
	}
	networkOnvifEventSubscribe(&networkInfo);
	ring_reset(&pPacketQueue->ringBuf);
	netSocketSetNonBlock(pRtspInfo->pstRTSPCB->iRTSPTcpSock);
	netSocketSetBufferRecvSize(pRtspInfo->pstRTSPCB->iRTSPTcpSock, 128*1024);  //512
	netSocketSetBufferSendSize(pRtspInfo->pstRTSPCB->iRTSPTcpSock,8*1024);
	pPacketQueue->startFlag = 0;

	if(0==streamNo )
	{
		BufferSize = RTSP_DATA_MAIN_BUFER_SIZE;
	}
	else  if(1==streamNo)
	{
		BufferSize = RTSP_DATA_SUB_BUFER_SIZE ;
	}

	while(1)
	{
		//gpioSetNetworkPower(!checkNetworkStatus("eth0"));
		runFlag =  GetMessageFromQueue( channelNo ,streamNo );
		if(runFlag == FLAG_EXIT || runFlag == FLAG_STOP || runFlag == FLAG_RESTART)
		{
			break;
		}
		else if(runFlag == FLAG_PAUSE)
		{
			myMsleep(40);
			continue;
		}
		else if(runFlag == FLAG_STOP)
		{
			break;
		}
		else if(runFlag == FLAG_RESTART)
		{
			break;
		}
#if 0
		else if(runFlag == FLAG_PATROL_NEXT_CHANNEL)
		{
			if(memcmp(&networkInfoOld, &networkInfo, sizeof(network_info_t)) != 0)
			{//
				memcpy(&networkInfoOld, &networkInfo, sizeof(network_info_t));
				break;
			}
			memcpy(&networkInfoOld, &networkInfo, sizeof(network_info_t));
		}
#endif


#if 1
		if((KeepAlive%2000) == 0)
		{
			ret=  RTSP_KeepAlive(pRtspInfo,"GET_PARAMETER");
			// fprintf(stderr,"&&&&&&&&&&&&&&&&&keepalive  %d\n",ret);
			KeepAlive =1;
		}
		else
		{
			KeepAlive++;
		}
#endif
		int len = rtspRecvStreamToRingBuf(pRtspInfo, pPacketQueue,&length);
		if(len >= 0)
		{
			while(1)
			{
				if(ring_used_size(&pPacketQueue->ringBuf) > BufferSize)
				{
					ret= RtspParseRtpPacketInfo(pRtspInfo, pPacketQueue,pPacketHead,pAudioFrameQueue,pVideoFrameQueue,
							pVideoFrameNo, pAudioFrameNo,forceRestartNum,channelNo,streamNo,&countStreamFrameRate);
					if(ret >=0)
					{			
					  SetRecvBit(&pPacketHead->isKey,  channelNo,  streamNo, pPacketHead->len , FrameNO ,pVideoFrameNo);
					}
					else
					{
						setNetworkChannelRecvBitrate(channelNo, streamNo,0);
						ret = -1;
						break;
					}

				}
				else
				{
					myMsleep(1);
					break;
				}
			}
		}
		if(len > 0)
		{
			timeoutNum = 0;
		}
		if(len == 0)
		{
			myMsleep(2);
			timeoutNum++;
			if(timeoutNum >= 40)
			{
				Printf("rtsp  recv timeoutNum =%d channelNo =%d\r\n",timeoutNum,channelNo);
				ret = -1;
			}
		}
		if(ret == -1 || len < 0 )
		{
			Printf("rtspCloseStream  ret =%d  len=%d  pRtspInfo->pstRTSPDesc->hostAddress =%s\n",ret,len,pRtspInfo->pstRTSPDesc->hostAddress);
			rtspCloseStream(pRtspInfo);
			ONVIF_UnSubscribe(&networkInfo);
			ONVIF_free(&networkInfo);
			myMsleep(30);
			break;
		}
		if(streamNo == 0)
		{
			usleep(200);
		}
		else
		{
			myMsleep(2);
		}
	}
	myMsleep(10);
	rtspCloseStream(pRtspInfo);
	setNetworkChannelRecvBitrate(channelNo, streamNo,0);
	return 0;
}

int Private_Protocol_GetStream( 	int streamSocketFd,ARG_STREAM * stream_info, stream_packet_queue_t *pPacketQueue,  stream_frame_queue_t *pVideoFrameQueue,
		int forceRestartNum, int *videoFrameLen, int *videoFrameNo, int * audioFrameNo,int channelNo ,int streamNo ,unsigned long *SeqNo ,int * FrameNO )
{

	int len =0;
	int runFlag =-1;
	int timeoutNum=0;
	int ret=0;
	stream_packet_head_t packetHead;
	int framerate = 0;
	ring_reset(&pPacketQueue->ringBuf);
	
	netSocketSetBufferRecvSize(streamSocketFd, 128*1024);  //512
	netSocketSetBufferSendSize(streamSocketFd,64*1024);
	netSocketSetNonBlock(streamSocketFd);
	//Printf("22222222222222222222222  channelNo =%d streamNo =%d\n",channelNo,streamNo);
    int GetResolutionTime = 3;
	while(1)
	{
		//gpioSetNetworkPower(!checkNetworkStatus("eth0"));
		runFlag =  GetMessageFromQueue( channelNo ,streamNo );
		if(runFlag == FLAG_EXIT)
		{
			break;
		}
		else if(runFlag == FLAG_PAUSE)
		{
			myMsleep(40);
			continue;
		}
		else if(runFlag == FLAG_STOP)
		{
			break;
		}
		else if(runFlag == FLAG_RESTART)
		{
			break;
		}

		len= Private_Protocol_RecvStreamToRingBuf( streamSocketFd, stream_info,  pPacketQueue, SeqNo);
		if (len >0)
		{
			ret = Private_Protocol_ParseStreamFromRingBuf( streamSocketFd, stream_info,  pPacketQueue,  &packetHead, videoFrameLen);
			if(ret >= 0)
			{
			 SetRecvBit(&packetHead.isKey,  channelNo,  streamNo, packetHead.len , FrameNO ,videoFrameNo);			
             if(streamNo == 0 && GetResolutionTime)
             	{
				  g_networkRecvBitrate[channelNo].Width = stream_info->usWidth;
				  g_networkRecvBitrate[channelNo].Height =  stream_info->usHeight;
				  GetResolutionTime--;
				  fprintf(stderr,"width =%d ,height =%d\r\n",g_networkRecvBitrate[channelNo].Width,g_networkRecvBitrate[channelNo].Height);
             	}		
				Private_Protocol_PacketToFrameQueue( streamSocketFd, stream_info,  pPacketQueue,  &packetHead,pVideoFrameQueue, pVideoFrameQueue, forceRestartNum, videoFrameLen, videoFrameNo, audioFrameNo);
			}
			else
			{
				setNetworkChannelRecvBitrate(channelNo, streamNo,0);
				Printf("Private_Protocol_ParseStreamFromRingBuf  fail\n");
				ret = -1;
			}

			timeoutNum = 0;
		}
		else if(len==0)
		{
			timeoutNum ++;
		}

		if(-1== len  || -1==ret || timeoutNum>=2) /*timeout 一次为3秒 (3*2)还没有数据则退出重新连接*/
		{
			myMsleep(10);
			close(streamSocketFd);
			// SetSessionID(channelNo,0);
			break;
		}

	}
	close(streamSocketFd);
	setNetworkChannelRecvBitrate(channelNo, streamNo,0);
	return ret;
}

int ONVIF_free(network_info_t *networkInfo)
{
	Remote_Device_Info pDeviceInfo;
	strcpy(pDeviceInfo.ip,networkInfo->onvifAddr) ;
	strcpy(pDeviceInfo.userName , networkInfo->userName);
	strcpy(pDeviceInfo.password,networkInfo->userPwd);
	pDeviceInfo.port = networkInfo->onvifPort;
	return  0;
}

/*  */
#if USED_THREAD_POOL
static void * networkThread(char *arg, int argLen)
#else
static void * networkThread(void *param)
#endif
{
	unsigned int channelNo = 0;
	unsigned int streamNo = 0;
	int ret = 0;
	int runFlag = 0;
	int videoFrameNo = 0, audioFrameNo = 0;//
	int videoFrameLen = 0;//, audioFrameLen = 0;
	int forceRestartNum = 0;//
	rtsp_info_t *pRtspInfo = NULL;//
	stream_packet_queue_t *pPacketQueue = NULL;
	//	stream_packet_queue_t packetQueue;
	//	stream_frame_queue_t videoFrameQueue;
	stream_frame_queue_t *pVideoFrameQueue = NULL;
	stream_frame_queue_t *pAudioFrameQueue = NULL;
	stream_packet_head_t pPacketHead;
	int patrolFlag = 0;//  0  1
	network_info_t networkInfo;
	network_info_t networkInfoOld;//
	int onvifUrlFlag = 1;
	link_queue_data_t networkMsg = {0};
	int queueNum = 0;
	int connectFlag = 0;//
	int  FrameNO = 0;
	pteClient_t * pClient = NULL;
	int  Pri_Protocol = 0;
	int RTSPFlag = 0;
	unsigned long SeqNo =0;
	 ARG_STREAM  stream_info;
	 int SetTime = 3;

#if USED_THREAD_POOL
	unsigned int devChannel = 0;
	if(argLen != sizeof(devChannel))
	{
		Printf("thread arg len is error, %d.%d\r\n", argLen, sizeof(dev_channel_t));
	}
	memcpy(&devChannel, arg, sizeof(devChannel));
	channelNo = ((devChannel) & 0xFFFF0000) >> 16;
	streamNo = ((devChannel) & 0x0000FFFF);
#else
	channelNo = ((int)(int *)(param) & 0xFFFF0000) >> 16;
	streamNo = ((int)(int *)(param) & 0x0000FFFF);
#endif

	//Printf("networkThread  channelNo = %d, streamNo %d\r\n", channelNo, streamNo);

	g_networkDevChannel[channelNo][streamNo].channelNo = channelNo;
	g_networkDevChannel[channelNo][streamNo].streamNo = streamNo;

	int tmpRunFlag = networkGetRunFlag(channelNo, streamNo);
	if((tmpRunFlag != 0) && (tmpRunFlag != FLAG_EXIT))
	{
		Printf("this channel %d, streamNo %d , is already invite stream\r\n", channelNo, streamNo);
		return NULL;
	}
	sleep(1);
	networkGetNetworkInfo(channelNo, streamNo, &networkInfo);

	pPacketQueue = (stream_packet_queue_t *)malloc(sizeof(stream_packet_queue_t));
	assert(pPacketQueue);
	memset(pPacketQueue, 0, sizeof(stream_packet_queue_t));

	if(streamInitPacketQueue(pPacketQueue,networkInfo.protocol) == -1)
	{
		Printf("init packet queue error\r\n");
		return NULL;
	}
	g_networkDevChannel[channelNo][streamNo].pPacketQueue = pPacketQueue;

	pVideoFrameQueue = (stream_frame_queue_t *)malloc(sizeof(stream_frame_queue_t));
	assert(pVideoFrameQueue);
	memset(pVideoFrameQueue, 0, sizeof(stream_frame_queue_t));
	if(streamInitFrameQueue(pVideoFrameQueue) == -1)//
	{
		Printf("init frame queue error\r\n");
		return NULL;
	}
	g_networkDevChannel[channelNo][streamNo].pVideoFrameQueue = pVideoFrameQueue;
	networkSetStreamFrameQueue(channelNo, streamNo, (void *)pVideoFrameQueue);//
	networkSetRunFlag(channelNo, streamNo, FLAG_RUN);
	setPthreadPriority(PTHREAD_SCHEDULED_PRIORITY_HIGH_EST);

	while(1)
	{
		int flag =0;
		networkGetNetworkInfo(channelNo, streamNo, &networkInfo);
		//Printf("qqqqqqqqqqqqqqqqqqqqq channelNo =%d networkInfo.protocol =%d\n",channelNo,networkInfo.protocol);
		switch(networkInfo.protocol)
		{
			case NETWORK_ONVIF:
				if(NULL==pRtspInfo)
				{
					pRtspInfo = rtspInit();
					if(pRtspInfo == NULL)
					{
						Printf("rtsp init error\r\n");
						flag =1;
					}
					g_networkDevChannel[channelNo][streamNo].pRtspInfo = pRtspInfo;
				}
				break;
			case NETWORK_PROTOCOL:
				if(NULL == pClient)
				{
					ret = PC_InitCtx();
					if(0!=ret)
					{
						flag = 1;
					}
					pClient = PC_CreateNew();
					if(NULL==pClient)
					{
						flag = 1;
					}
				}
				break;
			case NETWORK_RTSP:
				if(NULL==pRtspInfo)
				{
					pRtspInfo = rtspInit();
					if(pRtspInfo == NULL)
					{
						Printf("rtsp init error\r\n");
						flag =1;
					}

					g_networkDevChannel[channelNo][streamNo].pRtspInfo = pRtspInfo;
				}
				break;

			default:
				if(NULL==pRtspInfo)
				{
					pRtspInfo = rtspInit();
					if(pRtspInfo == NULL)
					{
						Printf("rtsp init error\r\n");
						flag =1;
					}
					g_networkDevChannel[channelNo][streamNo].pRtspInfo = pRtspInfo;
				}
				break;
		}
		if(1==flag)
		{
			break;
		}

		//gpioSetNetworkPower(!checkNetworkStatus("eth0"));
		runFlag =  GetMessageFromQueue( channelNo ,streamNo );
		if(connectFlag == 1)
		{
			switch(networkInfo.protocol)
			{
				case NETWORK_ONVIF:
					rtspCloseStream(pRtspInfo);
					break;
				case NETWORK_PROTOCOL:
					close(pClient->sockFd );
					pClient->sockFd =-1;
					//	private_protocol_closeStream(private_protocol);
					break;
				case NETWORK_RTSP:
					rtspCloseStream(pRtspInfo);
					break;
				default:
					rtspCloseStream(pRtspInfo);
					break;
			}
			//streamPushPacketBufferToFrameBuffer(pPacketQueue);
			//streamFreeAllFrameBuffer(pVideoFrameQueue);
			connectFlag = 0;
			g_networkDevChannel[channelNo][streamNo].isConnect = 0;
		}

		if(networkGetStreamUsedNum(channelNo, streamNo) == 0)
		{//
			//Printf("channelNo %d, streamNo %d, exit\r\n", channelNo, streamNo);
			break;
		}
		//runFlag = networkGetRunFlag(channelNo, streamNo);
		if(runFlag == FLAG_EXIT)
		{
			Printf("exit network thread channelNo %d, streamNo %d\r\n", channelNo, streamNo);
			break;
		}
		else if(runFlag == FLAG_PAUSE)
		{
			myMsleep(40);
			continue;
		}
		else if(runFlag == FLAG_STOP)
		{
			patrolFlag = 0;
			switch(networkInfo.protocol)
			{
				case NETWORK_ONVIF:
					rtspCloseStream(pRtspInfo);
					break;
				case NETWORK_PROTOCOL:
					close(pClient->sockFd );
					pClient->sockFd =-1;
					//	private_protocol_closeStream(private_protocol);
					break;
				case NETWORK_RTSP:
					rtspCloseStream(pRtspInfo);
					break;

				default:
					rtspCloseStream(pRtspInfo);
					break;
			}
			//streamPushPacketBufferToFrameBuffer(pPacketQueue);
			//Printf("stop network thread channelNo %d, streamNo %d\r\n", channelNo, streamNo);
			myMsleep(40);
			networkSetRunFlag(channelNo, streamNo, FLAG_PAUSE);
			runFlag = FLAG_PAUSE;
			continue;
		}
		else if(runFlag == FLAG_RESTART)
		{
			networkSetRunFlag(channelNo, streamNo, FLAG_RUN);
			runFlag = FLAG_RUN;
		}
		networkGetNetworkInfo(channelNo, streamNo, &networkInfo);

#if 1  
		if(networkInfo.enable == 0)
		{
			myMsleep(50);
			continue;
		}
#endif  
		if(networkInfo.protocol == NETWORK_ONVIF)
		{

			if((onvifUrlFlag++%40) != 0){
				myMsleep(30);
				continue;
			}
            if(SetTime > 0  && streamNo == 0)
            	{
                  ret= SetIPCTimeAndDate(&networkInfo);
				  if(ret==0)
				  	{
                      SetTime =0;
				    }
				   else
				  	{
                     SetTime--;
				    }
			    }
			if(getNetworkInfoByOnvif(&networkInfo, streamNo) < 0)
			{
				myMsleep(30);
				continue;
			}
			ret=ONVIF_GetStream(pRtspInfo, networkInfo, pPacketQueue, &pPacketHead,pVideoFrameQueue, pVideoFrameQueue,forceRestartNum,
					&videoFrameNo,  &audioFrameNo, channelNo,  streamNo,&connectFlag ,&FrameNO);
			onvifUrlFlag = 0;
			if(0==ret)
			{
				myMsleep(10);
				continue;
			}
		}
		else if(networkInfo.protocol == NETWORK_PROTOCOL)
		{

			if((Pri_Protocol++%20) != 0){
				myMsleep(50);
				continue;
			}

			USER_INFO userInfo;
			memset(&userInfo,0,sizeof(USER_INFO));
			snprintf(userInfo.ucUsername,sizeof(userInfo.ucUsername),"%s",networkInfo.userName);
			snprintf(userInfo.ucPassWord,sizeof(userInfo.ucPassWord),"%s",networkInfo.userPwd);
			ret = PC_Login(pClient,inet_addr(networkInfo.onvifAddr),networkInfo.onvifPort,&userInfo);

			// Printf("%s:%d\r\n",networkInfo.onvifAddr,networkInfo.onvifPort);
			//Printf("PRIVATE_PROTOCOL :  %s  \n streamno = %d channel=%d username=%s passwd+%s \n",networkInfo.onvifAddr,streamNo ,channelNo,networkInfo.userName,networkInfo.userPwd);
			//ret = private_protocol_login(private_protocol,inet_addr(networkInfo.onvifAddr),
			//		networkInfo.onvifPort,networkInfo.userName,networkInfo.userPwd);



			if(ret==-1)
			{
				myMsleep(50);
				continue;
			}
			else
			{
				connectFlag = 1;
				if(pClient->sockFd >= MAX_NETWORK_SUPPORT_NUM)
				{
					Printf("can not support , socket %d is too big\r\n", pClient->sockFd);
					//streamPushPacketBufferToFrameBuffer(pPacketQueue);
					sleep(1);
					return 0;
				}
				gp_networkDevChannel[pClient->sockFd] = &g_networkDevChannel[channelNo][streamNo];
				g_networkDevChannel[channelNo][streamNo].isConnect = 1;
				forceRestartNum++;
			}
			//ret = PC_GetStream(pClient,0,0);
			//Printf("11111111111111111111111111111111111111111  channelNo =%d streamNo =%d\n",channelNo,streamNo);
			ret = PC_GetStream(pClient,channelNo,streamNo);
			if(ret <0)
			{
				Printf("PC_GetStream fail \n");
			}

			Private_Protocol_GetStream(pClient->sockFd, &stream_info,pPacketQueue,  pVideoFrameQueue,
					forceRestartNum, &videoFrameLen, &videoFrameNo, &audioFrameNo,channelNo , streamNo,&SeqNo,&FrameNO);



			Pri_Protocol = 0;
		}
		else if(networkInfo.protocol == NETWORK_RTSP)
		{
			if((RTSPFlag++%40) != 0){
				myMsleep(50);
				continue;
			}
			if(0==streamNo)
			{
				rtspParseDetailURL(networkInfo.rtspmainurl,networkInfo.addr,&networkInfo.port,networkInfo.fileName);
			}
			else if(1==streamNo)
			{
				rtspParseDetailURL(networkInfo.rtspsuburl,networkInfo.addr,&networkInfo.port,networkInfo.fileName);
			}

			ret=RTSP_GetStream(pRtspInfo, networkInfo, pPacketQueue, &pPacketHead,pVideoFrameQueue, pVideoFrameQueue,forceRestartNum,
					&videoFrameNo,  &audioFrameNo, channelNo,  streamNo,&connectFlag,&FrameNO);
			RTSPFlag = 0;
			if(0==ret)
			{
				continue;
			}
		}

		usleep(20*1000);
	}
#if 0
	switch(networkInfo.protocol)
	{
		case NETWORK_ONVIF:
			if(0!=pRtspInfo){
				rtspCloseStream(pRtspInfo);
				rtspUninit(pRtspInfo);
				ONVIF_UnSubscribe(&networkInfo);
				ONVIF_free(&networkInfo);
			}
			break;
		case NETWORK_PROTOCOL:
			fprintf(stderr,"@@@@@@@@@@@@@@%s:%d",__func__,__LINE__);
			private_protocol_closeStream(private_protocol);
			private_protocol_stream_free(private_protocol);
			break;
		default:
			if(0!=pRtspInfo){
				rtspCloseStream(pRtspInfo);
				rtspUninit(pRtspInfo);
				ONVIF_UnSubscribe(&networkInfo);
				ONVIF_free(&networkInfo);
			}
			break;
	}
#endif

	if(NULL!=pRtspInfo)
	{
		rtspCloseStream(pRtspInfo);
		rtspUninit(pRtspInfo);
		ONVIF_UnSubscribe(&networkInfo);
		ONVIF_free(&networkInfo);
	}
	if(NULL!=pClient)
	{
		PC_Delete(pClient);
		pClient = NULL;
	}
	// streamPushPacketBufferToFrameBuffer(pPacketQueue);
	networkSetRunFlag(channelNo, streamNo, FLAG_EXIT);
	//rtspUninit(pRtspInfo);
	networkSetStreamFrameQueue(channelNo, streamNo, NULL);
	streamUninitPacketQueue(pPacketQueue,networkInfo.protocol);
	streamUninitFrameQueue(pVideoFrameQueue);
	Printf("network thread exit, channelNo = %d, streamNo = %d\r\n", channelNo, streamNo);
	return NULL;
}


/*  */
int networkCheckStreamExist(int channelNo, int streamNo)
{
	if((channelNo >= 0) && (channelNo <= 0xFF) && (streamNo >= 0) && (streamNo <= 0x3F))
	{
		return g_networkDevChannel[channelNo][streamNo].isConnect;
	}

	return 0;
}

/* 获取实时流使用次数 */
int networkGetStreamUsedNum(int channelNo, int streamNo)
{
	int usedNum = 0;

	if((channelNo >= 0) && (channelNo <= 0xFF) && (streamNo >= 0) && (streamNo <= 0x3F))
	{
		//pthread_mutex_lock(&g_networkDevChannel[channelNo][streamNo].streamMutex);
		pthread_mutex_lock(&g_streamMutex);
		usedNum = g_networkDevChannel[channelNo][streamNo].useNumDecode + g_networkDevChannel[channelNo][streamNo].useNumRecord + g_networkDevChannel[channelNo][streamNo].useNumProxy;
		pthread_mutex_unlock(&g_streamMutex);
		//pthread_mutex_unlock(&g_networkDevChannel[channelNo][streamNo].streamMutex);
	}
	else
	{
		return 0;
	}

	return usedNum;
}

/*
 * 增加实时流使用次数
 * type:        谁请求了流  1：解码  2：存储  3：转发
 */
int networkAddStreamUsedNum(int channelNo, int streamNo, int type)
{
	int ret = 0;

	if((channelNo >= 0) && (channelNo <= 0xFF) && (streamNo >= 0) && (streamNo <= 0x3F))
	{
		//pthread_mutex_lock(&g_networkDevChannel[channelNo][streamNo].streamMutex);
		pthread_mutex_lock(&g_streamMutex);
		if(type == NETWORK_INVITE_TYPE_DECODE)
		{
			g_networkDevChannel[channelNo][streamNo].useNumDecode++;
		}
		else if(type == NETWORK_INVITE_TYPE_RECORD)
		{
			g_networkDevChannel[channelNo][streamNo].useNumRecord++;
		}
		else if(type == NETWORK_INVITE_TYPE_PROXY)
		{
			g_networkDevChannel[channelNo][streamNo].useNumProxy++;
		}
		else
		{
			Printf("add stream num, can not support type 0x%x\r\n", type);
		}
		//Printf("networkAddStreamUsedNum = %d.%d.%d\r\n", g_networkDevChannel[channelNo][streamNo].useNumDecode, g_networkDevChannel[channelNo][streamNo].useNumRecord, g_networkDevChannel[channelNo][streamNo].useNumProxy);

		ret = g_networkDevChannel[channelNo][streamNo].useNumDecode + g_networkDevChannel[channelNo][streamNo].useNumRecord + g_networkDevChannel[channelNo][streamNo].useNumProxy;
		pthread_mutex_unlock(&g_streamMutex);
		//pthread_mutex_unlock(&g_networkDevChannel[channelNo][streamNo].streamMutex);
	}

	return ret;
}   

int ONVIF_UnSubscribe( network_info_t *networkInfo)
{
	Remote_Device_Info pDeviceInfo;
	strcpy(pDeviceInfo.ip,networkInfo->onvifAddr) ;
	strcpy(pDeviceInfo.userName , networkInfo->userName);
	strcpy(pDeviceInfo.password,networkInfo->userPwd);
	pDeviceInfo.port = networkInfo->onvifPort;
	return ONVIF_EventUnSubscribe(&pDeviceInfo);
}



/* 减少实时流使用次数 */
int networkDelStreamUsedNum(int channelNo, int streamNo, int type)
{
	int usedNum = 0;

	if((channelNo >= 0) && (channelNo <= 0xFF) && (streamNo >= 0) && (streamNo <= 0x3F))
	{
		//pthread_mutex_lock(&g_networkDevChannel[channelNo][streamNo].streamMutex);
		pthread_mutex_lock(&g_streamMutex);
		if(type == NETWORK_INVITE_TYPE_DECODE)
		{
			if(g_networkDevChannel[channelNo][streamNo].useNumDecode > 0)
			{
				g_networkDevChannel[channelNo][streamNo].useNumDecode--;
			}
		}
		else if(type == NETWORK_INVITE_TYPE_RECORD)
		{
			if(g_networkDevChannel[channelNo][streamNo].useNumRecord > 0)
			{
				g_networkDevChannel[channelNo][streamNo].useNumRecord--;
			}
		}
		else if(type == NETWORK_INVITE_TYPE_PROXY)
		{
			if(g_networkDevChannel[channelNo][streamNo].useNumProxy > 0)
			{
				g_networkDevChannel[channelNo][streamNo].useNumProxy--;
			}
		}
#if 0
		else if(type == NETWORK_INVITE_TYPE_ALL)
		{
			g_networkDevChannel[channelNo][streamNo].useNumDecode = 0;
			g_networkDevChannel[channelNo][streamNo].useNumRecord = 0;
			g_networkDevChannel[channelNo][streamNo].useNumProxy = 0;
		}
#endif
		else
		{
			Printf("del stream num, can not support type 0x%x\r\n", type);
		}
		usedNum = g_networkDevChannel[channelNo][streamNo].useNumDecode + g_networkDevChannel[channelNo][streamNo].useNumRecord + g_networkDevChannel[channelNo][streamNo].useNumProxy;
		pthread_mutex_unlock(&g_streamMutex);
		//pthread_mutex_unlock(&g_networkDevChannel[channelNo][streamNo].streamMutex);
	}

	return usedNum;
}

/* 获取实时流帧队列 */
void *networkGetStreamFrameQueue(int channelNo, int streamNo)
{
	return g_networkDevChannel[channelNo][streamNo].pVideoFrameQueue;
}

/* 设置实时流帧队列 */
void networkSetStreamFrameQueue(int channelNo, int streamNo, void *pFrameQueue)
{
	g_networkDevChannel[(channelNo & MAX_SUPPORT_DEV_NUM)][(streamNo & MAX_SUPPORT_DEV_CHANNEL_NUM)].pVideoFrameQueue = pFrameQueue;
}

/*
 * 根据设备通道号请求视频流
 * type:	谁请求了流  1：解码  2：存储  3：转发
 */
int networkStartStream(int channelNo, int streamNo, int type)
{
	unsigned int devChannel;
	int num = 0;

	devChannel = ((channelNo & MAX_SUPPORT_DEV_NUM) << 16) + (streamNo & MAX_SUPPORT_DEV_CHANNEL_NUM);
	num = networkAddStreamUsedNum(channelNo, streamNo, type);
	if(num == 1)
	{//增加该设备通道使用次数，如果为第一次请求，则真正请求该设备通道视频
		link_queue_data_t networkMsg = {0};
		networkMsg.type = NETWORK_MSG_QUEUE_TYPE_RUN;
		pthread_mutex_lock(&g_msgMutex);
		queueLinkIn(&g_networkDevChannel[channelNo][streamNo].msgQueue, networkMsg);
		pthread_mutex_unlock(&g_msgMutex);
#if USED_THREAD_POOL
		if(threadPoolAddWorker(NULL, networkThread, (char *)&devChannel, sizeof(devChannel)) != 0)
		{
			Printf("invite channelNo %d, streamNo %d stream error\r\n", channelNo, streamNo);
			networkStopStream(channelNo, streamNo, type);
			return -1;
		}
#else
		if(pthread_create(&g_threadId, NULL, networkThread, (void *)devChannel) < 0)
		{
			Printf("invite channelNo %d, streamNo %d stream error\r\n", channelNo, streamNo);
			networkStopStream(channelNo, streamNo, type);
			return -1;
		}
#endif
		return 1;
	}

	return 0;
}

/* 根据设备通道号重新请求视频流 */
void networkRestartStream(int channelNo, int streamNo)
{
#if 0
	if(networkGetRunFlag(channelNo, streamNo) == FLAG_RUN)
	{
		networkSetRunFlag(channelNo, streamNo, FLAG_RESTART);
		networkWaitStreamFlag(channelNo, streamNo, FLAG_RUN);
	}
#else
	link_queue_data_t networkMsg = {0};
	networkMsg.type = NETWORK_MSG_QUEUE_TYPE_RESTART;
	pthread_mutex_lock(&g_msgMutex);
	queueLinkIn(&g_networkDevChannel[channelNo][streamNo].msgQueue, networkMsg);
	pthread_mutex_unlock(&g_msgMutex);
#endif
}

/*
 * 根据设备通道号停止视频流
 * type:        谁关闭了流  1：解码  2：存储  3：转发
 */
int networkStopStream(int channelNo, int streamNo, int type)
{
#if 0
	if(networkDelStreamUsedNum(channelNo, streamNo, type) <= 0)
	{
		networkSetRunFlag(channelNo, streamNo, FLAG_EXIT);
	}
#else
	link_queue_data_t networkMsg = {0};
	int num = 0;

	num = networkDelStreamUsedNum(channelNo, streamNo, type);
	if(num <= 0)
	{
		networkMsg.type = NETWORK_MSG_QUEUE_TYPE_EXIT;
		pthread_mutex_lock(&g_msgMutex);
		queueLinkIn(&g_networkDevChannel[channelNo][streamNo].msgQueue, networkMsg);
		pthread_mutex_unlock(&g_msgMutex);
	}
#endif
	return 0;
}
/**/
int networkSetCountStreamFrameRate(int channelNo, int streamNo,int value)
{
	if((channelNo<0)||(channelNo >MAX_DISPLAY_NUM))
	{
		Printf("networkSetCountStreamFrameRate channelNo fail  %d \n",channelNo);
		return -1;
	}
	//pthread_mutex_lock(&g_networkMutex);
	if(value>2)
	{
		g_networkStreamFrameRate[channelNo] = value -2;
	}
	else
	{
		g_networkStreamFrameRate[channelNo] = value;
	}
	//pthread_mutex_unlock(&g_networkMutex);

	//Printf("g_networkStreamFrameRate[%d] =%d\n",channelNo,value-2);
	return 0;
}
/**/
int networkGetCountStreamFrameRate(int channelNo, int streamNo)
{
	if((channelNo<0)||(channelNo >MAX_DISPLAY_NUM))
	{
		Printf("networkSetCountStreamFrameRate channelNo fail  %d \n",channelNo);
		return -1;
	}
	return   g_networkStreamFrameRate[channelNo];
}


/*  */
int networkWaitStreamFlag(int channelNo, int streamNo, int value)
{
	while(1)
	{
		myMsleep(40);
		if(value == networkGetRunFlag(channelNo, streamNo))
		{
			break;
		}
	}
	return 0;
}

/* 获取网络线程运行标记 */
int networkGetRunFlag(int channelNo, int streamNo)
{
	int runFlag = 0;

	if((channelNo >= 0) && (channelNo <= MAX_SUPPORT_DEV_NUM) && (streamNo >= 0) && (streamNo <= MAX_SUPPORT_DEV_CHANNEL_NUM))
	{
		pthread_mutex_lock(&g_networkMutex);
		runFlag = g_networkDevChannel[channelNo][streamNo].runFlag;
		pthread_mutex_unlock(&g_networkMutex);
		return runFlag;
	}
	else
	{
		return 0;
	}
	return 0;
}

/* 设置网络线程运行标记 */
void networkSetRunFlag(int channelNo, int streamNo, int value)
{
	//Printf("networkSetRunFlag channelNo %d, streamNo %d, value %d\r\n", channelNo, streamNo, value);
	if((channelNo >= 0) && (channelNo <= MAX_SUPPORT_DEV_NUM) && (streamNo >= 0) && (streamNo <= MAX_SUPPORT_DEV_CHANNEL_NUM))
	{
		pthread_mutex_lock(&g_networkMutex);
		g_networkDevChannel[channelNo][streamNo].runFlag = value;
		pthread_mutex_unlock(&g_networkMutex);
	}
}

/* 获取当前网络信息 */
void networkGetNetworkInfo(int channelNo, int streamNo, network_info_t *pNetworkInfo)
{
	streamNo = 0;
	if((pNetworkInfo != NULL) && (g_networkDevChannel[channelNo][streamNo].pNetworkInfo != NULL))
	{
		pthread_mutex_lock(&g_networkMutex);
		memcpy(pNetworkInfo, g_networkDevChannel[channelNo][streamNo].pNetworkInfo, sizeof(network_info_t));
		pthread_mutex_unlock(&g_networkMutex);
	}
}

/* 设置网络参数 */
int networkSetNetworkInfo(network_info_t *pNetworkInfo)
{
	int channelNo = pNetworkInfo->channelNo;
	int streamNo = pNetworkInfo->streamNo;

	streamNo = 0;

	if(g_networkDevChannel[channelNo][streamNo].pNetworkInfo == NULL)
	{
		g_networkDevChannel[channelNo][streamNo].pNetworkInfo = (network_info_t *)malloc(sizeof(network_info_t));
		assert(g_networkDevChannel[channelNo][streamNo].pNetworkInfo);
	}

	pthread_mutex_lock(&g_networkMutex);
	memcpy(g_networkDevChannel[channelNo][streamNo].pNetworkInfo, pNetworkInfo, sizeof(network_info_t));
	pthread_mutex_unlock(&g_networkMutex);

	//Printf("channelNo %d, streamNo == %d, userName (%s), pwd(%s)\r\n",
	//pNetworkInfo->channelNo, pNetworkInfo->streamNo, pNetworkInfo->userName, pNetworkInfo->userPwd);

	return 0;
}

#if 0
/* 设置网络参数 */
void statusSetNetworkInfo(int channelNo, int streamNo, network_info_t *pNetworkInfo)
{
	if(g_networkDevChannel[channelNo][streamNo].pNetworkInfo == NULL)
	{
		g_networkDevChannel[channelNo][streamNo].pNetworkInfo = (network_info_t *)malloc(sizeof(network_info_t));
		assert(g_networkDevChannel[channelNo][streamNo].pNetworkInfo);
		memcpy(g_networkDevChannel[channelNo][streamNo].pNetworkInfo, pNetworkInfo, sizeof(network_info_t));
	}
}
#endif


 int   SetIPCTimeAndDateWhenNTP(int channel)
{
   int i =0;
   int ret ;
   Remote_Device_Info pDeviceInfo;
   	network_info_t networkInfo;
   if(0xFA==channel)
   	{
	 for ( i = 0; i < MAX_DISPLAY_NUM && i<configGetDisplayNum(); i++)
	 	{
			ret = DbMainMsgChannelInfo(i,pDeviceInfo.ip,&pDeviceInfo.port,pDeviceInfo.userName,pDeviceInfo.password);
			if(-1==ret)
			{
				Printf( "DbMainMsgChannelInfo   get channel failed\n");
				continue;
			}
			strcpy(networkInfo.onvifAddr,pDeviceInfo.ip) ;
			strcpy(networkInfo.userName,pDeviceInfo.userName);
			strcpy(networkInfo.userPwd,pDeviceInfo.password);
			networkInfo.onvifPort=pDeviceInfo.port ;
			ret = SetIPCTimeAndDate(&networkInfo);	
		}
   	}
	else
	{
		ret = DbMainMsgChannelInfo(channel,pDeviceInfo.ip,&pDeviceInfo.port,pDeviceInfo.userName,pDeviceInfo.password);
		if(-1==ret)
		{
		Printf( "DbMainMsgChannelInfo   get channel failed\n");
		return 0;
		}
		strcpy(networkInfo.onvifAddr,pDeviceInfo.ip) ;
		strcpy(networkInfo.userName,pDeviceInfo.userName);
		strcpy(networkInfo.userPwd,pDeviceInfo.password);
		networkInfo.onvifPort=pDeviceInfo.port ;
		ret = SetIPCTimeAndDate(&networkInfo);	
	}
  return ret ;


}






int  SetIPCTimeAndDate(network_info_t *pNetworkInfo)
	{
	#if 1
		time_t t;
		struct tm * area;
		TimeAndDate  pTimeandDate,pTZ;
		int ret = 0;
		int type = 0;
		int hour = 0;
		int minute = 0;
		int ipc_hour= -1;
		int ipc_minute= 0;
		char * ptr = NULL;
	
		Remote_Device_Info DeviceInfo;
		memset(&DeviceInfo, 0, sizeof(Remote_Device_Info));
		strcpy(DeviceInfo.ip, pNetworkInfo->onvifAddr);
		DeviceInfo.port = pNetworkInfo->onvifPort;
		strcpy(DeviceInfo.userName, pNetworkInfo->userName);
		strcpy(DeviceInfo.password, pNetworkInfo->userPwd);
	
		//从数据库获取时区数据
	
		ret = GetTimeZone(pTimeandDate.TZ,&hour,&minute);
		if(ret==-1)
		{
			 Printf("Get timezome err\r\n");
		}	

	#if 0
		ret = ONVIF_GetSystemTimeAndDate(&DeviceInfo,&pTimeandDate);
		if(ret==0)
		{
	
			//china standard  time
			ptr=strstr(pTimeandDate.TZ,"CST");
			if(ptr)
			{
				ptr=strchr(pTimeandDate.TZ,'-');
				if(ptr)
				{
				  sscanf(ptr,"-%d",&ipc_hour);
                  switch(ipc_hour)
                  	{
                       case  12:
					   	    sprintf(pTimeandDate.TZ,"%s","GMT-04:00");
							 break;
                       case  11:
					   	    sprintf(pTimeandDate.TZ,"%s","GMT-03:00");
							 break;
					   case  10:
					   	    sprintf(pTimeandDate.TZ,"%s","GMT-02:00");
							 break;
					   case  9:
					   	    sprintf(pTimeandDate.TZ,"%s","GMT-01:00");
							 break;
                       case  8:
					   	    sprintf(pTimeandDate.TZ,"%s","GMT+00:00");
							 break;
						case  7:
					   	    sprintf(pTimeandDate.TZ,"%s","GMT+01:00");
							 break;	
						case  6:
					   	    sprintf(pTimeandDate.TZ,"%s","GMT+02:00");
							 break;	
						case  5:
					   	    sprintf(pTimeandDate.TZ,"%s","GMT+03:00");
							 break;	 
						case  4:
					   	    sprintf(pTimeandDate.TZ,"%s","GMT+04:00");
							 break;	
						case  3:
					   	    sprintf(pTimeandDate.TZ,"%s","GMT+05:00");
							 break;	
						case  2:
					   	    sprintf(pTimeandDate.TZ,"%s","GMT+06:00");
							 break;
						case  1:
					   	    sprintf(pTimeandDate.TZ,"%s","GMT+07:00");
							 break;
							 default :
							 	break;

				    }  
				
				}
				else 
				{ 
					ptr=strchr(pTimeandDate.TZ,'+');
					if(!ptr)
				    {
				     sprintf(pTimeandDate.TZ,"%s","GMT+08:00");
				    }
			     }
			}

			ptr = strstr(pTimeandDate.TZ,"UTC")
			if(ptr)
				{
                  
				  ptr=strchr(pTimeandDate.TZ,'+');
								  if(ptr)
								  {
									sscanf(ptr,"+%d:%d",&ipc_hour,&ipc_minute);
									if(ipc_hour == hour && ipc_minute == minute)
										{
										  type = UTC;
									    }
								  }

								  if()


				 
    }
			#endif
			if(strstr(pTimeandDate.TZ,"GMT"))
			{
	
				ptr=strchr(pTimeandDate.TZ,'+');
				if(ptr)
				{
				  sscanf(ptr,"+%d:%d",&ipc_hour,&ipc_minute);
				  type	= EAST;
				}
				else
				{
						 ptr =strchr(pTimeandDate.TZ,'-');
						if(ptr)
						{
						 sscanf(ptr,"-%d:%d",&ipc_hour,&ipc_minute);
						 type  = WEST;
						}
						else
						{
							/* 0时区*/
							ipc_hour = 0;
							ipc_minute = 0;
							type = UTC;
						}
				}
		}
			
		t = time(NULL);
	
		switch(type)
		{
			case WEST:
			t += (ipc_hour*3600 + ipc_minute *60 );
			   break;
			case EAST:
			t -= ( ipc_hour*3600 + ipc_minute *60 );
				break;
			case UTC:
			   break;
			default:
				break;
		}
	
		//sprintf(pTimeandDate.TZ,"%s",pTZ.TZ);
		//Printf("pTimeandDate.TZ = %s \r\n ",pTimeandDate.TZ);
		area = localtime(&t);
		pTimeandDate.type = 0;
		pTimeandDate.Year = area->tm_year + 1900;
		pTimeandDate.Month = area->tm_mon +1;
		pTimeandDate.Day = area->tm_mday;
		pTimeandDate.Second = area->tm_sec ;
		pTimeandDate.Hour = area->tm_hour ;
		pTimeandDate.Minute= area->tm_min ;
		pTimeandDate.isUTC = true;
	
		ret = ONVIF_SetSystemTimeAndDate(&DeviceInfo,&pTimeandDate);
#endif			
	
}





int  getNetworkInfoByOnvif(network_info_t *pNetworkInfo, int streamNo)
{
#if 1
	int result = 1;
	char urlBuf[1024] = { 0 };
	int sizeUri = 0;

	Remote_Device_Info DeviceInfo;
	Onvif_Profiles getProfilesRespone;

	memset(&DeviceInfo, 0, sizeof(Remote_Device_Info));
	memset(&getProfilesRespone, 0, sizeof(Onvif_Profiles));
	strcpy(DeviceInfo.ip, pNetworkInfo->onvifAddr);
	DeviceInfo.port = pNetworkInfo->onvifPort;
	strcpy(DeviceInfo.userName, pNetworkInfo->userName);
	strcpy(DeviceInfo.password, pNetworkInfo->userPwd);

	if(1!=result)
	{
		//fprintf(stderr, "============= ONVIF_GetDeviceInformation failed  :%s: %d ,%s \n",__func__,__LINE__ ,DeviceInfo.ip);
		return -1;
	}
	ONVIF_AddRemoteDevice(pNetworkInfo->channelNo,&DeviceInfo);
	result = ONVIF_GetProfiles(&DeviceInfo, &getProfilesRespone);
	if(-1 == result)
	{
		//Printf("get profiles failed onvif=%s:%d ,username=%s ,pwd=%s\n", pNetworkInfo->onvifAddr, pNetworkInfo->onvifPort, DeviceInfo.userName, DeviceInfo.password);
		gpioSetAlarm(1);
		return -1;
	}
	gpioSetAlarm(0);

	//每个profile token 对应一路码流

	sizeUri = getProfilesRespone.sizeOfProfiles;
	if(streamNo < sizeUri)
	{
		int j = 0;
		for ( j = streamNo ; j < sizeUri; j++)
		{
			if ( 2 ==  getProfilesRespone.Onvif_Profiles[j].videoConfig.videoEncoding )
			{
				break;
			}
		}

		if ( j >= sizeUri )
		{
			// fprintf(stderr, "=============got profile  %s: %d ,sizeUri =%d ,%s \n",__func__,__LINE__ ,sizeUri,DeviceInfo.ip);
			return -1;
		}
		result = ONVIF_GetStreamUri(&DeviceInfo, getProfilesRespone.Onvif_Profiles[j].profileToken, urlBuf);
		if(-1 == result)
		{
			Printf("get stream uri failed \n");
			return -1;
		}
		else
		{
			//Printf("channelNo %d, streamNo %d, url = (%s)\r\n", pNetworkInfo->channelNo, streamNo, urlBuf);
			//RTSP_ParseUrl(&pNetworkInfo->port, pNetworkInfo->addr, sizeof(pNetworkInfo->addr), pNetworkInfo->fileName, sizeof(pNetworkInfo->fileName), urlBuf, sizeof(urlBuf));
			//RTSP_ParseUrl(&pNetworkInfo->port, pNetworkInfo->addr, pNetworkInfo->fileName, urlBuf);
			rtspParseDetailURL(urlBuf, pNetworkInfo->addr,&pNetworkInfo->port,pNetworkInfo->fileName);
		}
	}
#endif

	return 0;
}

/*  */
static int networkOnvifEventSubscribe(network_info_t *pNetworkInfo)
{
	Remote_Device_Info DeviceInfo;

	if(1)//pNetworkInfo->streamNo == 0)
	{
		memset(&DeviceInfo, 0, sizeof(Remote_Device_Info));
		strcpy(DeviceInfo.ip, pNetworkInfo->onvifAddr);
		DeviceInfo.port = pNetworkInfo->onvifPort;
		strcpy(DeviceInfo.userName, pNetworkInfo->userName);
		strcpy(DeviceInfo.password, pNetworkInfo->userPwd);
		ONVIF_EventSubscribe(&DeviceInfo);
	}
}
/* 网络库初始化 */
int networkLibInit()
{
	int i = 0, j = 0;   

	memset(g_networkDevChannel, 0, sizeof(g_networkDevChannel));
	memset(gp_networkDevChannel, 0, sizeof(gp_networkDevChannel));
	pthread_mutex_init(&g_msgMutex, NULL);
	pthread_mutex_init(&g_networkMutex, NULL);
	pthread_mutex_init(&g_streamMutex, NULL);
	pthread_mutex_init(&g_loginstatus, NULL);
	for(i = 0; i < configGetDisplayNum(); i++)
	{
		for(j = 0; j < 2; j++)
		{
			queueLinkInit(&g_networkDevChannel[i][j].msgQueue);
		}
	}

	return 0;
}

/* 网络库销毁 */
int networkLibDestroy()
{
	int i = 0, j = 0;

	pthread_mutex_lock(&g_msgMutex);
	for(i = 0; i < configGetDisplayNum(); i++)
	{
		for(j = 0; j < 2; j++)
		{
			queueLinkDestroy(&g_networkDevChannel[i][j].msgQueue);
		}
	}
	pthread_mutex_unlock(&g_msgMutex);
	pthread_mutex_destroy(&g_msgMutex);
	pthread_mutex_destroy(&g_networkMutex);
	pthread_mutex_destroy(&g_streamMutex);

	return 0;
}

