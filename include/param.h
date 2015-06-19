
#ifndef __PARAM_H__
#define __PARAM_H__

#include "sdk_struct.h"

#define MAX_SUPPORT_CHANNEL_NUM         64


#define PROC_MSG_KEY		0x2013
#define PROC_MSG_MAIN_KEY	0x2014
#define PROC_MSG_QT_KEY		0x2015
#define PROC_MEM_KEY		0x2016
#define PROC_SEM_KEY		0x2017


#define MAX_SEARCH_FILE_NUM	1000//查询录像文件最大个数


typedef struct proc_msg_t
{
	long Des;///< Where this message go. See /ref Sys_Msg_Def.h
	int type;//消息类型  /< command to control system server.
	int subType;//消息子类型
	int src;///< Where this message from. See /ref Sys_Msg_Def.h
	int result;//返回值  0：成功  其余失败
	int shmid;///< Share memory which was used to communicate with system server.
	int length;///< Data length.
	int offset;///< Offset to begin of share memory.
}proc_msg_t;


/* 消息类型 */
typedef enum proc_msg_type_e
{
	PROC_MSG_TYPE_PARAM_GET = 0,		//获取参数
	PROC_MSG_TYPE_PARAM_SET,		//设置参数
	PROC_MSG_TYPE_PARAM_INSERT,		//插入
	PROC_MSG_TYPE_PARAM_MODIFY,		//修改参数
	PROC_MSG_TYPE_PARAM_DEL,		//删除
	PROC_MSG_TYPE_CONTROL,			//控制
}proc_msg_type_e;
/* 消息子类型 */
typedef enum proc_msg_sub_type_param_e
{
	PROC_MSG_SUB_TYPE_PARAM_DEV_CHANNEL_INFO = 0,		//设备通道信息
	PROC_MSG_SUB_TYPE_PARAM_VIEW_POS_INFO,			//视频窗口位置信息
	PROC_MSG_SUB_TYPE_PARAM_NETWORK_INFO,			//网络参数
	PROC_MSG_SUB_TYPE_PARAM_RECORD_INFO,			//录像参数
	PROC_MSG_SUB_TYPE_PARAM_CHANNEL_INFO,			//录像通道信息
	PROC_MSG_SUB_TYPE_PARAM_TIME_INFO,			//时间信息
	PROC_MSG_SUB_TYPE_PARAM_VERSION_INFO,			//版本信息
	PROC_MSG_SUB_TYPE_PARAM_MANAGE_INFO,			//管理信息
	PROC_MSG_SUB_TYPE_PARAM_USER_INFO,			//用户信息

	PROC_MSG_SUB_TYPE_PARAM_DISPLAY_NUM,			//获取分屏数
	
	PROC_MSG_SUB_TYPE_PARAM_LOG,                 // 日志
	PROC_MSG_SUB_TYPE_PARAM_DISK,                // 硬盘
	PROC_MSG_SUB_TYPE_PARAM_IPC_VIDEO_INFO,     //获取视频,亮度...
	PROC_MSG_SUB_TYPE_PARAM_IPC_ENCODE_INFO,    // 获取编码参数，帧率...
	PROC_MSG_SUB_TYPE_PARAM_DEV_ONE_CHANNEL_INFO,	
	PROC_MSG_SUB_TYPE_PARAM_GENERAL_INFO,
}proc_msg_sub_type_param_e;
typedef enum proc_msg_sub_type_control_e
{
	PROC_MSG_SUB_TYPE_CONTROL_SEARCH_RECORD = 0,		//查询设备录像
	PROC_MSG_SUB_TYPE_CONTROL_PLAY_RECORD_DISPLAY_NUM,	//回放总分屏数
	PROC_MSG_SUB_TYPE_CONTROL_SET_PLAY_RECORD,		//设置回放的录像条目
	PROC_MSG_SUB_TYPE_CONTROL_PLAY_RECORD_TIME,		//设置当前回放起始时间
	PROC_MSG_SUB_TYPE_CONTROL_PLAY_RECORD_SPEED,		//设置回放速度
	PROC_MSG_SUB_TYPE_CONTROL_PLAY_RECORD_STATUS,		//设置回放状态
	PROC_MSG_SUB_TYPE_CONTROL_PLAY_RECORD_CUR_POS,		//获取当前回放的位置
	PROC_MSG_SUB_TYPE_CONTROL_VIEW_DISPLAY_NUM,		//预览分屏数
	
	PROC_MSG_SUB_TYPE_CONTROL_DISK,                // 硬盘
	PROC_MSG_SUB_TYPE_CONTROL_IPC_VIDEO_INFO,      //视频参数
	PROC_MSG_SUB_TYPE_CONTROL_IPC_ENCODE_INFO,     //编码参数
}proc_msg_sub_type_control_e;



