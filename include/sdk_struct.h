/*
 * =====================================================================================
 *
 *       Filename:  sdk_struct.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  20110526 023509
 *       Revision:  none
 *       Compiler:  gcc
 *
 *
 * =====================================================================================
 */
#ifndef __sdk_struct_h__
#define __sdk_struct_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


#define MAX_VER_STR_LEN         32      //版本信息长度
#define MAX_SERIAL_NO_LEN       32      //序列号长度
#define MAX_DVR_NAME_LEN        32      //设备名称长度
#define MAX_CHANN_NAME_LEN      32      //通道名称长度
#ifdef  HI3531
#define MAX_CHANN_NUM           16      //最大通道个数
#elif   HI3521
#define MAX_CHANN_NUM           16      //最大通道个数
#elif   HI3520D
#define MAX_CHANN_NUM           8      //最大通道个数
#elif   HI3535
#define MAX_CHANN_NUM           25      //最大通道个数
#endif
#define MAX_STRING_LEN  128
#define MAX_USER_NAME_LEN       32      //用户名长度
#define MAX_PASSWD_LEN          32      //密码长度
#define MAX_ALARM_IN_NAME_LEN   32      //报警输入名称长度
#define MAX_WEEK_NUM            8       //星期天数
#define MAX_TIME_SEG_NUM        6       //时间段个数
#define MAX_ALARM_OUT_NUM       16      //最大报警输出个数
#define MAX_ALARM_IN_NUM        16      //最大报警输入个数
#define MAX_IP_ADDR_LEN         20      //IP地址长度
#define MAX_RTSP_MAINURL_LEN	128
#define MAX_RTSP_SUBURL_LEN		128
#define MAX_MAC_ADDR_LEN        20      //MAC地址长度
#define MAX_ETHERNET_NUM        4       //网卡个数
#define MAX_RECORD_ITEM_LEN     64      //录像记录名称长度
#define MAX_LOG_DESC_LEN        128     //日志记录内容长度
#define MAX_USER_NUM            17      //最大用户个数
#define MAX_PRESET_NUM          128     //最大云台预置位个数
#define MAX_CRUISE_LINE_NUM     3     //最大云台巡航线个数
#define MAX_CRUISE_PRESET_NUM   32      //每条巡航中最大支持的预置位个数
#define MAX_CHANN_CONN_NUM      8       //每个通道最大支持的链接数
#define MAX_DISK_NUM            8       //最大磁盘个数
#define MAX_ALARM_DESC_LEN      64      //报警记录内容长度
#define MAX_SCREEN_NUM          4       //本地最大屏幕个数
#define MAX_WINDOWS_NUM         32      //本地预览视频最大窗口个数
#define MAX_PREVIEW_MODE        8       //最大预览布局个数
#define MAX_OVERLAY_NUM         4       //每个通道最大覆盖区域个数
#define MAX_MOTION_NUM          4       //每个通道最大移动侦测区域个数
#define MAX_OSD_STR_LEN         64      //OSD字符串长度
#define MAX_NTP_SETVER_STR_LEN  32      //NTP服务器地址长度
#define MAX_BACKUP_ITEM			16		//最大备份段数

#define MAX_RESOLUTION_NUM    32
#define MAX_VDA_SENSITIVE_LEVEL 9
#define MAX_STREAM_NUM    		2     	/* 最大码流路数, 0:主码流；1:从码流 */
#define MAX_STOR_FRAGMENT_LEN	16		// sizeof(stor_fragment_t)+6个int的保留大小

#define MAX_NAL_DESC_SIZE       ((4+9*(4+4)))  //视频帧数据前段部分长度

#define MAX_DEV_ID_LEN          32      //最大设备ID长度
#define MAX_WIFI_AP_NUM			16		//最大WIFI-AP个数
#define MAX_IF_NAME_LEN         8       //网络接口名称最大长度

#define SN_FILE_FREEIP    "/data/FreeIpSn.txt"          // 保存FreeIp序列号的文件
#define SN_FILE_DANALE   "/data/sn.txt"		     // 保存DANALE序列号的文件
#define SN_LEN                 64						// 序列号的最大长度						
#define MAX_BLOCK_ROW  64  // 移动侦测遮蔽区域的最大行数
#define MAX_DDNS_TYPE     8                  // 最大的DDNS类型
#define MAX_P2P_TYPE     2			  // 最大的P2P类型
#define MAX_REC_SEG     6			  // 最大的录像段数
#define MAX_LOG_ITEM   512                  // 最大的日志条数
#define DB_OPR_ERROR  -1                   // 数据库操作失败
#define DATABASE_FILE		"/data/system.db"  // 数据库文件

	/*----------- 消息定义 -----------*/

#define MAX_MSG_SIZE        (64*1024)
#define CHANN_TYPE_ALL      0xFA
#define PARAM_MASK_ALL		0xFFFFFFFF		// 恢复整机参数


typedef enum _WEEKDAY{
	SUNDAY = 0
	,MONDAY
	,TUESDAY
	,WEDNESDAY
	,THURSDAY
	,FRIDAY
	,SATURDAY
}WEEKDAY;

typedef enum _SDK_DEV_TYPE {
	SDK_DEV_TYPE_DVR_16_3520 = 0x00	//DVR 3520 16-D1
	, SDK_DEV_TYPE_DVR_16_3531 = 0x01   //DVR 3531 16-D1(960)
	, SDK_DEV_TYPE_DVR_4_3531  = 0x02   //DVR 3531 04-HD-SDI

	, SDK_DEV_TYPE_NVR_8_3520  = 0x03   //NVR 3520 08-720P
	, SDK_DEV_TYPE_NVR_9_3531  = 0x04	//DVR 3531 09-1080P
	, SDK_DEV_TYPE_NVR_16_3531 = 0x05	//NVR 3531 16-720P
	, SDK_DEV_TYPE_NVR_25_3531 = 0x06	//NVR 3531 25-720P

	, SDK_DEV_TYPE_DECC_12_3531 = 0x07	//DEC CARD 12in, 2out
	, SDK_DEV_TYPE_DECB_12_3531	= 0x08	//DEC BOX  12in, 2out
	, SDK_DEV_TYPE_DVR_8_3531   = 0x09  //DVR 3531 08-HD-SDI
	, SDK_DEV_TYPE_NVR_32_3531  = 0x0A  //NVR 3531 32-720P

	, SDK_DEV_TYPE_NVR_4_3521   = 0x0B  //NVR 3521 04-1080P
	, SDK_DEV_TYPE_NVR_9_3521   = 0x0C  //NVR 3521 09-720P
	, SDK_DEV_TYPE_NVR_16_3521  = 0x0D  //NVR 3521 16-720P

	, SDK_DEV_TYPE_BUTT
}SDK_DEV_TYPE_E;

typedef enum _SDK_MSG_DIR{
	SDK_MSG_RESP= 0x00
	, SDK_MSG_REQ = 0x01
}SDK_MSG_DIR_E;

typedef struct sdk_msg_s {
	uint16_t magic;         //保留，扩展(如设备类型)
	uint16_t version;       //版本号
#if 1
	union {
		struct {
			uint8_t src;    //源模块ID
			uint8_t dst;    //目标模块ID
		}__attribute__((packed)) id;
		uint16_t session_id;//用户会话ID(设备端分配)
	}__attribute__((packed));
	uint16_t sequence_id;   //命令序号
#endif
#if 1
	uint16_t msg_id;        //命令ID
	uint8_t  f_dir:1;       //（SDK_MSG_REQ, SDK_MSG_RESP)
	uint8_t  f_pkg:1;       //1:extend字段用作分包;
	uint8_t  f_res:6;       //保留
	uint8_t  ack;           //错误码
#endif
	uint32_t args;          //命令参数(设置、获取参数;或者是子命令类型[用户管理、升级、磁盘管理])
	uint32_t extend;        //扩展字段(8:pkg_total,8:pkg_num, 8:, 8:)
	uint32_t chann;         //通道号(CHANN_TYPE_ALL)
	uint32_t size;          //消息长度
	uint8_t  data[0];       //消息体
	//uint8_t *data;
}sdk_msg_t;

/*----------- 数据库操作 -----------*/
/*
	  数据库的update和select操作分别使用SDK_PARAM_SET和SDK_PARAM_GET表示。

 */
typedef enum _SDK_PARAM_OPERATE{
	SDK_PARAM_SET	= 0x01	    //设置update
	, SDK_PARAM_GET        	    //查询select
	, SDK_PARAM_INSERT             // 插入insert
	, SDK_PARAM_DELETE		    // 删除delete
}SDK_PARAM_OPERATE_E;



/*----------- 错误码定义 -----------*/

typedef enum _SDK_ERR_CODE {
	SDK_ERR_SUCCESS = 0x00        //成功
	, SDK_ERR_FAILURE               //失败
	, SDK_ERR_PASS                  //密码错误
	, SDK_ERR_USER_NOT_EXIST		//用户名不存在
	, SDK_ERR_ROOT_USER				//root用户不能删除
	, SDK_ERR_TOO_MANY_USER			//用户已满(最多可添加16个用户名)
	, SDK_ERR_USER_ALREADY_EXIST	//用户名已存在
	, SDK_ERR_NET_CFG				//配置网络参数失败
	, SDK_ERR_PERMIT                //权限错误
	, SDK_ERR_CHANN                 // 10 通道号错误
	, SDK_ERR_CONN_MAX              //超出最大链接数
	, SDK_ERR_CONN                  //链接错误
	, SDK_ERR_SEND                  //数据发送错误
	, SDK_ERR_RECV                  //数据接收错误
	, SDK_ERR_BUSY                  //设备正忙
	, SDK_ERR_DATA                  //数据格式错误
	, SDK_ERR_CMD                   //命令字错误
	, SDK_ERR_VER                   //协议版本错误
	, SDK_ERR_NO_DISK               //无磁盘
	, SDK_ERR_DISK_ERR              // 20 磁盘错误
	, SDK_ERR_RESOURCE              //资源不可用
	, SDK_ERR_FLASH                 //FLASH读写错误
	, SDK_ERR_SET_SYSTIME			//设置系统时间错误
	, SDK_ERR_SET_ENCODE_PARAM		//设置编码参数错误
	, SDK_ERR_SET_MD_PARAM			//设置移动侦测参数错误
	, SDK_ERR_SET_OSD_PARAM			//设置OSD参数错误
	, SDK_ERR_SET_OVERLAY_PARAM		//设置遮挡区域参数错误
	, SDK_ERR_SET_TV_PARAM			//设置边距大小错误
	, SDK_ERR_SET_IMAGE_ATTR		//设置图像属性错误
	, SDK_ERR_LOG_QUERY				//查询日志失败
	, SDK_ERR_LOG_EMPTY				//清空日志失败
	, SDK_ERR_LOG_BACKUP			//备份日志失败
	, SDK_ERR_SERIAL_NO				//串口号错误
	, SDK_ERR_SERIAL_OPEN			//打开串口失败
	, SDK_ERR_SERIAL_PARAM_SET		//设置串口参数失败
	, SDK_ERR_DISK_MOUNT			//磁盘挂载失败
	, SDK_ERR_UMOUNT				//磁盘卸载失败
	, SDK_ERR_BACKUP				//备份失败	
	, SDK_ERR_LOG_DEL				//删除日志失败

	, SDK_ERR_GET_DISK_PART_INOF	//获取磁盘分区信息错误
	, SDK_ERR_UPGRADING				//正在升级中
	, SDK_ERR_UPGRADE_CHECK_HEAD	//校验升级包头错误
	, SDK_ERR_UPGRADE_FILE_FIND		// 查找不到升级文件
	, SDK_ERR_UPGRADE				// 升级失败

	, SDK_ERR_NVR_NOT_LOGIN				// 前端设备还没登陆成功

}SDK_ERR_CODE_E;

/*----------- 告警类型定义 -----------*/

typedef enum _SDK_EVENT_TYPE{
	SDK_EVENT_MANUAL_REC = 0x000	//手动录像事件
	, SDK_EVENT_ALARM_IN    		//输入告警事件
	, SDK_EVENT_MOTION              //移动侦测事件
	, SDK_EVENT_LOST                //视频丢失事件
	, SDK_EVENT_HIDE                //视频遮挡事件(目前不做)
	, SDK_EVENT_TIMER_REC           //定时录像事件
	, SDK_EVENT_HD_ERR              //磁盘错误事件
	, SDK_EVENT_HD_IS_EXIST			//录像盘是否存在事件 (pmsg->extend: 不存在/存在[0/1])

	/*8******* 专用于上报给其它模块的消息 *************/
	, SDK_EVENT_REC_MSG				//录像是否开启事件
	, SDK_EVENT_ALARM_IN_MSG
	, SDK_EVENT_MOTION_MSG
	, SDK_EVENT_LOST_MSG
	, SDK_EVENT_HIDE_MSG
	, SDK_EVENT_SYS_RESET           //系统复位事件, extend:重启/关机.
	/******** ***********************/

	/*13*********add 20120601 for  NVR***************/
	, SDK_DEV_LOGIN				//登录
	, SDK_DEV_TIMEOUT           //登录超时
	, SDK_DEV_LOGOUT            //注销
	, SDK_STREAM_OPEN	//正在传送
	, SDK_STREAM_CLOSE  		//连接关闭

	/************add end******************/
	, SDK_EVENT_ALL
	,SDK_CHL_RESET              //远程配置通道通知
	, SDK_DISKFORMAT		//远程硬盘格式化通知
	, SDK_UPDATE			//升级升级通知
	, SDK_EVENT_BUTT
}SDK_EVENT_TYPE_E;


