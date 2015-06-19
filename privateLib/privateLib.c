#include "privateLib.h"
#include "logLib.h"
#include "buffer_ring.h"
#include "commonLib.h"
#include "LongseDes.h"
#include <pthread.h>


#if 0
//#define PRIVATE_LOG(fmt, args...) do { fprintf(stderr, "\n========= [%s ] %s line %d " fmt "\n", __FILE__, __FUNCTION__, __LINE__, ##args ); fflush(stderr);   } while ( 0 )
#define PRIVATE_LOG(fmt, args...) do { fprintf(stderr,fmt "\n", ##args ); fflush(stderr);   } while ( 0 )
#else
#define PRIVATE_LOG(fmt, args...)
#endif


#if 0
// extern static unsigned long SESSIONID[MAX_DISPLAY_NUM]  ;
extern pthread_mutex_t g_loginstatus;
int LOGINED[MAX_DISPLAY_NUM] ;
unsigned long SESSIONID[MAX_DISPLAY_NUM]  ;


   
void GetLoginStatus(int channelno,unsigned int * loginstatus)
{
    pthread_mutex_lock(&g_loginstatus);
    *loginstatus = LOGINED[channelno];
    pthread_mutex_unlock(&g_loginstatus);
}
void SetLoginStatus(int channelno,unsigned int loginstatus)
{
    pthread_mutex_lock(&g_loginstatus);
    LOGINED[channelno] = loginstatus;
    pthread_mutex_unlock(&g_loginstatus);
}
void GetSessioID(int channelno,unsigned int * SessionID)
{
    pthread_mutex_lock(&g_loginstatus);
     *SessionID = SESSIONID[channelno];
    pthread_mutex_unlock(&g_loginstatus);
}
void SetSessionID(int channelno,unsigned int SessionID)
{
    pthread_mutex_lock(&g_loginstatus);
     SESSIONID[channelno] = SessionID;
    pthread_mutex_unlock(&g_loginstatus);
}



static private_protocol_info_t g_private_protocol_Infos[MAX_PRIVATE_PROTOCOL_CHANNEL_NUM] = {0};
private_protocol_stream_info_t *private_protocol_stream_init(int channelNum,int streamNum)
{
    private_protocol_stream_info_t *pStreamInfo = 0;

    if(MAX_PRIVATE_PROTOCOL_CHANNEL_NUM<=channelNum || 0>channelNum) return 0;
    if(MAX_PRIVATE_PROTOCOL_STREAM_NUM<=streamNum || 0>streamNum) return 0;


    do{
        pStreamInfo = (private_protocol_stream_info_t*)malloc(sizeof(private_protocol_stream_info_t));
        memset(pStreamInfo,0,sizeof(private_protocol_stream_info_t));

        pStreamInfo->streamSocketFd = -1;
        pStreamInfo->parent = g_private_protocol_Infos+channelNum;
        pStreamInfo->stream_info = (ARG_STREAM *)malloc(sizeof(ARG_STREAM));
        if(0==pStreamInfo->stream_info)break;
        memset(pStreamInfo->stream_info,0,sizeof(ARG_STREAM));

        g_private_protocol_Infos[channelNum].pStreams[streamNum] = pStreamInfo;
        private_protocol_heartbeat();
        return pStreamInfo;
    }while(0);

    if(0!=pStreamInfo){
        PP_FREE(pStreamInfo->stream_info);
    }
    return 0;
}



void private_protocol_stream_free(private_protocol_stream_info_t *pStreamInfo)
{
    private_protocol_info_t *pInfo = pStreamInfo->parent;
    if(0==pStreamInfo) return;

    if(pStreamInfo == pInfo->pStreams[0]) pInfo->pStreams[0] = 0;
    if(pStreamInfo == pInfo->pStreams[1]) pInfo->pStreams[1] = 0;
    if(0==pInfo->pStreams[0] && 0==pInfo->pStreams[0]){
        PP_CLOSE_FD(pInfo->cmdSocketFd);
    }

    PP_CLOSE_FD(pStreamInfo->streamSocketFd);
    PP_FREE(pStreamInfo->stream_info);
    PP_FREE(pStreamInfo);
}

