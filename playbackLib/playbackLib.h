/*
*******************************************************************************
** 
**  : 
*******************************************************************************
*/

#ifndef __PLAYBACK_LIB_H__
#define __PLAYBACK_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "param.h"
extern int PlayBackChn[4];


typedef enum PLAYBACK_STAUTS_E
{
	PLAYBACK_STATUS_QUIT = 0,
	PLAYBACK_STATUS_OPEN,
	PLAYBACK_STATUS_CHANGE_POS,
	PLAYBACK_STATUS_HAND_CHANGE_ITEM,
	PLAYBACK_STATUS_AUTO_CHANGE_ITEM,
	PLAYBACK_STATUS_DECODE,
	PLAYBACK_STATUS_FAST_FORWARD,
	PLAYBACK_STATUS_CLOSE,
	PLAYBACK_STATUS_WAIT,
	PLAYBACK_STATUS_DOWNLOAD,
	PLAYBACK_STATUS_DOWNLOAD_END,
}PLAYBACK_STAUTS_E;

typedef enum BUCKUP_RECORD_E
{
	BACKUP_RECORD_SUCCSS =100,
	BACKUP_RECORD_WRITE_USB_FAIL =200,
	BACKUP_RECORD_READ_FILE_FAIL =300,
	BACKUP_RECORD_UNKOWN_ERROR =400,
}BACKUP_RECORD_E;
	

typedef struct playback_rtsp_t
{//rtsp
	int width;
	int height;
	int frameRate;
	int startTime;
	int endTime;
	unsigned char sps[256];
	int spsLen;
	unsigned char pps[256];
	int ppsLen;
}playback_rtsp_t;


/*  */
int playbackSetNum(int num,int ch);

/*  */
int playbackGetCurPos(control_play_cur_pos_t *pCurPos);

/*
 * 
 * pSearchFile	
 * itemNum		
 * startItem		
 * offset		  
 * displayChannel	 
 * */
//int playbackStart(record_search_file_t *pSearchFile, int itemNum, int startItem, int offset, int displayChannel);
int playbackStart(record_search_file_t *pSearchFile, int itemNum, int startTime, int displayChannel);

/*  */
//int playbackControl(int speed, int startItem, int offset, int displayChannel);
int playbackControl(int speed, int startTime, int displayChannel);

/*  */
int playbackGetPlayTime();

/*  */
int playbackStop(int displayChannel);

/*  */
int playbackCmd(int cmd, int displayChannel, int speed);

/*  */
int playbackStopAll();

/**/
int GetPlatStatus();

/**/
int getPlaybackNum();

/*  */
void playbackLibInit();

/*  */
void playbackLibDestroy();



/* backupRecord.c */

/*  */
int playbackStartBackupRecord(sdk_record_backup_t *pBackup);

/*  */
int playbackGetProcess();



/* playbackRemote.c */
void playbackRemoteUpdateParam();


/* playbackRtsp.c */
int playbackRtspGetFileTimeRange(int diskNo, int recordNo, int startAddr, int endAddr, playback_rtsp_t *pRtsp);


#ifdef __cplusplus
}
#endif


#endif//__PLAYBACK_LIB_H__