/**********************************  参数  **********************************/
/* 数据库中的数据对应的结构体 */
typedef struct param_dev_channel_info_t
{//设备通道信息
//	char name[32];//设备名称
	int enable;//是否使能
	int viewPos;//显示位置
	int channelNo;//设备号
	int streamNo;//通道号
	char addr[64];//地址
	int port;//端口
	char userName[32];//用户名
	char userPwd[32];//用户密码
	int protocol;//协议类型
}param_dev_channel_info_t;

typedef struct param_view_pos_info_t
{//视频窗口显示位置信息
	int pos;//显示位置
	int channelNo;//设备号
	int streamNo;//通道号
}param_view_pos_info_t;

typedef struct param_disk_info_t
{//硬盘信息
	unsigned char recordFlag;//是否用于录像
	unsigned char mountFlag;//是否已经挂载
	unsigned char formatFlag;//是否已经格式化
	unsigned char unused;

	unsigned char uuid[16];
	char devName[16];//设备名
	char volumeName[16];//硬盘卷名
	char mountPath[16];//挂在路径
}param_disk_info_t;

typedef struct param_network_info_t
{//网络参数
	int dhcpEnable;
	char ip[16];//
	char netmask[16];
	char gw[16];
	char mac[32];
	char dns[16];
	char dns2[16];
}param_network_info_t;

typedef struct param_record_channel_t
{
	int channelNo;//通道号
	int streamType;//流类型  0：主码流  1：子码流
	int time[4];//时间段， 每个bit表示一个时段，没半个小时为一个时间段。
}param_record_channel_t;
typedef struct param_record_info_t
{//录像参数
	int enable;
	int recordMode;
	param_record_channel_t recordChannel[MAX_SUPPORT_CHANNEL_NUM];//每个通道的配置数
}param_record_info_t;

typedef struct time_info_t
{
	int year;
	int mon;
	int day;
	int hour;
	int min;
	int sec;
}time_info_t;
typedef struct sntp_info_t
{
	int enable;
	char serverAddr[64];
	int timeZone;
}sntp_info_t;
typedef struct param_time_info_t
{//时间参数
	time_info_t curTime;//当前时间
	sntp_info_t sntp;
}param_time_info_t;

typedef struct param_version_info_t
{//版本信息
	char version[64];
	char date[32];
}param_version_info_t;

typedef struct param_system_manage_t
{//系统管理
	int timerRebootEnable;//使能定时重启
	int week;//星期
	int hour;
	int min;
	int sec;
//	int time;//时间  时：分：秒
}param_system_manage_t;

typedef struct param_display_num_t
{//分屏数
	int num;
}param_display_num_t;

typedef struct param_user_manage_t
{//用户管理
	char userName[32];
	char userPwd[32];
	unsigned int power;//用户权限  每个bit表示一种权限  bit0:视频浏览  bit1:参数设置  bit2:回放
}param_user_manage_t;

typedef struct
{
	// 硬盘号对应插槽号，1 ... N
	unsigned int id;             // 硬盘号 1~99内部, 100 ~ 外部
	unsigned int status;         // 正常/异常
	unsigned int type;           // 不使用/读写
	unsigned int capacity;       // 容量
}param_hdd_info_t;

