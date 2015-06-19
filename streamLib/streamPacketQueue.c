
/*
* 本模块的使用请参考 StreamServiceThread
*/

#include "streamLib.h"
#include "commonLib.h"
#include "streamPacketQueue.h"
#include "streamFrameQueue.h"

#include "ncx_slab.h"
#include "buffer_ring.h"
/* 初始化包队列 */
int streamInitPacketQueue(stream_packet_queue_t *pPacketQueue,int protocol)
{
#if 0
	int i = 0;

	pPacketQueue->packetTotalNum = MAX_ONE_FRAME_HAVE_PACKET_NUM;//缓冲包的数目

	/* 分配链表的总内存空间 */
	if(pPacketQueue->pPacketBuf != NULL)
	{
		free(pPacketQueue->pPacketBuf);
		pPacketQueue->pPacketBuf = NULL;
	}
	pPacketQueue->pPacketBuf = (stream_packet_t *)malloc(sizeof(stream_packet_t) * pPacketQueue->packetTotalNum);
	if(pPacketQueue->pPacketBuf == NULL)
	{
		return -1;
	}

	/* 初始化free/used队列 */
	memset(pPacketQueue->pPacketBuf, 0, sizeof(stream_packet_t) * pPacketQueue->packetTotalNum);
	pPacketQueue->freeHead.next = (stream_packet_t *)&pPacketQueue->pPacketBuf[0];

    if(protocol==0)
    {
        for(i = 0; i < pPacketQueue->packetTotalNum - 1; i++)
        {//将buffer链接起来
            pPacketQueue->pPacketBuf[i].next = (stream_packet_t *)&pPacketQueue->pPacketBuf[i+1];
#if 1
            pPacketQueue->pPacketBuf[i].pPacketBuf = (char *)malloc(MAX_STREAM_PACKET_SIZE);
            if( pPacketQueue->pPacketBuf[i].pPacketBuf ==NULL)
            {
                usleep(10*1000);
                pPacketQueue->pPacketBuf[i].pPacketBuf = (char *)malloc(MAX_STREAM_PACKET_SIZE);
                continue;
            }
#endif

		}
	}
	else if(protocol==1)
	{
		for(i = 0; i < pPacketQueue->packetTotalNum - 1; i++)
		{//buffer
			pPacketQueue->pPacketBuf[i].next = (stream_packet_t *)&pPacketQueue->pPacketBuf[i+1];
		}
	}
	pPacketQueue->pFreeLast = (stream_packet_t *)&pPacketQueue->pPacketBuf[i];
	pPacketQueue->freeBufNum = pPacketQueue->packetTotalNum;
	pPacketQueue->usedBufNum = 0;
	pPacketQueue->copyBufNum =0;



#if 0
	pPacketQueue->pPoolBuf = (char *)malloc(MAX_FRAME_BUFFER_LEN);
	memset(pPacketQueue->pPoolBuf, 0, MAX_FRAME_BUFFER_LEN);
	pPacketQueue->sp = (ncx_slab_pool_t*) pPacketQueue->pPoolBuf;
	pPacketQueue->sp->addr = pPacketQueue->pPoolBuf;
	pPacketQueue->sp->min_shift = 3;
	pPacketQueue->sp->end = pPacketQueue->pPoolBuf + MAX_FRAME_BUFFER_LEN;
	ncx_slab_init(pPacketQueue->sp);

	ncx_slab_stat_t stat;
	ncx_slab_stat(pPacketQueue->sp, &stat);
#endif

#endif

    int ret = 0;
    ret = ring_init(&pPacketQueue->ringBuf, MAX_FRAME_BUFFER_LEN);
    ret = PacketBuffer_init(&pPacketQueue->packetBuf,MAX_FRAME_BUFFER_LEN);

    return ret;
}