/*----------- 音视频类型定义 -----------*/
typedef enum _SDK_AUDIO_SAMPLE_RATE
{
	SDK_AUDIO_SAMPLE_R8K        = 0,   /* 8K Sample rate     */
	SDK_AUDIO_SAMPLE_R11_025K   = 1,   /* 11.025K Sample rate*/
	SDK_AUDIO_SAMPLE_R16K       = 2,   /* 16K Sample rate    */
	SDK_AUDIO_SAMPLE_R22050     = 3,   /* 22.050K Sample rate*/
	SDK_AUDIO_SAMPLE_R24K       = 4,   /* 24K Sample rate    */
	SDK_AUDIO_SAMPLE_R32K       = 5,   /* 32K Sample rate    */
	SDK_AUDIO_SAMPLE_R44_1K     = 6,   /* 44.1K Sample rate  */
	SDK_AUDIO_SAMPLE_R48K       = 7,   /* 48K Sample rate    */
	SDK_AUDIO_SAMPLE_BUTT,
}SDK_AUDIO_SAMPLE_RATE_E;

typedef enum _SDK_AUDIO_CODEC_FORMAT
{
	SDK_AUDIO_FORMAT_NULL	        = 0,   /*                    */
	SDK_AUDIO_FORMAT_G711A	        = 1,   /* G.711 A            */
	SDK_AUDIO_FORMAT_G711Mu	        = 2,   /* G.711 Mu           */
	SDK_AUDIO_FORMAT_ADPCM	        = 3,   /* ADPCM              */
	SDK_AUDIO_FORMAT_G726_16        = 4,   /* G.726              */
	SDK_AUDIO_FORMAT_G726_24        = 5,   /* G.726              */
	SDK_AUDIO_FORMAT_G726_32        = 6,   /* G.726              */
	SDK_AUDIO_FORMAT_G726_40        = 7,   /* G.726              */
	SDK_AUDIO_FORMAT_AMR	        = 8,   /* AMR encoder format */
	SDK_AUDIO_FORMAT_AMRDTX	        = 9,   /* AMR encoder formant and VAD1 enable */
	SDK_AUDIO_FORMAT_AAC	        = 10,  /* AAC encoder        */
	SDK_AUDIO_FORMAT_ADPCM_DVI4	    = 11,  /* ADPCM              */
	SDK_AUDIO_FORMAT_ADPCM_IMA	    = 12,  /* ADPCM              */
	SDK_AUDIO_FORMAT_MEDIA_G726_16  = 13,  /* G.726              */
	SDK_AUDIO_FORMAT_MEDIA_G726_24  = 14,  /* G.726              */
	SDK_AUDIO_FORMAT_MEDIA_G726_32  = 15,  /* G.726              */
	SDK_AUDIO_FORMAT_MEDIA_G726_40  = 16,  /* G.726              */
	SDK_AUDIO_FORMAT_BUTT,
}SDK_AUDIO_CODEC_FORMAT_E;

typedef enum _SDK_AUDIO_SAMPLE_WIDTH {
	SDK_AUDIO_SAMPLE_WIDTH_8  = 0,    /* 8bits */
	SDK_AUDIO_SAMPLE_WIDTH_16 = 2,    /* 16bits */
	SDK_AUDIO_SAMPLE_WIDTH_BUTT,
}SDK_AUDIO_SAMPLE_WIDTH_E;


/*
 * 音频帧信息(子结构体)
 */
typedef struct sdk_a_frame_info_s {
	uint8_t encode_type;        //编码类型 1: G711A
	uint8_t samples;            //采样频率 0: 8000, 
	uint8_t bits;               //位宽     1: 16bit, 
	uint8_t channels;           //通道数
	uint8_t res[4];
}sdk_a_frame_info_t;


typedef enum _SDK_VIDEO_ENCODE_FORMAT {
	SDK_VIDEO_FORMAT_H264    = 0, //H.264
	SDK_VIDEO_FORMAT_MPEG4   = 1, //MPEG4
	SDK_VIDEO_FORMAT_MJPEG   = 2, //MJPEG
}SDK_VIDEO_ENCODE_FORMAT_E;


typedef enum _SDK_VIDEO_STANDARD {
	SDK_VIDEO_STANDARD_PAL  = 0, // PAL
	SDK_VIDEO_STANDARD_NTSC = 1, //NTSC
}SDK_VIDEO_STANDARD_E;


typedef enum _SDK_VIDEO_RESOLUTION
{
	SDK_VIDEO_RESOLUTION_QCIF    = 0 /* 176 * 144 */
	,SDK_VIDEO_RESOLUTION_CIF     = 1 /* 352 * 288 */
	,SDK_VIDEO_RESOLUTION_HD1     = 2 /* 704 * 288 */
	,SDK_VIDEO_RESOLUTION_D1      = 3 /* 704 * 576 */
	,SDK_VIDEO_RESOLUTION_960H    = 4 /* 960 * 576 */
	,SDK_VIDEO_RESOLUTION_QVGA    = 5 /* 320 * 240 */
	,SDK_VIDEO_RESOLUTION_VGA     = 6 /* 640 * 480 */
	,SDK_VIDEO_RESOLUTION_XGA     = 7 /* 1024 * 768 */
	,SDK_VIDEO_RESOLUTION_SXGA    = 8 /* 1400 * 1050 */
	,SDK_VIDEO_RESOLUTION_UXGA    = 9 /* 1600 * 1200 */
	,SDK_VIDEO_RESOLUTION_QXGA    = 10/* 2048 * 1536 */
	,SDK_VIDEO_RESOLUTION_WVGA    = 11/* 854  * 480  */
	,SDK_VIDEO_RESOLUTION_WSXGA   = 12/* 1680 * 1050 */
	,SDK_VIDEO_RESOLUTION_WUXGA   = 13/* 1920 * 1200 */
	,SDK_VIDEO_RESOLUTION_WQXGA   = 14/* 2560 * 1600 */
	,SDK_VIDEO_RESOLUTION_720p    = 15/* 1280 * 720  */
	,SDK_VIDEO_RESOLUTION_1024p   = 16/*1280 * 1024 */
	,SDK_VIDEO_RESOLUTION_1080p   = 17/* 1920 * 1080 */
	,SDK_VIDEO_RESOLUTION_960p    = 18/* 1280 * 960  */
	,SDK_VIDEO_RESOLUTION_SVGA    = 19/* 800  * 600  */
	,SDK_VIDEO_RESOLUTION_BUTT      // ----------

}SDK_VIDEO_RESOLUTION_E;


typedef enum _SDK_CODEC_AO_DEV_E
{
	SDK_CODEC_COMM_AO_DEV = 0
	, SDK_CODEC_HDMI_AO_DEV = 1
	, SDK_CODEC_AO_DEV_BUTT
}SDK_CODEC_AO_DEV_E;

typedef enum _SDK_CODEC_AO_MOD_E
{
	SDK_CODEC_PREVIEW_AO_MOD = 0
	, SDK_CODEC_PLAYBACK_AO_MOD = 1
	, SDK_CODEC_TALK_AO_MOD = 2
	, SDK_CODEC_MUTE_AO_MOD = 3
}SDK_CODEC_AO_MOD_E;



/*
 * 视频帧信息(子结构体) 
 */
typedef struct sdk_v_frame_info_s {
	uint8_t encode_type;        //编码类型 0: H.264
	uint8_t standard;           //制式     0: PAL, 1: NTSC
	uint8_t resolution;         //分辨率   SDK_VIDEO_RESOLUTION_E
	uint8_t frame_rate;         //帧率     1-25/30 
	uint16_t width;             //宽
	uint16_t height;            //高
}sdk_v_frame_info_t;



typedef enum _SDK_FRAME_TYPE {
	SDK_VIDEO_FRAME_I = 0x01,   //I帧
	SDK_VIDEO_FRAME_P = 0x02,   //P帧
	SDK_VIDEO_FRAME_B = 0x03,   //B帧
	SDK_AUDIO_FRAME_A = 0x04,   //音频帧A
}SDK_FRAME_TYPE_E;

/*
 * 音视频帧结构体定义
 */
typedef struct sdk_frame_s {
	uint16_t  magic;            //保留，扩展 
	uint8_t   res[1];           //保留
	uint8_t   frame_type;       //帧类型, 视频（I, P, B）, 音频（A）
	uint32_t  frame_size;       //帧长度
	uint32_t  frame_no;         //帧序号
	uint32_t  sec;              //帧时间（秒）
	uint32_t  usec;             //帧时间（微秒）
	uint64_t  pts;              //帧PTS
	union
	{
		sdk_v_frame_info_t video_info;//视频帧信息
		sdk_a_frame_info_t audio_info;//音频帧信息
	};
	uint8_t   data[0];          //帧数据
}__attribute__ ((packed))sdk_frame_t;
/*
* nal结构体
*/
typedef struct sdk_nal_desc_s {
	uint32_t nal_num;           /*NAL个数*/
	struct nal_d_s {
		uint32_t nal_off;       /*NAL地址*/
		uint32_t nal_size;      /*NAL大小*/
	}nal[0];
}sdk_nal_desc_t;


/*----------- 配置参数结构定义 -----------*/


/*
 * 时间结构体定义
 */
typedef struct sdk_time_s{
	uint32_t year;
	uint32_t mon;
	uint32_t day;
	uint32_t hour;
	uint32_t min;
	uint32_t sec;
}sdk_time_t;

/*
 * 详细时间结构体定义
 */
typedef struct sdk_date_s{
	sdk_time_t _time;
	uint8_t     wday;
	uint8_t     tzone;
	uint8_t     res[2];
}sdk_date_t;



/*
* 区域结构体定义
 */
typedef struct sdk_rect_s{
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
}sdk_rect_t;

/*
 * 点结构体定义
 */
typedef struct sdk_point_s{
	uint16_t x;
	uint16_t y;
}sdk_point_t;

/*
 * 时间段结构体定义
 */
typedef struct sdk_sched_time_s{
	uint8_t enable;	    //激活, 当定时录像时表示录像类型;
	uint8_t record_type;  /* bit0: Record    bit1: Motion */
	uint8_t res[2];
	uint8_t start_hour; //开始时间
	uint8_t start_min;
	uint8_t stop_hour;  //结束时间
	uint8_t stop_min;
}sdk_sched_time_t;

/*
 * 网络升级流程： SDK_UPGRAD_REQUEST => recv upg_pkg => SDK_UPGRAD_DISK_FILE => SDK_UPGRAD_PROGRESS;
 * GUI升级流程 :  SDK_UPGRAD_FIND_FILE => SDK_UPGRAD_DISK_FILE => SDK_UPGRAD_PROGRESS;
*/
typedef enum _SDK_UPGRAD_OP {
	SDK_UPGRAD_REQUEST   = 0x001  //升级请求				upgrade_packet_t
	, SDK_UPGRAD_DISK_FILE          //升级包在磁盘文件
	, SDK_UPGRAD_MEM_FILE           //升级包在内存
	, SDK_UPGRAD_PROGRESS           //升级进度
	, SDK_UPGRAD_FIND_FILE          //GUI查询升级包文件列表	upgrad_pkg_file_t
}SDK_UPGRAD_OP_E;


#define MAX_UPGRADE_DEVNAME_LEN		16
#define MAX_UPGRADE_VERNAME_LEN		16
#define MAX_UPGRADE_DATENAME_LEN	16
#define MAX_UPGRADE_FILENAME_LEN	64
#define MAX_UPGRADE_FILEPATH_LEN	128

/*
* GUI查询升级包文件列表
*/
typedef struct upgrad_pkg_file_s {
	uint8_t   device_type[MAX_UPGRADE_DEVNAME_LEN];//设备类型
	uint8_t   ver[MAX_UPGRADE_VERNAME_LEN];        //版本
	uint8_t   date[MAX_UPGRADE_DATENAME_LEN];       //日期
	uint32_t  size;           //大小
	uint8_t   name[MAX_UPGRADE_FILENAME_LEN];       //名字 aa.upg
	uint8_t   file_path[MAX_UPGRADE_FILEPATH_LEN];  //升级文件全路经 /tmp/aa.upg
}upgrad_pkg_file_t;

#if 0
	//分别对应数据库的操作SDK_PARAM_OPERATE_E中的枚举值
	typedef enum _SDK_USER_OP {
		SDK_USER_MODI=0x01         //update
			, SDK_USER_QUERY       //select
			, SDK_USER_ADD          //insert
			, SDK_USER_DEL          //delete
	}SDK_USER_OP_E;
#endif

typedef enum _SDK_USER_OP {
	SDK_USER_AAA = 0x001  //sdk_user_t
	, SDK_USER_ADD          //sdk_user_right_t
	, SDK_USER_DEL          //sdk_user_t
	, SDK_USER_MODI         //sdk_user_right_t
	, SDK_USER_QUERY        //sdk_user_t, sdk_user_right_t
}SDK_USER_OP_E;

typedef enum _SDK_DISK_OP {
	SDK_DISK_QUERY		= 0x001		//sdk_disk_t
	, SDK_DISK_FORMAT
	, SDK_DISK_PROGRESS
	, SDK_DISK_UMOUNT					//卸载U盘 chann字段表示"盘号"
}SDK_DISK_OP_E;


/*
* 用户定义(用户名＋密码)
 */
typedef struct sdk_user_s {
	uint8_t user_name[MAX_USER_NAME_LEN];
	uint8_t user_pwd[MAX_PASSWD_LEN];
}sdk_user_t;


