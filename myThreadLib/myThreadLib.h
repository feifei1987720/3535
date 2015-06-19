/*
*******************************************************************************
**  Copyright (c) 2012, All rights reserved.
**    
**  : epoll,epoll
*******************************************************************************
*/

#ifndef __MY_THREAD_LIB_H__
#define __MY_THREAD_LIB_H__

#include <pthread.h>


#ifdef __cplusplus
extern "C" {
#endif



#define PTHREAD_SCHEDULED_PRIORITY_HIGH_EST	1// 
#define PTHREAD_SCHEDULED_PRIORITY_HIGH		2//   
#define PTHREAD_SCHEDULED_PRIORITY_NORMAL	3// 
#define PTHREAD_SCHEDULED_PRIORITY_LOW		4//   
#define PTHREAD_SCHEDULED_PRIORITY_LOW_EST	5// 



/*********************************************************************
 * thread_worker_t
 * 
 *********************************************************************/
typedef struct __thread_worker_t
{
	void (*process)(char *arg, int argLen);/**/
	char arg[4096];/**/
	int argLen;//
	struct __thread_worker_t *next;
}thread_worker_t;

/**/
typedef struct __thread_pool_t
{
	int shutdown;/**/
	int maxThreadNum;//
	int minThreadNum;//

	int curQueueSize;/**/
	int curThreadNum;//
	int curUsedThreadNum;//

	pthread_mutex_t queueLock;
	pthread_cond_t queueReadyCond;

	thread_worker_t *pQueueHead;/**/
	pthread_t *pThreadId;/* id */
}thread_pool_t;



/************************** threadPool.c **************************/
/*  */
thread_pool_t * threadPoolInit(int maxThreadNum, int minThreadNum);
/**/
int threadPoolAddWorker(thread_pool_t *pThreadPool, void (*process)(char *arg, int argLen), char *arg, int argLen);
/**/  /*1050*/
void threadPollManage(thread_pool_t *pThreadPool);
/**/
int threadPoolDestroy(thread_pool_t *pThreadPool);





/************************** myThreadLib.c **************************/
/*  */
pthread_attr_t *commonGetPthreadScheduledAttr(int priority);
/*  */
void myThreadLibInit();




/************************** myThreadScheduled.c **************************/
/*  */
int setPthreadPriority(int value);
/* cpu */
void setPthreadWorkCpu(int no);

#ifdef __cplusplus
}
#endif


#endif//__MY_THREAD_LIB_H__

