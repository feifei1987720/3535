#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "procMsgLib.h"
#include "commonLib.h"





/* 进程间消息通信初始化 */
int procMsgInit(int *pMsgId, int *pSemId)
{
	int semId = 0;
	int msgId = 0;

	if(ShareMemInit(PROC_MEM_KEY) < 0)
	{
		Printf("Share Memory init fail\n");
		return -1;
	}
	semId = Sem_init(PROC_SEM_KEY);

	if((msgId = Msg_Init(PROC_MSG_KEY)) < 0)
	{
		Sem_exit(semId);
		Printf("Msg_Init fail\n");

		return -1;
	}

	if(pSemId != NULL)
	{
		*pSemId = semId;
	}
	if(pMsgId != NULL)
	{
		*pMsgId = msgId;
	}

	return 0;
}

/* 清空消息队列 */
void procMsgClean(int msgId, int msgType)
{
	proc_msg_t msgbuf;

	while(msgrcv(msgId, &msgbuf, sizeof(msgbuf) - sizeof(long), msgType, IPC_NOWAIT) >= 0);
}

/* 进程间消息通信销毁 */
int procMsgDestroy(int semId)
{
	Sem_exit(semId);
	return 0;
}