/*
 * 用户权限定义
*/
typedef struct sdk_user_right_s {

	sdk_user_t user;   
	uint32_t local_right;           //本地GUI权限
	/*bit.0: 本地控制云台*/
	/*bit.1: 本地手动录象*/
	/*bit.2: 本地回放*/
	/*bit.3: 本地设置参数*/
	/*bit.4: 本地查看状态、日志*/
	/*bit.5: 本地高级操作(升级，格式化，重启，关机)*/
	/*bit.6: 本地查看参数 */
	/*bit.7: 本地管理模拟和IP camera */
	/*bit.8: 本地备份 */
	/*bit.9: 本地关机/重启 */

	uint32_t remote_right;          //远程权限
	/*bit.0: 远程控制云台*/
	/*bit.1: 远程手动录象*/
	/*bit.2: 远程回放 */
	/*bit.3: 远程设置参数*/
	/*bit.4: 远程查看状态、日志*/
	/*bit.5: 远程高级操作(升级，格式化，重启，关机)*/
	/*bit.6: 远程发起语音对讲*/
	/*bit.7: 远程预览*/
	/*bit.8: 远程请求报警上传、报警输出*/
	/*bit.9: 远程控制，本地输出*/
	/*bit.10: 远程控制串口*/	
	/*bit.11: 远程查看参数 */
	/*bit.12: 远程管理模拟和IP camera */
	/*bit.13: 远程关机/重启 */

	uint8_t local_backup_right[MAX_CHANN_NUM];     //通道权限
	uint8_t net_preview_right[MAX_CHANN_NUM];

}sdk_user_right_t;


/*
 * 网络链接索引定义
 */
typedef enum _SDK_NET_IDX {
	SDK_NET_IDX_ETH0      = 0x00 //eth0   sdk_eth_cfg_t
		, SDK_NET_IDX_ETH1		= 0x01 //eth1   sdk_eth_cfg_t
		, SDK_NET_IDX_PPPOE0    = 0x02 //pppoe  sdk_pppoe_t
		, SDK_NET_IDX_WIFI0  	= 0x03 //wifi   sdk_wifi_cfg_t
		, SDK_NET_IDX_3G0		= 0x04 //3g     sdk_3g_cfg_t
		, SDK_NET_IDX_PPPOE1    = 0x05
		, SDK_NET_IDX_WIFI1     = 0x06
		, SDK_NET_IDX_3G1       = 0x07
		, SDK_NET_IDX_BUTT      = 0x08
}SDK_NET_IDX_E;

/*
 * 网络配置（子结构体）
 */
typedef struct sdk_ip_info_s {
	uint8_t if_name[MAX_IF_NAME_LEN];//接口名称(保留)
	uint8_t ip_addr[MAX_IP_ADDR_LEN];//IP ADDR
	uint8_t mask[MAX_IP_ADDR_LEN];   //IP MASK
	uint8_t gateway[MAX_IP_ADDR_LEN];//网关
	uint8_t mac[MAX_MAC_ADDR_LEN];   //MAC STRING
	uint8_t dns1[MAX_IP_ADDR_LEN];   //DNS1
	uint8_t dns2[MAX_IP_ADDR_LEN];   //DNS2
	uint8_t enable_dhcp;             //是否开启DHCP
	uint8_t dns_auto_en;             //自动获取DNS(当开启DHCP时可自动获取DNS)
	uint8_t enable_p2p;
	uint8_t res[1];
}sdk_ip_info_t;

/*
 * 有线网口配置
 */
typedef struct sdk_eth_cfg_s {

	sdk_ip_info_t ip_info; //IP信息配置

	uint8_t if_mode;       //网卡模式
	uint8_t res[3];
}sdk_eth_cfg_t;


/*
 * pppoe配置
 */
typedef struct sdk_pppoe_s {

	sdk_ip_info_t ip_info;              //PPPOE 获取到的IP地址

	uint8_t user[MAX_USER_NAME_LEN];    //PPPOE 用户名
	uint8_t pass[MAX_PASSWD_LEN];       //PPPOE 密码
	uint8_t enable;                    	//是否开启PPPOE
	uint8_t if_no;                      //网络接口
	uint8_t res[2];                     //保留

}sdk_pppoe_t;


/*
 * wifi 配置
 */
typedef struct sdk_wifi_cfg_s {

	sdk_ip_info_t ip_info;

	uint8_t essid[MAX_DEV_ID_LEN];		//服务区别号
	uint8_t pwd[MAX_PASSWD_LEN];		//密码
	uint32_t enable;				    //0：禁止，1：wifi client 2: wifi ap
	uint32_t encrypt_type;				//加密类型0:NONE  1:WPA  2:WPA2 3:WEP
	uint32_t auth_mode;					//认证方式0:NONE  1:EAP 2:PSK 3:OPEN 4:SHARED 
	uint32_t secret_key_type;			//密钥管理方式 0:NONE  1:AES 2:TKIP 只对应于加密类型为WPA/WPA2的情况

	uint32_t stat;					    //无线网卡连接状态 0 无连接 1 正在连接2 成功
}sdk_wifi_cfg_t;

/*
 * 3g 配置
 */
typedef struct sdk_3g_cfg_s {

	sdk_ip_info_t   ip_info;            //3g PPP 获取到的IP地址

	uint32_t enable;
	uint32_t mod_id;
	uint32_t stat;
	uint8_t  res[64];
}sdk_3g_cfg_t;


/*
 * 网络配置
 */
typedef struct sdk_net_mng_cfg_s {

	uint8_t        def_if_no;                     //默认网络接口(当设备有多个网络链接时优先使用指定的链接)
	uint8_t        res[3];

	uint8_t        multicast[MAX_IP_ADDR_LEN];    //组播地址
	uint8_t        net_manager[MAX_IP_ADDR_LEN];  //管理服务器IP

	uint16_t       net_manager_port;              //管理服务器PORT
	uint16_t       http_port;                     //DVR HTTP PORT
	uint16_t       dvr_cmd_port;                  //DVR SERVICE PROT
	uint16_t       dvr_data_port;                 //保留

}sdk_net_mng_cfg_t;

/*
 * upnp 配置
 */
typedef struct sdk_upnp_s{
	uint8_t enable;			//是否开启UPnP
	uint8_t net_type;		//网卡: SDK_NET_IF_E
	uint8_t en_auto;		//使用模式,0:固定模式,1:自动模式
	uint8_t res;            //保留

	uint32_t time;			//更新时间
	uint8_t server_ip[MAX_IP_ADDR_LEN];//服务器ip

	uint16_t cmd_port;		//命令端口:jxj的是3321
	uint16_t data_port;	    //数据端口:jxj的是7554
	uint16_t web_port;		//web端口:jxj的是80
	uint16_t res_port;      //保留
	uint16_t cmd_stat;		//命令端口映射的状态,成功:映射后的外部端口号, 失败:0
	uint16_t data_stat;	    //数据端口映射的状态,成功:映射后的外部端口号, 失败:0
	uint16_t web_stat;		//web端口映射的状态,成功:映射后的外部端口号,  失败:0
	uint16_t res_stat;      //保留

}sdk_upnp_t;

/*
 * DVR常规配置参数
 */

typedef struct sdk_comm_cfg_s
{
	uint8_t 	dvr_name[MAX_DVR_NAME_LEN]; //DVR 名字
	uint32_t 	dvr_id;                    	//DVR ID,(遥控器)
	uint32_t 	recycle_record;            	//是否循环录像,0:不是; 1:是
	uint8_t 	language;					//语言0: 中文  1 :英语
	uint8_t  	video_mode;					//视频制式0:PAL  1 NTSC
	uint8_t  	out_device;					//输出设备  0: CVBS; 1: VGA; 2: HDMI
	uint8_t  	resolution; 				//(主屏)分辨率0 :1024 * 768    1 : 1280 * 720   2: 1280*1024  3: 1920*1080
	uint8_t 	standby_time; 				//菜单待机时间 1~60分钟  0: 表示不待机
	uint8_t  	boot_guide;					//开机向导 0 : 不启动向导  1 : 启动向导
	uint8_t  	resolution2; 				//(副屏)分辨率0 :1024 * 768    1 : 1280 * 720   2: 1280*1024  3: 1920*1080
	uint8_t  	wizardEnable;         // 开机向导
	uint8_t  	transparent;           //透明度
} sdk_comm_cfg_t;


/* 
 *
 * DVR系统配置参数
 */

typedef struct sdk_sys_cfg_s {

	/*---------- 以下信息不可更改 ----------*/
	uint8_t serial_no[MAX_SERIAL_NO_LEN];       //序列号
	uint8_t device_type[MAX_VER_STR_LEN];       //设备型号
	uint8_t software_ver[MAX_VER_STR_LEN];      //软件版本号
	uint8_t software_date[MAX_VER_STR_LEN];     //软件生成日期
	uint8_t panel_ver[MAX_VER_STR_LEN];         //前面板版本
	uint8_t hardware_ver[MAX_VER_STR_LEN];      //硬件版本

	uint32_t dev_type;                          //设备类型SDK_DEV_TYPE_E    J_DevType_E from j_sdk.h

	uint8_t ana_chan_num;                       //模拟通道个数
	uint8_t ip_chan_num;                        //数字通道数
	uint8_t dec_chan_num;                       //解码路数
	uint8_t stream_num;                         //保留

	uint8_t audio_in_num;                       //语音输入口的个数
	uint8_t audio_out_num;                      //语音输出口的个数
	uint8_t alarm_in_num;                       //报警输入个数
	uint8_t alarm_out_num;                      //报警输出个数

	uint8_t net_port_num;                       //网络口个数
	uint8_t rs232_num;                          //232串口个数
	uint8_t rs485_num;                          //485串口个数
	uint8_t usb_num;                            //USB口的个数

	uint8_t hdmi_num;                           //HDMI口的个数
	uint8_t vga_num;                            //VGA口的个数
	uint8_t cvbs_num;                           //cvbs口的个数
	uint8_t aux_out_num;                        //辅口的个数

	uint8_t disk_ctrl_num;                      //硬盘控制器个数
	uint8_t disk_num;                           //硬盘个数
	uint8_t res2[2];                            //保留

	uint32_t max_encode_res;                    //最大编码分辨率
	uint32_t max_display_res;                   //最大显示分辨率

	uint32_t dvr_bit;                           //DVR保留
	uint8_t  dvr_byte[4];                       //DVR保留
}sdk_sys_cfg_t;

/*
 * 平台信息 子结构体
 *
 */

typedef struct sdk_screen_slot_s {
	uint32_t magic;				/* 0x55aa55aa */
	uint32_t index[MAX_SCREEN_NUM];
}sdk_screen_slot_t;

/*
 * 平台信息
 *
 */

typedef struct sdk_platform_s {
	uint8_t pu_id[MAX_USER_NAME_LEN];           
	uint8_t cms_ip[MAX_IP_ADDR_LEN];            //平台mds ip
	uint8_t mds_ip[MAX_IP_ADDR_LEN];            //平台mds ip
	uint32_t cms_port;                          //平台cms端口号
	uint32_t mds_port;                          //平台mds端口号
	uint32_t protocol;                          //0:tcp   1:udp
	uint32_t plat_enable;                       //平台启用

	sdk_screen_slot_t slot;						//解码器屏号
}sdk_platform_t;

/*
 * 
 * 系统版本信息
 */

typedef struct sdk_version_s {
	uint8_t serial_no[MAX_SERIAL_NO_LEN];       /* 产品序列号 */
	uint8_t device_type[MAX_VER_STR_LEN];       /* 产品型号 */
	uint8_t software_ver[MAX_VER_STR_LEN];	    /* 版本号 */
	uint8_t software_date[MAX_VER_STR_LEN];	    /* 版本日期 */
	uint8_t panel_ver[MAX_VER_STR_LEN];         //前面板版本
	uint8_t hardware_ver[MAX_VER_STR_LEN];      //硬件版本
}sdk_version_t;


/*
 * 恢复默认参数
 */

typedef enum _SDK_PARAM_MASK_ID {
	SDK_PARAM_MASK_ENCODE = 0x00    	//bit.0: 编码参数
		, SDK_PARAM_MASK_PTZ             	//bit.1: PTZ云台参数
		, SDK_PARAM_MASK_RECORD      		//bit.2: 录像参数
		, SDK_PARAM_MASK_NET             	//bit.3: 网络参数
		, SDK_PARAM_MASK_CHANNEL    		//bit.4: 通道参数
		, SDK_PARAM_MASK_IMAGE_ATTR			//bit.5: 视频属性
		, SDK_PARAM_MASK_SERIAL				//bit.6: 串口参数
		, SDK_PARAM_MASK_OVERLAY			//bit.7: 遮挡区域参数
		, SDK_PARAM_MASK_MOTION				//bit.8: 移动侦测
		, SDK_PARAM_MASK_COMM				//bit.9: 常规参数
		, SDK_PARAM_MASK_OSD				//bit.10: osd参数
		, SDK_PARAM_MASK_USER				//bit.11: 用户管理
		, SDK_PARAM_MASK_ALARM_IN			//bit.12: 外部报警输入参数
		, SDK_PARAM_MASK_HIDE				//bit.13: 视频遮挡侦测参数
		, SDK_PARAM_MASK_VIDEO_LOST			//bit.14: 视频丢失参数
		, SDK_PARAM_MASK_TV					//bit.15: 边距调节参数
		, SDK_PARAM_MASK_PREVIEW			//bit.16: 巡回参数
		, SDK_PARAM_MASK_TOUR               //bit.17: 巡回参数
		, SDK_PARAM_MASK_EMAIL			  	//bit.18
		, SDK_PARAM_MASK_DDNS			  	//bit.19
		,SDK_PARAM_MASK_DISPLAY                     //bit 20
		, SDK_PARAM_MASK_BUIT
}SDK_PARAM_MASK_ID_E;


typedef struct sdk_default_param {

	uint32_t param_mask;    //参数内容掩码 SDK_PARAM_MASK_ID_E
	uint32_t res[1];
}sdk_default_param_t;


/*
 * 系统运行状态信息
 */