/* 销毁流 */
void streamUninitPacketQueue(stream_packet_queue_t *pPacketQueue,int protocol)
{

#if 0
	int i ;
	if(1 == protocol)
	{
		for(i = 0; i < pPacketQueue->packetTotalNum - 1; i++)
		{//buffer
			if(pPacketQueue->pPacketBuf[i].pPacketBuf!= NULL)
			{
				free(pPacketQueue->pPacketBuf[i].pPacketBuf);
				pPacketQueue->pPacketBuf[i].pPacketBuf = NULL;
			}
		}
	}

	if(pPacketQueue->pPacketBuf != NULL)
	{

		free(pPacketQueue->pPacketBuf);
		pPacketQueue->pPacketBuf = NULL;
	}

	if(pPacketQueue->pPoolBuf != NULL)
	{
		free(pPacketQueue->pPoolBuf);
		pPacketQueue->pPoolBuf = NULL;
	}
 #endif
	ring_destroy(&pPacketQueue->ringBuf);
    PacketBuffer_destroy(&pPacketQueue->ringBuf);
}

stream_packet_t * private_protocol_streamGetPacketWriteBuffer(stream_packet_queue_t *pPacketQueue)
{
#if TEMP
	stream_packet_t *pTmpBuf = NULL;
	do
	{
		if(pPacketQueue->freeHead.next == NULL)
		{//
			//  Printf("can not find free buffer, used num %d, free num %d\r\n", pPacketQueue->usedBufNum, pPacketQueue->freeBufNum);
			streamPushPacketBufferToFrameBuffer(pPacketQueue);//bufferbuffer
			break;
		}

		/* buffer */
		pTmpBuf = pPacketQueue->freeHead.next;
		pPacketQueue->freeHead.next = pPacketQueue->freeHead.next->next;
#if 0
		pPacketQueue->freeBufNum--;
		if(pPacketQueue->freeBufNum == 0)
		{
			pPacketQueue->pFreeLast = NULL;
		}
#endif

		if(pTmpBuf->pPacketBuf != NULL)
		{
			// Printf("WWWWWWWWWWWWWWWWWWWWWWWWWWWW\r\n");
			free(pTmpBuf->pPacketBuf);
			//ncx_slab_free(pPacketQueue->sp, pTmpBuf->pPacketBuf);
			pTmpBuf->pPacketBuf = NULL;
		}

		memset(pTmpBuf, 0, sizeof(stream_packet_t));

	}while(0);

	return pTmpBuf;
#endif
}


/*********************************************************
 * 获取空闲的包buffer
 * packetSeq:	包序号
 *********************************************************/
stream_packet_t * streamGetPacketWriteBuffer(stream_packet_queue_t *pPacketQueue)
{
#if TEMP
    stream_packet_t *pTmpBuf = NULL;
    do
    {
        if(pPacketQueue->freeHead.next == NULL)
        {////无可用的缓冲空间
            //  Printf("can not find free buffer, used num %d, free num %d\r\n", pPacketQueue->usedBufNum, pPacketQueue->freeBufNum);
            streamPushPacketBufferToFrameBuffer(pPacketQueue);//将现有的包buffer都丢入帧buffer
            break;
        }

        /* 取得第一个空闲buffer，并将其移出空闲链表 */
        pTmpBuf = pPacketQueue->freeHead.next;
        pPacketQueue->freeHead.next = pPacketQueue->freeHead.next->next;
#if 1
        pPacketQueue->freeBufNum--;
        if(pPacketQueue->freeBufNum == 0)
        {
            pPacketQueue->pFreeLast = NULL;
        }
#endif

#if 0
        if(pTmpBuf->pPacketBuf != NULL)
        {
           // Printf("WWWWWWWWWWWWWWWWWWWWWWWWWWWW\r\n");
            free(pTmpBuf->pPacketBuf);
            //ncx_slab_free(pPacketQueue->sp, pTmpBuf->pPacketBuf);
            pTmpBuf->pPacketBuf = NULL;
        }
 #endif

		//  memset(pTmpBuf, 0, sizeof(stream_packet_t));
		pTmpBuf->next = NULL;
		pTmpBuf->pre = NULL;



	}while(0);

	return pTmpBuf;
#endif
}

