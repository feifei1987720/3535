
#ifndef __NETWORK_LIB_H__
#define __NETWORK_LIB_H__

#include "global.h"
#include "streamLib.h"
#include "rtspCLib.h"
#include "queueLib.h"

#define MAX_ADDR_LEN		64//ip地址的长度

typedef enum network_invite_type_e
{
	NETWORK_INVITE_TYPE_DECODE = 0X01,	//解码
	NETWORK_INVITE_TYPE_RECORD = 0x02,	//存储
	NETWORK_INVITE_TYPE_PROXY = 0x04,	//代理、转发

	NETWORK_INVITE_TYPE_ALL = 0xFF,		//所有
}network_invite_type_e;

typedef enum network_msg_queue_e
{//解码消息队列类型
	NETWORK_MSG_QUEUE_TYPE_PASUE = 0,
	NETWORK_MSG_QUEUE_TYPE_RUN = 1,
	NETWORK_MSG_QUEUE_TYPE_EXIT = 2,
	NETWORK_MSG_QUEUE_TYPE_RESTART = 3,
	NETWORK_MSG_QUEUE_TYPE_STOP = 4,
	NETWORK_MSG_QUEUE_TYPE_NEXT = 5,
	NETWORK_MSG_QUEUE_TYPE_WAIT = 6,
}network_msg_queue_e;

typedef enum
{
  NETWORK_ONVIF = 0x02,
  NETWORK_RTSP = 0x03, 
  NETWORK_PROTOCOL =0x04,
                       
}video_protocol;



typedef struct network_info_t
{
	unsigned char enable;

	unsigned char channelNo;
	unsigned char streamNo;

	int protocol;//通讯协议  0：onvif 
	char onvifAddr[32];
	int onvifPort;

	char addr[32];//地址  ：ip、域名      如果protocol = 平台， 则该地址为mdu的地址 
	int port;//端口                                 如果protocol = 平台， 则该端口为mdu的端口
	char userName[32];//用户名
	char userPwd[32];//密码
	unsigned char unused[3];//网络协议  0：rtsp 1:平台 
	unsigned char transport;//传输协议 0：tcp  1：udp
//	unsigned short channel;//通道
	char fileName[64];//请求的文件名

	/* 平台特有的信息 */
	char guid[32];
	char mduId[32];
	char domainId[32];

	char rtspsuburl[128];                                // rtsp协议的url
       char rtspmainurl[128];  
}network_info_t;

typedef struct network_dev_channel_info_t
{////解码信息
	unsigned char runFlag;// 网络线程运行标记  0：未运行  1：线程已经在运行
	unsigned char isConnect;// 0  1

	unsigned char channelNo;//设备号
	unsigned char streamNo;//通道号

	LINK_QUEUE_T msgQueue;//消息队列

	/* 网络 */
	network_info_t *pNetworkInfo;//网络信息
	rtsp_info_t *pRtspInfo;
	char *pRecvBuf;//数据接收缓冲区
	int recvBufLen;//已经接收的数据长度

	/* 视频流 */
//	pthread_mutex_t streamMutex;
	unsigned char usedFlag;//是否可以使用
	unsigned char useNumDecode;//解码库使用次数
	unsigned char useNumRecord;//录像库使用次数
	unsigned char useNumProxy;//转发使用次数
	stream_packet_queue_t *pPacketQueue;
	stream_frame_queue_t *pVideoFrameQueue;
}network_dev_channel_info_t;

/*  */
int networkSetStreamConnect(int channelNo, int streamNo, int value);
int networkCheckStreamExist(int channelNo, int streamNo);

/* 获取实时流使用次数 */
int networkGetStreamUsedNum(int channelNo, int streamNo);

/* 增加实时流使用次数 */
int networkAddStreamUsedNum(int channelNo, int streamNo, int type);

/* 减少实时流使用次数 */
int networkDelStreamUsedNum(int channelNo, int streamNo, int type);

/* 获取实时流帧队列 */
void *networkGetStreamFrameQueue(int channelNo, int streamNo);

/* 设置实时流帧队列 */
void networkSetStreamFrameQueue(int channelNo, int streamNo, void *pFrameQueue);

/* 根据设备通道号请求视频流 */
int networkStartStream(int channelNo, int streamNo, int type);

/* 根据设备通道号重新请求视频流 */
void networkRestartStream(int channelNo, int streamNo);

/* 根据设备通道号停止视频流 */
int networkStopStream(int channelNo, int streamNo, int type);

/* 等待设备通道指定的状态 */
int networkWaitStreamFlag(int channelNo, int streamNo, int value);


/* 获取网络线程运行标记 */
int networkGetRunFlag(int channelNo, int streamNo);

/* 设置网络线程运行标记 */
void networkSetRunFlag(int channelNo, int streamNo, int value);

/* 获取当前网络信息 */
void networkGetNetworkInfo(int channelNo, int streamNo, network_info_t *pNetworkInfo);

/* 设置网络参数 */
int networkSetNetworkInfo(network_info_t *pNetworkInfo);

int setNetworkChannelRecvBitrate(unsigned int channelNo,unsigned int streamNo,unsigned int bitrate);

int getNetworkChannelRecvBitrate(unsigned int channelNo,unsigned int streamNo,void *pRecvData, int* recvDataLen ,int* exten);



int   SetIPCTimeAndDateWhenNTP(int channel);

/* 网络库初始化 */
int networkLibInit();

/* 网络库销毁 */
int networkLibDestroy();


#endif//__NETWORK_LIB_H__

