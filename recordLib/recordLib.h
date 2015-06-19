#ifndef __RECORD_LIB_H__
#define __RECORD_LIB_H__


#include <pthread.h>
#include "mylist.h"
#include "queueLib.h"
#include "streamLib.h"
#include "param.h"
#include "global.h"

#define DISK_PATH "/disk"

#define MAX_FILE_SIZE			(1024*1024*1024)//每个录像文件的大小
#ifdef HI3535
#define MMAP_RECORD_DATA_SIZE		(2560*1024)//录像文件映射录像区域大小
#else
#define MMAP_RECORD_DATA_SIZE		(1536*1024)//录像文件映射录像区域大小
#endif
#define MMAP_RECORD_I_FRAME_SIZE	(16*1024)//录像文件映射I帧索引区域大小


#define INDEX_MAIN_NO			0//主索引文件编号
#define INDEX_BAK_NO			1//备份索引文件编号
#define RECORD_START_NO			2//录像文件起始编号

/* 录像文件类型 */
#define RECORD_FILE_TYPE_NORMAL		0X01//普通录像
#define RECORD_FILE_TYPE_ALARM		0X02//告警录像
#define RECORD_FILE_TYPE_HAND		0X04//手动录像

/*  */
#define RECORD_STATUS_NOT_INIT		1//未初始化
#define RECORD_STATUS_RECORD		2//正常录像
#define RECORD_STATUS_SWITCH		3//切换
#define RECORD_STATUS_SWITCH_ITEM	4//切换
#define RECORD_STATUS_SWITCH_RECORD_NO	5//切换录像文件
#define RECORD_STATUS_SWITCH_DISK_NO	6//切换硬盘
#define RECORD_STATUS_RESTART		7//重新启动
#define RECORD_STATUS_STOP		8//停止录像
#define RECORD_STATUS_WRITE_ERROR	9//录像写入错误
#define RECORD_STATUS_PAUSE                 10//录像暂停

/* 硬盘状态 */
#define DISK_STATUS_NOT_INIT		0//硬盘未初始化
#define DISK_STATUS_NORMAL		1//正常
#define DISK_STATUS_USING		2//正在使用
#define DISK_STATUS_ERROR		3//硬盘损坏
#define DISK_STATUS_STOP_USED		4//停止使用

/* 录像库标记 */
typedef enum __record_update_flag_e__
{
	RECORD_UPDATE_FLAG_PARAM = 0,		//录像参数变化
	RECORD_UPDATE_FLAG_DISK_STATUS,		//硬盘发生变化
	MOTION_UPDATE_FLAG_PARAM,			//移动侦测参数发生变化
}record_update_flag_e;

/* 消息队列中消息类型 */
typedef enum __msg_queue_type_e__
{
	RECORD_MSG_QUEUE_TYPE_NORMAL = 0,		//正常操作
	RECORD_MSG_QUEUE_TYPE_START_RECORD,		//开始录像
	RECORD_MSG_QUEUE_TYPE_STOP_RECORD,		//停止录像
	RECORD_MSG_QUEUE_TYPE_STOP_ALL_RECORD,		//停止所有录像
	RECORD_MSG_QUEUE_TYPE_DEL,			//删除录像文件
	RECORD_MSG_QUEUE_TYPE_GET_STATUS,		//获取状态
	RECORD_MSG_QUEUE_TYPE_SWITCH_DISK_NO,		//切换硬盘
	RECORD_MSG_QUEUE_TYPE_UPDATE_RECORD_NUM_ADD,	//更新录像文件数目  使用的数目 +1
	RECORD_MSG_QUEUE_TYPE_UPDATE_RECORD_NUM_DEL,	//更新录像文件数目  使用的数目 -1
	RECORD_MSG_QUEUE_TYPE_START_USED_DISKNO,	//开始使用某个硬盘
	RECORD_MSG_QUEUE_TYPE_STOP_USED_DISKNO,		//停止使用某个硬盘
}msg_queue_type_e;

typedef unsigned int RECORD_HANDLE;//录像文件句柄
typedef unsigned int INDEX_HANDLE;//索引文件句柄
typedef unsigned int INDEX_FILE_HANDLE;//索引文件句柄



typedef enum __record_error_e__
{//录像库的错误号
	/* 录像文件 */
	RECORD_LIB_ERROR_RECORD = -200,				//录像错误

	/* 索引文件 */
	RECORD_LIB_ERROR_INDEX = -100,				//索引错误

	RECORD_LIB_ERROR_INDEX_HANDLE_NULL,			//索引文件句柄为空
	RECORD_LIB_ERROR_INDEX_FD_ERROR,			//索引文件描述符错误

	RECORD_LIB_ERROR_INDEX_FILE_MALLOC,			//
	RECORD_LIB_ERROR_INDEX_FILE_HANDLE_NULL,		//索引文件中文件索引区域句柄为空
	RECORD_LIB_ERROR_INDEX_FILE_MMAP_NULL,			//索引文件中文件索引区域映射为空
	RECORD_LIB_ERROR_INDEX_FILE_ITEM_NO_IS_TOO_BIG,		//索引文件中文件索引条目编号超出范围
	RECORD_LIB_ERROR_INDEX_FILE_ITEM_STATUS_NOT_MATCH,	//索引文件中文件索引条目状态不匹配

	RECORD_LIB_ERROR_ERROR = -1,
	RECORD_LIB_ERROR_SUCCESS = 0,
}record_error_e;


