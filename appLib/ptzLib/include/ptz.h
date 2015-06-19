/********************************************************
  File name:
  Author:
  Date:
  Description:
********************************************************/
#ifndef __PTZ_H__
#define __PTZ_H__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#define MAX_PTZ_NUM 32

typedef enum _PtzCmd_t_
{
	PTZ_MSG_UP_START = 0x001
	, PTZ_MSG_UP_STOP
	, PTZ_MSG_DOWN_START
	, PTZ_MSG_DOWN_STOP
	, PTZ_MSG_LEFT_START
	, PTZ_MSG_LEFT_STOP
	, PTZ_MSG_RIGHT_START    
	, PTZ_MSG_RIGHT_STOP
	, PTZ_MSG_LEFT_UP_START
	, PTZ_MSG_LEFT_UP_STOP = 10
	, PTZ_MSG_RIGHT_UP_START
	, PTZ_MSG_RIGHT_UP_STOP
	, PTZ_MSG_LEFT_DOWN_START
	, PTZ_MSG_LEFT_DOWN_STOP
	, PTZ_MSG_RIGHT_DOWN_START
	, PTZ_MSG_RIGHT_DOWN_STOP
	, PTZ_MSG_PRESET_SET
	, PTZ_MSG_PRESET_CALL
	, PTZ_MSG_PRESET_DEL
	, PTZ_MSG_ZOOM_ADD_START = 20
	, PTZ_MSG_ZOOM_ADD_STOP
	, PTZ_MSG_ZOOM_SUB_START
	, PTZ_MSG_ZOOM_SUB_STOP
	, PTZ_MSG_FOCUS_ADD_START
	, PTZ_MSG_FOCUS_ADD_STOP
	, PTZ_MSG_FOCUS_SUB_START
	, PTZ_MSG_FOCUS_SUB_STOP
	, PTZ_MSG_IRIS_ADD_START
	, PTZ_MSG_IRIS_ADD_STOP
	, PTZ_MSG_IRIS_SUB_START
	, PTZ_MSG_IRIS_SUB_STOP = 31
	, PTZ_MSG_GOTO_ZERO_PAN
	, PTZ_MSG_FLIP_180
	, PTZ_MSG_SET_PATTERN_START
	, PTZ_MSG_SET_PATTERN_STOP
	, PTZ_MSG_RUN_PATTERN
	, PTZ_MSG_SET_AUXILIARY
	, PTZ_MSG_CLEAR_AUXILIARY
	, PTZ_MSG_AUTO_SCAN_START
	, PTZ_MSG_AUTO_SCAN_STOP
	, PTZ_MSG_RANDOM_SCAN_START
	, PTZ_MSG_RANDOM_SCAN_STOP
	, PTZ_MSG_LEFT_OFFSET
	, PTZ_MSG_RIGHT_OFFSET
	, PTZ_MSG_DOWN_OFFSET
	, PTZ_MSG_UP_OFFSET
	, PTZ_MSG_ZOOM_MULTIPLE
	, PTZ_MSG_POINT_CENTER
	, PTZ_MSG_VIEW_CENTER
	, PTZ_MSG_BUTT
}PtzCmd_t;


typedef enum _PtzProtocol_t_{
	PP_PELCO_D,
	PP_PELCO_P	
}PtzProtocol_t;

typedef struct _PtzSerialConfig_t_
{
	int nBaudRate;
	char nProtocol;
	char nAddr;
	char nDataBit;
	char nStopBit;
	char nCheckType;
	char resv[3];
}PtzSerialConfig_t;

/***************************************************************************

在系统初始化的时候需要调用ptzInit打开串口，获取参数给每个通道的串口属性赋值setPtzSerial
*****************************************************************************/
int ptzInit(char *pSerialDevFile);
int setPtzSerial(int s32Chn, PtzSerialConfig_t *para);
int ptzCmdDeal(int s32Chn, PtzCmd_t cmd, int s32Speed, void *other);
int PtzDelInit(void *other);


#endif/*__PTZ_H__*/