/* 将流buffer插入到使用队列中 */
int streamInsertQueue(stream_packet_queue_t *pPacketQueue, stream_packet_t *pStreamBuf)
{
#if TEMP
	int ret = 0;
	stream_packet_t *pTmpBuf = NULL;

	pTmpBuf = pPacketQueue->pUsedLast;
	if(pTmpBuf == NULL)
	{
		//		Printf("the first packet, seq %d\r\n", pStreamBuf->packetHead.seq);
		pPacketQueue->usedHead.next = pStreamBuf;
		pPacketQueue->pUsedLast = pStreamBuf;
		pStreamBuf->next = NULL;
		pStreamBuf->pre = NULL;
	}
	else
	{
		pTmpBuf->next = pStreamBuf;
		pStreamBuf->next = NULL;
		pStreamBuf->pre = pTmpBuf;
		pPacketQueue->pUsedLast = pStreamBuf;
	}
	pPacketQueue->usedBufNum++;

	return ret;
#endif
}

/*********************************************************
 * 获取使用队列的第一个包buffer
 *********************************************************/
stream_packet_t * streamGetPacketUsedBuffer(stream_packet_queue_t *pPacketQueue)
{

#if TEMP
	stream_packet_t * pTmpBuf = NULL;

	do
	{
		if(pPacketQueue->usedHead.next == NULL)
		{//无可用的缓冲空间
			pPacketQueue->copyBufNum= 0;
			//pPacketQueue->freeBufNum= 0;
			//Printf("can not find used buffer, used num %d, free num %d\r\n", pPacketQueue->usedBufNum, pPacketQueue->freeBufNum);
			break;
		}

		/* 取得第一个空闲buffer，并将其移出空闲链表 */
		pTmpBuf = pPacketQueue->usedHead.next;
		pPacketQueue->usedHead.next = pPacketQueue->usedHead.next->next;
		pPacketQueue->usedBufNum--;
		if(pPacketQueue->usedBufNum == 0)
		{
			pPacketQueue->pUsedLast = NULL;
		}
	}while(0);


	return pTmpBuf;
#endif
}


void private_protocol_streamFreeUsedBuffer(stream_packet_queue_t *pPacketQueue, stream_packet_t *pStreamBuf)
{
#if TEMP
	stream_packet_t *pTmpBuf = NULL;

#if 1
	if(pStreamBuf->pPacketBuf != NULL)
	{
		 free(pStreamBuf->pPacketBuf);
		//ncx_slab_free(pPacketQueue->sp, pStreamBuf->pPacketBuf);
		pStreamBuf->pPacketBuf = NULL;
	}
#endif


	memset(pStreamBuf, 0, sizeof(stream_packet_t));

	/* buffer */
	pTmpBuf = pPacketQueue->pFreeLast;
	if(pTmpBuf == NULL)
	{

		pPacketQueue->freeHead.next = pStreamBuf;
		pPacketQueue->pFreeLast = pStreamBuf;
	}
	else
	{

		pTmpBuf->next = pStreamBuf;
		pStreamBuf->pre = pTmpBuf;
		pPacketQueue->pFreeLast = pStreamBuf;
	}
	pPacketQueue->freeBufNum++;
	pPacketQueue->copyBufNum++;

	// fprintf(stderr,"!!!!!!!!!!!pPacketQueue->pPacketBufNum =%d ,pStreamBuf->packetHead.channel =%d\r\n",pPacketQueue->pPacketBufNum,pStreamBuf->packetHead.channel);
#endif
}


/* 将使用过的包buffer插入到未使用的链表中 */
void streamFreeUsedBuffer(stream_packet_queue_t *pPacketQueue, stream_packet_t *pStreamBuf)
{

#if TEMP
	stream_packet_t *pTmpBuf = NULL;

#if 0
	if(pStreamBuf->pPacketBuf != NULL)
	{
		Printf("WWWWWWWWWWWWWWWWWWWWWWWWWWWW\r\n");
		// free(pStreamBuf->pPacketBuf);
		//ncx_slab_free(pPacketQueue->sp, pStreamBuf->pPacketBuf);
		pStreamBuf->pPacketBuf = NULL;
	}
#endif


	//  memset(pStreamBuf, 0, sizeof(stream_packet_t));
	pStreamBuf->next = NULL;
	pStreamBuf->pre = NULL;

	/* 将buffer插入到未使用链表的最后 */
	pTmpBuf = pPacketQueue->pFreeLast;
	if(pTmpBuf == NULL)
	{

		pPacketQueue->freeHead.next = pStreamBuf;
		pPacketQueue->pFreeLast = pStreamBuf;
	}
	else
	{

		pTmpBuf->next = pStreamBuf;
		pStreamBuf->pre = pTmpBuf;
		pPacketQueue->pFreeLast = pStreamBuf;
	}
	pPacketQueue->freeBufNum++;
	pPacketQueue->copyBufNum++;

	// fprintf(stderr,"!!!!!!!!!!!pPacketQueue->pPacketBufNum =%d ,pStreamBuf->packetHead.channel =%d\r\n",pPacketQueue->pPacketBufNum,pStreamBuf->packetHead.channel);

#endif
}

