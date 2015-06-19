#ifndef __MY_PTHREAD_H__
#define __MY_PTHREAD_H__

/*  */
int initPthreadScheduled();

/*  */
int setPthreadPriority(int value);

/*  */
int setPthreadPriorityMax();

pthread_attr_t *getPthreadScheduledAttr();

#endif//__MY_PTHREAD_H__

