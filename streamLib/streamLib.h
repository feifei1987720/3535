#ifndef __STREAM_LIB_H__
#define __STREAM_LIB_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sched.h>
#include <sys/resource.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "ncx_slab.h"
#include "buffer_ring.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define FRAME_TYPE_VIDEO 0//视频帧
#define FRAME_TYPE_AUDIO 1//音频帧

#define NAL_TYPE_SLICE      1
#define NAL_TYPE_IDR        5
#define NAL_TYPE_SEI        6
#define NAL_TYPE_SPS        7
#define NAL_TYPE_PPS        8
#define NAL_TYPE_SEQ_END    9
#define NAL_TYPE_STREAM_END 10



//#define MAX_DISPLAY_CHANNEL_NUM		1//  最大显示几屏画面
//#define MAX_DECODE_CHANNEL_NUM		MAX_DISPLAY_CHANNEL_NUM//最大解几路码流
//#define MAX_MCTP_NUM			MAX_DISPLAY_CHANNEL_NUM//最大的网页线程数
//#define MAX_PLAT_NUM			1//最大的平台线程数
//#define MAX_RECORD_NUM			1//最大的录像线程数
//#define MAX_RETORE_ALARM_NUM		MAX_DISPLAY_CHANNEL_NUM//最大的报警和一键复位线程
//#define MAX_AUDIO_NUM			MAX_DISPLAY_CHANNEL_NUM//最大的音频线程
//#define MAX_MAC_NUM			MAX_DISPLAY_CHANNEL_NUM//最大的修改MAC地址线程
//#define MAX_RECV_CHANNEL_NUM		MAX_DISPLAY_CHANNEL_NUM//最大接收几路视频流
#define MAX_FRAME_QUEUE_NUM		(40)//帧缓冲队列的数目
#define MAX_FRAME_BUFFER_LEN 		(1024 * 1024)//最大的一帧的长度
#define MAX_STREAM_PACKET_SIZE		(1500)//最大包长度
#define MAX_ONE_FRAME_HAVE_PACKET_NUM	(MAX_FRAME_BUFFER_LEN/MAX_STREAM_PACKET_SIZE)//一帧最大有多少个包
#define MAX_YUV_QUEUE_NUM		3//yuv队列的数目
#define MAX_ADDR_LEN			64//ip地址的长度
#ifdef HI3535
#define MAX_ALARM_QUEUE_NUM		(150)//
#else
#define MAX_ALARM_QUEUE_NUM		(50)//
#endif



typedef enum frame_video_e
{
	/* 视频帧类型 */
	FRAME_VIDEO_TYPE_H264 = 0,
	FRAME_VIDEO_TYPE_MPEG4 = 1,
	FRAME_VIDEO_TYPE_JPEG = 2,

	/* 视频帧分辨率 */
	FRAME_VIDEO_RESOLUTION_352x288 = 0,
	FRAME_VIDEO_RESOLUTION_720x576 = 1,
	FRAME_VIDEO_RESOLUTION_1280x720 = 2,
	FRAME_VIDEO_RESOLUTION_1920X1080 = 3,
	FRAME_VIDEO_RESOLUTION_2048X1536 = 4,

}frame_video_e;

typedef enum frame_audio_e
{
	/* 音频帧类型 */
	FRAME_AUDIO_TYPE_G711 = 0,//

	/* 音频采样率 */
	FRAME_AUDIO_SAMPLE_RATE_8K = 0,//8K
	FRAME_AUDIO_SAMPLE_RATE_32K = 1,//32K
	/* 音频位宽 */
	FRAME_AUDIO_BIT_WIDE_8 = 0,//8位
	FRAME_AUDIO_BIT_WIDE_16 = 1,//16位

	/* 音频通道 */
	FRAME_AUDIO_CHANNEL_1 = 0,//单通道
	FRAME_AUDIO_CHANNEL_2 = 1,//双通道

}frame_audio_e;


typedef struct stream_packet_head_t
{
	unsigned short seq;//该包的seq号
	char channel;//  该包的通道  0：视频  2：音频
	//char unused[1];//保留
	char isKey;
	unsigned int pts;
	unsigned short offset;//有效数据的起始地址
	unsigned int len;//有效数据的长度
}stream_packet_head_t;

typedef struct stream_packet_t
{
	struct stream_packet_t *next;
	struct stream_packet_t *pre;
	stream_packet_head_t packetHead;
    char * pPacketBuf;  //[MAX_STREAM_PACKET_SIZE];//rtp
}stream_packet_t;