typedef struct __record_channel_info_t__
{
	int status;//状态  0：未初始化  1：正常运行  2：停止录像  
	int diskNo;//硬盘编号 从0开始
	INDEX_FILE_HANDLE indexFileHandle;
	unsigned short channelNo;
	unsigned short streamNo;
	int recordType;//录像类型
	unsigned int recordTime;//录像时长 单位秒 
	int sendRecordAlarm;//发送录像状态给界面  0：未发送  1：已发送

	int recordNo;//录像文件编号
	int recordUsedNum;//录像文件使用次数
	RECORD_HANDLE recordHandle;

	int indexNo;
	int itemNo;

	int startStream;//开始接收到视频流
	unsigned int startTime;//开始接收到视频流
	unsigned int endTime;//最近一次接收到视频流
	int waitIFrame;//等待I帧标记  0:需要等待I帧  1：已经等到I帧
	int frameNo;//帧序号
	int iFrameCount;//I帧计数器
	stream_frame_queue_t *pFrameQueue;//帧队列
	int readBegin;
	int readEnd;
	int preReadBegin;
	int preReadEnd;
	unsigned int iFrameWriteAddr;
	unsigned int dataWriteAddr;

	struct kernel_list_head list;
	//struct list_head list;
}record_channel_info_t;

typedef struct record_info_t
{//硬盘信息
	pthread_t recordThreadId;//线程Id
	pthread_mutex_t recordMutex;//锁

	int runFlag;//运行标记

//	unsigned char diskUuid[16];//硬盘uuid
//	char mountPath[16];//挂在路径
	int mountFlag;//是否挂载
	int recordFlag;//是否用于录像  0：不用于  1：用于
	int diskNo;//硬盘编号
	int diskStatus;//硬盘工作状态  1：未初始化  2：正常使用  3：硬盘损坏
	int devChannelNum;//该硬盘被多少个设备通道在使用
	unsigned int writeTotalByte;//写入的总字节数
	unsigned int writeTotalTime;//写入的总耗时时间 （秒）
	unsigned int writeSpeed;//写入速率 （每分钟写入的数据量）

	int usedRecordNoNum;//已经使用的录像文件数目
	int freeRecordNoNum;//空闲的录像文件数目
	int usingRecordNoNum;//正在使用的录像文件数目
	int oldRecordNo;//最老的录像文件
	int oldRecordNoUsedNum;//使用次数

	/* 一个硬盘对应一个索引文件和一个备份索引文件，所以各个通道可以共用，统一放在硬盘信息结构体中 */
	INDEX_HANDLE indexHandle;//索引句柄
	INDEX_HANDLE indexHandleBak;//备份索引句柄
}record_info_t;

typedef struct __record_manage_channel_t__
{//录像管理线程信息
	int diskNo;
	int channelNo;
	int streamNo;//
	int recordType;//录像类型
	int recordStartTime;//录像文件起始时间 秒
	int recordTime;//录像时长 秒

	struct kernel_list_head list;
	//struct list_head list;
}record_manage_channel_t;

/*
 * 初始化硬盘
 * 获取当前每个通道正在写入的位置
 * */
record_info_t * recordLibInit();

/*
 * 录像库销毁
 * */
void recordLibdestroy();

/*
 * 设置更新录像相关标记
 * type:	0：录像逻辑 1：硬盘可用
 * no:		编号
 * value:	值
 */
void recordSetUpdateFlag(int type, int no, int value);

/*
 * 开启一个设备通道录像
 * channelNo:	设备号
 * streamNo：	通道号
 * recordType:	录像类型
 * recordTime:	录像时长 单位秒
 * */
void recordStartDevChannel(unsigned char channelNo, unsigned char streamNo, unsigned char recordType, int recordTime);

/*
 * 停止一个设备通道录像
 * channelNo:	设备号
 * streamNo：	通道号
 * */
void recordStopDevChannel(unsigned char channelNo, unsigned char streamNo);

/* 停止所有设备通道录像 */
void recordStopAllDevChannel();

/* 开始使用某个硬盘 */
void recordStartUsedDiskNo(int diskNo);

/* 停止使用某个硬盘 */
void recordStopUsedDiskNo(int diskNo);

/* 获取硬盘的空闲容量 */
int recordGetFreePercent(int diskNo);

/* 获取硬盘使用状态 */
int recordGetDiskUsedStatus(int diskNo);

/**/
int beepGetStatus(unsigned char channel);

/**/
int beepGetTime(unsigned char channel);


/*
 * 查找设备通道录像文件
 * channelNo:		设备号
 * streamNo：		通道号
 * recordType:		录像类型
 * recordTime:		录像时长 单位秒
 * pRecordSearchFile:	查询到的文件
 * */
int recordFindDevChannelRecord(unsigned char channelNo, unsigned char streamNo, unsigned char recordType, unsigned int recordStartTime, unsigned int recordEndTime, record_search_file_t *pRecordSearchFile);




#endif//__RECORD_LIB_H__

