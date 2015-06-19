/*
*******************************************************************************

*******************************************************************************
*/

#ifndef __DECODE_LIB_H__
#define __DECODE_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif



/* 获取解码线程运行标记 */
int decodeGetRunFlag(int viewPos);
/* 设置解码线程运行标记 */
void decodeSetRunFlag(int viewPos, int value);

void decodeEableAudio(int flag);

/* 开启解码 */
int decodeStartDecode(int channelNo, int streamNo, int viewPos, int restartNetwork);
/* 停止解码 */
void decodeStopDecode(int viewPos);
/* 停止所有解码 */
void decodeStopAllDecode();

/* 获取当前窗口的状态，以及正在操作的通道号、流号 */
int decodeGetCurViewChannelNoAndStreamNo(int viewPos, int *pChannelNo, int *pStreamNo);
void decodeSetViewChannelNoAndStreamNo(int viewPos, int channelNo, int streamNo);

/* 暂停解码 */
void decodePauseDecode(int viewPos);
/* 暂停所有解码 */
void decodePauseAllDecode();

/* 恢复解码 */
void decodeRestartDecode(int viewPos);
/* 恢复所有解码 */
void decodeRestartAllDecode();

/* 等待单个解码线程的运行状态 */
void decodeWaitRunFlag(int viewPos, int runFlag);
/* 等待所有解码线程的运行状态 */
void decodeWaitAllRunFlag(int runFlag);



/* 全屏某个窗口 */
int decodeLibFullScreen(int viewPos);
/* 4窗口 */
int decodeLibFourScreen(int viewPos);
/* 6窗口 */
int decodeLibSixScreen(int viewPos);
/* 8窗口 */
int decodeLibEightScreen(int viewPos);
/* 9窗口 */
int decodeLibNineScreen(int viewPos);
/* 16窗口 */
int decodeLibSixteenScreen(int viewPos);

int decodeLib24Screen(int viewPos);
/* 25 */
int decodeLib25Screen(int viewPos);
/* 36窗口 */
int decodeLib36Screen(int viewPos);


/* 获取分屏数 */
int decodeGetDisplayNum();

int decodeLibInit(int resolution, int displayNum);
int decodeLibReset(int resolution, int displayNum);
void decodeLibdestroy(int displayNum);

int AppStartVideo();
int SwitchScreenDisplay(int ViewPos, int DisplayNum);
int SwitchScreenPlayBackDisplay(int chn[4], int DisplayNum);




#ifdef __cplusplus
}
#endif


#endif//__DECODE_LIB_H__