void private_protocol_closeStream(private_protocol_stream_info_t *pStreamInfo)
{
    if(0==pStreamInfo) return;
    PP_CLOSE_FD(pStreamInfo->streamSocketFd);
}


static pthread_t g_heartbeatThread[2] = {0};
void private_protocol_heartbeat()
{
    if(0==g_heartbeatThread[0]){
        pthread_create(g_heartbeatThread,0,(void*(*)(void*))(private_protocol_sendHeartbeat),0);
    }
    if(0==g_heartbeatThread[1]){
        pthread_create(g_heartbeatThread,0,(void*(*)(void*))(private_protocol_recvHeartbeat),0);
    }
}


int private_protocol_wait(private_protocol_info_t *pInfo, int sec)
{
    if(0==pInfo) return -1;

    sec = 3;
    while(pInfo->isWaitReply){
        if(sec-- > 0) sleep(1);
        else break;
    }
    return (0==pInfo->isWaitReply) ? 0 : -1;
}

/*
 * 这个线程用于发送心跳
*/
void private_protocol_sendHeartbeat()
{
    private_protocol_info_t * pInfo;
    char buff[ARG_BUFF_LEN];
    ARG_CMD *pCmd = (ARG_CMD*)buff;

    struct timeval timeout;
    int i;

    pCmd->ulFlag = ARG_CMD_HEAD;
    pCmd->ulVersion = ARG_SDK_VERSION_1_1;
    pCmd->usCmd = CMD_ACT_HEADRTBEAT;

    while (1) {
        timeout.tv_sec = 4;
        timeout.tv_usec = 0;

        select(0,0,0,0,&timeout);

        for(i=0;i<MAX_PRIVATE_PROTOCOL_CHANNEL_NUM;i++){
            pInfo = g_private_protocol_Infos+i;
            if(1==pInfo->isWaitHeartbeatReply){
                PP_CLOSE_FD(pInfo->cmdSocketFd);
                if(pInfo->pStreams[0]){
                    PP_CLOSE_FD(pInfo->pStreams[0]->streamSocketFd);
                }
                if(pInfo->pStreams[1]){
                    PP_CLOSE_FD(pInfo->pStreams[1]->streamSocketFd);
                }
                printf("heart beat timeout !\n");
                continue;
            }
            if(pInfo->cmdSocketFd > 0){
                pCmd->ulID =pInfo->userId;
                send(pInfo->cmdSocketFd,(void*)pCmd,ARG_HEAD_LEN,0);
                pInfo->isWaitHeartbeatReply = 1;
                printf("send heartbeat cmd!\n");
            }
        }
    }
}

