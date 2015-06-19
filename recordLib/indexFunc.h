#ifndef __INDEX_FUNC_H__
#define __INDEX_FUNC_H__


#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "recordLib.h"


#define INDEX_BLOCK_SIZE			(64*1024)//索引文件中的块大小

#define MMAP_INDEX_RECORD_SIZE			(2 * INDEX_BLOCK_SIZE)//索引文件映射录像区域大小
#define MMAP_INDEX_INDEX_SIZE			(2 * INDEX_BLOCK_SIZE)//索引文件映射索引区域大小
#define MMAP_INDEX_CHANNEL_SIZE			(2 * INDEX_BLOCK_SIZE)//索引文件映射通道区域大小
#define MMAP_INDEX_DATA_SIZE			(INDEX_BLOCK_SIZE)//索引文件映射数据区域大小

#define INDEX_RECORD_INFO_START_BLOCK		1//索引文件中录像文件的区域所占的起始块	1~2
#define INDEX_INDEX_INFO_START_BLOCK		3//索引文件中索引信息所占的起始块	3~4
#define INDEX_CHANNEL_INFO_START_BLOCK		5//索引文件中通道信息所占的起始块	5~6
#define INDEX_UNUSED_START_BLOCK		7//索引文件中保留区域所占的起始块	7~9
#define INDEX_INDEX_DATA_START_BLOCK		10//索引文件中索引数据所占的起始块	10~最后


#define INDEX_USED_FLAG_NULL		0//空
#define INDEX_USED_FLAG_NOT_USE		1//未使用
#define INDEX_USED_FLAG_USING		2//正在使用
#define INDEX_USED_FLAG_USED		3//已经使用

#define INDEX_FILE_STATUS_NULL		0//空
#define INDEX_FILE_STATUS_DEL		1//被删除
#define INDEX_FILE_STATUS_WRITE		2//正常写入
#define INDEX_FILE_STATUS_OK		3//写入完成




typedef struct __index_record_info_t__
{//索引文件中的录像信息
	unsigned short usedNum:13;//录像块被使用的次数
	unsigned short usedFlag:3;//是否使用  1:未使用 2：正在使用  3:使用完毕
	unsigned char channelNo;//使用该个录像文件的设备号
	unsigned char streamNo;//使用该个录像文件的通道号
}index_record_info_t;
#define ONE_RECORD_INFO_SIZE		(sizeof(index_record_info_t))4//一个录像文件描述信息的大小

typedef struct __index_index_info_t__
{//索引文件中的索引信息
	unsigned short usedFlag:3;//是否使用  1:未使用 2：正在使用 3:使用完毕
	unsigned short unused:13;//保留
	unsigned char channelNo;//使用该个索引区域的设备号
	unsigned char streamNo;//使用该个索引区域的通道号
}index_index_info_t;
#define ONE_INDEX_INFO_SIZE		(sizeof(index_index_info_t))//4//一个索引区域描述信息的大小

typedef struct __index_channel_info_t__
{//索引文件中的通道信息
	unsigned short usedFlag:3;//是否使用  1:未使用 2：正在使用  3:使用完毕
	unsigned short unused:13;//保留
#if 0
	unsigned char channelNo;//设备号  暂不使用
	unsigned char streamNo;//通道号  暂不使用
#else
	unsigned char unused1[2];//保留
#endif
	unsigned short curIndexNo;//当前正在操作的索引区域号 (索引区域号从0开始)
	unsigned short curItemNo;//当前正在操作的索引区域的偏移的条目数 (索引条目编号从0开始)
}index_channel_info_t;
#define ONE_CHANNEL_INFO_SIZE		(sizeof(index_channel_info_t))//8//一个通道描述信息的大小

