#ifndef __RECORD_FUNC_H__
#define __RECORD_FUNC_H__


#include "recordLib.h"


#define MAX_RECORD_FILE_PATH_LEN	64//录像文件路径最大长度

#define RECORD_TYPE_DATA		0//写入录像文件的数据类型 I帧
#define RECORD_TYPE_I_FRAME		1//帧数据

//#define RECORD_I_FRAME_INDEX_SIZE	8//录像文件I帧索引的最大长度

#define RECORD_FILE_I_FRAME_SIZE	(10*(MAX_FILE_SIZE/1024))//文件大小

/* 录像文件分区 */
#define RECORD_FILE_I_FRAME_START_ADDR	0//录像文件中 I帧索引的起始地址
#define RECORD_FILE_DATA_START_ADDR	RECORD_FILE_I_FRAME_SIZE//录像文件中 数据的起始地址




typedef struct __record_info_mmap_t__
{//录像文件映射信息
	char *pMmapAddr;//录像文件映射地址
	unsigned int startAddr;//对应的文件起始地址
	unsigned int syncAddr;//数据未同步的起始地址
	unsigned int syncLen;//未同步的数据长度
}record_info_mmap_t;
typedef struct __record_file_info_t__
{//录像文件信息
	char path[MAX_RECORD_FILE_PATH_LEN];//录像文件路径
	int recordNo;//录像文件编号
	int dataFd;//数据 描述符
	int iFrameFd;//i帧 描述符

	record_info_mmap_t mmapIframe;//录像文件I帧索引区映射地址
	record_info_mmap_t mmapData;//录像文件数据区映射地址
}record_file_info_t;

typedef struct __record_i_frame_index_t__
{
//	int no;
	unsigned int time;//该帧记录时间
	unsigned int startAddr;//该帧对应的数据区起始地址
}record_i_frame_index_t;


/**************************************** recordFunc.c ****************************************/
/*
 * 打开录像文件
 * pPath：		录像文件路径
 * type：		打开方式 只读、只写、读写
 * fileNo:		录像文件编号
 * iFrameOffset:	录像帧索引偏移地址
 * dataOffset:		录像偏移地址
 *
 * 返回值：	0：成功  1:剩余空间不足  其余:失败
 * */
int recordOpen(RECORD_HANDLE *pHandle, char *pPath, int type, int fileNo, unsigned int iFrameOffset, unsigned int dataOffset);

/*
 * 映射录像文件
 * handle：		录像文件路径
 * dataOffset:		录像偏移地址
 * iFrameOffset:	录像帧索引偏移地址
 *
 * 返回值：	-1：失败  0:成功
 * */
int recordMmap(RECORD_HANDLE handle, unsigned int dataOffset, unsigned int iFrameOffset);

/*
 * 重新映射录像文件  当映射的内存区域不够的时候，需要向后继续映射
 * handle：		录像文件路径
 * dataOffset:		录像偏移地址
 * iFrameOffset:	录像帧索引偏移地址
 *
 * 返回值：	-1：失败  0:成功
 * */
int recordReMmap(RECORD_HANDLE handle);

/*
 * 写入录像数据
 * handle：	录像文件句柄
 * type：	写入的数据类型  0：录像数据  1：I帧索引
 * writeBuf：	写入buf
 * writeLen：	写入长度
 *
 * 返回值：	0：成功  其他：失败
 * */
int recordWrite(RECORD_HANDLE handle, int type, char *writeBuf, int writeLen);

/*
 * 检测当前的缓冲buf是否足够写入
 * handle：	录像文件句柄
 * frameLen:	写入的帧长度
 *
 * 返回值：	0：成功  1:需要重新映射区域  其他：失败
 * */
int recordCheckBufIsEnough(RECORD_HANDLE handle, int frameLen);

/*
 * 检测当前写入的录像文件是否有足够的空间
 * handle：	录像文件句柄
 *
 * 返回值：	0：成功  1:需要重新映射区域  其他：失败
 * */
int recordCheckFileIsEnough(RECORD_HANDLE handle);

/*
 * 同步录像数据
 * handle：	录像文件句柄
 *
 * 返回值：	0：成功  其他：失败
 * */
int recordSync(RECORD_HANDLE handle);

/*
 * 关闭录像文件
 * handle：	录像文件句柄
 *
 * 返回值：	0：成功  其他：失败
 * */
int recordClose(RECORD_HANDLE handle);


void recordFuncInit();

void recordFuncDestroy();


#endif//__RECORD_FUNC_H__