/*
 * 这个线程用于接收心跳.
*/
void private_protocol_recvHeartbeat()
{
    private_protocol_info_t * pInfo;
    char buff[ARG_BUFF_LEN];
    ARG_CMD *pCmd = (ARG_CMD*)buff;

    struct timeval timeout;
    fd_set fds;
    int maxFd;

    int recvLen;
    int rsl;
    int i;

    while(1){
        FD_ZERO(&fds);
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        maxFd = -1;

        for(i=0;i<MAX_PRIVATE_PROTOCOL_CHANNEL_NUM;i++){
            pInfo = &g_private_protocol_Infos[i];
            if(0!=pInfo && pInfo->cmdSocketFd > 0){
                FD_SET(pInfo->cmdSocketFd,&fds);
                if(pInfo->cmdSocketFd>maxFd) maxFd = pInfo->cmdSocketFd;
            }
        }

        rsl = select(maxFd+1,&fds,0,0,&timeout);
        if(-1==rsl){ sleep(1); printf("error\n"); continue;}
        else if(0 == rsl) { printf("timeout\n"); continue; }

        for(i=0;i<MAX_PRIVATE_PROTOCOL_CHANNEL_NUM;i++){
            pInfo = &g_private_protocol_Infos[i];
            if(0!=pInfo){
                if(1 == pInfo->isWaitReply){
                    printf("waiting cmd, skip...\n");
                    usleep(10*1000);
                    continue;
                }
                rsl = FD_ISSET(pInfo->cmdSocketFd,&fds);
                if(rsl == 1){
                    //这里有可能接收到除heartbeat 以外的命令的
                    recvLen = recv(pInfo->cmdSocketFd,(void*)pCmd,ARG_HEAD_LEN,0);
                    if(0==recvLen){
                        PP_CLOSE_FD(pInfo->cmdSocketFd);
                    }
                    if(ARG_HEAD_LEN != recvLen) continue;
                    if(pCmd->ulBufferSize>0){
                        recv(pInfo->cmdSocketFd,(void*)(buff+ARG_HEAD_LEN),pCmd->ulBufferSize,0);
                    }
                    if(pCmd->usCmd == CMD_ACT_HEADRTBEAT){
                        printf("get heartbet response! rsl =%u\n",pCmd->ucState);
                    }
                    pInfo->isWaitHeartbeatReply = 0;
                }
            }
        }
    }
}


int private_protocol_sendCmd(private_protocol_stream_info_t* pStreamInfo, int cmdType, char *inData, int inLen, char *outData, int *outLen)
{
    if(0== pStreamInfo) return -1;

    private_protocol_info_t *pInfo =pStreamInfo->parent;

    //如果别的线程真正发送或等待命令则返回失败
    if(pInfo->isWaitReply) return -1;

    char buff[ARG_BUFF_LEN] = {0};
    ARG_CMD *cmd = (ARG_CMD *)buff;
    int socketFd;
    int rsl;

    socketFd = (CMD_GET_STREAM==cmdType) ? pStreamInfo->streamSocketFd : pInfo->cmdSocketFd;
    if(0>socketFd) return -1;

    cmd->ulFlag = ARG_CMD_HEAD;
    cmd->ulVersion = ARG_SDK_VERSION_1_1;
    cmd->usCmd = cmdType;
    cmd->ulID = pInfo->userId;
    cmd->ulBufferSize = inLen;
    if(cmd->ulBufferSize > 0){
        memcpy(buff+ARG_HEAD_LEN,(void*)inData,cmd->ulBufferSize);
    }

    do{
        //表示有函数等待回复,让接收心跳的线程不要接收这个消息
        pInfo->isWaitReply = 1;
        rsl = send(socketFd,(void*)buff,ARG_HEAD_LEN+cmd->ulBufferSize,0);
        if(rsl != (int)(ARG_HEAD_LEN+cmd->ulBufferSize)){
            PRIVATE_LOG("send data failed! rsl = %d",rsl);
            break;
        }

        rsl = SK_SelectWait(socketFd,3000);
        if(0!=rsl) break;

        rsl = recv(socketFd,(void*)cmd,ARG_HEAD_LEN,0);
        if(ARG_HEAD_LEN != rsl) break;

        if(cmd->ulBufferSize>0){
            rsl = recv(socketFd,(void*)(buff+ARG_HEAD_LEN),cmd->ulBufferSize,0);
            if(cmd->ulBufferSize != (unsigned long)rsl) break;
        }
        if(cmd->ulBufferSize>0){
            if(outData!=0) memcpy((void*)outData,(void*)(buff+ARG_HEAD_LEN),cmd->ulBufferSize);
            if(outLen!=0) *outLen = cmd->ulBufferSize;
        }

//      if(cmd->usCmd != cmdType) continue; //有可能读取到心跳线程的回复.在这里进行过滤
        if(CMD_ACT_LOGIN==cmd->usCmd && CMD_SUCCESS==cmd->ucState){
            pInfo->userId = cmd->ulID;
        }

        pInfo->isWaitReply = 0;
        pInfo->isWaitHeartbeatReply = 0;
        return cmd->ucState;
    }while(0);
    pInfo->isWaitReply = 0;
    return -1;
}