void AudiostreamFreeUsedBuffer(stream_packet_queue_t *pPacketQueue, stream_packet_t *pStreamBuf)
{
#if TEMP
    stream_packet_t *pTmpBuf = NULL;

#if 0
    if(pStreamBuf->pPacketBuf != NULL)
    {
      //  Printf("WWWWWWWWWWWWWWWWWWWWWWWWWWWW\r\n");
         free(pStreamBuf->pPacketBuf);
        //ncx_slab_free(pPacketQueue->sp, pStreamBuf->pPacketBuf);
        pStreamBuf->pPacketBuf = NULL;
    }
#endif


    //  memset(pStreamBuf, 0, sizeof(stream_packet_t));
    pStreamBuf->next = NULL;
    pStreamBuf->pre = NULL;

    /* buffer */
    pTmpBuf = pPacketQueue->pFreeLast;
    if(pTmpBuf == NULL)
    {
        pPacketQueue->freeHead.next = pStreamBuf;
        pPacketQueue->pFreeLast = pStreamBuf;
    }
    else
    {
        pTmpBuf->next = pStreamBuf;
        pStreamBuf->pre = pTmpBuf;
        pPacketQueue->pFreeLast = pStreamBuf;
    }
     pPacketQueue->freeBufNum++;
    // pPacketQueue->copyBufNum++;

    // fprintf(stderr,"!!!!!!!!!!!pPacketQueue->pPacketBufNum =%d ,pStreamBuf->packetHead.channel =%d\r\n",pPacketQueue->freeBufNum,pStreamBuf->packetHead.channel);
#endif
}