typedef struct __index_file_item_t__
{//索引文件中的文件索引条目
	unsigned short fileStatus:3;//文件状态  1：未使用  2：正在使用 3：使用完毕  4：删除
	unsigned short fileType:13;//文件类型  bit0：定时录像 bit1：告警录像 bit2：手动录像
	unsigned char unused[6];
	unsigned short recordUsedNum:13;//录像块被使用的次数
	unsigned short unused1:3;
	unsigned short recordNo;// 录像文件（%04X，则为录像文件名）
	unsigned int startTime;//录像起始时间（秒）
	unsigned int endTime;//录像结束时间（秒）
	unsigned int startAddr;//该文件第一帧对应的录像文件中I帧索引的地址
	unsigned int endAddr;//该文件最后一帧对应的录像文件中I帧索引的地址
	unsigned int dataStartAddr;//录像文件起始地址
	unsigned int dataEndAddr;//录像数据文件结束地址
}index_file_item_t;
#define ONE_FILE_INFO_SIZE		(sizeof(index_file_item_t))//32//一个文件索引描述信息的大小

#define MAX_SUPPORT_RECORD_NUM		(MMAP_INDEX_RECORD_SIZE/ONE_RECORD_INFO_SIZE)//最大支持多少个录像文件数目
#define MAX_ITEM_NUM_FOR_ONE_INDEXNO	(MMAP_INDEX_DATA_SIZE/ONE_FILE_INFO_SIZE)//一个索引区域包含多少个索引条目
#define MAX_INDEXNO_NUM			((MMAP_INDEX_CHANNEL_SIZE/ONE_INDEX_INFO_SIZE) < (MAX_FILE_SIZE/INDEX_BLOCK_SIZE) ? (MMAP_INDEX_CHANNEL_SIZE/ONE_INDEX_INFO_SIZE) : (MAX_FILE_SIZE/INDEX_BLOCK_SIZE))//索引区域的最大数目

typedef struct __index_info_mmap_t__
{//索引文件映射信息
	char *pMmapAddr;//映射地址
	unsigned int startAddr;//对应的文件起始地址
	unsigned int syncAddr;//数据未同步的起始地址
	unsigned int syncLen;//未同步的数据长度
}index_info_mmap_t;
typedef struct __index_info_t__
{//索引文件信息
	char path[64];//索引文件路径
	int fd;//描述符
	int fileNo;//录像文件编号
//	int recordNo;//录像文件编号
//	unsigned char channelNo;//设备号
//	unsigned char streamNo;//通道号
//	unsigned char unused[2];//保留

	index_info_mmap_t mmapRecord;//索引文件录像索引区映射地址
	index_info_mmap_t mmapIndexInfo;//索引文件索引信息区映射地址
	index_info_mmap_t mmapChannel;//索引文件通道区映射地址
//	index_info_mmap_t mmapIndexData;//索引文件索引数据区映射地址
}index_info_t;


typedef struct __index_file_info_t__
{//文件索引信息
	INDEX_HANDLE handle;
	unsigned short curIndexNo; //当前正在操作的索引区域号 (索引区域号从0开始)
	unsigned short curItemNo;//当前正在操作的索引区域的偏移的条目数 (索引条目编号从0开始)
//	unsigned char channelNo;//设备号
//	unsigned char streamNo;//通道号
//	unsigned short recordNo;// 录像文件（%04X，则为录像文件名）
	char *pMmapAddr;//映射地址
}index_file_info_t;



/***************************************** indexFunc.c *****************************************/
/*
 * 打开索引文件
 * pPath：		录像文件路径
 * fileNo:		索引文件编号
 *
 * 返回值：	0：失败  >0:成功
 * */
INDEX_HANDLE indexOpen(char *pPath, int fileNo);

/*
 * 同步数据
 * startAddr：	映射文件的起始地址
 * offset：	要写入的偏移地址
 * dataLen：	要写入的数据长度
 *
 * 返回值：	0：成功  其余：失败
 * */
int indexSync(char *pStartAddr, int offset, int dataLen);

/*
 * 关闭索引文件
 * handle：	索引文件句柄
 *
 * 返回值：	0：成功  其他：失败
 * */
int indexClose(INDEX_HANDLE handle);




/***************************************** indexRecordFunc.c *****************************************/
/*
 * 初始化所有录像文件使用状态
 * 
 * 返回值：	==0:成功  其余：失败
 * */
int indexInitRecordNoFlag(INDEX_HANDLE handle, int totalNum);

/*
 * 获取一个未使用的录像文件编号
 * 
 * 返回值：	>=RECORD_START_NO:成功 编号从2开始  其余：失败
 * */
int indexGetUnusedRecordNo(INDEX_HANDLE handle);

