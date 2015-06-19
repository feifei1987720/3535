#ifndef __PROC_MSG_LIB_H__
#define __PROC_MSG_LIB_H__


#include "param.h"


#ifdef __cplusplus
extern "C" {
#endif

/* 进程间消息通信初始化 */
int procMsgInit(int *pMsgId, int *pSemId);

/* 清空消息队列 */
void procMsgClean(int msgId, int msgType);

/* 进程间消息通信销毁 */
int procMsgDestroy(int semId);






/********************************** 进程间通信 客户端调用 **********************************/

/* 查询参数 */
int procGetParam(int paramType, char *pRecvBuf, int bufLen);
int procGetParamExt(int paramType, char *data, int dataLen, char *pRecvBuf, int bufLen);
/* 设置参数 */
int procSetParam(int paramType, char *pSendBuf, int bufLen);

/* 添加数据 */
int procAddData(int paramType, char *pSendBuf, int bufLen);
/* 删除数据 */
int procDelData(int paramType, char *pSendBuf, int bufLen);



/* 查询录像文件 */
int procSearchRecordFile(int channelNo, int streamNo, int startTime, int endTime, char *pRecvBuf, int len);
/* 设置回放分屏数目 */
int procSetPlaybackNum(int num);
/* 回放录像 */
int procPlayRecordFile(int displayChannel, int speed, record_search_file_t *pSearchFile, int fileNum, int startTime, int offset);
/* 停止一个回放 */
int procStopPlayback(int displayChannel);
/* 停止所有回放 */
int procStopAllPlayback();
/* 回放通道的某一位置的视频 */
int procPlayRecordTime(int displayChannel, int startTime);
/* 获取当前回放的录像的位置 */
int procGetCurPlaybackPos(control_play_cur_pos_t *pPlayItem, int size);



/* 获取所有的设备通道信息 */
int procGetAllDevChannelInfo(char *pRecvBuf, int len);

/* 设置预览分屏数目 */
int procSetDisplayNum(int num, int pos);

/* QT与主进程通讯初始化 */
void procMsgQtInit();

/* QT与主进程通讯销毁 */
void procMsgQtDestroy();


#ifdef __cplusplus
}
#endif

#endif//__PROC_MSG_LIB_H__