/* 获取一帧视频 */
int streamGetFrameBuf(stream_packet_queue_t *pPacketQueue, char *pStreamBuf, int streamBufLen)
{
#if TEMP
	int streamLen = 0;

	stream_packet_t *pTmpBuf = NULL;
	int status = 1;//1//1：获取一帧的开始 3：获取后续的帧  2：获取一帧结束  0：总共就一个包  5：出错
	int offset = 0;//已经拷贝的帧数据
	unsigned short packetNo = 0;//包序号

	static int count = 0;


	while(1)
	{
		if((status == 1) || (status == 3))
		{
			if(count%200 == 0)
			{
				Printf("used buffer num %d, free num %d, total num %d\r\n", pPacketQueue->usedBufNum, pPacketQueue->freeBufNum, pPacketQueue->packetTotalNum);
			}
			count++;
			pTmpBuf = pPacketQueue->usedHead.next;
			if(pTmpBuf == NULL)
			{
				//				Printf("used buffer queue is NULL\r\n");
				return -2;
			}

			/* 将buffer从已经使用的链表中移出 */
			pPacketQueue->usedHead.next = pTmpBuf->next;
			if(pPacketQueue->usedHead.next != NULL)
			{
				pPacketQueue->usedHead.next->pre = NULL;
			}
			pPacketQueue->usedBufNum--;
			if(pPacketQueue->usedBufNum == 0)
			{
				pPacketQueue->pUsedLast = NULL;
			}
			//			Printf("status %d, seqType %d, seq %d\r\n", status, pTmpBuf->packetHead.seqType, pTmpBuf->packetHead.seq);
		}

		switch(status)
		{
			case 1://第一包
#if 0
				if((pTmpBuf->packetHead.seqType == 1) || (pTmpBuf->packetHead.seqType == 0))
				{//一帧的开始
					if(pTmpBuf->packetHead.seqType == 0)
					{//是第一包也是最后一包
						status = 0;
					}
					else
					{
						status = 3;
					}
					if((pTmpBuf->packetHead.len > 0) && (pTmpBuf->packetHead.len <= (streamBufLen - offset)))
					{
						memcpy(pStreamBuf, pTmpBuf->streamBuf, pTmpBuf->packetHead.len);
						offset += pTmpBuf->packetHead.len;
					}
					else
					{
						Printf("1 error, len = %d, total len = %d, offset = %d\r\n", pTmpBuf->packetHead.len, streamBufLen, offset);
						status = 5;
					}
				}
#endif
				packetNo = pTmpBuf->packetHead.seq;
				streamFreeUsedBuffer(pPacketQueue, pTmpBuf);
				break;
			case 2://结束
				streamLen = offset;
				break;
			case 3://其余包
				if(packetNo + 1 != pTmpBuf->packetHead.seq)
				{//包不连续，则获取该帧错误
					Printf("seq error, %d.%d\r\n", packetNo, pTmpBuf->packetHead.seq);
					status = 5;
				}
				else
				{
#if 0
					if((pTmpBuf->packetHead.len > 0) && (pTmpBuf->packetHead.len <= (streamBufLen - offset)))
					{
						memcpy(pStreamBuf + offset, pTmpBuf->streamBuf, pTmpBuf->packetHead.len);
						offset += pTmpBuf->packetHead.len;
						if(pTmpBuf->packetHead.seqType == 3)
						{//中间包
							status = 3;
						}
						else if(pTmpBuf->packetHead.seqType == 2)
						{//最后一包
							status = 2;
						}
					}
					else
					{
						Printf("2 error, len = %d, total len = %d, offset = %d\r\n", pTmpBuf->packetHead.len, streamBufLen, offset);
						status = 5;
					}
#endif
				}
				packetNo = pTmpBuf->packetHead.seq;
				streamFreeUsedBuffer(pPacketQueue, pTmpBuf);
				break;
			case 0:
				streamLen = offset;
				break;
			case 5://
				streamLen = -1;
				break;
			default:
				streamLen = -1;
				break;
		}

		if(streamLen == -1)
		{
			Printf("get stream buf error\r\n");
			break;
		}
		else if(streamLen > 0)
		{//获取数据成功
			break;
		}
		else
		{//正在获取数据
		}
	}


	return streamLen;
#endif
}

/* 获取包队列当前包的个数 */
int streamPacketBufferNum(stream_packet_queue_t *pPacketQueue)
{
#if TEMP
	int bufNum = 0;

	bufNum = pPacketQueue->usedBufNum;

	return bufNum;
#endif
}

/* 将包缓冲队列中的数据刷到帧缓冲中 */
void streamPushPacketBufferToFrameBuffer(stream_packet_queue_t *pPacketQueue)
{
#if TEMP
	if(streamPacketBufferNum(pPacketQueue) <= 0)
	{
		return;
	}
#if 0
	int streamLen = 0;
	char streamBuf[MAX_STREAM_FRAME_SIZE] = {0};

	//	Printf("start push left packet buf to frame buf\r\n");
	while(1)
	{
		streamLen = streamGetFrameBuf(pPacketQueue, streamBuf, sizeof(streamBuf));
		if(streamLen == -2)
		{//包队列已经被清空
			break;
		}
		else if(streamLen == -1)
		{//获取一帧失败
		}
		else if(streamLen > 0)
		{//取到完整的一帧，则插入帧缓冲
			//			streamFrameSendFrameToNetPool(pPacketQueue, streamBuf, streamLen);
		}
	}

	//	Printf("end push left packet buf to frame buf\r\n");
#else
	stream_packet_t * pUsedPacket = NULL;

	while(1)
	{//清空已使用的包队列
		pUsedPacket = streamGetPacketUsedBuffer(pPacketQueue);
		if(pUsedPacket == NULL)
		{
			break;
		}
		streamFreeUsedBuffer(pPacketQueue, pUsedPacket);
	}
#endif

   #endif
}

