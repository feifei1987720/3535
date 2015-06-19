/*
 *******************************************************************************
 **  Copyright (c) 2013, All rights reserved.
 **    QQ125174730
 **  : 
 *******************************************************************************
 */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "commonLib.h"
#include "param.h"
#include "procMsgLib.h"

static int g_msgId = 0;
static int g_semId = 0;

/*
 * 消息数据
 * msgType:     消息类型
 * msgSubType:  消息子类型
 * pSendData:   发送数据缓冲
 * sendLen:     发送数据长度
 * pRecvData:   接收数据缓冲
 * recvDataLen: 接收数据缓冲长度
 *
 * 返回值：     >=：成功(接收的数据长度)   其余：失败
 * */
static int procOperateMsgData(int msgType, int msgSubType, void *pSendData, int sendLen, char *pRecvData, int recvDataLen)
{
	proc_msg_t msgbuf;
	int ret = 0;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = PROC_MSG_MAIN_KEY;
	msgbuf.src = PROC_MSG_QT_KEY;
	msgbuf.type = msgType;
	msgbuf.subType = msgSubType;
	msgbuf.offset = 0;

	switch(msgType)
	{
		case PROC_MSG_TYPE_PARAM_GET:
		case PROC_MSG_TYPE_PARAM_SET:
		case PROC_MSG_TYPE_PARAM_INSERT:
		case PROC_MSG_TYPE_PARAM_DEL:
		case PROC_MSG_TYPE_PARAM_MODIFY:
		{
			msgbuf.result = 0;
			msgbuf.length = sendLen;

			printf("1ccccccccccccccc sendLen = %d\r\n", sendLen);
			if(msgbuf.length > 0)
			{
				Sem_lock(g_semId);
				ShareMemWrite(msgbuf.offset, pSendData, msgbuf.length);
				Sem_unlock(g_semId);
			}
			msgsnd(g_msgId, &msgbuf, sizeof(msgbuf) - sizeof(long), 0);
			msgrcv(g_msgId, &msgbuf, sizeof(msgbuf) - sizeof(long), PROC_MSG_QT_KEY, 0);

			printf("recv len = %d, offset = %d\r\n", msgbuf.length, msgbuf.offset);
			if((msgbuf.length > 0) && (recvDataLen >= msgbuf.length) && (pRecvData != NULL))
			{
				Sem_lock(g_semId);
				ShareMemRead(msgbuf.offset, pRecvData, msgbuf.length);
				Sem_unlock(g_semId);
			}
//			printf("recv buf = (%s)\r\n", pRecvData);
			ret = msgbuf.length;

			break;
		}
		case PROC_MSG_TYPE_CONTROL:
		{
			msgbuf.result = 0;
			msgbuf.length = sendLen;

			printf("1bbbbbbbbbbbbb sendLen = %d\r\n", sendLen);
			if(msgbuf.length > 0)
			{
				Sem_lock(g_semId);
				ShareMemWrite(msgbuf.offset, pSendData, msgbuf.length);
				Sem_unlock(g_semId);
			}
			msgsnd(g_msgId, &msgbuf, sizeof(msgbuf) - sizeof(long), 0);
			msgrcv(g_msgId, &msgbuf, sizeof(msgbuf) - sizeof(long), PROC_MSG_QT_KEY, 0);

			printf("recv len = %d, offset = %d\r\n", msgbuf.length, msgbuf.offset);
			if((msgbuf.length > 0) && (recvDataLen >= msgbuf.length) && (pRecvData != NULL))
			{
				Sem_lock(g_semId);
				ShareMemRead(msgbuf.offset, pRecvData, msgbuf.length);
				Sem_unlock(g_semId);
			}
//			printf("recv buf = (%s)\r\n", pRecvData);
			ret = msgbuf.length;

			switch(msgSubType)
			{
				case PROC_MSG_SUB_TYPE_CONTROL_SEARCH_RECORD:
				{
					break;
				}
				case PROC_MSG_SUB_TYPE_CONTROL_PLAY_RECORD_DISPLAY_NUM:
				{
					break;
				}
				case PROC_MSG_SUB_TYPE_CONTROL_VIEW_DISPLAY_NUM:
				{
					break;
				}
				case PROC_MSG_SUB_TYPE_CONTROL_SET_PLAY_RECORD:
				{
					break;
				}
				case PROC_MSG_SUB_TYPE_CONTROL_PLAY_RECORD_TIME:
				{
					break;
				}
				case PROC_MSG_SUB_TYPE_CONTROL_PLAY_RECORD_SPEED:
				{
					break;
				}
				case PROC_MSG_SUB_TYPE_CONTROL_PLAY_RECORD_STATUS:
				{
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		default:
		{
			printf("not support msg type %d\r\n", msgType);
			break;
		}
	}

	return ret;
}

/* 设置回放分屏数目 */
int procSetPlaybackNum(int num)
{
	control_play_record_num_t playRecordNum;

	playRecordNum.num = num;

	return procOperateMsgData(PROC_MSG_TYPE_CONTROL, PROC_MSG_SUB_TYPE_CONTROL_PLAY_RECORD_DISPLAY_NUM, &playRecordNum, sizeof(control_play_record_num_t), NULL, 0);
}

/* 回放录像 */
int procPlayRecordFile(int displayChannel, int speed, record_search_file_t *pSearchFile, int fileNum, int startTime, int offset)
{
	control_play_record_t playRecord;

	playRecord.status = 0;
	playRecord.displayChannel = displayChannel;
	playRecord.speed = speed;
	playRecord.fileNum = fileNum;
	memcpy(playRecord.searchFile, pSearchFile, fileNum * sizeof(record_search_file_t));
	playRecord.startTime = startTime;
//	playRecord.startFileNum = startFileNum;
//	playRecord.offset = offset;

	return procOperateMsgData(PROC_MSG_TYPE_CONTROL, PROC_MSG_SUB_TYPE_CONTROL_SET_PLAY_RECORD, &playRecord, sizeof(control_play_record_t), NULL, 0);
}

/* 停止一个回放 */
int procStopPlayback(int displayChannel)
{
	control_play_status_t playRecord;

	playRecord.status = 1;
	playRecord.displayChannel = displayChannel;

	return procOperateMsgData(PROC_MSG_TYPE_CONTROL, PROC_MSG_SUB_TYPE_CONTROL_PLAY_RECORD_STATUS, &playRecord, sizeof(control_play_status_t), NULL, 0);
}

/* 停止所有回放 */
int procStopAllPlayback()
{
	control_play_status_t playRecord;

	playRecord.status = 1;
	playRecord.displayChannel = 0xFF;

	return procOperateMsgData(PROC_MSG_TYPE_CONTROL, PROC_MSG_SUB_TYPE_CONTROL_PLAY_RECORD_STATUS, &playRecord, sizeof(control_play_status_t), NULL, 0);
}

/* 回放通道的某一位置的视频 */
int procPlayRecordTime(int displayChannel, int startTime)
{
	control_play_time_t playTime;

	playTime.displayChannel = displayChannel;
	playTime.startTime = startTime;

	return procOperateMsgData(PROC_MSG_TYPE_CONTROL, PROC_MSG_SUB_TYPE_CONTROL_PLAY_RECORD_TIME, &playTime, sizeof(control_play_time_t), NULL, 0);
}

/* 获取当前回放的录像的位置 */
int procGetCurPlaybackPos(control_play_cur_pos_t *pPlayItem, int size)
{
	return procOperateMsgData(PROC_MSG_TYPE_CONTROL, PROC_MSG_SUB_TYPE_CONTROL_PLAY_RECORD_CUR_POS, NULL, 0, (char *)pPlayItem, size);
}

/* 设置预览分屏数目 */
int procSetDisplayNum(int num, int pos)
{
	control_view_display_num_t displayNum;

	displayNum.num = num;
	displayNum.viewPos = pos;

	return procOperateMsgData(PROC_MSG_TYPE_CONTROL, PROC_MSG_SUB_TYPE_CONTROL_VIEW_DISPLAY_NUM, &displayNum, sizeof(control_view_display_num_t), NULL, 0);
}

/* 查询录像文件 */
int procSearchRecordFile(int channelNo, int streamNo, int startTime, int endTime, char *pRecvBuf, int len)
{
	/* 查询录像文件 */
	control_search_record_t searchRecord;

	memset(&searchRecord, 0, sizeof(control_search_record_t));
	searchRecord.channelNo = channelNo;
	searchRecord.streamNo = streamNo;
	searchRecord.startTime = startTime;
	searchRecord.endTime = endTime;
	return procOperateMsgData(PROC_MSG_TYPE_CONTROL, PROC_MSG_SUB_TYPE_CONTROL_SEARCH_RECORD, &searchRecord, sizeof(control_search_record_t), pRecvBuf, len);
}


/* 获取所有的设备通道信息 */
int procGetAllDevChannelInfo(char *pRecvBuf, int len)
{
	return procOperateMsgData(PROC_MSG_TYPE_PARAM_GET, PROC_MSG_SUB_TYPE_PARAM_DEV_CHANNEL_INFO, NULL, 0, pRecvBuf, len);
}

/* 查询参数 */
int procGetParam(int paramType, char *pRecvBuf, int bufLen)
{
	return procOperateMsgData(PROC_MSG_TYPE_PARAM_GET, paramType, NULL, 0, pRecvBuf, bufLen);
}

int procGetParamExt(int paramType, char *data, int dataLen, char *pRecvBuf, int bufLen)
{
	return procOperateMsgData(PROC_MSG_TYPE_PARAM_GET, paramType, data, dataLen, pRecvBuf, bufLen);
}

/* 设置参数 */
int procSetParam(int paramType, char *pSendBuf, int bufLen)
{
	return procOperateMsgData(PROC_MSG_TYPE_PARAM_SET, paramType, pSendBuf, bufLen, NULL, 0);
}

/* 添加数据 */
int procAddData(int paramType, char *pSendBuf, int bufLen)
{
	return procOperateMsgData(PROC_MSG_TYPE_PARAM_INSERT, paramType, pSendBuf, bufLen, NULL, 0);
}

/* 删除数据 */
int procDelData(int paramType, char *pSendBuf, int bufLen)
{
	return procOperateMsgData(PROC_MSG_TYPE_PARAM_DEL, paramType, pSendBuf, bufLen, NULL, 0);
}

/* QT与主进程通讯初始化 */
void procMsgQtInit()
{
	procMsgInit(&g_msgId, &g_semId);
	printf("qt g_semId = %d, g_msgId = %d\r\n", g_semId, g_msgId);

	procMsgClean(g_msgId, PROC_MSG_QT_KEY);
}

/* QT与主进程通讯销毁 */
void procMsgQtDestroy()
{
	procMsgDestroy(g_semId);
}

#if 0
int main()
{
	int i = 0;
	char recvBuf[512 * 1024] = {0};
	char sendBuf[512 * 1024] = {0};
	int len = 0;
	record_search_file_t *pSearchFile = NULL;
	param_dev_channel_info_t *pDevChannelInfo = NULL;

	procMsgInit(&g_semId, &g_msgId);

	procMsgClean(g_msgId, PROC_MSG_QT_KEY);

	while(1)
	{
		pDevChannelInfo = (param_dev_channel_info_t *)recvBuf;
		len = getAllDevChannelInfo(recvBuf, sizeof(recvBuf));
		printf("ccccccccccccc len = %d\r\n", len);
		if(len > 0)
		{
			for(i = 0; i < len/sizeof(param_dev_channel_info_t); i++)
			{
				printf("channelNo %d\r\n", pDevChannelInfo[i].channelNo);
				printf("streamNo %d\r\n", pDevChannelInfo[i].streamNo);
				printf("addr %s\r\n", pDevChannelInfo[i].addr);
				printf("port %d\r\n", pDevChannelInfo[i].port);
				printf("userName %s\r\n", pDevChannelInfo[i].userName);
				printf("userPwd %s\r\n", pDevChannelInfo[i].userPwd);
			}
		}
#if 0
		len = searchRecordFile(0, 0, 0, time(0), recvBuf, sizeof(recvBuf));
		if(len > 0)
		{
			printf("len === %d, ll %d\r\n", len, sizeof(record_search_file_t));
			pSearchFile = (record_search_file_t *)recvBuf;
			for(i = 0; i < len/sizeof(record_search_file_t); i++)
			{
				Printf("item %d, diskNo %X, fileType %d, recordNo 0x%04X, startTime %d, endTime %d, startAddr %d, endAddr %d\r\n",
						i, pSearchFile[i].diskNo, pSearchFile[i].fileType, pSearchFile[i].recordNo, pSearchFile[i].startTime, pSearchFile[i].endTime, pSearchFile[i].startAddr, pSearchFile[i].endAddr);
			}
			setPlaybackNum(1);

			playRecordFile(0, 1, pSearchFile, len/sizeof(record_search_file_t), 0, 0);
		}
#endif
		sleep(10);
	}

	procMsgExit();
}
#endif