int private_protocol_login(private_protocol_stream_info_t *pStreamInfo, unsigned int ip, unsigned int port, char *name, char *passwd)
{
    if(0==pStreamInfo) return -1;

    private_protocol_info_t *pInfo = pStreamInfo->parent;

    USER_INFO userInfo;
    char serialNum[ARG_SERIALNUM_LEN]={0};
    char passwdBuff[16]={0};
    int rsl;

    memset(&userInfo,0,sizeof(USER_INFO));
    if(0==pInfo) return -1;
    pInfo->ip = ip;
    pInfo->port = port;


    if(pInfo->pStreams[0] != pStreamInfo &&
       pInfo->pStreams[1] != pStreamInfo) return -1;


    rsl = private_protocol_wait(pInfo,3);
    if(0!=rsl) return -1;
    //如果已经登录成功则返回成功
    if(0<pInfo->cmdSocketFd) return 0;

    do{
        pInfo->ip = ip;
        pInfo->port = port;
        pInfo->userId = 0;
        pInfo->cmdState = 0;

        pInfo->cmdSocketFd = SK_ConnectTo(ip,port);
        if(0>pInfo->cmdSocketFd){ break; }

        rsl = private_protocol_sendCmd(pStreamInfo,CMD_GET_SERIALNUM,0,0,serialNum,0);
        if(0!=rsl) break;

        sprintf((char*)userInfo.ucUsername,"%s",name);
        sprintf(passwdBuff,"%s",passwd);
        DES_Encode((char*)userInfo.ucPassWord,passwdBuff,serialNum,16);
        DES_Encode((char*)userInfo.ucSerialNum,serialNum,serialNum,ARG_SERIALNUM_LEN);
        rsl = private_protocol_sendCmd(pStreamInfo,CMD_ACT_LOGIN,(char*)&userInfo,sizeof(USER_INFO),0,0);
        if(0!=rsl) break;

        return 0;
    }while(0);

    Printf("login failed!\n");
    PP_CLOSE_FD(pInfo->cmdSocketFd);
    if(pInfo->pStreams[0]){
        PP_CLOSE_FD(pInfo->pStreams[0]->streamSocketFd);
    }
    if(pInfo->pStreams[1]){
        PP_CLOSE_FD(pInfo->pStreams[1]->streamSocketFd);
    }
    pInfo->ip = 0;
    pInfo->port = 0;
    pInfo->userId = 0;
    pInfo->cmdState = 0;

    return -1;
}

int private_protocol_getStream(private_protocol_stream_info_t *pStreamInfo, int channelNo, int streamNo)
{
    private_protocol_info_t *pInfo = pStreamInfo->parent;
    STREAM_INFO streamInfo;
    int rsl;

    if(0==pInfo){ return -1; }

    memset(&streamInfo,0,sizeof(STREAM_INFO));
    streamInfo.ucCH = channelNo;
    streamInfo.ucStreamType = streamNo;

    PP_CLOSE_FD(pStreamInfo->streamSocketFd);
    do{

        rsl = private_protocol_wait(pInfo,3);
        if(rsl != 0) break;

        pStreamInfo->streamSocketFd = SK_ConnectTo(pInfo->ip,pInfo->port);
        if(0>pStreamInfo->streamSocketFd) break;

        rsl = private_protocol_sendCmd(pStreamInfo,CMD_GET_STREAM,(char*)&streamInfo,sizeof(STREAM_INFO),0,0);
        if(0!=rsl) break;

        printf("get stream success!\n");

        return 0;
    }while(0);
    printf("get tream failed! rsl = %d\n",rsl);
    PP_CLOSE_FD(pStreamInfo->streamSocketFd);
    return -1;
}
#endif

/*
 * ******************************************************************************************************************************************************************************************************************
 * *********************************************************** 分割线 **********************************************************************************************************************************************
 * ******************************************************************************************************************************************************************************************************************
*/