typedef enum _SDK_STATUS_OP {
	SDK_STATUS_DEVICE = 0x01  //设备运行状态
		, SDK_STATUS_DISK           //磁盘状态
		, SDK_STATUS_CHANN          //通道状态
		, SDK_STATUS_ALARM_IN       //输入报警状态
		, SDK_STATUS_ALARM_OUT      //输出报警状态
}SDK_STATUS_OP_E;

typedef struct sdk_chan_status_s {

	uint8_t chan_type;          //通道类型 0：本地，1：IP
	uint8_t record_state;       //录像状态,0-不录像,1-录像
	uint8_t signal_state;       //信号状态, 0:正常，1：信号丢失
	uint8_t encode_state;       //编码状态, 0:不编码，1：编码

	uint32_t bit_rate;          //实际码率
	uint32_t conn_num;          //通道连接数
	uint8_t  conn_ip[MAX_CHANN_CONN_NUM][MAX_IP_ADDR_LEN];//连接IP
}sdk_chan_status_t;


typedef struct sdk_disk_status_s {
	uint32_t volume;        //硬盘的容量
	uint32_t free_space;    //硬盘的剩余空间
	uint32_t disk_state;   //硬盘的状态,0-活动,1-休眠,2-不正常
}sdk_disk_status_t;

/*
 * 系统状态(后续拆分成 SDK_STATUS_OP_E)
 */

typedef struct sdk_status_s { 

	uint32_t            device_status;                      //设备状态，0-正常,1-CPU占用率太高,超过85%,2-硬件错误
	sdk_disk_status_t   disk_status[MAX_DISK_NUM];          //硬盘状态
	sdk_chan_status_t   chan_status[MAX_CHANN_NUM];         //通道状态
	uint8_t             alarm_in_status[MAX_ALARM_IN_NUM];  //报警端口的状态,0-没有报警,1-有报警
	uint8_t             alarm_out_status[MAX_ALARM_OUT_NUM];//报警输出端口的状态,0-没有输出,1-有报警输出

	uint8_t             local_display;                      //本地显示状态, 0:正常，1：不正常
	uint8_t             audio_chan_status;                  //语音通道的状态 0-未使用，1-使用中, 0xff无效
	uint8_t             res[2];

}sdk_status_t;




/*
 * 日志
 */
// 日志操作选项,分别对应数据库的操作SDK_PARAM_OPERATE_E中的枚举值
typedef enum _SDK_LOG_OP {
	SDK_LOG_QUERY=0x02,       // select
	SDK_LOG_INSERT, 		 //insert
	SDK_LOG_CLEAR,                //delete
}SDK_LOG_OP_E;


// 日志查询操作
typedef enum _SDK_LOG_QUERY {
	SDK_LOG_QUERY_NUM = 0x00, // 查询某种日志类型中日志的总数
	SDK_LOG_QUERY_CONTENT     // 查询日志的内容
}SDK_LOG_QUERY_E;


// 日志主类型和子类型枚举
// 日志主类型，logId的高8位
typedef enum _SDK_LOG_MAIN_TYPE
{
	LOG_QUERY_ALL=0,      // 全部日志
	LOG_SYS_MANAGE = 0x01, // 系统管理
	LOG_USER_MANAGE,          // 用户管理
	LOG_PARAMS_SETUP,        // 参数配置
	LOG_FILE_OPERATE,          // 文件操作
	LOG_STATUS_WARNING,    // 状态提示
	LOG_ALARM_EVENT            // 告警事件
}SDK_LOG_MAIN_TYPE_E;

//以下是日志子类型的枚举，logId的低8位
//  子类型枚举:  "用户管理"
typedef enum _SDK_LOG_USER_MANAGER_TYPE
{
	LOG_USER_LOGIN = 0x01,    //用户登录
	LOG_USER_LOGOUT,            // 用户退出
	LOG_USER_ADD,                  // 添加用户
	LOG_USER_DEL,                  // 删除用户
	LOG_USER_MODIFY,           // 修改用户
	LOG_USER_ADD_GROUP,  //增加用户组
	LOG_USER_DEL_GROUP,   // 删除用户组
	LOG_USER_MODIFY_GROUP  // 修改用户组
}SDK_LOG_USER_MANAGER_TYPE_E;


//  子类型枚举:  "系统管理"
typedef enum _SDK_LOG_SYS_OPT_TYPE
{
	LOG_SYS_START = 0x01,  // 开机
	LOG_SYS_SHUTDOWN,     // 关机
	LOG_SYS_REBOOT,          // 重启
	LOG_SYS_UPGRADE,       // 升级
	LOG_SYS_DEFAULT       // 恢复出厂设置
}SDK_LOG_SYS_OPT_TYPE_E;

/* 子类型枚举:  参数配置 */
typedef enum _SDK_LOG_PARAM_SETUP_TYPE
{
	LOG_COMMON_SETUP = 0x01, 	//配置普通设置参数 
	LOG_COMMON_DEFAULT,	//常规参数恢复为默认值
	LOG_ENCODE_SETUP,		//配置编码参数
	LOG_ENCODE_DEFAULT,	//编码参数恢复为默认值
	LOG_RECODE_SETUP,		//配置录像参数
	LOG_RECODE_DEFAULT,	//录像参数恢复为默认值
	LOG_NETWORK_SETUP,	//配置网络参数
	LOG_NETWORK_DEFAULT,//网络参数恢复为默认值
	LOG_ALARM_SETUP,		//配置报警参数
	LOG_ALARM_DEFAULT,	//报警参数恢复为默认值
	LOG_MOTION_SETUP,		//配置移动侦测参数
	LOG_MOTION_DEFAULT,	//移动侦测参数恢复为默认值
	LOG_PTZ_SETUP,			//配置巡航参数
	LOG_PTZ_DEFAULT,		//巡航参数恢复为默认值
	LOG_OUTMODE_SETUP,	//配置输出模式参数

	LOG_OUTMODE_DEFAULT,//输出模式参数恢复为默认值
	LOG_CHN_SETUP,		//通道配置
	LOG_CHN_DEFAULT,	       //通道配置恢复为默认值
	LOG_IMAGE_COLOR_SETUP,		//配置图像颜色参数
	LOG_IMAGE_COLOR_DEFAULT,		//图像颜色恢复为默认值
	LOG_PREVIEW_SETUP,			//配置预览参数
	LOG_PREVIEW_DEFAULT,			//预览参数恢复为默认值
	LOG_USER_PARAM_DEFAULT,		//用户参数恢复为默认值
	LOG_SENSOR_SETUP,				//配置传感器参数
	LOG_SENSOR_DEFAULT,			//传感器参数恢复为默认值
	LOG_DDNS_SETUP,				//配置DDNS参数
	LOG_SMTP_SETUP,				//配置SMTP参数
	LOG_CLOUD_SETUP,				//配置云端参数
	LOG_UPNP_SETUP,				//配置UPNP参数
	LOG_SERIAL_DEFAULT,                   // 串口参数恢复为默认值
	LOG_OVERLAY_DEFAULT,                 // 区域遮蔽参数恢复为默认值
}SDK_LOG_PARAM_SETUP_TYPE_E;

/* 子类型枚举: 文件操作 */
typedef enum _SDK_LOG_FILE_OPT_TYPE
{
	LOG_FILE_UPLOAD = 0x01,	//上传本地文件到云端
	LOG_FILE_DOWNLOAD,		//下载云端文件到本地
	LOG_FORMAT_DISK,			//格式化硬盘<N>
	LOG_READ_WRITE_DISK,		//修改硬盘<N>为读写硬盘
	LOG_READ_ONLY_DISK,		//修改硬盘<N>为只读硬盘
	LOG_RECODE_BACKUP,		//录像备份
}SDK_LOG_FILE_OPT_TYPE_E;


/* 子类型枚举: 状态提示 */
typedef enum _SDK_LOG_STATUS_WARNING_TYPE
{
	LOG_STATUS_SYNC_NTP = 0x01,			//同步网络时间服务器（NTP）
	LOG_STATUS_AUTO_MAINTAIN_ON,		//开启系统自动维护
	LOG_STATUS_AUTO_MAINTAIN_OFF,	//关闭系统自动维护	
	LOG_STATUS_AUTO_RECODE_ON,		//开启录像<自动>
	LOG_STATUS_AUTO_RECODE_OFF,		//关闭录像<自动>
	LOG_STATUS_MANUAL_RECODE_ON,		//开启录像<手动>
	LOG_STATUS_MANUAL_RECODE_OFF,	//关闭录像<手动>
	LOG_STATUS_RECODE_ALLDAY_ON,		//开启全天录像
	LOG_STATUS_RECODE_ALLDAY_OFF,		//关闭全天录像
	LOG_STATUS_DHCP_ON,				//开启DHCP，自动获取网络参数
	LOG_STATUS_NETWORK_MANUAL_SETUP,	//手动设置网络参数	
	LOG_STATUS_MOTION_DETECT_ON,		//开启移动侦测
	LOG_STATUS_MOTION_DETECT_OFF,		//关闭移动侦测
	LOG_STATUS_LINK_BUZZER_ON,		//联动蜂鸣器
	LOG_STATUS_LINK_BUZZER_OFF,		//取消联动蜂鸣器

	LOG_STATUS_LINK_RECODE_CH_ON,		//联动录像通道	
	LOG_STATUS_LINK_RECODE_CH_OFF,	//取消联动录像通道
	LOG_STATUS_SENSOR_WARNING_ON,	//开启传感器告警
	LOG_STATUS_SENSOR_WARNING_OFF,	//关闭传感器告警
	LOG_STATUS_PTZ_CHN_ON,				//开启通道巡航
	LOG_STATUS_PTZ_CHN_OFF,			//关闭通道巡航
	LOG_STATUS_ADD_PRESET,				//添加巡航点
	LOG_STATUS_DEL_PRESET,				//删除巡航点
	LOG_STATUS_AUTO_SCAN_ON,			//开启自动扫描
	LOG_STATUS_AUTO_SCAN_OFF,			//关闭自动扫描
	LOG_STATUS_DDNS_ON,				//开启DDNS
	LOG_STATUS_UPNP_ON,				//开启UPNP
	LOG_STATUS_UPNP_OFF,				//开启UPNP	

	LOG_STATUS_P2P_ON,					//开启P2P
	LOG_STATUS_P2P_OFF,					//关闭P2P
	LOG_STATUS_SMTP_ON,				//开启SMTP	
	LOG_STATUS_SMTP_OFF,				//关闭SMTP	
	LOG_STATUS_AUDIO_ON,					//开启声音	
	LOG_STATUS_MUTE     						//静音
}SDK_LOG_STATUS_WARNING_TYPE_E;

/* 子类型枚举: 告警事件 */
typedef enum _SDK_LOG_WARNING_EVENT_TYPE
{
	LOG_WARNING_NO_DISK = 0x01,     			//无硬盘
	LOG_WARNING_DISK_FORMAT_FAIL,			//硬盘<N>格式化失败
	LOG_WARNING_DISK_FORMAT_SUCC,			//硬盘<N>格式化成功
	LOG_WARNING_DISK_ABNORMAL,				//硬盘异常
	LOG_WARNING_DISK_NO_SPACE,				//硬盘空间不足
	LOG_WARNING_ENCODE_PARAM_GET_FAIL,	//编码参数获取失败
	LOG_WARNING_ENCODE_PARAM_SET_FAIL,	//编码参数保存失败	
	LOG_WARNING_ENCODE_PARAM_ABNORMAL,	//编码参数异常
	LOG_WARNING_CLEAR_LOG,					//清除日志
	LOG_WARNING_RECODE_ABNORMAL,			//录像文件异常	
	LOG_WARNING_NETWORK_ABNORMAL,			//网络连接断开
	LOG_WARNING_NETWORK_OK,					//网络连接正常
	LOG_WARNING_IP_CONFLICT,					//IP地址冲突
	LOG_WARNING_MAC_CONFLICT,				//MAC地址冲突
	LOG_WARNING_DDNS_CONNECT_FAIL,			//DDNS连接失败

	LOG_WARNING_DDNS_CONNECT_SUCC,			//DDNS连接成功
	LOG_WARNING_NTP_SYNC_FAIL,				//NTP网络时间更新失败
	LOG_WARNING_NTP_SYNC_SUCC,				//NTP网络时间更新成功
	LOG_WARNING_UPNP_FAIL,					//UPNP端口映射失败
	LOG_WARNING_UPNP_SUCC,					//UPNP端口映射成功
	LOG_WARNING_EMAIL_SEND_FAIL,			//邮件发送失败
	LOG_WARNING_EMAIL_SEND_SUCC,			//邮件发送成功
	LOG_WARNING_P2P_LINK_ABNORMAL,			//P2P连接异常
	LOG_WARNING_P2P_LINK_OK,					//P2P连接正常
	LOG_WARNING_UPLOAD_FAIL,				//本地文件上传到云端失败
	LOG_WARNING_UPLOAD_SUCC,				//本地文件上传到云端成功
	LOG_WARNING_DOWNLOAD_FAIL,				//云端文件下载到本地失败
	LOG_WARNING_DOWNLOAD_SUCC,				//云端文件下载到本地成功
	LOG_WARNING_SENSOR,						//传感器告警
	LOG_WARNING_VEDIO_LOSS,					//视频丢失告警

	LOG_WARNING_BACKUP_FAIL,					//文件备份失败
	LOG_WARNING_NO_REMOTE_DEVICE,			//未发现远程设备
	LOG_WARNING_NO_PTZ_DEVICE				//无云台设备
}SDK_LOG_WARNING_EVENT_TYPE_E;
/*日志枚举结束*/

/*
 * 系统日志子类型
 */
