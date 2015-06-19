/*
*******************************************************************************
**  
**  : epoll,epoll
*******************************************************************************
*/
#ifndef __QUEUE_LIB_H__
#define __QUEUE_LIB_H__


#ifdef __cplusplus
extern "C" {
#endif


/* cycleQueue.c */
/* 定义循环队列类型 */
typedef int queueCycleElemType;
typedef struct
{
	queueCycleElemType  *base;//元素
	int front;//第一个
	int rear;//最后一个
	int maxQueueNum;//最大队列的数目
}CYCLE_QUEUE_T;

/* 1、初始化循环队列 */
void queueCycleInit(CYCLE_QUEUE_T *Q, int maxQueueNum);
/* 2、销毁循环队列 */
void queueCycleDestroy(CYCLE_QUEUE_T *Q);
/* 3、清空循环队列 */
void queueCycleClear(CYCLE_QUEUE_T *Q);
/* 4、判断空队列 */
int queueCycleEmpty(CYCLE_QUEUE_T Q);
/* 5、求循环队列长度 */
int queueCycleLength(CYCLE_QUEUE_T Q);
/* 6、取队头元素 */
void queueCycleGetHead(CYCLE_QUEUE_T Q, queueCycleElemType *e);
/* 7、入队列 */
int queueCycleIn(CYCLE_QUEUE_T *Q, queueCycleElemType e);
/* 8、出队列 */
int queueCycleOut(CYCLE_QUEUE_T *Q, queueCycleElemType *e);
/* 9、遍历循环队列并输出元素 */
void queueCycleTraverse(CYCLE_QUEUE_T Q);





/* linkQueue.c */
/* 定义链式队列类型 */
typedef struct link_queue_data_t
{
	int type;//类型
	char data[1024];
}link_queue_data_t;
typedef link_queue_data_t queueLinkElemType;
typedef struct QNode
{
	queueLinkElemType elemData;
	struct QNode *next;
}QNode, *QueuePtr;
typedef struct
{
	QueuePtr front;
	QueuePtr rear;
}LINK_QUEUE_T;

/* 1、初始化链式队列 */
void queueLinkInit(LINK_QUEUE_T *Q);
/* 2、销毁链式队列 */
void queueLinkDestroy(LINK_QUEUE_T *Q);
/* 3、清空链式队列 */
void queueLinkClear(LINK_QUEUE_T *Q);
/* 4、判断空队列 */
int queueLinkEmpty(LINK_QUEUE_T Q);
/* 5、求链式队列长度 */
int queueLinkLength(LINK_QUEUE_T Q);
/* 6、取队头元素 */
queueLinkElemType *queueLinkGetHead(LINK_QUEUE_T Q);
/* 7、入队列 */
void queueLinkIn(LINK_QUEUE_T *Q, queueLinkElemType e);
/* 8、出队列 */
int queueLinkOut(LINK_QUEUE_T *Q, queueLinkElemType *e);
/* 9、遍历链式队列并输出元素 */
void queueLinkTraverse(LINK_QUEUE_T Q);
/* 约瑟夫问题 */
void queueLinkJoseffer(int n);



#ifdef __cplusplus
}
#endif


#endif//__QUEUE_LIB_H__

