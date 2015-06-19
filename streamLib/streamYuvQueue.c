
#include "streamYuvQueue.h"
#include "streamLib.h"
#include "commonLib.h"

/* Yuv */
int streamInitYuvQueue(stream_yuv_queue_t *pYuvQueue)
{
	int i = 0;
	stream_yuv_t *pYuv = NULL;

	memset(pYuvQueue, 0, sizeof(stream_yuv_queue_t));
	pYuvQueue->packetTotalNum = MAX_YUV_QUEUE_NUM;

	pYuv = malloc(sizeof(stream_yuv_t) * pYuvQueue->packetTotalNum);
	if(pYuv == NULL)
	{
		return -1;
	}
	memset(pYuv, 0, sizeof(stream_yuv_t) * pYuvQueue->packetTotalNum);

	pYuvQueue->freeHead.next = (stream_yuv_t *)&pYuv[0];
	for(i = 0; i < pYuvQueue->packetTotalNum - 1; i++)
	{//yuvbuf
		pYuv[i].next = (stream_yuv_t *)&pYuv[i+1];
	}
	pYuvQueue->pFreeLast = (stream_yuv_t *)&pYuv[i];
	pYuvQueue->freeBufNum = pYuvQueue->packetTotalNum;

	pYuvQueue->pYuv = pYuv;

	for(i = 0; i < pYuvQueue->packetTotalNum; i++)
	{//yuv no 
		pYuv[i].yuvNo = 0;//i;
	}

	pthread_mutex_init(&pYuvQueue->yuvMutex, NULL);

	return 0;
}

/* Yuv */
void streamUninitYuvQueue(stream_yuv_queue_t *pYuvQueue)
{
	streamFreeYuvBuffer(pYuvQueue);

	pthread_mutex_lock(&pYuvQueue->yuvMutex);

	if(pYuvQueue->pYuv != NULL)
	{
		free(pYuvQueue->pYuv);
		pYuvQueue->pYuv = NULL;
	}

	pthread_mutex_unlock(&pYuvQueue->yuvMutex);
	pthread_mutex_destroy(&pYuvQueue->yuvMutex);
}

#if 0
/* yuvbuffer */
void streamInitYuvDataBuffer()
{
	stream_yuv_t *pYuv = NULL;
	stream_yuv_t *pTmpYuv = NULL;

	pthread_mutex_lock(&pYuvQueue->yuvMutex);
	/* Yuv */
	pYuv = pYuvQueue->usedHead.next;
	while(pYuv)
	{
		pTmpYuv = pYuv->next;
		free(pYuv);
		pYuv = NULL;
		pYuv = pTmpYuv;
	}

	/* Yuv */
	pYuv = pYuvQueue->freeHead.next;
	while(pYuv)
	{
		pTmpYuv = pYuv->next;
		free(pYuv);
		pYuv = NULL;
		pYuv = pTmpYuv;
	}
	pthread_mutex_unlock(&pYuvQueue->yuvMutex);
}

int streamUninitYuvDataBuffer()
{
	stream_yuv_t *pYuv = NULL;
	stream_yuv_t *pTmpYuv = NULL;

	pthread_mutex_lock(&pYuvQueue->yuvMutex);
	/* Yuv */
	pYuv = pYuvQueue->usedHead.next;
	while(pYuv)
	{
		pTmpYuv = pYuv->next;
		free(pYuv);
		pYuv = NULL;
		pYuv = pTmpYuv;
	}

	/* Yuv */
	pYuv = pYuvQueue->freeHead.next;
	while(pYuv)
	{
		pTmpYuv = pYuv->next;
		free(pYuv);
		pYuv = NULL;
		pYuv = pTmpYuv;
	}
	pthread_mutex_unlock(&pYuvQueue->yuvMutex);
}
#endif

/*********************************************************
 * yuv buffer
 *********************************************************/
stream_yuv_t * streamGetYuvWriteBuffer(stream_yuv_queue_t *pYuvQueue)
{
	stream_yuv_t *pTmpBuf = NULL;

	pthread_mutex_lock(&pYuvQueue->yuvMutex);

	do
	{
		if(pYuvQueue->freeHead.next == NULL)
		{//buffer
#if 1
//			Printf("have not free yuv buffer, used num %d, free num %d\r\n", pYuvQueue->usedBufNum, pYuvQueue->freeBufNum);
			/* used bufferused */
			pTmpBuf = pYuvQueue->usedHead.next;
			if(pTmpBuf != NULL)
			{
				pYuvQueue->usedHead.next = pTmpBuf->next;
				pYuvQueue->usedBufNum--;
				if(pYuvQueue->usedHead.next == NULL)
				{
					pYuvQueue->pUsedLast = NULL;
					if(pYuvQueue->usedBufNum != 0)
					{
						Printf("3 used buf num error\r\n");
						//Exit();
					}
				}
				else
				{
					pYuvQueue->usedHead.next->pre = NULL;
					if(pYuvQueue->usedBufNum == 0)
					{
						Printf("4 used buf num error\r\n");
						//Exit();
					}
				}
			}
#endif
		}
		else
		{
			/* buffer */
			pTmpBuf = pYuvQueue->freeHead.next;
			pYuvQueue->freeHead.next = pYuvQueue->freeHead.next->next;
			pYuvQueue->freeBufNum--;
			if(pYuvQueue->freeHead.next == NULL)
			{
				pYuvQueue->pFreeLast = NULL;
			}
			else
			{
				pYuvQueue->freeHead.next->pre = NULL;
				if(pYuvQueue->freeBufNum == 0)
				{
					Printf("free buf num error\r\n");
					//Exit();
				}
			}
			pTmpBuf->pre = NULL;
			pTmpBuf->next = NULL;
		}
	}while(0);
//	Printf("used num %d, free num %d\r\n", pYuvQueue->usedBufNum, pYuvQueue->freeBufNum);

	pthread_mutex_unlock(&pYuvQueue->yuvMutex);

	return pTmpBuf;
}