typedef struct stream_packet_queue_t
{//包队列
#if 0
	unsigned int packetTotalNum;//包队列总的长度
	stream_packet_t *pPacketBuf;//包队列总的buf

    unsigned int freeBufNum;//空闲的包数目
	stream_packet_t freeHead;//空闲的包队列头
	stream_packet_t *pFreeLast;//空闲的包队列尾
     unsigned int copyBufNum;//
	unsigned int usedBufNum;//使用的包数目
    // int pPacketBufNum;
	stream_packet_t usedHead;//使用的包队列头
	stream_packet_t *pUsedLast;//使用的包队列尾

	/*  */
	char *pPoolBuf;//
	ncx_slab_pool_t *sp;
#endif



    Packet_buffer_t packetBuf;
    ring_buffer_t ringBuf;//buf
    unsigned char startFlag;//

}stream_packet_queue_t;

typedef struct stream_frame_head_t
{
	unsigned char type;//帧格式  0：视频  1：音频
	unsigned char source;//0:ipc  1:平台  (因为帧的组包方式不一样。 0：sps、pps、I、P都是单独的一帧   1：sps、pps、I是一帧，P是一帧)
	unsigned short frameNo;//帧号
	struct timeval tv;//
	unsigned int pts;
	unsigned char save[4];//
	//unsigned char save[8];//保留字段
	union
	{
		struct
		{
			unsigned char format;//编码格式         0:H264     1:mpeg4    2:jpeg  
			unsigned char resolution;//分辨率       0:352*288  1:720*576  2:1280*720  3:1920*1080
			unsigned short bitRate;//码率  bitRate*1024
			unsigned short width;//宽
			unsigned short height;//高
			unsigned char frameRate;//帧率
			unsigned char forceRestartNum;//强制重启编码器
			unsigned char unused[2];//保留
		}video;
		struct
		{
			unsigned char format;//编码格式 0:G711 
			unsigned char sampleRate;//采样率  0：8K  1：32K
			unsigned char channel;//通道  0：单通道  1：双通道
			unsigned char bitWide;//位宽  0:8位  1：16位
			unsigned char unused[8];//保留
		}audio;
	}info;
	int len;//帧长度
}stream_frame_head_t;
typedef struct stream_frame_t
{
	stream_frame_head_t frameHead;
	char pFrameBuf[0];//帧数据
}stream_frame_t;

/* 帧队列 */
typedef struct stream_frame_queue_t
{
	int writePos;
	pthread_mutex_t frameMutex;
	pthread_cond_t frameCond;
	stream_frame_t *pFrameInfo[MAX_FRAME_QUEUE_NUM];
}stream_frame_queue_t;

typedef struct stream_yuv_head_t
{//yuv头信息
	unsigned char resolution;//分辨率  0：352*288  1:720*576  2:1280*720  3:1920*1080
	unsigned char frameRate;//帧率
	unsigned short bitRate;//码率  bitRate*1024
	unsigned short width;//宽
	unsigned short height;//高
	int len;//帧长度
}stream_yuv_head_t;
typedef struct stream_yuv_t
{//yuv信息
	int yuvNo;//no;//
	stream_yuv_head_t yuvHead;
//	BufferGfx_Dimensions srcDim;
//	BufferGfx_Dimensions dstDim;
	char *pYuvBuf;//一个yuv数据的buf，指向pYuvDataBuf中的某段内存
	unsigned long pYuvBufPhyAddr;//yuvbuf的物理地址
	struct stream_yuv_t *next;
	struct stream_yuv_t *pre;
}stream_yuv_t;
typedef struct stream_yuv_queue_t
{//yuv队列
	stream_yuv_t *pYuv;//yuv总队列的buf
	pthread_mutex_t yuvMutex;//yuv队列互斥锁
	int packetTotalNum;//包队列总的长度

	char *pYuvDataBuf;//yuv数据的buf
	int freeBufNum;//空闲的包数目
	stream_yuv_t freeHead;//空闲的包队列头
	stream_yuv_t *pFreeLast;//空闲的包队列尾

	int usedBufNum;//使用的包数目
	stream_yuv_t usedHead;//使用的包队列头
	stream_yuv_t *pUsedLast;//使用的包队列尾
}stream_yuv_queue_t;


typedef struct stream_alarm_t
{//
	unsigned char type;//
	unsigned char channelNo;
	unsigned char streamNo;
	unsigned char status;//0    1  
}stream_alarm_t;
/*  */
typedef struct stream_alarm_queue_t
{
	int writePos;
	pthread_mutex_t alarmMutex;
	pthread_cond_t alarmCond;
	stream_alarm_t alarmInfo[MAX_ALARM_QUEUE_NUM];
}stream_alarm_queue_t;



/* streamLib.c */
/* 流初始化 */
int streamLibInit();
/* 流销毁 */
int streamLibUninit();