int Private_Protocol_RecvStreamToRingBuf(int streamSocketFd,ARG_STREAM * stream_info, stream_packet_queue_t * pPacketQueue,unsigned long * SeqNO)
{
    //PRIVATE_LOG("Socket=%d",private_protocol->Socket);
    int ret = 0;
    int recvLen = 0;
    int len = -1;
    char *pBuf = NULL;
    int nTotal = 0;        //مه؛و؛مهممهم­ون
    int nRead = 0;        //موهrecvنمىو؛مهم­ون
    int nLeft = 0;        //ملننهم­ون
    int nBytes = 0;        //مممو؛ننمهممهله
    char HeadBuff[64]={0};
    int timeoutNum = 0;
    ret = netSocketSelectWait(streamSocketFd, 2000);
    if(ret == 1)
    {

        len = recv(streamSocketFd,HeadBuff , sizeof(ARG_STREAM), 0);
        ARG_STREAM * pHeadinfo =  (ARG_STREAM *)HeadBuff;
        PRIVATE_LOG("Socket=%d,size=%d,bMediaType =%d,serialno=%d,height=%d,wigth =%d",streamSocketFd,pHeadinfo->bSize,pHeadinfo->bMediaType,
                    pHeadinfo->ucSerialNum,pHeadinfo->usHeight,pHeadinfo->usWidth);
        if(len == sizeof(ARG_STREAM))
        {
#if 1
            if((HeadBuff[0] & 0xff) != 0x01||(HeadBuff[1] & 0xff)!=0x00)
            {
                PRIVATE_LOG(stderr,"###############%s:%d pBuf[0] = %d,pBuf[1]=%d##########################\n",__func__,__LINE__,HeadBuff[0],HeadBuff[1]);
                return -1;
            }
            PRIVATE_LOG("Socket=%d,size=%d,bMediaType =%d,serialno=%d,height=%d,wigth =%d,recvLen = %d,dataEnd =%d",streamSocketFd,pHeadinfo->bSize,pHeadinfo->bMediaType,
                        pHeadinfo->ucSerialNum,pHeadinfo->usHeight,pHeadinfo->usWidth,recvLen,pPacketQueue->ringBuf.dataEnd);

            stream_info->bMediaType = pHeadinfo->bMediaType;
            stream_info->bSize = pHeadinfo->bSize;
            stream_info->ucFrameRate = 30;
            stream_info->ucBitRate = 2048;
            stream_info->usHeight = pHeadinfo->usHeight;
            stream_info->usWidth = pHeadinfo->usWidth;
            stream_info->ucSerialNum= pHeadinfo->ucSerialNum;
            stream_info->usCH=pHeadinfo->usCH;

            if( ((*SeqNO + 1) != pHeadinfo->ucSerialNum))
            {
                fprintf(stderr,"####################lost packet , seqNo = %d ~ %d\r\n", *SeqNO, pHeadinfo->ucSerialNum);
            }

            *SeqNO = (pHeadinfo->ucSerialNum);
#endif

            //usleep(2*1000);
            pPacketQueue->startFlag = 1;


            recvLen = ring_free_size_ex(&pPacketQueue->ringBuf);
            pBuf = ring_get_start_buf(&pPacketQueue->ringBuf);

#if 1
            if(recvLen < pHeadinfo->bSize)
            {
                ring_reset(&pPacketQueue->ringBuf);
                pBuf = ring_get_start_buf(&pPacketQueue->ringBuf);
            }
#endif

#if 1
            nLeft = pHeadinfo->bSize;
            do
            {  
                nRead = recv(streamSocketFd, pBuf +nBytes,nLeft,0);//نننن
                PRIVATE_LOG("socket=%d,len=%d,size=%d,recvLen =%d,pPacketQueue->ringBuf.dataEnd =%d",private_protocol->streamSocketFd,nRead,pHeadinfo->bSize,recvLen,pPacketQueue->ringBuf.dataEnd);
                if(-1 == nRead)//و؛نلم¤و
                {   
                    PRIVATE_LOG("socket=%d,ret=%d ;%s",private_protocol->streamSocketFd,nRead,strerror(errno));
                    if (EINTR == errno || EWOULDBLOCK == errno ||errno ==EAGAIN)//
                    {
                        //fprintf(stderr,"socket=%d  ,  %s \n",private_protocol->Socket,strerror(errno));
                                  		 usleep(10*1000);
						timeoutNum ++;    
						if(timeoutNum >= 200){ /*200*(15)بسغ3s سغ3m؛أ؛سذتصحيصقز؛ضح؛ظت*/
							return -1;
						} 
                        continue;//نهو؛منن
                    }   
                    else
                    {
                       // fprintf(stderr,"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&socket=%d :%s",private_protocol->Socket,strerror(errno));
                        return -1;
                    }
                }
                //و؟نمه؛ن­م
                if (0 == nRead)
                {
                    fprintf(stderr,"%s:%d    come here\n",__func__,__LINE__);
                    PRIVATE_LOG("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&socket=%d",streamSocketFd);
                    return -1;
                }
                if(nRead < nLeft)
                {
                    PRIVATE_LOG("socket=%d",streamSocketFd);
                    usleep(2*1000);
                }

                timeoutNum = 0;
                nTotal += nRead;
                nLeft -= nRead;
                nBytes += nRead;


            } while (nTotal != pHeadinfo->bSize);    //مه؛و؛مهممهم­ونله­لىوو؛مهم¤من

            ring_set_data_end(&pPacketQueue->ringBuf, nTotal );
           // fprintf(stderr,"socket=%d;seqno =%d, nTotal=%d size=%d height =%d,wigth=%d \r\n",private_protocol->Socket,private_protocol->stream_info->ucSerialNum,nTotal,pHeadinfo->bSize,pHeadinfo->usHeight,pHeadinfo->usWidth);
#endif
        }
        else
        {
            len = -1;
            PRIVATE_LOG("socket=%d,%s",streamSocketFd,strerror(errno));
        }
    }
    return len;
}