/*
 * 获取录像文件信息
 * recordNo:	录像文件编号
 *
 * pDevNo:	设备号
 * pChannelNo:	通道号
 * pUsedNum:	使用次数
 * 
 * 返回值：	返回该录像文件使用状态  ， < 0：失败  
 * */
int indexGetRecordNoInfo(INDEX_HANDLE handle, int recordNo, int *pDevNo, int *pChannelNo, int *pUsedNum);

/*
 * 获取所有录像文件状态
 *
 * pUsedNum:		已经使用录像文件数目
 * pFreeNum：		空闲的数目
 * pUsingNum：		正在使用的数目
 * pOldRecordNo:	最老的录像文件
 * pOldRecordNoUsedNum:	最老的录像文件使用次数
 * 
 * 返回值：	0:成功  ， < 0：失败  
 * */
int indexGetAllRecordNoInfo(INDEX_HANDLE handle, int *pUsedNum, int *pFreeNum, int *pUsingNum, int *pOldRecordNo, int *pOldRecordNoUsedNum);

/*
 * 检测录像文件
 * recordNo:		录像文件编号
 * type:		录像文件的状态
 * recordUsedNum:	录像文件使用的次数
 * channelNo:		设备号
 * streamNo:		通道号
 * 
 * 返回值：	=0:正常  ， < 0：失败  
 * */
int indexCheckRecordNoInfo(INDEX_HANDLE handle, int recordNo, int type, int recordUsedNum, unsigned char channelNo, unsigned char streamNo);

/*
 * 查找最老的录像文件
 *
 * pRecordNo:		录像文件
 * pRecordUsedNum:	该录像文件使用次数
 *
 * 返回值：		0：找到合适的录像文件  1：有空闲的录像文件  其余：失败
 * */
int indexFindOldRecordNo(INDEX_HANDLE handle, int *pRecordNo, int *pRecordUsedNum);

/*
 * 修改录像文件使用状态
 * recordNo:	录像文件编号
 * type:	状态
 * channelNo:	设备号
 * streamNo:	通道号
 * 
 * 返回值：	>=0:成功  其余：失败
 * */
int indexModifyRecordNoFlag(INDEX_HANDLE handle, int type, int recordNo, unsigned char channelNo, unsigned char streamNo);

/*
 * 查找某个设备的某个通道所占用的所有录像文件编号
 *
 * channelNo:       	设备号
 * streamNo:   	通道号
 * pRecordNo:           录像文件编号;
 * pRecordNoUsedNum:    录像文件使用次数
 * 
 * 返回值：     >=0:成功  其余：失败
 * */
int indexFindDevChannelAllUsedRecordNo(INDEX_HANDLE handle, unsigned char channelNo, unsigned char streamNo, unsigned short *pRecordNo, int *pRecordNoUsedNum);

/*
 * 删除某个设备的某个通道所占用的录像文件
 *
 * channelNo:	设备号
 * streamNo:	通道号
 * 
 * 返回值：	>=0:成功  其余：失败
 * */
int indexDelDevChannelRecordNo(INDEX_HANDLE handle, unsigned char channelNo, unsigned char streamNo);




/***************************************** indexIndexFunc.c *****************************************/
/*
 * 获取未使用的文件索引区域块编号
 * */
int indexGetUnusedIndexNo(INDEX_HANDLE handle);

/*
 * 修改索引区域使用状态
 * type:	状态
 * channelNo:	设备号
 * streamNo:	通道号
 * 
 * 返回值：	>=0:成功  其余：失败
 * */
int indexModifyIndexNoFlag(INDEX_HANDLE handle, int type, int indexNo, unsigned char channelNo, unsigned char streamNo);

/*
 * 查找某个设备的某个通道所占用的索引区域编号
 * channelNo:	设备号
 * streamNo:	通道号
 * pIndexNo:    索引区域信息
 * 
 * 返回值：	>=0:成功  其余：失败
 * */
int indexFindDevChannelUsedIndexNo(INDEX_HANDLE handle, unsigned char channelNo, unsigned char streamNo, unsigned short *pIndexNo);