//用于格式化命令
typedef struct {
	unsigned int id;             // 硬盘号 1 - 1000
}control_hdd_format_t;

//用于设置类型(Read/Write)
typedef struct{
	unsigned int id;            // 硬盘号  1 - 1000  
	unsigned int type;          
}param_hdd_type_t;

typedef struct para_video_info_t
{//ipc视频信息
	int channelNo;  //设备通道号
	int Brightness; //亮度
	int Contrast;   //对比度
	int Statuation; //饱和度
	int Sharpness;  //锐度
}para_video_info_t;

typedef struct param_encodSet_info_t
{
	int channelNo;  //设备通道号
	int stream;     //码流  0:主码流  1：次码流
	int resolution; //分辨率
	int frame;      //帧率
	int bit_rate;   //比特率
	int ratecontrol;//码率控制  0 ：定码率  1：变码率

}param_encodSet_info_t;

typedef struct param_general_info_t
{//通用信息
	int nvrNo;  //nvr编号
	int displayResolution;  //显示分辨率
	int autoQuitTime;   //自动推出时间
}param_general_info_t;



/**********************************  查询  **********************************/
typedef struct control_search_record_t
{//查询设备录像
	int channelNo;//设备号
	int streamNo;//通道号
	int recordType;//录像类型  0xFF：所有
	int startTime;//起始时间
	int endTime;//结束时间
}control_search_record_t;

#if 0
typedef struct record_search_file_t
{//录像文件信息
	unsigned short diskNo;//硬盘号
	unsigned short recordNo;// 录像文件（%04X，则为录像文件名）
	unsigned short fileType;//文件类型  bit0：定时录像 bit1：告警录像 bit2：手动录像
	unsigned char unused[2];//
	unsigned int startTime;//录像起始时间（秒）
	unsigned int endTime;//录像结束时间（秒）
	unsigned int startAddr;//该文件第一帧对应的录像文件中I帧索引的地址
	unsigned int endAddr;//该文件最后一帧对应的录像文件中I帧索引的地址
	unsigned int dataStartAddr;//录像文件起始地址
	unsigned int dataEndAddr;//录像数据文件结束地址
}record_search_file_t;
#endif

typedef struct control_search_log_t
{//查询日志
	time_info_t logstartTime;
	time_info_t logendTime;
	int logType;//日志类型  0：所有日志  1：系统日志  2：网络日志  3：操作日志  4：错误日志
}control_search_log_t;
typedef struct log_search_into_t
{//日志信息
	int logType;
	time_info_t logTime;
	char logContent[64];//日志内容
	int  user;        //用户类型
}log_search_into_t;



/**********************************  控制  **********************************/
typedef struct control_play_record_t
{//回放信息
	int status;//状态  0：播放  1：停止单个 2：停止所有
	int displayChannel;//通道窗口号
	int speed;//播放速度
	int fileNum;//播放的文件总数
	record_search_file_t searchFile[MAX_SEARCH_FILE_NUM];
	int startTime;//播放时间
	int startFileNum;//播放哪个文件
	int offset;//播放该文件的偏移位置
}control_play_record_t;

typedef struct control_play_time_t
{//回放具体录像
	int displayChannel;
	int startTime;
	int startFileNum;//播放哪个文件
	int offset;//播放该文件的偏移位置
}control_play_time_t;

typedef struct control_play_speed_t
{//回放速度
	int speed;
}control_play_speed_t;

typedef struct control_play_status_t
{//回放状态
	int displayChannel;
	int status;//0：播放  1：停止  2：暂停
}control_play_status_t;

typedef struct control_play_cur_pos_t
{//当前回放位置
	unsigned int time;//当天的偏移时间
}control_play_cur_pos_t;

typedef struct control_play_record_num_t
{//回放通道数目
	int num;
}control_play_record_num_t;

typedef struct control_view_display_num_t
{//预览通道数目
	int num;//分屏数
	int viewPos;//起始分屏的编号 0~36
}control_view_display_num_t;



#endif//__PARAM_H__