typedef enum _L_SYSTEM_MINOR_TYPE {
	L_SYSTEM_MINOR_STARTUP = 0x0001   //开机
		, L_SYSTEM_MINOR_SHUTDOWN           //关机
		, L_SYSTEM_MINOR_REBOOT				//重启

		, L_SYSTEM_MINOR_ALL     = 0xffff   //所有系统日志
}L_SYSTEM_MINOR_TYPE_E;

/*
 * 告警日志子类型
 */
typedef enum _L_ALARM_MINOR_TYPE {
	L_ALARM_MINOR_DI_START = 0x0001   //输入告警发生
		, L_ALARM_MINOR_DI_STOP             //输入告警停止
		, L_ALARM_MINOR_MD_START            //移动侦测告警发生
		, L_ALARM_MINOR_MD_STOP             //移动侦测告警停止
		, L_ALARM_MINOR_VL_START			//视频丢失告警发生
		, L_ALARM_MINOR_VL_STOP				//视频丢失告警停止

		, L_ALARM_MINOR_ALL		= 0xffff	//所有告警日志
}L_ALARM_MINOR_TYPE_E;

/*
 * 操作日志子类型
 */
typedef enum _L_OPERATE_MINOR_TYPE {
	L_OPERATE_MINOR_LOGIN = 0x0001	// 登陆	
		, L_OPERATE_MINOR_LOGOUT 			// 注销
		, L_OPERATE_MINOR_USER_ADD			// 用户管理-增加
		, L_OPERATE_MINOR_USER_DEL			// 用户管理-删除
		, L_OPERATE_MINOR_USER_MODI			// 用户管理-修改
		, L_OPERATE_MINOR_SETTIME 			// 设置系统时间
		, L_OPERATE_MINOR_FORMAT_DISK 		// 格式化硬盘
		, L_OPERATE_MINOR_DEFAULT 			// 恢复默认
		, L_OPERATE_MINOR_UPGRADE 			// 升级
		, L_OPERATE_MINOR_PLAYBACK 			// 回放
		, L_OPERATE_MINOR_PTZ 				// 云台控制
		, L_OPERATE_MINOR_BACKUP 			// 备份
		, L_OPERATE_MINOR_RECORD_START 		// 启动录像
		, L_OPERATE_MINOR_RECORD_STOP 		// 停止录像
		, L_OPERATE_MINOR_CLR_ALARM 		// 清除报警
		, L_OPERATE_MINOR_TALKBACK_START 	// 对讲开始
		, L_OPERATE_MINOR_TALKBACK_STOP 	// 对讲结束
		, L_OPERATE_MINOR_LOG_DEL			// 删除日志
		, L_OPERATE_MINOR_LOG_EMPTY			// 清空日志
		, L_OPERATE_MINOR_LOG_BACKUP		// 备份日志
		, L_OPERATE_MINOR_MANUAL_RECORD_START 		// 手动启动录像
		, L_OPERATE_MINOR_MANUAL_RECORD_STOP 		// 手动停止录像	
		, L_OPERATE_MINOR_FORMAT_DISK_U 	// 格式化U盘

		, L_OPERATE_MINOR_ALL	= 0xffff	// 所有操作日志
}L_OPERATE_TYPE_E;

/*
 * 参数日志子类型
 */
typedef enum _L_PARAM_MINOR_TYPE {
	L_PARAM_MINOR_PARAM_NETWORK = 0x0001 	// 网络参数配置
		, L_PARAM_MINOR_PARAM_UART 				// 串口参数配置
		, L_PARAM_MINOR_PARAM_PTZ 				// 云台参数配置
		, L_PARAM_MINOR_PARAM_CHAN 				// 通道参数配置
		, L_PARAM_MINOR_PARAM_VENC 				// 编码参数配置
		, L_PARAM_MINOR_PARAM_TV 				// TV参数配置
		, L_PARAM_MINOR_PARAM_PIC_ATTR			// 图像属性配置
		, L_PARAM_MINOR_PARAM_RECORD			// 录像参数配置
		, L_PARAM_MINOR_PARAM_MOTION			// 移动侦测配置
		, L_PARAM_MINOR_PARAM_VIDEOLOST			// 视频丢失配置
		, L_PARAM_MINOR_PARAM_ALARMIN			// 输入告警配置
		, L_PARAM_MINOR_PARAM_POLL				// 轮巡配置
		, L_PARAM_MINOR_PARAM_PREVIEW			// 预览配置
		, L_PARAM_MINOR_PARAM_OVERLAY			// 遮挡区域配置
		, L_PARAM_MINOR_PARAM_OSD				// OSD配置
		, L_PARAM_MINOR_PARAM_COMM				// 常规配置

		, L_PRARM_MINOR_ALL	= 0xffff			// 所有参数日志
}L_PARAM_MINOR_TYPE_E;

/*
 * 异常日志子类型
 */
typedef enum _L_EXCEPTION_MINOR_TYPE {
	L_EXCEPTION_HD_FULL 		= 0x0001   	//硬盘满
		, L_EXCEPTION_HD_ERROR           		//硬盘错误
		, L_EXCEPTION_NET_DISCONNECT			//网络断开
		, L_EXCEPTION_IP_CONFLICT				//IP冲突

		, L_EXCEPTION_MINOR_ALL     = 0xffff   	//所有异常日志
}L_EXCEPTION_MINOR_TYPE_E;

/*
 * 
 */

#if 0
typedef struct sdk_log_item_s {
	sdk_time_t  time;                       //时间
	uint32_t    major_type;                 //类型LOG_MAJOR_TYPE_E 
	uint32_t    minor_type;                 //0x0000:直接显示desc内容，否则通过L_XXX_MINOR_TYPE_E解析内容.
	uint32_t    args;                       /*子类型参数(args=0时不用解析,否则根据minor_type解析内容。
						  如果minor_type和通道号相关，args表示通道号;如果minor_type和告警相关，args表示告警输入号)*/
	uint8_t     user[MAX_USER_NAME_LEN];    //操作用户
	uint8_t     ip_addr[MAX_IP_ADDR_LEN];   //用户IP
	uint8_t     desc[MAX_LOG_DESC_LEN];     //日志内容(minor_type=0时有效)
}sdk_log_item_t;
*/
#endif

typedef struct sdk_log_item_s {
	sdk_time_t  time;                       // 日志产生的时间
	uint8_t     user[MAX_USER_NAME_LEN];    // 用户名
	uint16_t    logId;   // 日志ID，前16位是日志类型，后16位是该类型的日志信息中的日志序号
}sdk_log_item_t;

/*
 * 日志查询条件
 */
typedef struct sdk_log_cond_s {
	sdk_time_t  begin_time;                 //开始时间
	sdk_time_t  end_time;                   //结束时间
	uint8_t    type;                       //类型
	//uint8_t     user[MAX_USER_NAME_LEN];    //操作用户
	//uint8_t     ip_addr[MAX_IP_ADDR_LEN];   //用户IP
}sdk_log_cond_t;



/*
 * 报警信息定义
 */
typedef struct sdk_alarm_info_s {
	sdk_time_t  time;                   //报警时间
	uint32_t    type;                   //报警类型
	uint32_t    args;                   //报警参数
	uint8_t     desc[MAX_ALARM_DESC_LEN];//报警内容
}sdk_alarm_info_t;


/*
 * 串口参数配置
 */
typedef struct sdk_serial_param_s {
	uint32_t baud_rate;             //串口波特率 115200, 57600, ...
	uint8_t  data_bit;              //数据位 5, 6, 7, 8 
	uint8_t  stop_bit;              //停止位 1, 2
	uint8_t  parity;                //校验 0:无，1:奇校验 2:偶校验 
	uint8_t  flow_ctl;              //流控
}sdk_serial_param_t;

/*
 * 串口用途配置
 */
typedef struct sdk_serial_func_cfg_s {
	uint8_t type;	// 0: normal, 1: ptz, 2...
	uint8_t res[3];
	sdk_serial_param_t serial_param;
}sdk_serial_func_cfg_t;


/*
 * 云台配置参数
 */
/*
 *  _0: debug, _1: ptz/normal, _2: rs232, _3 mcu
 *  -------------------------, _2: mcu;
 *  0: 1
 *   
 */
typedef struct sdk_ptz_param_s {

	sdk_serial_param_t comm;                //串口参数，gui固定只配置串口1
	uint8_t          address;               //云台地址
	uint8_t          protocol;              //云台协议SDK_PTZ_PROTOCOL_E
	uint8_t          res[2];
}sdk_ptz_param_t;


//预置点
typedef struct  sdk_preset_s
{
	uint8_t  num;                       //预置点编号
	uint8_t enable;                    //预置点使能状态
	uint8_t res[2];     

}sdk_preset_t;


typedef struct sdk_preset_all_s
{
	int num;                            //预置点个数  ，大于128时，当做128处理
	sdk_preset_t  preset[MAX_PRESET_NUM];

}sdk_preset_all_t;
//每个关键点
typedef struct sdk_preset_param_s 
{
	uint8_t enable;                                //  使能标志
	uint8_t res[2];                                 // 保留位  
	uint8_t num;                                   //关键点编号                 
	uint8_t speed;     //                            //
	uint8_t dwell_time;//
	sdk_preset_t  preset;                      //对应的预置点

}sdk_preset_param_t;
//巡航路线
typedef struct sdk_cruise_param_s 
{
	uint8_t         line;                        //巡航路线编号
	uint8_t         enable;                    //巡航路线是否使能
	uint8_t         res[3];                    //保留位
	sdk_preset_param_t  cruise_preset[MAX_CRUISE_PRESET_NUM];   //此路线所有的预置点

}sdk_cruise_param_t;


typedef struct sdk_cruise_line_s 
{ 
	//此通道所有的巡航线路所有的关键点信息
	sdk_cruise_param_t  cruise_line[MAX_CRUISE_LINE_NUM];  
	//此通道所有的预置点
	sdk_preset_all_t  preset_all;

}sdk_cruise_line_t;

//云台区域选择放大缩小(私有 快球专用)


/*
 * 
 * 预览配置(后继讨论修改)
 */


typedef enum _SDK_SCREE_SPLIT {
	SDK_SCREE_ONE  = 0x00
		, SDK_SCREE_FOUR
		, SDK_SCREE_NINE
		, SDK_SCREE_SIXTEEN
		, SDK_SCREE_THIRTY_TWO
		, SDK_SCREE_BUTT
}SDK_SCREE_SPLIT_E;


/*
 * 屏幕配置
 * sdk_msg_t.chann 表示屏号
 */
typedef struct sdk_preview_s {
	uint8_t windows[MAX_PREVIEW_MODE][MAX_WINDOWS_NUM];
	uint8_t enable;     //开启/禁止屏幕输出（主屏不能禁止）
	uint8_t init_mode;      //开机时初始化分屏模式,0-1画面,1-4画面,2-6画面,3-8画面,4-9画面,5-16画面,6-25画面,7-36画面,0xff:最大画面
	uint8_t res[2];         //保留
}sdk_preview_t;

/*
 * GUI预览巡回配置
 * sdk_msg_t.chann 表示屏号
 */
typedef struct sdk_tour_s {
	uint8_t windows[MAX_PREVIEW_MODE][MAX_WINDOWS_NUM];//切换顺序,step[i]为 0xff表示不用
	uint8_t enable;         //开启/禁止巡回功能
	uint8_t enable_audio;   //是否声音预览,0-不预览,1-预览
	uint8_t switch_time;    //切换时间(秒)
	uint8_t res[1];         //保留
}sdk_tour_t;


/*
 * VO绑定定义
 * sdk_msg_t.chann 表示屏号
 */
typedef struct sdk_vo_binding_s {

	uint8_t mode;        //分屏模式,0-1画面,1-4画面,2-6画面,3-8画面,4-9画面,5-16画面,6-25画面,7-36画面,0xff:最大画面
	uint8_t win_num;     //窗口个数
	uint8_t res[2];      //保留
	uint8_t windows[MAX_WINDOWS_NUM];//窗口内容;

}sdk_vo_binding_t;

/*
 * 用于解码器屏幕控制
 */
typedef struct sdk_win_s {
	uint8_t valid;          //valid: 0表示无视频源
	uint8_t action;
	uint8_t res[1];         //保留
	uint8_t win_no;
	uint8_t enc_name[48];
	uint8_t enc_chn;		//编码器通道号 解码器用到
	uint8_t level;			//码流类型 解码器用到
	uint8_t url[192];       //视频源URL
}sdk_win_t;

typedef struct sdk_screen_s {
	uint8_t valid;                 //valid: 0表示显示屏被禁止
	uint8_t mode;                  //分屏模式,0:1画面,1:4画面,2:6画面,3:8画面,4:9画面,5:16画面,0xff:最大画面
	uint8_t res[1];                //保留
	uint8_t count;
	sdk_win_t win[MAX_WINDOWS_NUM];//sdk_win_t
}sdk_screen_t;
/*
 *音频配置
 */
typedef struct sdk_audio_cfg_s {

	uint8_t encode_type;        //编码类型 1: G711A
	uint8_t samples;            //采样频率 0: 8000, 
	uint8_t bits;               //位宽     1: 16bit, 
	uint8_t channels;           //通道数

	uint8_t bufsize;            //缓存大小,以帧为单位
	uint8_t resample;           //0 无采样 2 2->1 4 4->1 6 6->1
	uint16_t sample_per_frm;     //每一帧采样点个数
}sdk_audio_cfg_t;


/*
 * GUI查询设备列表时使用
 */
