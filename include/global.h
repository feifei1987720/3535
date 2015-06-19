#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <pthread.h>

//#include "appLib.h"
//#include "codeLib.h"
//#include "commonLib.h"
//#include "myThreadLib.h"
//#include "queueLib.h"


#if defined (__cplusplus)
extern "C" {
#endif


#define CPU_3520A			0
#define CPU_3531			1
#define CPU_3521			2
#define CPU_3520D                  3
#define CPU_3535			4

#ifdef  HI3531
#define MAX_DISPLAY_NUM 		32//最大分屏数目
#define MAX_SUPPORT_ESATA_NUM		2//最大支持的esata数目
#define MAX_SUPPORT_SATA_NUM		8//最大支持的sata数目
#elif HI3521
#define MAX_DISPLAY_NUM 		16//
#define MAX_SUPPORT_ESATA_NUM		2//esata
#define MAX_SUPPORT_SATA_NUM		2//sata
#elif HI3520D
#define MAX_DISPLAY_NUM 		8//
#define MAX_SUPPORT_ESATA_NUM		2//esata
#define MAX_SUPPORT_SATA_NUM		1//sata
#elif HI3535
#define MAX_DISPLAY_NUM 		25//
#define MAX_SUPPORT_ESATA_NUM		2//esata
#define MAX_SUPPORT_SATA_NUM		8//sata
#endif

#define MAX_SUPPORT_DISK_NUM		(MAX_SUPPORT_SATA_NUM + MAX_SUPPORT_ESATA_NUM)//最大支持的硬盘数目

/* 3531 两盘位的io为 3进3出
 * 	八盘位的io为 16进4出
*/
#define MAX_ALARM_IN			4//最大报警输入个数
#define MAX_ALARM_OUT			4//最大报警输出个数


#define MAX_SUPPORT_DEV_NUM		(0xFF)//最大支持的设备数目
#define MAX_SUPPORT_DEV_CHANNEL_NUM	(0x3F)//一个设备最大的通道数目


#define NVR_VERSION			"v5.1.22"

#define FLAG_PAUSE                      0//暂停
#define FLAG_RUN                        1//运行
#define FLAG_EXIT                       2//退出
#define FLAG_RESTART                    3//重启
#define FLAG_STOP                       4//停止
#define FLAG_PATROL_NEXT_CHANNEL        5//轮询到下一个通道
#define FLAG_WAIT                       6//等待


/*************************************** 全局通道信息 ***************************************/
typedef struct dev_channel_stream_info_t
{
	pthread_mutex_t mutex;
	unsigned int usedFlag:1;//是否可以使用
	unsigned int usedNum:31;//被使用的次数
	void *pPacketQueue;//包队列
	void *pFrameQueue;//帧队列
	void *pYuvQueue;//yuv队列
}dev_channel_stream_info_t;

typedef struct dev_channel_network_info_t
{
	pthread_mutex_t mutex;
	int runFlag;//网络线程运行标记  0：未运行  1：线程已经在运行
	void *pNetworkInfo;//网络信息指针
}dev_channel_network_info_t;

#if 0
typedef struct dev_channel_decode_info_t
{
	pthread_mutex_t mutex;
	int runFlag;//解码线程运行标记  0：未运行  1：线程已经在运行
	void *pDecodeInfo;//解码信息指针
}dev_channel_decode_info_t;
#endif

typedef struct dev_channel_record_info_t
{
	pthread_mutex_t recordMutex;
	int recordFlag;//运行状态
}dev_channel_record_info_t;

typedef struct dev_channel_info_t
{//设备通道信息
	pthread_mutex_t devChannelMutex;
//	spinlock_t  devChannelSpinlock;
	int flag;//标记 
//	int enable;
	unsigned char channelNo;//设备号
	unsigned char streamNo;//通道号
	dev_channel_stream_info_t streamInfo;//流信息
	dev_channel_network_info_t networkInfo;//网络信息
//	dev_channel_decode_info_t decodeInfo;//解码信息
}dev_channel_info_t;


typedef struct dev_channel_t
{//设备通道信息
	unsigned char channelNo;//设备号
	unsigned char streamNo;//通道号
	int pos;//显示的分屏位置
}dev_channel_t;


#if defined (__cplusplus)
}
#endif

#endif//__GLOBAL_H__

