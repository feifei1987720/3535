/*
 *  StreamServiceThread
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

#include "myMalloc.h"
#include "streamLib.h"
#include "commonLib.h"


static stream_alarm_queue_t g_alarmQueue;

int streamInitAlarmQueue(stream_alarm_queue_t *pAlarmQueue)
{
	if(pAlarmQueue == NULL)
	{
		pAlarmQueue = &g_alarmQueue;
	}

	memset(pAlarmQueue, 0x00, sizeof(stream_alarm_queue_t));

	if(pthread_mutex_init(&pAlarmQueue->alarmMutex, NULL) != 0)
	{
		perror("pthread_mutex_init");
		exit(0);
	}
	pthread_cond_init(&pAlarmQueue->alarmCond, NULL);

	return 0;
}

void streamUninitAlarmQueue(stream_alarm_queue_t *pAlarmQueue)
{
	if(pAlarmQueue == NULL)
	{
		pAlarmQueue = &g_alarmQueue;
	}

	pthread_mutex_lock(&pAlarmQueue->alarmMutex);
	pthread_cond_broadcast(&pAlarmQueue->alarmCond);
	pthread_mutex_unlock(&pAlarmQueue->alarmMutex);

	pthread_mutex_destroy(&pAlarmQueue->alarmMutex);	 	
	pthread_cond_destroy(&pAlarmQueue->alarmCond);
}

/*  */
int streamWaitAlarm(stream_alarm_queue_t *pAlarmQueue)
{
	int ret = 0;
	struct timeval curTime;
	struct timespec timeOut;

	if(pAlarmQueue == NULL)
	{
		pAlarmQueue = &g_alarmQueue;
	}

	pthread_mutex_lock(&pAlarmQueue->alarmMutex);

	/*  */
	gettimeofday(&curTime, NULL);
	timeOut.tv_sec = curTime.tv_sec + 1;
	timeOut.tv_nsec = curTime.tv_usec * 1000;
	ret = pthread_cond_timedwait(&pAlarmQueue->alarmCond, &pAlarmQueue->alarmMutex, &timeOut);
	if(ret == ETIMEDOUT)
	{
		ret = -1;
	}

	pthread_mutex_unlock(&pAlarmQueue->alarmMutex);

	return 0;
}

/*  */
void streamInsertAlarmToAlarmQueue(stream_alarm_queue_t *pAlarmQueue, stream_alarm_t *pAlarm)
{
	int writePos = 0;

	if(pAlarmQueue == NULL)
	{
		pAlarmQueue = &g_alarmQueue;
	}

	pthread_mutex_lock(&pAlarmQueue->alarmMutex);

	writePos = pAlarmQueue->writePos;
	memcpy(&pAlarmQueue->alarmInfo[writePos], pAlarm, sizeof(stream_alarm_t));
	pAlarmQueue->writePos = (writePos + 1) % MAX_ALARM_QUEUE_NUM;

	pthread_mutex_unlock(&pAlarmQueue->alarmMutex);
}

/*  */
int streamGetAlarmFromNetPool(stream_alarm_queue_t *pAlarmQueue, int *pReadBegin, int *pReadEnd, stream_alarm_t *pAlarm)
{
	int ret = -1;
	int writePos = 0;

	if(pAlarmQueue == NULL)
	{
		pAlarmQueue = &g_alarmQueue;
	}

	if((*pReadBegin == *pReadEnd) && (*pReadEnd == pAlarmQueue->writePos))
	{
		//Printf("pReadBegin =%d  *pReadEnd=%d pAlarmQueue->writePos=%d\n",*pReadBegin,*pReadEnd,pAlarmQueue->writePos);
		return -1;
	}

	//pthread_mutex_lock(&pAlarmQueue->alarmMutex);	

	if((*pReadBegin == -1) && (*pReadEnd == -1))
	{
		*pReadBegin = writePos;
		*pReadEnd = writePos;
		//pthread_mutex_unlock(&pAlarmQueue->alarmMutex);
		//Printf("pReadBegin =%d  *pReadEnd=%d pAlarmQueue->writePos=%d\n",*pReadBegin,*pReadEnd,pAlarmQueue->writePos);
		return -1;
	}

	writePos = pAlarmQueue->writePos;
	//Printf("cur read %d, end %d,  cur write %d\r\n", *pReadBegin, *pReadEnd, writePos);

	if((*pReadBegin <= *pReadEnd && *pReadEnd <= writePos) || (*pReadEnd <= writePos && writePos <= *pReadBegin) || (writePos <= *pReadBegin && *pReadBegin <= *pReadEnd))
	{
		*pReadEnd = writePos;
		memcpy(pAlarm, &pAlarmQueue->alarmInfo[*pReadBegin], sizeof(stream_alarm_t));
		*pReadBegin = (*pReadBegin + 1) % MAX_ALARM_QUEUE_NUM;
		ret = 0;
	}
	else
	{
		Printf("failed : lost alarm, rb(%d), re(%d), wp(%d)!\r\n", *pReadBegin, *pReadEnd, writePos);
		*pReadEnd = writePos;
		*pReadBegin = (*pReadBegin + 10) % MAX_ALARM_QUEUE_NUM;//writePos;
	}

	//pthread_mutex_unlock(&pAlarmQueue->alarmMutex);

	return ret;
}


