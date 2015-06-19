#ifndef __PLAYBACK_FUNC_H__
#define __PLAYBACK_FUNC_H__


#include "streamLib.h"

typedef struct playback_file_info_t
{
	int iFrameFd;
	int iFrameStartAddr;
	int iFrameEndAddr;
	int iFrameCurAddr;

	int dataFd;
	int dataStartAddr;
	int dataEndAddr;
	int dataCurAddr;

	int startTime;//录像的起始时间
	int endTime;//结束时间
}playback_file_info_t;



/* 打开回放录像文件 */
int playbackRecordOpen(playback_file_info_t *pPlaybackFileInfo, char *pFilePath, int iFrameStartAddr, int iFrameEndAddr, int offset);

/*
 * 读取回放录像文件
 * pPlaybackFileInfo:	回放文件信息
 * speed:		回放速度
 * pFrame:		一帧视频
 * */
int playbackRecordRead(playback_file_info_t *pPlaybackFileInfo, int speed, stream_frame_t *pFrame);

/* 关闭回放录像文件 */
void playbackRecordClose(playback_file_info_t *pPlaybackFileInfo);


#endif//__PLAYBACK_FUNC_H__