/***************************************** indexChannelFunc.c *****************************************/
/*
 * 获取通道正在使用的文件索引区域块编号
 *
 * channelNo:	设备号
 * streamNo:	通道号
 *
 * pIndexNo:	索引区域编号
 * pItemNo:	索引条目编号
 *
 * 返回值：	0：成功  其余：失败
 * */
int indexGetFileCurIndexNo(INDEX_HANDLE handle, unsigned char channelNo, unsigned char streamNo, int *pIndexNo, int *pItemNo);

/*
 * 修改通道区域使用状态
 * type:	状态
 * indexNo:	当前使用的索引编号
 * itemNo:	当前使用的索引区域中的条目编号
 * channelNo:	设备号
 * streamNo:	通道号
 * 
 * 返回值：	>=0:成功  其余：失败
 * */
int indexModifyChannelIndexNoFlag(INDEX_HANDLE handle, int type, int indexNo, int itemNo, unsigned char channelNo, unsigned char streamNo);





/***************************************** indexFileFunc.c *****************************************/
/*
 * 打开索引文件的文件索引区域
 * pPath：	录像文件路径
 * indexNo:	索引区域编号
 * itemNo:	索引条目编号
 *
 * 返回值：	0：失败  >0:成功
 * */
INDEX_FILE_HANDLE indexOpenFileInfo(INDEX_HANDLE handle, int indexNo, int itemNo);

/*
 * 映射索引文件中的文件索引区域
 * handle：		索引文件句柄
 * indexNo:		文件索引所在的编号
 *
 * 返回值：		返回映射的指针，NULL:失败
 * */
char * indexMmapFileInfo(INDEX_HANDLE handle, int indexNo);

/*
 * 关闭索引文件的文件索引区域
 * fileHandle:	文件索引句柄
 *
 * 返回值：	0：失败  >0:成功
 * */
void indexCloseFileInfo(INDEX_FILE_HANDLE fileHandle);

/*
 * 更新文件索引条目信息
 * handle：	索引文件句柄
 * fileHandle：	索引文件中索引区域句柄
 *
 * fileStatus:	
 * fileType:	
 * recordNo:	
 * recordUsedNum:
 * startTime:	
 * endTime:	
 * startAddr:	
 * endAddr:	
 * dataEndAddr:	
 * 以上字段中，为0时，表示不修改原有数据
 *
 * 返回值：	0：成功  其他：失败
 * */
int indexUpdateFileInfo(INDEX_HANDLE handle, INDEX_FILE_HANDLE fileHandle, int fileStatus, int fileType, 
		int recordNo, int recordUsedNum, unsigned int startTime, unsigned int endTime, unsigned int startAddr, unsigned int endAddr, unsigned int dataStartAddr, unsigned int dataEndAddr);

/*
 * 获取当前正在操作的录像文件编号
 *
 * handle:		索引文件句柄
 * fileHandle：		索引文件中文件索引区域句柄
 *
 * pRecordNo:		正在使用的录像文件编号
 * pRecordUsedNum:	该录像文件使用的次数
 * pIFrameStartAddr：	下一个I帧写入的起始地址
 * pDataStartAddr：	下一个录像数据写入的起始地址
 *
 * 返回值：		0：成功  其他：失败
 *  */
int indexGetCurUsedRecordNo(INDEX_HANDLE handle, INDEX_FILE_HANDLE fileHandle, int *pRecordNo, int *pRecordUsedNum, unsigned int *pIFrameStartAddr, unsigned int *pDataStartAddr);

/*
 * 查找合适的条目
 *
 * handle:              索引文件句柄
 * fileHandle：         索引文件中文件索引区域句柄
 * startTime:           起始时间
 * endTime：            结束时间
 * pRecordNo:           录像文件编号
 * pRecordUsedNum:      录像文件使用的次数
 * recordNoNum:         录像文件数目
 *
 * 返回值：             返回找到的文件条目   <0：失败
 *  */
int indexFindItem(INDEX_HANDLE handle, INDEX_FILE_HANDLE fileHandle, int diskNo,int recordType, int startTime, int endTime, unsigned short *pRecordNo, int *pRecordUsedNum, int recordNoNum, record_search_file_t *pRecordSearchFile, int itemCount);



#endif//__INDEX_FUNC_H__