#define SPS 7
#define PPS 8

int Private_Protocol_ParseStreamFromRingBuf(int streamSocketFd,ARG_STREAM * stream_info, stream_packet_queue_t * pPacketQueue,   stream_packet_head_t *pPacketHead, int *pVideoFrameLen)
{


    unsigned char * PacketBuffAddr = NULL;
    int channel = 0;
    int len = 0;
    len = stream_info->bSize;
    channel = stream_info->usCH;
    pPacketHead->channel =  stream_info->bMediaType;
    pPacketHead->seq =stream_info->ucSerialNum;
    pPacketHead->offset = 0;
    pPacketHead->len =  len ;
    PacketBuffAddr = GetPacketBufferAddr(&pPacketQueue->packetBuf,FRAME_START);
    ring_pop_n(&pPacketQueue->ringBuf, PacketBuffAddr, len,MAX_FRAME_BUFFER_LEN);

   if(SPS== (*(PacketBuffAddr+4) & 0x1F))
     {
	pPacketHead->isKey = 1;
		
     }

    *pVideoFrameLen = len + 4;

    return 0;
}

int Private_Protocol_PacketToFrameQueue(int streamSocketFd,ARG_STREAM * stream_info, stream_packet_queue_t *pPacketQueue, stream_packet_head_t *pPacketHead, stream_frame_queue_t *pAudioFrameQueue, stream_frame_queue_t *pVideoFrameQueue,
                                        int forceRestartNum,  int *pVideoFrameLen, int *pVideoFrameNo, int * pAudioFrameNo)
{
    int videoFrameLen = 0, audioFrameLen = 0;
    int frameOffset = 0;//
    stream_frame_t *pVideoFrame = NULL, *pAudioFrame = NULL;

    /* audio */
    if(pPacketHead->channel == 0)
    {//
#if 0
        audioFrameLen = pPacket->packetHead.len;
        streamFrameQueueLock(pAudioFrameQueue);
        pAudioFrame = streamGetFrameQueueBuffer(pAudioFrameQueue, audioFrameLen + 4);//4
        if(pAudioFrame != NULL)
        {
            pAudioFrame->frameHead.type = FRAME_TYPE_AUDIO;
            pAudioFrame->frameHead.frameNo = ((*pVideoFrameNo)++)&0xFFFF;
            //			pAudioFrame->frameHead.pts = time(0);//pUsedPacket->packetHead.pts;
            //			pAudioFrame->frameHead.ipcPts = pPacket->packetHead.pts;
            gettimeofday(&pAudioFrame->frameHead.tv, NULL);
            pAudioFrame->frameHead.info.audio.format = 0;//FRAME_AUDIO_TYPE_G711;
            pAudioFrame->frameHead.info.audio.sampleRate = FRAME_AUDIO_SAMPLE_RATE_32K;
            pAudioFrame->frameHead.info.audio.channel = FRAME_AUDIO_CHANNEL_1;
            pAudioFrame->frameHead.info.audio.bitWide = FRAME_AUDIO_BIT_WIDE_16;
            pAudioFrame->frameHead.len = 0;
            pAudioFrame->pFrameBuf[0] = 0x00;
            pAudioFrame->pFrameBuf[1] = 0x01;
            pAudioFrame->pFrameBuf[2] = 0xA0;
            pAudioFrame->pFrameBuf[3] = 0;
            //memcpy(pAudioFrame->pFrameBuf + 4, &pPacket->packetBuf[pPacket->packetHead.offset + sizeof(rtp_head_t)], audioFrameLen);
            memcpy(pAudioFrame->pFrameBuf + 4, pPacket->pPacketBuf + pPacket->packetHead.offset + sizeof(rtp_head_t), audioFrameLen);
            streamInsertFrameToFrameQueue(pAudioFrameQueue, pAudioFrame);
        }
        steramFrameQueueUnlock(pAudioFrameQueue);
#endif
    }
    else
    {
            videoFrameLen = *pVideoFrameLen;
            streamFrameQueueLock(pVideoFrameQueue);
            pVideoFrame = streamGetFrameQueueBuffer(pVideoFrameQueue, videoFrameLen);
            if(pVideoFrame != NULL)
            {
                pVideoFrame->frameHead.type = FRAME_TYPE_VIDEO;
                pVideoFrame->frameHead.frameNo = ((*pVideoFrameNo)++)&0xFFFF;
                pVideoFrame->frameHead.source = 0;
                pVideoFrame->frameHead.info.video.format = FRAME_VIDEO_TYPE_H264;
                pVideoFrame->frameHead.info.video.width = stream_info->usWidth;
                pVideoFrame->frameHead.info.video.height = stream_info->usHeight;
                pVideoFrame->frameHead.info.video.frameRate = stream_info->ucFrameRate;
                pVideoFrame->frameHead.info.video.bitRate = stream_info->ucBitRate/1024;
                pVideoFrame->frameHead.info.video.forceRestartNum = forceRestartNum;
                pVideoFrame->frameHead.len = videoFrameLen;
                frameOffset = 4;
                pVideoFrame->pFrameBuf[0] = 0x00;
                pVideoFrame->pFrameBuf[1] = 0x00;
                pVideoFrame->pFrameBuf[2] = 0x00;
                pVideoFrame->pFrameBuf[3] = 0x01;
                memcpy(pVideoFrame->pFrameBuf, pPacketQueue->packetBuf.BufferAddr,videoFrameLen );
                gettimeofday(&pVideoFrame->frameHead.tv, NULL);
               pVideoFrame->frameHead.pts = pPacketHead->pts; //time(0);
                streamInsertFrameToFrameQueue(pVideoFrameQueue, pVideoFrame);

            }
            steramFrameQueueUnlock(pVideoFrameQueue);

        }

        return 0;
}