/* streamFrameQueue.c */
/* 帧初始化 */
int streamInitFrameQueue(stream_frame_queue_t *pFrameQueue);
/* 帧销毁 */
void streamUninitFrameQueue(stream_frame_queue_t *pFrameQueue);
//////////////////////////////////////////////////////////////////////
/* 为了减少一次拷贝，所以增加以下几个函数，用的时候注意锁*/
/* 锁住帧队列 */
void streamFrameQueueLock(stream_frame_queue_t *pFrameQueue);
/* 解锁帧队列 */
void steramFrameQueueUnlock(stream_frame_queue_t *pFrameQueue);
/* 获取一帧的缓冲 */
stream_frame_t * streamGetFrameQueueBuffer(stream_frame_queue_t *pFrameQueue, int frameLen);
/* 插入一帧到帧队列中   （指针赋值）*/
void streamInsertFrameToFrameQueue(stream_frame_queue_t *pFrameQueue, stream_frame_t *pFrame);
//////////////////////////////////////////////////////////////////////
#if 0
/* 发送数据到帧队列中 */
int streamSendFrameToNetPool(stream_frame_queue_t *pFrameQueue, char *pStreamBuf, int streamBufLen);
#endif
/* 等待一帧视频 */
int streamWaitFrame(stream_frame_queue_t *pFrameQueue);
/* 获取一帧视频 */
stream_frame_t * streamGetFrameFromNetPool(stream_frame_queue_t *pFrameQueue, int *pReadBegin, int *pReadEnd);
/* 释放一帧视频 */
void streamFreeFrameBuffer(stream_frame_queue_t *pFrameQueue, stream_frame_t *pFrameInfo);
/* 释放所有帧视频buf */
void streamFreeAllFrameBuffer(stream_frame_queue_t *pFrameQueue);





/* streamPacketQueue.c */
/************************************************************
 * 初始化流缓冲
 * pStreamInfo 某个视频窗口对应的流信息
 ************************************************************/
int streamInitPacketQueue(stream_packet_queue_t *pPacketQueue,int protocol);
/*  */
void streamUninitPacketQueue(stream_packet_queue_t *pPacketQueue,int protocol);
/*********************************************************
 *获取空闲的包buffer
 *********************************************************/
stream_packet_t * streamGetPacketWriteBuffer(stream_packet_queue_t *pPacketQueue);
/* 将包buffer插入到使用队列中 */
int streamInsertQueue(stream_packet_queue_t *pPacketQueue, stream_packet_t *pStreamBuf);
/* 获取使用队列的第一个包buffer */
stream_packet_t * streamGetPacketUsedBuffer(stream_packet_queue_t *pPacketQueue);
/* 将使用过的包buffer插入到未使用的链表中 */
void streamFreeUsedBuffer(stream_packet_queue_t *pPacketQueue, stream_packet_t *pStreamBuf);
/* 获取一帧视频 */
int streamGetFrameBuf(stream_packet_queue_t *pPacketQueue, char *pStreamBuf, int streamBufLen);
/* 获取包队列当前包的个数 */
int streamPacketBufferNum(stream_packet_queue_t *pPacketQueue);
/* 将包缓冲队列中的数据刷到帧缓冲中 */
void streamPushPacketBufferToFrameBuffer(stream_packet_queue_t *pPacketQueue);


stream_packet_t * private_protocol_streamGetPacketWriteBuffer(stream_packet_queue_t *pPacketQueue);

void private_protocol_streamFreeUsedBuffer(stream_packet_queue_t *pPacketQueue, stream_packet_t *pStreamBuf);

/* streamYuvQueue.c */
/* 初始化Yuv队列 */
int streamInitYuvQueue(stream_yuv_queue_t *pYuvQueue);
/* 销毁Yuv队列 */
void streamUninitYuvQueue(stream_yuv_queue_t *pYuvQueue);
/*********************************************************
 * 获取空闲的buffer
 *********************************************************/
stream_yuv_t * streamGetYuvWriteBuffer(stream_yuv_queue_t *pYuvQueue);
/* 将流buffer插入到使用队列中 */
int streamInsertYuvQueue(stream_yuv_queue_t *pYuvQueue, stream_yuv_t *pYuv);
/* 获取一帧 used yuv buffer */
stream_yuv_t * streamGetYuvUsedBuffer(stream_yuv_queue_t *pYuvQueue);
/* 将使用过的包buffer插入到未使用的链表中 */
void streamFreeYuvUsedBuffer(stream_yuv_queue_t *pYuvQueue, stream_yuv_t *pYuv);
/* 释放Yuv队列的内存 */
void streamFreeYuvBuffer(stream_yuv_queue_t *pYuvQueue);



/* streamAlarmQueue.c */
int streamInitAlarmQueue(stream_alarm_queue_t *pAlarmQueue);
void streamUninitAlarmQueue(stream_alarm_queue_t *pAlarmQueue);
int streamWaitAlarm(stream_alarm_queue_t *pAlarmQueue);
void streamInsertAlarmToAlarmQueue(stream_alarm_queue_t *pAlarmQueue, stream_alarm_t *pAlarm);
int streamGetAlarmFromNetPool(stream_alarm_queue_t *pAlarmQueue, int *pReadBegin, int *pReadEnd, stream_alarm_t *pAlarm);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif//__STREAM_LIB_H__