/*  yuv buffer */
int streamInsertYuvQueue(stream_yuv_queue_t *pYuvQueue, stream_yuv_t *pYuv)
{
	int ret = 0;
	stream_yuv_t *pTmpBuf = NULL;

	pthread_mutex_lock(&pYuvQueue->yuvMutex);

	pTmpBuf = pYuvQueue->pUsedLast;
	if(pTmpBuf == NULL)
	{//
		pYuvQueue->usedHead.next = pYuv;
		pYuvQueue->pUsedLast = pYuv;
		pYuv->next = NULL;
		pYuv->pre = NULL;
	}
	else
	{//
		pTmpBuf->next = pYuv;
		pYuvQueue->pUsedLast = pYuv;
		pYuv->next = NULL;
		pYuv->pre = pTmpBuf;
	}

	pYuvQueue->usedBufNum++;

	pthread_mutex_unlock(&pYuvQueue->yuvMutex);

	return ret;
}

/*  used yuv buffer */
stream_yuv_t * streamGetYuvUsedBuffer(stream_yuv_queue_t *pYuvQueue)
{
	stream_yuv_t *pTmpBuf = NULL;

	pthread_mutex_lock(&pYuvQueue->yuvMutex);
	
	do
	{
		pTmpBuf = pYuvQueue->usedHead.next;
		if(pTmpBuf == NULL)
		{//
			
//			Printf("can not find yuv used buffer, used num %d, free num %d\r\n", pYuvQueue->usedBufNum, pYuvQueue->freeBufNum);
			break;
		}


		/* used bufferused */
		pYuvQueue->usedHead.next = pTmpBuf->next;
		pYuvQueue->usedBufNum--;
		if(pYuvQueue->usedHead.next == NULL)
		{
			pYuvQueue->pUsedLast = NULL;
			if(pYuvQueue->usedBufNum != 0)
			{
				Printf("1 used buf num error\r\n");
				//Exit();
			}
		}
		else
		{
			pYuvQueue->usedHead.next->pre = NULL;
			if(pYuvQueue->usedBufNum == 0)
			{
				Printf("2 used buf num error\r\n");
				//Exit();
			}
		}
	}while(0);

	pthread_mutex_unlock(&pYuvQueue->yuvMutex);

	return pTmpBuf;
}

/* buffer */
void streamFreeYuvUsedBuffer(stream_yuv_queue_t *pYuvQueue, stream_yuv_t *pYuv)
{
	stream_yuv_t *pTmpBuf = NULL;

	pthread_mutex_lock(&pYuvQueue->yuvMutex);

	/* buffer */
	pTmpBuf = pYuvQueue->pFreeLast;
	if(pTmpBuf == NULL)
	{
		pYuvQueue->freeHead.next = pYuv;
		pYuvQueue->pFreeLast = pYuv;
		pYuv->next = NULL;
		pYuv->pre = NULL;
	}
	else
	{
		pTmpBuf->next = pYuv;
		pYuvQueue->pFreeLast = pYuv;
		pYuv->next = NULL;
		pYuv->pre = pTmpBuf;
	}
	pYuvQueue->freeBufNum++;

	pthread_mutex_unlock(&pYuvQueue->yuvMutex);
}

/* Yuv */
void streamFreeYuvBuffer(stream_yuv_queue_t *pYuvQueue)
{
	stream_yuv_t *pYuv = NULL;

	pthread_mutex_lock(&pYuvQueue->yuvMutex);

	/* Yuv */
	pYuv = pYuvQueue->usedHead.next;
	while(pYuv != NULL)
	{
		if(pYuv->pYuvBuf != NULL)
		{
			//TI_MEM_FREE(pYuv->pYuvBuf);
			free(pYuv->pYuvBuf);
			pYuv->pYuvBuf = NULL;
		}
		pYuv = pYuv->next;
	}

	/* Yuv */
	pYuv = pYuvQueue->freeHead.next;
	while(pYuv != NULL)
	{
		if(pYuv->pYuvBuf != NULL)
		{
			//TI_MEM_FREE(pYuv->pYuvBuf);
			free(pYuv->pYuvBuf);
			pYuv->pYuvBuf = NULL;
		}
		pYuv = pYuv->next;
	}

	pthread_mutex_unlock(&pYuvQueue->yuvMutex);
}