typedef enum _MANUFACT_TYPE{
	MANUFACT_JXJ     = 0x00    /* JXJ设备 */
		,MANUFACT_RTSP_CH = 0x01    /* 在通道配置中指定2个RTSP URL 直接打开视频 */
		,MANUFACT_ONVIF   = 0x02    /* ONVIF NVT */
		,MANUFACT_RTSP    = 0x03    /* 标准RTSP URL, 不使用通道配置参数 (用作解码器设备) */
		,MANUFACT_PRIVATE = 0x04
		,MANUFACT_ONVIF_PRIVATE = 0x05 
		,MANUFACT_ONVIF_MULTISUBNET = 0x06  //多网段搜索
}MANUFACT_TYPE_E;



typedef struct sdk_device_s {
	uint8_t   ip_addr[MAX_IP_ADDR_LEN];     //IP
	uint16_t  port;                         //port
	uint8_t  macaddr[MAX_IP_ADDR_LEN];
	uint8_t   dns_en;                       // 1:0:
	uint8_t   device_type;                  //  0:IPC 1: DVR 2:NVR
	uint8_t   chann_num;                    //
	uint8_t   chann_stream_num;             // 1:  2:  3:
	uint8_t   manufacturer;                 // MANUFACT_TYPE_E
	uint8_t   devmantype;                   //
	uint8_t   dns_path[64];                 //
	uint8_t   dst_id[32];                   // 
	uint8_t   index;                        // 所属网段索引
	sdk_user_t user;                        //+ 
}sdk_device_t;


typedef struct sdk_private_device_s
{
   uint8_t  ip_addr[MAX_IP_ADDR_LEN] ; //[MAX_IP_ADDR_LEN];     //IP
  uint16_t  port;     
  uint8_t  macaddr[MAX_IP_ADDR_LEN];
  uint8_t  dev_type;                          //  0 ipc   1 nvr  
  uint8_t  protocol;                           // 0   onvif  1  
  //port
  sdk_user_t user;    
} sdk_private_device_t;

/*
 *
 * 通道配置（支持IP接入）
 */
#if 0
typedef struct sdk_channel_s {
	uint8_t   enable;                        //是否启用
	uint8_t   is_local;                      //是否为本地视频源		1:本地; 0:远程
	//local vi info
	uint8_t   video_in;                      //本地视频源序号
	uint8_t   video_format;                  //本地视频源信号格式    0：PAL， 1：NTSC    
	//channel name
	uint8_t   name[MAX_CHANN_NAME_LEN];      //通道名称
	//net channel info
	sdk_user_t user;                        //远程设备用户名+密码 
	uint8_t  chann_no;                      //接入远程设备通道号
	uint8_t  chann_protocol;                //接入通道码流协议 0: TCP , 1: UDP
	uint8_t  chann_audio_en:1;              //接入通道是否支持音频 1: 支持   0: 不支持
	uint8_t  chann_win_mode:2;              //预览码流模式 0: 自动, 1: 主码流, 2: 子码流
	uint8_t  chann_bit_res:5;               //保留
	uint8_t  chann_byte_res;                //保留

	//net device info
	sdk_device_t device;                    //远程设备信息

	//device.manufacturer extern info
	//uint8_t  url0[128];                    //URL0
	//uint8_t  url1[128];                    //URL1
}sdk_channel_t;
#endif

typedef struct sdk_channel_s {
	uint8_t   enable;                        //是否启用
	uint8_t   name[MAX_CHANN_NAME_LEN];      //通道名称
	sdk_user_t user;                        //远程设备用户名+密码 
	uint8_t  chann_no;                      //接入远程设备通道号
	uint8_t  chann_protocol;                //接入通道码流协议 0: TCP , 1: UDP
	uint8_t  chann_win_mode;              //预览码流模式 0: 自动, 1: 主码流, 2: 子码流

	uint8_t   ip_addr[MAX_IP_ADDR_LEN];     //IP
	uint16_t  port;                         //port
	uint8_t   manufacturer;                 // MANUFACT_TYPE_E
	uint8_t  chann_byte_res[5];				//保留
	uint8_t  rtsp_main_url[MAX_RTSP_MAINURL_LEN];
	uint8_t  rtsp_sub_url[MAX_RTSP_SUBURL_LEN];
}sdk_channel_t;


typedef struct sdk_resolution_s
{
	uint16_t height;
	uint16_t width;

}sdk_resolution_t;

/*
 * 通道编码参数(子结构体)
 */
typedef struct sdk_av_enc_s {
	uint8_t res_num;                   //分辨率个数
	uint8_t resolution_current;   //当前分辨率索引
	uint8_t resolution;	

	sdk_resolution_t  resolution_all[MAX_RESOLUTION_NUM];


	uint8_t bitrate_type;   //码率类型 0--变码流;   1--定码流 SDK_CODEC_BR_TYPE_E
	uint8_t pic_quilty;     //编码质量0-最好，1-次好，2-较好，3-一般，4-较差，5-差  
	uint8_t frame_rate;     //编码帧率1-25(PAL)/30
	uint8_t gop;            //I 帧间隔1-200
	uint8_t video_enc;      //视频编码格式0--h264 1--MJPEG 2--JPEG  SDK_CODEC_ENC_TYPE_E
	uint8_t audio_enc;      //音视编码格式0:G711A
	uint8_t mix_type;       //音视频流: 0, 视频流: 1.
	uint16_t bitrate;       //编码码率 32kbps-16000kbps
	uint8_t level;          //编码等级，h264: 0--baseline, 1--main, 2--high;
	uint8_t h264_ref_mod; 
}sdk_av_enc_t;

/*
 * 编码参数索引(pmsg->extend传递)
 */
typedef enum _SDK_ENC_IDX {
	SDK_ENC_IDX_MAIN  = 0
		,SDK_ENC_IDX_SECOND= 1
		,SDK_ENC_IDX_ALL   = 0xFF
}SDK_ENC_IDX_T;
/*
 * 通道编码参数
 */

typedef struct sdk_encode_s {
	sdk_av_enc_t main;          //主码流
	sdk_av_enc_t second;        //子码流
	sdk_av_enc_t res;           //保留
}sdk_encode_t;

/*
	venc 
*/
typedef struct sdk_venc_s
{
	unsigned char channelNo;
	unsigned char encodeType;//0  h264
	unsigned char mainResolution;
	unsigned char mainEncodeMode; //0:  video  1:audio  2:video and audio
	unsigned char mainFrameRate;
	unsigned char mainBiteRate;
	unsigned char subResolution;
	unsigned char subEncodeMode;//0:  video  1:audio  2:video and audio
	unsigned char subBitRate;
	unsigned char subFrameRate;
}sdk_venc_t;

/*
 *
 *视频覆盖区域设置
 */

typedef struct sdk_overlay_cfg_s {
	uint8_t   enable;                  //是否开启
	uint8_t   max_num;
	uint8_t   res[2];
	uint32_t   mask;                    //bit.0: area[0], bit.1: area[1]
	sdk_rect_t area[MAX_OVERLAY_NUM];   //区域坐标
}sdk_overlay_cfg_t;


typedef struct _sdk_codec_area_zoom_ch_info_s
{
	int is_enable;
	sdk_rect_t area;
}sdk_codec_ch_area_zoom_info_t;


/*
 *
 *TV设置
 */
typedef struct sdk_vo_sideSize_s
{
	int top_offset;
	int bottom_offset;
	int left_offset;
	int right_offset;
}sdk_vo_sideSize_t;

/*
 *
 *视频OSD区域(子结构体)
 */

typedef struct sdk_osd_info_s {

	sdk_point_t pos;            //位置
	uint8_t     valid;          // 当前区域是否有效
	uint8_t     font;           // 字体类型
	// [7 6 5 4 - 3 2 1 0]
	// bit[4-7]: display type, 0: time, 1: string, 2 .....
	// bit[0-3]: display format 
	//           sample time format:
	//           bit[2-3], 0: YYYYMMDD, 1: MMDDYYYY, 2: DDMMYYYY
	//           bit[1],   0: '-'       1: '/'
	//           bit[0],   0: 24hour    1: ampm
	uint8_t     format;         // 显示格式
	uint8_t     str_len;        // 字符串长度
	uint8_t     str[MAX_OSD_STR_LEN];//ascii+gb2312字符串
}sdk_osd_info_t;


/*
 *
 *视频OSD配置
 */
typedef struct sdk_osd_cfg_s {
	sdk_osd_info_t  time;        //OSD时间
	sdk_osd_info_t  chann_name;  //OSD通道名称
	//  sdk_osd_info_t  vloss;       //保留
	uint16_t max_width;
	uint16_t max_height;
	uint8_t  st_info_en;
	uint8_t  res1[3];
	uint8_t  res2[64];

}sdk_osd_cfg_t;



/*
 *
 *图像属性配置
 */
typedef struct sdk_image_attr_s {
	uint8_t brightness;         //亮度
	uint8_t contrast;           //对比度
	uint8_t saturation;         //饱和度
	uint8_t hue;                //色度
	uint8_t sharpness;          //锐度
	uint8_t res[3];
}sdk_image_attr_t;

/*
 *
 *自动维护配置
 */
typedef struct sdk_autoMaintain_attr_s {
	uint32_t interval;         //星期 0:never  1:every 2:mon  3:tue 4:wed 5 :thu 6 :fri 7:sat 8:sun
	uint32_t hour;           //时间0-23
	uint8_t res[3];
}sdk_autoMaintain_attr_t;

/*
 *自动维护
 */
typedef enum _Maintain_Auto
{
	SDK_MAIN_AUTO_NEVER,
	SDK_MAIN_AUTO_EVERY_DAY,
	SDK_MAIN_AUTO_MONDAY,
	SDK_MAIN_AUTO_TUESDAY,
	SDK_MAIN_AUTO_WEDNESDAY,
	SDK_MAIN_AUTO_THURSDAY,
	SDK_MAIN_AUTO_FRIDAY,
	SDK_MAIN_AUTO_STAURDAY,
	SDK_MAIN_AUTO_SUNDAY
}Maintain_Auto;


/*
 * 抓拍结构体定义
 */
typedef struct sdk_snap_pic_s {
	uint16_t width;             //宽度
	uint16_t height;            //高度
	uint8_t  encode;            //编码格式,JPEG
	uint8_t  quilty;            //图像质量
	uint8_t  res[2];            //保留
}sdk_snap_pic_t;

typedef enum _SDK_DISK_STATUS {
	SDK_DISK_STATUS_DISK_UNMOUNT = 0x00	// 未挂载
		, SDK_DISK_STATUS_DISK_NOMAL			// 正常
		, SDK_DISK_STATUS_DISK_USING			// 正在使用
		, SDK_DISK_STATUS_DISK_BUTT
}SDK_DISK_STATUS_E;

/*
 * 磁盘信息结构体
 */
typedef struct sdk_disk_s {
	uint32_t disk_no;       //磁盘号
	uint32_t disk_type;     //磁盘类型（0:SATA，1:USB，2:ISCSI; 3:NFS）
	uint32_t status;        //磁盘状态 SDK_DISK_STATUS_E
	uint32_t total_size;    //磁盘容量(MB)
	uint32_t free_size;     //可用容量(MB)
	uint32_t is_backup;     //是否备份磁盘 0:录像盘; 1:备份盘
	uint32_t is_raid;       //是否raid //属于哪个盘组
}sdk_disk_t;

//本地盘组信息配置

//网络硬盘结构配置


/*
 * 录像参数（子结构体）
 */
typedef struct sdk_record_sched_s {
	uint8_t is_allday;                              //是否全天录像
	uint8_t record_time_num;                // 录像的时间段总数
	uint8_t res[2];                                 //保留
	sdk_sched_time_t sched_time[MAX_TIME_SEG_NUM];  //布防时间段
}sdk_record_sched_t;

/*
 * 录像参数
 */
typedef struct sdk_record_cfg_s {
	uint8_t enable;                                //开启定时录像
	uint8_t res[3];                                 //保留
	sdk_record_sched_t record_sched[MAX_WEEK_NUM];  //布防时间段(0:星期日; 1:星期一,2:星期二,... ,6:星期六)
	uint32_t pre_record_time;                       //预录时间
	uint32_t record_duration_time;                  //录像保留时间
	uint8_t  enable_redundancy;                     //是否冗余备份
	uint8_t  enable_audio;                          //是否录制音频
	uint8_t  res2[2];                                //保留

}sdk_record_cfg_t;



/*
 * 录像记录定义
 */
typedef struct sdk_record_item_s {
	uint8_t item_name[MAX_RECORD_ITEM_LEN]; //记录名称
	uint32_t item_handle[MAX_STOR_FRAGMENT_LEN];   //sizeof(stor_fragment_t)+6个res
	sdk_time_t start_time;                  //开始时间
	sdk_time_t stop_time;                   //结束时间
	uint32_t item_size;                     //数据大小
	uint8_t is_locked;                      //是否锁定
	uint8_t record_type;                    //录像类型
	uint8_t res[2];                         //保留
	// card no;
}sdk_record_item_t;

/*
 * 录像查询条件
 */
typedef struct sdk_record_cond_s {
	uint8_t is_locked;              //锁定状态
	uint8_t record_type;            //录像类型
	uint8_t res[2];                 //保留
	sdk_time_t start_time;          //开始时间
	sdk_time_t stop_time;           //结束时间
	// card no;    
}sdk_record_cond_t;

/*************LSL20130327*********************/

