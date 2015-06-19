
#ifndef __PLAYBACK_REMOTE_H__
#define __PLAYBACK_REMOTE_H__


#ifdef HI3520D
#define MAX_SUPPORT_REMOTE_PLAYBACK_NUM		8
#elif HI3521
#define MAX_SUPPORT_REMOTE_PLAYBACK_NUM		16
#elif HI3531
#define MAX_SUPPORT_REMOTE_PLAYBACK_NUM		16
#elif HI3535
#define MAX_SUPPORT_REMOTE_PLAYBACK_NUM		16
#endif

#define MAX_PATH				260


#define CMD_NVR_BASE				0x10000			//nvr信令基准值

#define CMD_QUERYRECFILE_STATUS			(CMD_NVR_BASE + 0x62)	//客户端通知录像服务器查询录像文件状态
#define CMD_QUERYRECFILE			(CMD_NVR_BASE + 0x63)	//客户端通知录像服务器查询录像文件
#define CMD_DOWNLOADRECFILE			(CMD_NVR_BASE + 0x64)	//客户端通知录像服务器下载录像文件
#define	CMD_QUERY_STOP				(CMD_NVR_BASE + 0x57)	//通知中心停止查询
#define CMD_REMOTEFILESTATUS				(CMD_NVR_BASE + 0x6B)	//录像服务器发送录像文件名给客户端状态
#define CMD_REMOTEFILEENDSTATUS			(CMD_NVR_BASE + 0x6C)	//录像服务器发送录像文件名给客户端，通知结束状态
#define CMD_REMOTEFILE				(CMD_NVR_BASE + 0x66)	//录像服务器发送录像文件名给客户端
#define CMD_REMOTEFILEEND			(CMD_NVR_BASE + 0x67)	//录像服务器发送录像文件名给客户端，通知结束
#define CMD_NVR_DOWNLOADRECFILE_COMPLETE	(CMD_NVR_BASE + 0x90) //下载结束
#define CMD_QUERY_DEVICE			(0x68)   //查询设备（在远程查询设备的时候先查询远端设备，然后根据选择的设备在进行查询文件）
#define CMD_DEVICE_DATA				(0x69)   //查询到的设备数据
#define CMD_QUERY_DEVICE_STOP			(0x6A)   //设备查询结束
#define CMD_PLAYRECFILE_HEADER_REQ		(0x165)	//请求文件头
#define CMD_PLAYRECFILE_SEEK_REQ		(0x265)	//回放录像位置跳转
#define CMD_PLAYRECFILE_PLAY_REQ		(0x365)	//回放继续发送
#define CMD_PLAYRECFILE_PAUSE_REQ		(0x465)	//回放录像暂停
#define CMD_PLAYRECFILE_STOP_REQ		(0x565)	//回放录像停止
#define CMD_PLAYRECFILE_STEP_REQ		(0x665)      //帧进回复
#define CMD_PLAYRECFILE_HEADER_RES		(0x1165)	//回复文件头
#define CMD_PLAYRECFILE_SEEK_RES		(0x1265)	//回放录像位置跳转
#define CMD_PLAYRECFILE_PLAY_RES		(0x1365)	//回放继续发送
#define CMD_PLAYRECFILE_PAUSE_RES		(0x1465)	//回放录像暂停
#define CMD_PLAYRECFILE_STOP_RES		(0x1565)	//回放录像停止
#define CMD_PLAYRECFILE_STEP_RES		(0x1665)      //帧进回复
#define CMD_PLAYRECFILE_DATA			(0x300)	//音视频等数据
#define CMD_PLAYERCFILE_KEEPALIVE		(0xeeee)	//心跳 



typedef struct playback_remote_msg_t
{//远程回放消息
	int len;
	int cmd;
	char  pData[0];
}playback_remote_msg_t;

typedef struct playback_remote_search_condition_t
{//录像搜索条件
	unsigned int startTm;                // 开始时间
	unsigned int EndTm;                  // 结束时间
	unsigned int nChannelID;             // 通道号
	unsigned int nFileType;              // 文件类型  
	int     nReserver;                   // 预留
}playback_remote_search_condition_t;

typedef struct playback_remote_file_head_t
{//文件信息
	long fileType;//文件类型
	long fileSize;//文件大小
	long videoCoder;//视频编码
	long audioCoder;//音频编码
	long frameRate;//帧率
	long bitRate;//比特率
	long width;//图像宽
	long height;//图像高
	long totalFrames;//总共帧数
	long res[8];//保留
}playback_remote_file_head_t;

typedef struct playback_remote_frame_head_t
{//帧头信息
	long type;//数据类型 0表示视频  1表示音频  2表示文件结束
	long len;//数据长度
	long isKeyFrame;//是否为关键帧	.视频数据有效
	long index;//视频帧索引,视频数据有效
	long timestamp;//时间戳
	long res1;
	long res2;
	long res3;
}playback_remote_frame_head_t;

typedef struct playback_remote_play_t
{//播放请求
	long lframe;//请求播放的帧数
	long speed;//速度控制
	long res2;
	long res3;
}playback_remote_play_t;

typedef struct playback_remote_seek_t
{//录像回放拖动
	long lSecond;
	long res1;
	long res2;
	long res3;
}playback_remote_seek_t;



void playbackRemoteUpdateParam();

int playbackRemoteInit();

void playbackRemoteDestroy();

#endif//__PLAYBACK_REMOTE_H__