typedef struct record_search_file_t
{
	//录像文件信息 
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
/**********************************/

/*
 *
 * 回放控制命令字
 */


typedef enum _SDK_PB_GROUP_CONTROL {
	SDK_PB_CONTROL_ONNE  = 0x00 	//
		, SDK_PB_CONTROL_PAUSE  		// 暂停
		, SDK_PB_CONTROL_SETP          	// 单帧进
		, SDK_PB_CONTROL_NORMAL        	// 正常
		, SDK_PB_CONTROL_DRAG          	// 拖拽
		, SDK_PB_CONTROL_FORWARD       	// 前进
		, SDK_PB_CONTROL_BACKWARD      	// 后退
		, SDK_PB_CONTROL_QUERY_TIME		// 查询当前所播放帧的时间，单位秒
		, SDK_PB_CONTROL_FULLSCREEN
		, SDK_PB_CONTROL_BUTT
}SDK_PB_GROUP_CONTROL_E;

/*
 *
 * 前进播放速度枚举 [--------- | ---- -> -> ----]
 */
typedef enum _SDK_PB_GROUP_CONTROL_FORWARD {
	SDK_PB_CONTROL_FW_NORMAL = 0x00  				// 正常
		, SDK_PB_CONTROL_FF_1X     						// 1倍快进 (FF-fast forward-向前快进)
		, SDK_PB_CONTROL_FF_2X    						// 2倍快进
		, SDK_PB_CONTROL_FF_4X         					// 4倍快进
		, SDK_PB_CONTROL_FF_8X							// 8倍快进
		, SDK_PB_CONTROL_FF_16X							// 16倍快进
		, SDK_PB_CONTROL_SF_1_2X						// 1倍慢放 (SF-slow forward-向前慢放)
		, SDK_PB_CONTROL_SF_1_4X						// 2倍慢放
		, SDK_PB_CONTROL_SF_1_8X						// 4倍慢放
		, SDK_PB_CONTROL_SF_1_16X						// 8倍慢放

}SDK_PB_GROUP_CONTROL_FORWARD_E;

/*
 *
 * 后退播放速度枚举 [--- <- <- ---- | ----------]
 */
typedef enum _SDK_PB_GROUP_CONTROL_BACKWARD {
	SDK_PB_CONTROL_BW_NORMAL = 0x00  				// 正常
		, SDK_PB_CONTROL_FB_1X     						// 1倍快退 (FB-fast backward-向后快进)
		, SDK_PB_CONTROL_FB_2X    						// 2倍快退
		, SDK_PB_CONTROL_FB_4X         					// 4倍快退
		, SDK_PB_CONTROL_FB_8X							// 8倍快退
		, SDK_PB_CONTROL_FB_16X							// 16倍快退
		, SDK_PB_CONTROL_SB_1_2X						// 1倍慢退 (SB-slow backward-向后慢放)
		, SDK_PB_CONTROL_SB_1_4X						// 2倍慢退
		, SDK_PB_CONTROL_SB_1_8X						// 4倍慢退
		, SDK_PB_CONTROL_SB_1_16X						// 8倍慢退

}SDK_PB_GROUP_CONTROL_BACKWARD_E;


/*
 * 回放通道组定义
 */
#if 0
typedef struct sdk_pb_group_s {
	sdk_time_t start_time;             //开始时间
	sdk_time_t stop_time;              //结束时间
	uint32_t main_chann;               //主通道号
	uint8_t chann_mask[MAX_CHANN_NUM];//通道掩码（多路回放时）
}sdk_pb_group_t;
#else
typedef struct sdk_pb_group_s {
	record_search_file_t record_file_info;
	uint32_t main_chann;               //主通道号
	uint8_t chann_mask[MAX_CHANN_NUM];//通道掩码（多路回放时）
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
}sdk_pb_group_t;


#endif


/* ---------------------------------------*/
// ntp
// ddns
// wifi

typedef struct sdk_wifi_ap_s
{
	uint8_t essid[MAX_DEV_ID_LEN];		//服务区别号
	uint32_t encrypt_type;				//加密类型0:NONE  1:WPA  2:WPA2 3:WEP
	uint32_t auth_mode;					//认证方式0:NONE  1:EAP 2:PSK 3:OPEN 4:SHARED 
	uint32_t secret_key_type;			//密钥管理方式 0:NONE  1:AES 2:TKIP 只对应于加密类型为WPA/WPA2的情况
	uint32_t quality;					//信号质量 0-100
	uint32_t bit_rate;					//传输速率
}sdk_wifi_ap_t;

typedef struct sdk_wifi_ap_set_s
{
	uint32_t count;							//搜索到的wifi 个数
	sdk_wifi_ap_t wifi_ap[MAX_WIFI_AP_NUM]; //
}sdk_wifi_ap_set_t;



// email
// ftp
/* ---------------------------------------*/


/*
 *
 *设备信息（设备发现时用，如：搜索工具）
 */

typedef struct sdk_device_info_s {
	/*------ 设备配置信息 ------*/
	sdk_net_mng_cfg_t   net_mng;        //网络配置
	sdk_eth_cfg_t   eth_cfg;        //eth0
	sdk_sys_cfg_t   sys_cfg;        //系统配置
	/*---- 系统运行时信息  ----*/
	//需要不断完善
}sdk_device_info_t;

/*----------------- 报警配置结构体定义 -----------------*/

/*
 * ID
 */
typedef enum _SDK_PTZ_MSG_ID {
	SDK_PTZ_MSG_UP_START = 0x001
		, SDK_PTZ_MSG_UP_STOP
		, SDK_PTZ_MSG_DOWN_START
		, SDK_PTZ_MSG_DOWN_STOP
		, SDK_PTZ_MSG_LEFT_START
		, SDK_PTZ_MSG_LEFT_STOP
		, SDK_PTZ_MSG_RIGHT_START    
		, SDK_PTZ_MSG_RIGHT_STOP
		, SDK_PTZ_MSG_LEFT_UP_START
		, SDK_PTZ_MSG_LEFT_UP_STOP
		, SDK_PTZ_MSG_RIGHT_UP_START
		, SDK_PTZ_MSG_RIGHT_UP_STOP
		, SDK_PTZ_MSG_LEFT_DOWN_START
		, SDK_PTZ_MSG_LEFT_DOWN_STOP
		, SDK_PTZ_MSG_RIGHT_DOWN_START
		, SDK_PTZ_MSG_RITHT_DOWN_STOP
		, SDK_PTZ_MSG_PRESET_SET
		, SDK_PTZ_MSG_PRESET_CALL
		, SDK_PTZ_MSG_PRESET_DEL
		, SDK_PTZ_MSG_ZOOM_ADD_START
		, SDK_PTZ_MSG_ZOOM_ADD_STOP
		, SDK_PTZ_MSG_ZOOM_SUB_START
		, SDK_PTZ_MSG_ZOOM_SUB_STOP
		, SDK_PTZ_MSG_FOCUS_ADD_START
		, SDK_PTZ_MSG_FOCUS_ADD_STOP
		, SDK_PTZ_MSG_FOCUS_SUB_START
		, SDK_PTZ_MSG_FOCUS_SUB_STOP
		, SDK_PTZ_MSG_IRIS_ADD_START
		, SDK_PTZ_MSG_IRIS_ADD_STOP
		, SDK_PTZ_MSG_IRIS_SUB_START
		, SDK_PTZ_MSG_IRIS_SUB_STOP
		, SDK_PTZ_MSG_GOTO_ZERO_PAN
		, SDK_PTZ_MSG_FLIP_180
		, SDK_PTZ_MSG_SET_PATTERN_START
		, SDK_PTZ_MSG_SET_PATTERN_STOP
		, SDK_PTZ_MSG_RUN_PATTERN
		, SDK_PTZ_MSG_SET_AUXILIARY
		, SDK_PTZ_MSG_CLEAR_AUXILIARY
		, SDK_PTZ_MSG_AUTO_SCAN_START
		, SDK_PTZ_MSG_AUTO_SCAN_STOP
		, SDK_PTZ_MSG_RANDOM_SCAN_START
		, SDK_PTZ_MSG_RANDOM_SCAN_STOP
		, SDK_PTZ_MSG_LEFT_OFFSET
		, SDK_PTZ_MSG_RIGHT_OFFSET
		, SDK_PTZ_MSG_DOWN_OFFSET
		, SDK_PTZ_MSG_UP_OFFSET
		, SDK_PTZ_MSG_ZOOM_MULTIPLE
		, SDK_PTZ_MSG_POINT_CENTER
		, SDK_PTZ_MSG_VIEW_CENTER
		, SDK_PTZ_MSG_BUTT
}SDK_PTZ_MSG_ID_E;
/*
 * 
 */
typedef enum _SDK_PTZ_PROTOCOL {
	SDK_PTZ_PELCO_D = 0x01
		, SDK_PTZ_PELCO_P
		, SDK_PTZ_BUTT    = 0x20
}SDK_PTZ_PROTOCOL_E;


typedef enum _SDK_PTZ_OP_TYPE {
	SDK_PTZ_TYPE_NONE = 0x00
		, SDK_PTZ_TYPE_PRESET		// 预置位
		, SDK_PTZ_TYPE_CRUISE		// 巡航
		, SDK_PTZ_TYPE_BUTT
}SDK_PTZ_OP_TYPE_E;

/*
 *  报警联动定义
 */
typedef struct sdk_alarm_handle_s {

	uint8_t record_mask[MAX_CHANN_NUM];   //录像通道掩码
	uint8_t snap_mask[MAX_CHANN_NUM];     //抓拍通道掩码
	uint8_t alarm_out_mask[MAX_CHANN_NUM];//联动输出掩码      
	uint8_t ptz_type[MAX_CHANN_NUM];      //联动云台操作类型(预置位/巡航)	SDK_PTZ_OP_TYPE_E
	uint8_t ptz_param[MAX_CHANN_NUM];     //联动云台参数(预置位号，巡航号)
	uint8_t res_mask[MAX_CHANN_NUM];

	uint8_t  record_enable;
	uint8_t  record_time;                 //录像延时时长
	uint8_t  snap_enable;
	uint8_t  snap_interval;               //抓拍时间间隔(无效)

	uint8_t  snap_num;                    //连续抓拍张数
	uint8_t  beep_enable;                 //是否蜂鸣器
	uint8_t  beep_time;                   //蜂鸣时间
	uint8_t  ptz_enable;

	uint8_t  alarm_out_enable;
	uint8_t  alarm_out_time;              //联动输出时间
	uint8_t  res[2+4];                    //email, ftp, 3g;
}sdk_alarm_handle_t;

/*
 * 报警输入（IO）报警配置
 */
typedef struct sdk_alarm_in_cfg_s {
	uint8_t name[MAX_ALARM_IN_NAME_LEN];                        //报警输入名称
	uint8_t type;                                               //报警输入类型，1：常开(默认)，0：常闭
	uint8_t enable;                                             //允许联动
	uint8_t res[2];                                             //保留
	sdk_sched_time_t sched_time[MAX_WEEK_NUM][MAX_TIME_SEG_NUM];//布防时间段
	sdk_alarm_handle_t alarm_handle;                            //联动处理
}sdk_alarm_in_cfg_t;


/*
 * 视频移动侦测配置
 */
typedef enum _SDK_VDA_MOD_E {
	SDK_VDA_MD_MOD = 0
		, SDK_VDA_OD_MOD = 1
}SDK_VDA_MOD_E;

typedef struct sdk_vda_codec_cfg_s
{
	uint8_t enable;                                             //允许联动
	uint8_t sensitive;                                          //灵敏度(0[灵敏度最高]----6[最低])
	uint8_t mode;                                             	/* 0:MD 移动 1:OD 遮挡 */
	uint8_t res;
	uint32_t mask;												//按位
	sdk_rect_t area[MAX_MOTION_NUM];                            //区域
}sdk_vda_codec_cfg_t;

#if 0
typedef struct sdk_motion_cfg_v2_s {
	sdk_vda_codec_cfg_t codec_vda_cfg;						    			
	sdk_sched_time_t sched_time[MAX_WEEK_NUM][MAX_TIME_SEG_NUM];//?????
	sdk_alarm_handle_t alarm_handle;                            //????
}sdk_motion_cfg_v2_t;
#endif
typedef struct sdk_motion_cfg_v2_s {
	uint8_t enable;
	uint8_t beep_enable;
	uint8_t beep_time;
	uint8_t record_enable;
	uint8_t record_time;
	uint8_t smtp_enable;
	uint8_t ftp_enable;
	uint8_t sensitivity;
	uint32_t blockstatus[MAX_BLOCK_ROW];
}sdk_motion_cfg_v2_t;

typedef struct sdk_motion_cfg_s {
	//uint8_t scope[18][22];                                    //需要确认
	uint8_t enable;                                             //允许联动
	uint8_t sensitive;                                          //灵敏度(0[灵敏度最高]----6[最低])
	uint8_t res[2];                                             //保留
	uint32_t mask;							// 按位
	sdk_rect_t area[MAX_MOTION_NUM];                            //区域
	sdk_sched_time_t sched_time[MAX_WEEK_NUM][MAX_TIME_SEG_NUM];//布防时间段
	sdk_alarm_handle_t alarm_handle;                            //联动处理
}sdk_motion_cfg_t;

/*
 * 视频遮挡侦测配置
 */
typedef struct sdk_hide_cfg_s {
	uint8_t enable;                                         //允许联动
	uint8_t sensitive;                                          //灵敏度(0[灵敏度最高]----6[最低])
	uint8_t res[2];                                             //保留
	uint32_t mask;												 // 按位
	sdk_rect_t area[MAX_MOTION_NUM];                            //区域
	sdk_sched_time_t sched_time[MAX_WEEK_NUM][MAX_TIME_SEG_NUM];//布防时间段
	sdk_alarm_handle_t alarm_handle;                            //联动处理
}sdk_hide_cfg_t;

/*
 * 视频丢失侦测配置
 */
typedef struct sdk_lost_cfg_s {
	uint8_t enable;                                             //允许联动
	uint8_t res[3];                                             //保留
	sdk_sched_time_t sched_time[MAX_WEEK_NUM][MAX_TIME_SEG_NUM];//布防时间段
	sdk_alarm_handle_t alarm_handle;                            //联动处理
}sdk_lost_cfg_t;


/*
 * 录像控制
 */
typedef struct sdk_manual_record_s {
	uint8_t manual_record[MAX_CHANN_NUM];	// 手动录像
	uint8_t stop_record[MAX_CHANN_NUM];		// 禁止录像
	uint8_t res[MAX_CHANN_NUM];				// 保留位
}sdk_manual_record_t;

/*
 * 手动开启(停止)报警输入检测
 */
typedef struct sdk_manual_alarmin_s {
	uint8_t enable_alarmin[MAX_ALARM_IN_NUM];	// 0:停止; 1:开启(默认全开启)
	uint8_t res[MAX_ALARM_IN_NUM];				// 保留位
}sdk_manual_alarmin_t;

/*
 * 手动开启(停止)报警输出
 */
typedef struct sdk_manual_alarmout_s {
	uint8_t enable_alarmout[MAX_ALARM_OUT_NUM];	// 0:停止(默认全停止); 1:开启
	uint8_t res[MAX_ALARM_OUT_NUM];				// 保留位
}sdk_manual_alarmout_t;


/*
 * 录像备份
 */
typedef enum _SDK_BACKUP_OP {
	SDK_BACKUP_START		= 0x001		//sdk_record_backup_t
		, SDK_BACKUP_PROGRESS				//pmsg->chan:备份进度
		, SDK_BACKUP_BUTT
}SDK_BACKUP_OP_E;

/*
 * 备份输出文件格式
 */
typedef enum _SDK_BACKUP_FMT {
	SDK_BACKUP_FMT_JAV  = 0     /* jav */
		,SDK_BACKUP_FMT_AVI  = 1     /* avi */
}SDK_BACKUP_FMT_E;

typedef struct sdk_record_handle_s {
	uint32_t  	item_handle[MAX_STOR_FRAGMENT_LEN];
	uint32_t 	item_size;				//段大小
	sdk_time_t 	start_time;          	//开始时间
	sdk_time_t 	end_time;           	//结束时间
	uint8_t  	res[4];					//保留
}sdk_record_handle_t;

typedef struct sdk_record_backup_s {
	//sdk_record_handle_t item_arr[MAX_BACKUP_ITEM];
	record_search_file_t record_file_info;
	uint8_t item_num;				//实际备份段数
	uint8_t record_type;            //录像类型
	uint8_t out_fmt;                //备份输出文件格式  SDK_BACKUP_FMT_E
	uint8_t res[1];					//保留
}sdk_record_backup_t;


/*
 * 关闭系统
 */
typedef enum _SDK_CLOSE_SYS_OP {
	SDK_CLOSE_SYS_SHUTDOWN	= 0x001		//关机
		, SDK_CLOSE_SYS_REBOOT					//重启
		, SDK_COLSE_SYS_LOGOUT
		, SDK_CLOSE_SYS_BUTT
}SDK_CLOSE_SYS_OP_E;

/*
 * 音频控制
 */
typedef enum _SDK_AUDIO_CONTROL_OP {
	SDK_AUDIO_CONTROL_OPEN	= 0x001		//伴音
		, SDK_AUDIO_CONTROL_MUTE				//静音
		, SDK_AUDIO_CONTROL_TALK				//对讲
		, SDK_AUDIO_CONTROL_SPECIAL_CMD			//特殊命令(GUI通知主控进入回放界面，pmsg->extend:1进入; 0:退出)

		, SDK_AUDIO_CONTROL_BUTT
}SDK_AUDIO_CONTROL_OP_E;



/*
 * 域名解析
 */

#define 	MAX_DDNS_USER_LEN   	64				// DDNS用户名最大长度
#define 	MAX_DDNS_PWD_LEN    	32				// DDNS密码最大长度
#define 	MAX_DDNS_NAME_LEN   	128				// DDNS域名最大长度

typedef enum _SDK_DDNS_TYPE {
	SDK_DDNS_TYPE_DYNDNS	= 0x00		// Dyndns
		, SDK_DDNS_TYPE_3322				// 3322
		, SDK_DDNS_TYPE_ORAY				// Oray

		, SDK_DDNS_TYPE_BUTT
}SDK_DDNS_TYPE_E;

typedef enum _SDK_DDNS_ORAY_USERTYPE {
	SDK_ORAY_USERTYPE_COMMON	= 0x01	// 普通用户
		, SDK_ORAY_USERTYPE_EXPERT			// 专业用户

		, SDK_ORAY_USERTYPE_BUTT
}SDK_DDNS_ORAY_USERTYPE;

typedef struct sdk_ddns_cfg_s {
	uint8_t enable;						// DDNS 0-1-
	uint8_t type;						// DDNSSDK_DDNS_TYPE_E
	uint8_t user_type;					// SDK_DDNS_ORAY_USERTYPE ()

	uint32_t interval;					// (:)
	uint32_t port;						// DDNS
	uint8_t server[MAX_DDNS_NAME_LEN];	// DDNS(IP)
	uint8_t user[MAX_DDNS_USER_LEN];	// 
	uint8_t pwd[MAX_DDNS_PWD_LEN];		// 
	uint8_t domain[MAX_DDNS_NAME_LEN];	// (dyndns,domain',')
	uint8_t client_ip[MAX_IP_ADDR_LEN];	// IP
}sdk_ddns_cfg_t;


/*
 * ?????
 */

typedef enum SDK_VIDEO_CAP_RESOLUTION {
	SDK_VIDEO_CAP_RESOLUTION_QCIF      	= 0x1				//176*144  n176*120
		, SDK_VIDEO_CAP_RESOLUTION_CIF			= 0x2				//352*288  n352*240
		, SDK_VIDEO_CAP_RESOLUTION_HD1			= 0x4				//704*288  n704*240
		, SDK_VIDEO_CAP_RESOLUTION_D1			= 0x8				//704*576  n704*480
		, SDK_VIDEO_CAP_RESOLUTION_960H    		= 0x10				//960x576
		, SDK_VIDEO_CAP_RESOLUTION_QQVGA		= 0x20				//160*112
		, SDK_VIDEO_CAP_RESOLUTION_QVGA			= 0x40				//320*240
		, SDK_VIDEO_CAP_RESOLUTION_VGA			= 0x80				//640*480
		, SDK_VIDEO_CAP_RESOLUTION_SVGA			= 0x100				//800*600
		, SDK_VIDEO_CAP_RESOLUTION_UXGA			= 0x200				//1600*1200
		, SDK_VIDEO_CAP_RESOLUTION_720P			= 0x400				//1280*720
		, SDK_VIDEO_CAP_RESOLUTION_960			= 0x800				//1280*960
		, SDK_VIDEO_CAP_RESOLUTION_1080P		= 0x1000			//1920*1080
}SDK_VIDEO_CAP_RESOLUTION_E;

typedef struct device_chan_capability_s {
	uint32_t   res_cap[MAX_STREAM_NUM];			// SDK_VIDEO_CAP_RESOLUTION_E
	uint32_t   max_frame_rate[MAX_STREAM_NUM]; 	// 码流最大支持帧率
}device_chan_capability_t;

typedef struct sdk_device_capability_s {
	uint8_t    screen_num;                     			// 屏幕个数（3531两个，3521一个）
	uint8_t    eth_num;                        			// 网口个数（设备类型为xx_e2时两个，其它一个）
	uint8_t    res[2];                         			// 保留
	device_chan_capability_t ch_cap[MAX_CHANN_NUM];
}sdk_device_capability_t;


/* 
 * 获取对端设备的媒体URL (MAIN -> PUA)
 */
typedef struct sdk_media_url_s {
	uint8_t    type;        /* 0:real av */
	uint8_t    st_ch;       /* 0:main, 1:second */
	uint8_t    trans;       /* 0: udp, 1: tcp, 3: rtsp 4: http*/
	uint8_t    proto;       /* 0: Unicast, 1: multicast */
	uint8_t    ip[40];      /* requestor ip */
	int8_t     b_time[16];  /* record fmt:YYYYMMDDhhmmss */
	int8_t     e_time[16];  /* record fmt:YYYYMMDDhhmmss */
	int8_t     url[256];    /* url */
}sdk_media_url_t;


/*
 * 云台控制命令 (MAIN -> PUA)
 */
typedef struct sdk_ptz_ctl_s {
	uint8_t    cmd;     /* SDK_PTZ_MSG_ID_E */
	uint8_t    speed;   /* 1 - 100 */
	uint8_t    res;     /* res */
	uint8_t    val;     /* preset, track ...  */
	uint8_t    name[32];/* preset, track name */
}sdk_ptz_ctl_t;


typedef struct sdk_28181_conf_t
{
	int enable;
	char server_ip[32];
	int server_port;
	char server_id[64];
	char device_id[64];
	char channel_nvr_id[36][64];
	char pwd[32];
	char alarm_id[32];
}sdk_28181_conf_t;

typedef struct _sdk_ntp_conf_t
{
	signed char time_zone;
	char ntp_server[128];
	signed char enable;
	char interval;		/* 时间间隔*/
	char date_format;	/* 日期格式:	年月日*/
	char date_separator;	/* 日期分隔符"-" */
	char time_format;	/* 时间格式: 24 小时, 12 小时*/
	int port;			/* NTP端口号 */
}sdk_ntp_conf_t;


typedef struct _sdk_email_conf_t
{	
	char smtp_server[128];
	char user[32];
	char passwd[32];
	char receiver[32];
	char sender[32];
	char theme[32];
	uint8_t anonymity;
	uint8_t file_enable;	
	uint8_t encryption_type;
	uint16_t interval;
	uint8_t health_enable;
	uint16_t health_interval;
	uint16_t port;
	uint8_t email_enable;
}sdk_email_conf_t;

typedef enum _P2P_TYPE
{	
	P2P_FREE_IP = 0x00,
	P2P_DANALE
}P2P_TYPE;

typedef struct _sdk_p2p_conf_t
{	
	uint8_t P2pEn;    // P2P使能
	char SerialNum[64]; // 序列号
	char AndroidAppUrl[128];// 安卓APP的下载地址
	char IosAppUrl[128];       //苹果APP的下载地址
}sdk_p2p_conf_t;

typedef struct _sdk_zoom_t
{
	int x;
	int y;
	int w;
	int h;
	int ch;
	int full_screen;
}sdk_zoom_t;


typedef struct _sdk_winswitch_t
{
	int win_num;
	int ch[4];
	int fullscreen;
}sdk_winswitch_t;

typedef struct sdk_playback_t
{
	int s32X;
	int s32Y;
	int u32Width;
	int u32Height;
	int  bMulti;
}sdk_playback_t;

typedef enum _ZOOM_OP
{
	SDK_MAIN_OP_ZOOMSTART,
	SDK_MAIN_OP_ZOOMSTOP,
	SDK_MAIN_OP_ZOOMMOVE,
}ZOOM_OP;
/* ---------------------------------*/



typedef struct _sdk_ipc_devinfo_t
{
	char Manufacturer[MAX_STRING_LEN];
	char Model[MAX_STRING_LEN];
	char FirmwareVersion[MAX_STRING_LEN];
	char SerialNumber[MAX_STRING_LEN];
	char HardwareId[MAX_STRING_LEN];
}sdk_ipc_devinfo_t;

typedef struct _sdk_modify_ip_t
{	
	uint16_t port;
	uint8_t old_ip_addr[MAX_IP_ADDR_LEN];//old  IP ADDR
	uint8_t new_ip_addr[MAX_IP_ADDR_LEN];//new IP ADDR
	uint8_t  macaddr[MAX_IP_ADDR_LEN];
	uint8_t mask[MAX_IP_ADDR_LEN];   //IP MASK
	uint8_t gateway[MAX_IP_ADDR_LEN];//网关
	uint8_t user_name[MAX_USER_NAME_LEN];
	uint8_t user_pwd[MAX_PASSWD_LEN];
	uint8_t index ;	
}sdk_modify_ip;

typedef struct sdk_network_s
{
  char ip_addr[MAX_IP_ADDR_LEN];
  char netmask[MAX_IP_ADDR_LEN];
  char gateway[MAX_IP_ADDR_LEN];
  char macaddr[MAX_IP_ADDR_LEN];

}sdk_network_t;

typedef struct sdk_channel_define_s
{
	unsigned char IpChannelEnable[MAX_CHANN_NUM];
	unsigned char AhdChannelEnable[MAX_CHANN_NUM];
}sdk_channel_define_t;

typedef struct _sdk_poll_t
{
	uint8_t enable;
	uint8_t time;
	uint8_t mode;
}sdk_poll_t;

 typedef enum _Ad_Hoc_Network_status_
{
 STATUS_INIT = 0,
 STATUS_RUNNING,
 STATUS_END
}Ad_Hoc_Network_status;
typedef struct
{
	char bFtpEnable;					///使能开关
	char servier_ip[40+1]; ///服务器地址
	char username[32]; 			///用户名
	char password[32]; 		///密码
	char foldername[128];		///文件夹地址
	int port; 							///服务器地址
} sdk_ftp_config;


typedef struct  sdk_stream_info_s
{
	unsigned int MainRecvBitRate;
	unsigned int SubRecvBitRate;
	unsigned int Height;
	unsigned int Width;
	unsigned int FrameRate;
}sdk_stream_info_t ;

/*移动侦测灵敏度*/
typedef struct sdk_MotionDetectionAnalytics_s{
    int    Sensitivity;
    int    Row;
    int    Column;
}sdk_MotionDetectionAnalytics_t;


typedef struct sdk_MotionDetectionRule_s{
	int  ararmOnDelay;
	int  ararmOffDelay;
	int  activeCell[32];
}sdk_MotionDetectionRule_t;











#ifdef __cplusplus
}
#endif

#endif //__sdk_struct_h__

