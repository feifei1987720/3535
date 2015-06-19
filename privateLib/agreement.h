/*option    操作*/
/*scope     范围*/
/*info      信息*/
/*property  配置*/

/*时区对照规则共 49 时区*/
/*
 * 0  - 24 依次表示 东 [12.0] [11.5] [11.0] ~ [1.0] [0.5] [0.0] 时区
 * 25 - 48 依次表示 西 [0.5] [1.0] [1.5] ~ [11.0] [11.5] [12.0] 时区
*/
#ifndef AGREEMENT_H
#define AGREEMENT_H

#ifdef __cplusplus
extern "C"
{
#endif

#define ARG_HEAD_LEN        32
#define ARG_SERIALNUM_LEN   8
#define ARG_USERNAME_LEN    32
#define ARG_PASSWD_LEN      32
#define ARG_IPSTR_LEN       16

/*服务器端口*/
#define ARG_SERVER_PORT                   10000
/*收索端口*/
#define ARG_SEARCH_PORT                   10001

/*指令类型*/
#define CMD_ACT_LOGIN                     0x0000
#define CMD_ACT_LOGOUT                    0x0001
#define CMD_ACT_CREATE_USER               0x0002
#define CMD_ACT_DELETE_USER               0x0003
#define CMD_ACT_MODIFY_USER               0x0004
#define CMD_ACT_QUERY_USER                0x0005
#define CMD_ACT_HEADRTBEAT                0x0006
#define CMD_ACT_SEARCH                	  0x0007
#define CMD_ACT_SEARCH_RESPONSE           0x0008
#define CMD_ACT_MOTIONDETECTION           0x0009
#define CMD_ACT_PTZ_OPTION                0x0020
#define CMD_ACT_PTZ_ADDPSPOINT            0x0021
#define CMD_ACT_PTZ_DELPSPOINT            0x0022

#define CMD_ACT_WIFILIST                  0x0030
#define CMD_ACT_WIFTADDNET                0x0031
#define CMD_ACT_WIFIDELNET                0x0032
#define CMD_ACT_WIFICONNECT               0x0033
#define CMD_ACT_WIFIDISCONNECT            0x0034
#define CMD_ACT_LOGFILE_QUERY             0x0040
#define CMD_ACT_LOGFILE_GET               0x0041
#define CMD_ACT_LOGFILE_DELETE            0x0042
#define CMD_ACT_PLAYBACK_QUERYFILES       0x0050
#define CMD_ACT_PLAYBACK_OPTION           0x0051
#define CMD_ACT_PLAYBACK_DOWNLOAD         0x0052
#define CMD_ACT_STORAGE_OPTION            0X0055

#define CMD_ACT_SEARCH_DEV                0x0056

#define CMD_GET_SERIALNUM                 0x0101
#define CMD_GET_STREAM                    0x0102

#define CMD_GET_TIME_PROPERTY             0x1000
#define CMD_SET_TIME_PROPERTY             0x1001
#define CMD_GET_ENCODING_PROPERTY         0x1002
#define CMD_SET_ENCODING_PROPERTY         0x1003
#define CMD_GET_IMAGE_PROPERTY            0x1004
#define CMD_SET_IMAGE_PROPERTY            0x1005
#define CMD_GET_NET_PROPERTY              0x1006
#define CMD_SET_NET_PROPERTY              0x1007
#define CMD_GET_MOTIONDETECTION_PROPERTY  0x1008
#define CMD_SET_MOTIONDETECTION_PROPERTY  0x1009
#define CMD_GET_SMTP_PROPERTY             0x1010
#define CMD_SET_SMTP_PROPERTY             0x1011
#define CMD_GET_FTP_PROPERTY              0x1012
#define CMD_SET_FTP_PROPERTY              0x1013
#define CMD_SET_STORAGE_PROPERTY          0x1014
#define CMD_GET_STORAGE_PROPERTY          0x1015
#define CMD_SET_AUDIO_PROPERTY            0x1016
#define CMD_GET_AUDIO_PROPERTY            0x1017
#define CMD_SET_DDNS_PROPERTY             0x1018
#define CMD_GET_DDNS_PROPERTY             0x1019
#define CMD_SET_VIDEOCOVER_PROPERTY       0x1020
#define CMD_GET_VIDEOCOVER_PROPERTY       0x1021
#define CMD_SET_DISPLAY_PROPERTY          0x1022
#define CMD_GET_DISPLAY_PROPERTY          0x1023
#define CMD_SET_SYSREBOOT_PROPERTY        0x1024
#define CMD_GET_SYSREBOOT_PROPERTY        0x1025
#define CMD_SET_P2P_PROPERTY              0x1026
#define CMD_GET_P2P_PROPERTY              0x1027
#define CMD_SET_STREAM_PROPERTY           0x1028
#define CMD_GET_STREAM_PROPERTY           0x1029
#define CMD_SET_PTZ_PROPERTY              0x1030
#define CMD_GET_PTZ_PROPERTY              0x1031
#define CMD_SET_REGULAR_RECORD_PROPERTY   0x1032
#define CMD_GET_REGULAR_RECORD_PROPERTY   0x1033
#define CMD_SET_REGULAR_SNAP_PROPERTY     0x1034
#define CMD_GET_REGULAR_SNAP_PROPERTY     0x1035
#define CMD_SET_AGREEMENT_PROPERTY        0x1036
#define CMD_GET_AGREEMENT_PROPERTY        0x1037
#define CMD_SET_ALARM_PROPERTY            0x1038
#define CMD_GET_ALARM_PROPERTY            0x1039
#define CMD_SET_LDC_PROPERTY              0x1040
#define CMD_GET_LDC_PROPERTY              0x1041
#define CMD_SET_ROI_PROPERTY              0x1042
#define CMD_GET_ROI_PROPERTY              0x1043
#define CMD_SET_VIDEOCOVER_EX_PROPERTY    0x1044
#define CMD_GET_VIDEOCOVER_EX_PROPERTY    0x1045
#define CMD_SET_VIDEO_OUTPUT_PROPERTY     0x1046
#define CMD_GET_VIDEO_OUTPUT_PROPERTY     0x1047
#define CMD_SET_CHANNEL_PROPERTY          0x1048
#define CMD_GET_CHANNEL_PROPERTY          0x1049
#define CMD_SET_PRESET_POINT_PROPERTY     0x1050
#define CMD_GET_PRESET_POINT_PROPERTY     0x1051
#define CMD_SET_CRUISE_LINE_PROPERTY      0x1052
#define CMD_GET_CRUISE_LINE_PROPERTY      0x1053

#define CMD_GET_CHANNEL_SCOPE             0x1100
#define CMD_GET_ENCODING_SCOPE            0x1102
#define CMD_GET_AGREEMENT_SCOPE           0x1103
#define CMD_GET_ALARM_SCOPE               0x1104

#define CMD_GET_TIME_INFO                 0x1200
#define CMD_GET_STORAGE_INFO              0x1201
#define CMD_GET_P2P_INFO                  0x1202

#define CMD_SYS_REBOOT                    0x2000
#define CMD_SYS_RESET                     0x2001
#define CMD_SYS_UPGRADE                   0x2002
#define CMD_SYS_GETDEVINFO                0x2003
#define CMD_SYS_SETDEVINFO                0x2004

#define CMD_GET_SYS_COMMON_PROPERTY       0x2010
#define CMD_SET_SYS_COMMON_PROPERTY       0x2011


/*命令状态(错误码)*/
#define CMD_SUCCESS            0x0000    //  成功
#define CMD_VERSION_ERROR      0x0001    //  版本错误
#define CMD_NOSUPPORT          0x0002    //  命令不支持
#define CMD_LIMITED            0x0003    //  命令权限不足
#define CMD_ERRUSERORPASSWD    0x0004    //  用户名或密码错误
#define CMD_ID_ERROR           0x0005    //  用户ID错误
#define CMD_ARGE_ERROR         0x0006    //  参数错误
#define CMD_ARGE_LIMITED       0x0007    //  参数不支持

/*SDK版本*/
#define ARG_SDK_VERSION(_major, _minor)( \
    (((_major)&0xff)<<8) | \
    (((_minor)&0xff)) \
)
#define ARG_SDK_VERSION_1_1 ARG_SDK_VERSION(1,1)
//#define ARG_SDK_VERSION_1_2 ARG_SDK_VERSION(1,2)
//#define ARG_SDK_CUR_VERSION ARG_SDK_VERSION_1_2


/*开关枚举*/
typedef enum ARG_SWITCH_TYPE{
    ARG_SWITCH_DISABLE         = 0x00,       /*关闭*/
    ARG_SWITCH_ENABLE          = 0x01,       /*开启*/
}ARG_SWITCH_TYPE;

/*语言类型枚举*/
typedef enum ARG_LANGUAGE_TYPE{
    ARG_LANGUAGE_CHINESE = 0x00,              //中文
    ARG_LANGUAGE_ENGLISH,                     //英文
    ARG_LANGUAGE_RUSSIAN,                     //俄文
    ARG_LANGUAGE_KOREAN,                      //韩文
    ARG_LANGUAGE_POLAND,                      //波兰文
    ARG_LANGUAGE_FRENCH,                      //法文
    ARG_LANGUAGE_JAPANSES,                    //日文
    ARG_LANGUAGE_SPAIN,                       //西班牙文
    ARG_LANGUAGE_PORTUGAL,                    //葡萄牙
    ARG_LANGUAGE_ITALY,                       //意大利
    ARG_LANGUAGE_HEBREW,                      //希伯来语
    ARG_LANGUAGE_TURKISH,                     //土耳其
    ARG_LANGUAGE_ARAB,                        //阿拉伯
    ARG_LANGUAGE_GERMAN,                      //德语
}ARG_LANGUAGE_TYPE;

/*位置枚举类型*/
typedef enum ARG_POSITON_TYPE{
    ARG_POSITION_LT = 0x00,                  //左上
    ARG_POSITION_MT,                         //中上
    ARG_POSITION_RT,                         //右上
    ARG_POSITION_LM,                         //左中
    ARG_POSITION_MM,                         //中中
    ARG_POSITION_RM,                         //右中
    ARG_POSITION_LB,                         //左下
    ARG_POSITION_MB,                         //中下
    ARG_POSITION_RB,                         //右下
}ARG_POSITON_TYPE;

/*设备枚举类型*/
typedef enum ARG_DEV_TYPE{
    ARG_DEV_IPCAMERA = 0x00,
    ARG_DEV_DVR,
    ARG_DEV_DVR_CAR,
    ARG_DEV_NVR,
    ARG_DEV_NVR_CAR,

    ARG_DEV_DECODER  = 0x10,
    ARG_DEV_PC,
}ARG_DEV_TYPE;

/*平台枚举类型*/
typedef enum ARG_PLATFORM_TYPE{
    ARG_PLATFORM_PC = 0,

    ARG_PLATFORM_HISI3516A = 5,
    ARG_PLATFORM_HISI3518E,
    ARG_PLATFORM_HISI3531,
    ARG_PLATFORM_HISI3521,
    ARG_PLATFORM_HISI3535,
    ARG_PLATFORM_HISI3520D,
    ARG_PLATFORM_HISI3516C,
    ARG_PLATFORM_HISI3518C,

    ARG_PLATFORM_NXP8850 = 40,

    ARG_PLATFORM_AMBA_A5 = 60,
    ARG_PLATFORM_AMBA_S2L,

    ARG_PLATFORM_SONY4145 = 80,

    ARG_PLATFORM_GM8136 = 100,
}ARG_PLATFORM_TYPE;

/*编码枚举类型*/
typedef enum ARG_ENCODING_TYPE{
    ARG_ENCODING_H264 = 0x00,
    ARG_ENCODING_H265,
    ARG_ENCODING_MPEG,

    ARG_ENCODING_G711A = 0x10,
    ARG_ENCODING_G711U,
    ARG_ENCODING_G726,
    ARG_ENCODING_ADPCM,

    ARG_ENCODING_JPEG = 0x20,
    ARG_ENCODING_PNG,
    ARG_ENCODING_BMP,
}ARG_ENCODING_TYPE;
/*倍率枚举类型*/
typedef enum ARG_MULTIPLY_TYPE{
    ARG_MULTIPLY_P_1_1 = 0,            /*正  1/1 倍*/
    ARG_MULTIPLY_P_2_1,                /*正  2/1 倍*/
    ARG_MULTIPLY_P_4_1,                /*正  4/1 倍*/
    ARG_MULTIPLY_P_8_1,                /*正  8/1 倍*/
    ARG_MULTIPLY_P_16_1,               /*正  16/1 倍*/
    ARG_MULTIPLY_P_32_1,               /*正  32/1 倍*/
    ARG_MULTIPLY_P_64_1,               /*正  64/1 倍*/
    ARG_MULTIPLY_P_128_1,              /*正  128/1 倍*/

    ARG_MULTIPLY_P_1_2 = 10,           /*正  1/2 倍*/
    ARG_MULTIPLY_P_1_4,                /*正  1/4 倍*/
    ARG_MULTIPLY_P_1_8,                /*正  1/8 倍*/
    ARG_MULTIPLY_P_1_16,               /*正  1/16 倍*/
    ARG_MULTIPLY_P_1_32,               /*正  1/32 倍*/
    ARG_MULTIPLY_P_1_64,               /*正  1/64 倍*/
    ARG_MULTIPLY_P_1_128,              /*正  1/128 倍*/

    ARG_MULTIPLY_N_2_1 = 20,           /*负  2/1 倍*/
    ARG_MULTIPLY_N_4_1,                /*负  4/1 倍*/
    ARG_MULTIPLY_N_8_1,                /*负  8/1 倍*/
    ARG_MULTIPLY_N_16_1,               /*负  16/1 倍*/
    ARG_MULTIPLY_N_32_1,               /*负  32/1 倍*/
    ARG_MULTIPLY_N_64_1,               /*负  64/1 倍*/
    ARG_MULTIPLY_N_128_1,              /*负  128/1 倍*/

    ARG_MULTIPLY_N_1_2 = 30,           /*负  1/2 倍*/
    ARG_MULTIPLY_N_1_4,                /*负  1/4 倍*/
    ARG_MULTIPLY_N_1_8,                /*负  1/8 倍*/
    ARG_MULTIPLY_N_1_16,               /*负  1/16 倍*/
    ARG_MULTIPLY_N_1_32,               /*负  1/32 倍*/
    ARG_MULTIPLY_N_1_64,               /*负  1/64 倍*/
    ARG_MULTIPLY_N_1_128,              /*负  1/128 倍*/
}ARG_MULTIPLY_TYPE;

/*事件类型枚举*/
typedef enum ARG_EVENT_TYPE{
	ARG_EVENT_NULL = 0x00,             /*无*/
}ARG_EVENT_TYPE;

/*存储类型枚举*/
typedef enum ARG_STORAGE_TYPE{
    ARG_STORAGE_NULL      =  0x00,   /*无*/
    ARG_STORAGE_SDCARE    =  0x01,   /*sd卡 */
    ARG_STORAGE_USBDISK   =  0x02,   /*usb */
    ARG_STORAGE_HARDDISK  =  0x04,   /*硬盘 */
    ARG_STORAGE_FTP       =  0x08,   /*ftp */
    ARG_STORAGE_SMTP      =  0x10,   /*smtp */
    ARG_STORAGE_ALL       =  0x1f    /*所有*/
}ARG_STORAGE_TYPE;

/*旋转角度枚举*/
typedef enum ARG_ROTATE_TYPE{
    ARG_ROTATE_NULL   = 0x0,        /*无旋转*/

    ARG_ROTATE_P_45   = 0x10,       /*正 45 度*/
    ARG_ROTATE_P_90,                /*正 90 度*/
    ARG_ROTATE_P_135,               /*正 135 度*/
    ARG_ROTATE_P_180,               /*正 180 度*/
    ARG_ROTATE_P_225,               /*正 225 度*/
    ARG_ROTATE_P_270,               /*正 270 度*/
    ARG_ROTATE_P_315,               /*正 315 度*/
    ARG_ROTATE_P_360,               /*正 360 度*/

    ARG_ROTATE_N_45   = 0x20,       /*负 45 度*/
    ARG_ROTATE_N_90,                /*负 90 度*/
    ARG_ROTATE_N_135,               /*负 135 度*/
    ARG_ROTATE_N_180,               /*负 180 度*/
    ARG_ROTATE_N_225,               /*负 225 度*/
    ARG_ROTATE_N_270,               /*负 270 度*/
    ARG_ROTATE_N_315,               /*负 315 度*/
    ARG_ROTATE_N_360,               /*负 360 度*/
}ARG_ROTATE_TYPE;

/*协议类型枚举*/
typedef enum ARG_AGREEMENT_TYPE{
    ARG_AGREEMENT_NULL        = 0x00,        /*无*/

    ARG_AGREEMENT_RTSP        = 0x01,        /*rtsp协议*/
    ARG_AGREEMENT_XM          = 0x02,        /*大拿协议*/
    ARG_AGREEMENT_PRIVATE     = 0x04,        /*私有协议*/
    ARG_AGREEMENT_ONVIF       = 0x08,        /*onvif 协议*/
    ARG_AGREEMENT_HTTP        = 0x10,        /*http 协议*/
    ARG_AGREEMENT_TCP         = 0x20,        /*tcp 协议*/

    ARG_AGREEMENT_ALL         = 0x1f         /*所有协议*/
}ARG_AGREEMENT_TYPE;

/*报警枚举类型*/
typedef enum ARG_ALARM_TYPE{
    ARG_ALARM_NULL               = 0x00,        /*无*/
    ARG_ALARM_MOTIONDETECTION    = 0x01,        /*移动侦测报警*/
    ARG_ALARM_IO                 = 0x02,        /*io 报警*/
    ARG_ALARM_VIDEOCOVER         = 0x04,        /*视频遮挡报警*/

    ARG_ALARM_ALL                = 0x07,        /*所有*/
}ARG_ALARM_TYPE;

/*码流类型枚举*/
typedef enum ARG_STREAM_TYPE{
    ARG_STREAM_VIDEOSTREAM_0 = 0,      /*视频主码流*/
    ARG_STREAM_VIDEOSTREAM_1,          /*视频次码流*/
    ARG_STREAM_VIDEOSTREAM_2,          /*视频第三码流*/
    ARG_STREAM_VIDEOSTREAM_3,          /*视频第四码流*/

    ARG_STREAM_AUDIOSTREAM_0 = 10,     /*音频第一码流*/
    ARG_STREAM_AUDIOSTREAM_1,          /*音频第二码流*/
    ARG_STREAM_AUDIOSTREAM_2,          /*音频第三码流*/
    ARG_STREAM_AUDIOSTREAM_3,          /*音频第四码流*/

    ARG_STREAM_ALL  = 50,              /*所有*/
	ARG_STREAM_VIDEO_ALL,              /*视频所有*/
	ARG_STREAM_AUDIO_ALL,              /*音频所有*/
    ARG_STREAM_NULL,                   /*无*/
}ARG_STREAM_TYPE;

/*通信头类型枚举*/
typedef enum ARG_HEAD_TYPE{
    ARG_HEAD_CMD             = 0,      /*命令头*/
    ARG_HEAD_STREAM,                   /*流头*/

    ARG_HEAD_CMD2            = 10,     /*命令头2*/
    ARG_HEAD_VIDEOSTREAM,              /*视频流头*/
    ARG_HEAD_AUDIOSTREAM,              /*音频流头*/
    ARG_HEAD_TRANSPORT,                /*传输头*/
}ARG_HEAD_TYPE;


/*分辨率枚举*/
typedef enum ARG_RESOLUTION_TYPE{
    ARG_RESOLUTION_QVGA     =  0x00,   /*qvga    320*240*/
    ARG_RESOLUTION_WQVGA,              /*wqvga   400*240*/
    ARG_RESOLUTION_HVGA,               /*hvga    480*320*/
    ARG_RESOLUTION_VGA,                /*vga     640*480*/
    ARG_RESOLUTION_WVGA,               /*wvga    800*480*/
    ARG_RESOLUTION_SVGA,               /*svga    800*600*/
    ARG_RESOLUTION_WSVAG,              /*wsvga   1024*600*/
    ARG_RESOLUTION_XGA,                /*xga     1024*768*/

    ARG_RESOLUTION_720P     =  0x10,   /*720P    1280*720*/
    ARG_RESOLUTION_WXGA,               /*wxga    1280*800*/
    ARG_RESOLUTION_SXGA,               /*sxga    1280*1024*/
    ARG_RESOLUTION_WXGA_PLUS,          /*wxga+   1400*900*/
    ARG_RESOLUTION_SXGA_PLUS,          /*sxga+   1400*1050*/
    ARG_RESOLUTION_WSXGA,              /*wsxga   1600*1024*/
    ARG_RESOLUTION_WSXGA_PLUS,         /*wsxga   1680*1050*/
    ARG_RESOLUTION_UXGA,               /*uxga    1600*1200*/

    ARG_RESOLUTION_1080P    =  0x20,   /*1080P   1920*1080*/
    ARG_RESOLUTION_WUXGA,              /*wuxga   1920*1200*/
    ARG_RESOLUTION_QXGA,               /*qxga    2048×1536*/
    ARG_RESOLUTION_WQXGA,              /*wuxga   2560*1600*/
}ARG_RESOLUTION_TYPE;

/*数值特殊意义*/
typedef enum ARG_TYPICALNUM_TYPE{
    ARG_TYPICALNUM_INVALID          =   0xf0,   /*无效的*/

    ARG_TYPICALNUM_ALL              =   0xf1,   /*全部*/
    ARG_TYPICALNUM_ENABLE,
    ARG_TYPICALNUM_DISABLE,
}ARG_TYPICALNUM_TYPE;


/********************************************************************************************/
/*******************************         区域         ***************************************/
/********************************************************************************************/
/*矩形结构体*/
typedef struct ARG_RECT{
    unsigned long x1;                         /*左上角x*/
    unsigned long y1;                         /*左上角y*/
    unsigned long x2;                         /*右下角x*/
    unsigned long y2;                         /*右下角y*/
}ARG_RECT;
/*区域划定方式枚举*/
typedef enum AREA_DELIMIT_TYPE{
    AREA_DELIMIT_PIXAL      =    0x00,        /*实际坐标*/

    AREA_DELIMIT_HUNDRED     =   0x10,        /*百分比*/
    AREA_DELIMIT_THOUSAND,                    /*千分比*/
    AREA_DELIMIT_TEN_THOUSAND,                /*万分比*/

    AREA_DELIMIT_8_8         =   0x20,        /*块选 8*8*/
    AREA_DELIMIT_22_18,                       /*块选 22*18*/
}AREA_DELIMIT_TYPE;
/* 区域信息
 * 根据区域选定方式 ucDelimitType 有不同的表示方式.
 * ucDelimitType 的值为 AREA_DELIMIT_PIXAL 时. area.rect[8] 表示八组实际的坐标
 * ucDelimitType 的值为 AREA_DELIMIT_HUNDRED.  area.rect[8] 表示八组百分比区域
 * ucDelimitType 的值为 AREA_DELIMIT_8_8       area.ulBlock 表示 8*8 的块选区域.
*/
typedef struct AREA_INFO{
    unsigned char        ucDelimitType;       /*区域切分方式*/
    union{
        ARG_RECT         rect[8];             /*8组 矩形区域*/
        unsigned long    ulBlock[32];         /*最多32*32的块选区域, ulBlock[0]的第0位为1 表示从上到下第一行的左边第一个块选中*/
    }area;
    unsigned char        ucRes1[16];
}AREA_INFO;
/********************************************************************************************/
/*******************************        时间结构       ***************************************/
/********************************************************************************************/
typedef enum TIME_WEEK_DAY{
    TIME_WEEK_SUN         = (1<<1),             /*星期天*/
    TIME_WEEK_MON         = (1<<2),             /*星期一*/
    TIME_WEEK_TUE         = (1<<3),             /*星期二*/
    TIME_WEEK_WED         = (1<<4),             /*星期三*/
    TIME_WEEK_THU         = (1<<5),             /*星期四*/
    TIME_WEEK_FRI         = (1<<6),             /*星期五*/
    TIME_WEEK_SAT         = (1<<7),             /*星期六*/
}TIME_WEEK_DAY;

typedef enum TIME_SCOPE_MODE{
    TIME_SCOPE_NULL       = 0,                  /*无时间段*/
    TIME_SCOPE_ALL,                             /*全天时间段*/
    TIME_SCOPE_SAME,                            /*按第一个时间段*/
    TIME_SCOPE_DIFF,                            /*按每个具体时间段*/
}TIME_SCOPE_MODE;

/*小时信息*/
typedef struct TIME_HOUR_INFO{
    unsigned char         ucHour;               /*时: 0-23*/
    unsigned char         ucMinute;             /*分: 0-59*/
    unsigned char         ucSecond;             /*秒: 0-59*/
    unsigned char         ucRes[1];             /*保留*/
}TIME_HOUR_INFO;

/*日期信息*/
typedef struct TIME_DATE_INFO{
    unsigned short        usYear;               /*年: */
    unsigned char         ucMonth;              /*月: */
    unsigned char         ucDay;                /*日: */
}TIME_DATE_INFO;

/*时间信息*/
typedef struct TIME_INTO{
    TIME_DATE_INFO        date;                 /*日期*/
    TIME_HOUR_INFO        hour;                 /*时间*/
}TIME_INTO;

/*小时范围*/
typedef struct TIME_HOUR_SCOPE{
    TIME_HOUR_INFO        beginHour;            /*开始时间*/
    TIME_HOUR_INFO        endHour;              /*结束时间*/
}TIME_HOUR_SCOPE;

/*天小时范围*/
typedef struct TIME_DAY_HOUR_SCOPE{
    unsigned char         ucSocpeMode;          /*时间模式*/
    unsigned char         ucRes1[3];            /*保留*/
    TIME_HOUR_SCOPE       hourScope[6];         /*一天最多有6个时段*/
}TIME_DAY_HOUR_SCOPE;

/*周时间表*/
typedef struct TIME_WEEK_SCOPE{
    unsigned char         ucScopeModel;         /*时间模式*/
    unsigned char         ucRes1[3];            /*保留*/
    TIME_DAY_HOUR_SCOPE   dayScope[7];          /*一周7天. 0到6 表示星期天到星期六*/
}TIME_ALARM_SCOPE;
/********************************************************************************************/
/*******************************         命令头        ***************************************/
/********************************************************************************************/
/*命令头结构体*/
typedef struct ARG_CMD{
    unsigned short ulFlag;            //  00 00
    unsigned short ulVersion;         //  SDK 版本

    unsigned short usCmd;             //  命令类型
    unsigned short ucState;           //  命令状态
    unsigned long  ulID;              //  用户 ID

    unsigned long  ulBufferSize;      //  扩展内容大小(总大小等于 ulBufferSize*ulBufferNum)
	unsigned long  ulBufferNum;       //  扩展内容个数.

    unsigned long  ulRes[3];          //  没有使用
}ARG_CMD;
/*码流状态(暂时没有)*/
/*A/V*/
#define AUDIO_FRAME  0
#define VEDIO_FRAME  1
#define ARG_STREAM_MAGIC_NUM          0x00000024

/*帧类型枚举*/
typedef enum ARG_FRAME_TYPE{
    ARG_FRAME_KEY          =  0x00,    /*关键帧*/
    ARG_FRAME_NON_KEY      =  0x01,    /*非关键帧*/
}ARG_FRAME_TYPE;

typedef struct ARG_STREAM{
    unsigned short  ulFlag;            //  00 01

    unsigned char   ucState;           //  码流状态
    unsigned char   ucRes[1];          //  保留
    unsigned long   bMediaType:1;      //  A/V
    unsigned long   bFrameType:5;      //  填写 (ARG_FRAME_TYPE)
    unsigned long   bSize:22;          //  帧长度
    unsigned long   bSubStream:2;      //  子码流
    unsigned long   bRes:2;            //  保留
                                      
    unsigned char   usCH;              //  通道号 0 1 2 3
    unsigned char   usEncodeType;      //  编码类型 (ARG_ENCODING_TYPE) 
    unsigned char   usRes[2];          //  保留 字节对齐

    unsigned long   ucSerialNum;       //  帧序列
    unsigned long   ulTimeStamp;       //  时间戳

    unsigned short  usWidth;           //  宽度
    unsigned short  usHeight;          //  高度
    unsigned short  ucFrameRate;       //  帧率
    unsigned short  ucBitRate;         //  码率

    unsigned long   ulMagicNum;        //  用于验证头部正确 (数值为 ARG_STREAM_MAGIC_NUM 时正常)
}ARG_STREAM;

/********************************************************************************************/
/******************************* 新版本命令头(暂时不使用) ***************************************/
/********************************************************************************************/
typedef enum ARG_ERRORCODE_TYPE{
    ARG_ERRORCODE_SUCCESS               =  0x00,      /*成功*/
    ARG_ERRORCODE_VERSION_ERROR,                      /*版本错误*/

    ARG_ERRORCODE_USERPASSWD_ERROR      =  0x10,      /*错误的用户名密码*/
    ARG_ERRORCODE_ID_ERROR,                           /*用户未登录*/
    ARG_ERRORCODE_ID_LIMITED,                         /*用户权限不足*/

    ARG_ERRORCODE_CMD_NOSUPPORT         =  0x20,      /*命令不支持*/
    ARG_ERRORCODE_ARGE_ERROR,                         /*参数错误*/
    ARG_ERRORCODE_ARGE_NOSUPPORT,                     /*参数不支持*/
}ARG_ERRORCODE_TYPE;

typedef struct CMD_HEAD_INFO{
    unsigned short usCmd;                             /*命令类型:*/
    unsigned char  ucRole;                            /*命令角色: (0 请求)(1 回复)*/
    unsigned char  ucErrCode;                         /*命令状态: 填写 ARG_ERRORCODE_TYPE*/
    unsigned long  ulID;                              /*用户id号*/

    unsigned char  ucTCPCh;                           /*通信链路号*/
    unsigned char  ucCh;                              /*命令通道*/
    unsigned char  ucRes[2];                          /*保留*/
}CMD_HEAD_INFO;

typedef enum ARG_PACKINGSIZE_TYPE{
    ARG_PACKINGSIZE_NULL = 0x00,                      /*任意大小*/
    ARG_PACKINGSIZE_1K,                               /*1K*/
    ARG_PACKINGSIZE_10K,                              /*10K*/
    ARG_PACKINGSIZE_100K,                             /*100K*/
    ARG_PACKINGSIZE_1M,                               /*1M*/
    ARG_PACKINGSIZE_10M,                              /*10M*/
    ARG_PACKINGSIZE_100M,                             /*100M*/
    ARG_PACKINGSIZE_ALL,                              /*发完为止*/
}ARG_PACKINGSIZE_TYPE;

typedef enum ARG_TRANSPORT_TYPE{
    ARG_TRANSPORT_NULL      = 0x00,                   /*不传*/
    ARG_TRANSPORT_UPLOAD    = 0x01,                   /*上传*/
    ARG_TRANSPORT_DOWNLOAD  = 0x02                    /*下载*/
}ARG_TRANSPORT_TYPE;

typedef struct TRANSPORT_UPLOAD_INFO{
    unsigned long  ulTotleSize;                       /*总大小*/
    unsigned char  ucFileName[64];                    /*文件名*/
}TRANSPORT_UPLOAD_INFO;

typedef struct TRANSPORT_DOWNLOAD_INFO{
    unsigned long  ulTotleSize;                       /*总大小*/
    unsigned char  ucFileName[64];                    /*文件名*/
}TRANSPORT_DOWNLOAD_INFO;

typedef struct TRANSPORT_INFO{
    unsigned char  ucType;                            /*传输模式: ARG_TRANSPORT_TYPE*/
    unsigned char  ucPackSize;                        /*打包大小: ARG_PACKINGSIZE_TYPE*/
    unsigned char  ucRes1[2];                         /*保留1*/
    unsigned long  ulId;                              /*标示*/
    union{
        TRANSPORT_UPLOAD_INFO   up;                   /*上传信息*/
        TRANSPORT_DOWNLOAD_INFO down;                 /*下载信息*/
        unsigned char ucPlaceHolder[128];             /*占位符号*/
    }info;
    unsigned char  ucRes2[16];                        /*保留2*/
 }TRANSPORT_INFO;

typedef struct TRANSPORT_HEAD_INFO{
    unsigned long  ulTotleByte;                       /*总大小*/
    unsigned long  ulFinByte;                         /*数据起点*/
    unsigned long  ulId;                              /*标示*/

    unsigned char  ucPackSize;                        /*打包大小: ARG_PACKINGSIZE_TYPE*/
    unsigned char  ucRes1[3];                         /*保留*/
}TRANSPORT_HEAD_INFO;

typedef struct VIDEOSTREAM_HEAD_INFO{
    unsigned char  ucCh;                             /*通道号*/
    unsigned char  ucStreamType;                     /*流类型: ARG_STREAM_TYPE*/
    unsigned char  ucEncodeType;                     /*编码类型: ARG_ENCODING_TYPE*/
    unsigned char  ucFrameType;                      /*帧类型: 0非关键 1关键*/

    unsigned long  ucSerialNum;                      /*帧序列*/
    unsigned long  ulTimeStamp;                      /*时间戳: 当前帧较第一帧的时间偏移量 单位为毫秒*/
}VIDEOSTREAM_HEAD_INFO;

typedef struct AUDIOSTREAM_HEAD_INFO{
    unsigned char  ucCh;                             /*通道号*/
    unsigned char  ucStreamType;                     /*流类型: ARG_STREAM_TYPE*/
    unsigned char  ucEncodeType;                     /*编码类型: ARG_ENCODING_TYPE*/
    unsigned char  ucRes1[1];                        /*保留*/

    unsigned long  ucSerialNum;                      /*帧序列*/
    unsigned long  ulTimeStamp;                      /*时间戳: 当前帧较第一帧的时间偏移量 单位为毫秒*/
}AUDIOSTREAM_HEAD_INFO;

#define ARG_HEAD_MAGIC_NUM    0x1314
typedef struct ARG_HEAD{
    unsigned short  usHeadType;                      /*通信头类型: 填写 ARG_HEAD_TYPE*/
    unsigned short  usVersion;                       /*通信头版本*/
    unsigned short  usVerify;                        /*通信头校验位: 填写 ARG_HEAD_MAGIC_NUM (0x1314)*/
    unsigned char   ucRes1[1];                       /*保留1*/
    unsigned char   ulBuffNum;                       /*附带buff个数*/
    unsigned long   ulBuffSize;                      /*附带buff大小*/

    union{
        CMD_HEAD_INFO          cmd;                  /*命令*/
        VIDEOSTREAM_HEAD_INFO  video;                /*视频*/
        AUDIOSTREAM_HEAD_INFO  audio;                /*音频*/
        TRANSPORT_HEAD_INFO    transport;            /*传输*/
        unsigned long ulPlaceHolder[5];              /*占位符号*/
    }head;
}ARG_HEAD;

typedef struct SEARCH_INFO{
    unsigned char ucDevType;                         /*设备类型*/
    unsigned char ucDevName[35];                     /*设备名称*/

    unsigned long ulWebPort;                         /*网页端口*/
    unsigned long ulRtspPort;                        /*rtsp端口*/
    unsigned long ulTcpPort;                         /*tcp端口*/

    unsigned long ulIP;                              /*IP地址*/
    unsigned long ulNetMash;                         /*子网掩码*/
    unsigned long ulGateway;                         /*网关*/
    unsigned char ucMacAddress[6];                   /*mac地址*/
    unsigned char ucRes[2];                          /*保留*/
}SEARCH_INFO;

/********************************************************************************************/
/*******************************        用户管理       ***************************************/
/********************************************************************************************/
/*用户角色*/
typedef enum USER_ROLE_TYPE{
    USER_ROLE_ROOT    =  0x00,       /*超级管理员*/
    USER_ROLE_ADMIN   =  0x01,       /*管理员*/
    USER_ROLE_USER    =  0x02,       /*用户*/
}USER_ROLE_TYPE;

/*用户权限*/
typedef enum USER_POWER_TYPE{
    USER_POWER_REBOOT_OPTION                    = (1<<1),  /*重启操作*/
    USER_POWER_UPGRADE_OPTION                   = (1<<2),  /*升级操作*/
    USER_POWER_RESET_OPTION                     = (1<<3),  /*重置操作*/


    USER_POWER_USER_OPTION                      = (1<<8),  /*用户管理*/
    USER_POWER_STORAGE_OPTION                   = (1<<9),  /*存储配置*/
    USER_POWER_NET_CONFIG                       = (1<<10), /*网络配置*/
    USER_POWER_PTZ_OPTION                       = (1<<11), /*云台操作*/
    USER_POWER_SYS_COMMON_CONFIG                = (1<<12), /*系统一般配置*/
    USER_POWER_AUTO_MAINTAIN                    = (1<<13), /*自动维护*/

    USER_POWER_VIDEO_RECORDING_DOWNLOAD         = (1<<16), /*录像下载*/
    USER_POWER_VIDEO_RECORDING_CONFIG           = (1<<17), /*录像设置*/
    USER_POWER_VIDEO_OUPUT_CONFIG               = (1<<18), /*视频输出配置*/
    USER_POWER_LOGFILE_OPTION                   = (1<<19), /*日志文件操作*/
    USER_POWER_TERNINAL_CONFIG                  = (1<<20), /*前端配置*/
    USER_POWER_CHANNEL_CONFIG                   = (1<<21), /*通道配置*/
    USER_POWER_PRIVIEW_CONFIG                   = (1<<22), /*预览配置*/
}USER_POWER_TYPE;

#if defined  ARG_SDK_VERSION_1_1
typedef struct USER_INFO{
    unsigned char ucUsername[ARG_USERNAME_LEN];      /*用户名*/
    unsigned char ucPassWord[ARG_PASSWD_LEN];        /*密码*/
    unsigned char ucPower;                           /*权限  0管理员1 操作者 2用户*/
    unsigned char ucRes[3];

    unsigned char ucSerialNum[ARG_SERIALNUM_LEN];
}USER_INFO;
#elif defined ARG_SDK_VERSION_1_2
/*用户信息*/
typedef struct USER_INFO{
    unsigned char ucUsername[ARG_USERNAME_LEN];      /*用户名*/
    unsigned char ucPassWord[ARG_PASSWD_LEN];        /*密码*/
    unsigned char ucRole;                            /*用户角色: USER_ROLE_TYPE*/
    unsigned char ucRes1[3];                         /*保留1*/
    unsigned long ulPower;                           /*权限, 填写 USER_POWER_TYPE*/
    unsigned char ucRes2[4];                         /*保留2*/
}USER_INFO;
/*登录信息*/
typedef struct LOGIN_INFO{
    USER_INFO     userInfo;                          /*用户信息*/
    unsigned char ucSerialNum[ARG_SERIALNUM_LEN];
}LOGIN_INFO;
#endif



typedef struct STREAM_INFO{
    unsigned char  ucCH;                  /*通道号*/
    unsigned char  ucVideoStreamType;     /*视频码流类型*/
    unsigned char  ucAudioStreamType;     /*音频码流类型*/
    unsigned char  ucRes[1];              /*保留*/
}STREAM_INFO;


/*日志过滤条件*/
typedef enum LOG_EVENT_TYPE{
    LOG_EVENT_NULL   = 0x00,         /*无*/
    LOG_EVENT_SYS    = 0x01,         /*系统日志过滤*/
    LOG_EVENT_USER   = 0x02,         /*用户日志过滤*/
    LOG_EVENT_LOG    = 0x04,         /*配置日志过滤*/
    LOG_EVENT_FILE   = 0x08,         /*文件日志过滤*/
    LOG_EVENT_STAT   = 0x10,         /*状态日志过滤*/
    LOG_EVENT_ALARM  = 0x20,         /*报警日志过滤*/
    LOG_EVENT_ALL    = 0x3f          /*所有日志*/
}LOG_EVENT_TYPE;

typedef struct LOG_GREP_INFO{
    unsigned long ulLanguage;        /*mes语言*/

    unsigned long ulStartNo;         /*开始序号*/
    unsigned long ulNum;             /*数目*/

    unsigned long ulStartTime;       /*开始时间*/
    unsigned long ulEndTime;         /*结束时间*/

    unsigned char ulGrepType;        /*过滤类型*/
    unsigned char ucRes[3];          /*保留*/
}LOG_GREP_INFO;

typedef struct LOG_INFO{
	unsigned long totleNum;          /*日志文件总数*/
    unsigned long suitableNum;       /*匹配的日志文件总数*/

	unsigned long ulLanguage;        /*日志文件语言*/
	unsigned long ulEncodeType;      /*日志文件语言编码类型  0(UTF8)  1GB2312*/          
}LOG_INFO;

typedef struct LOG_EVENT_INFO{
    unsigned long ulNo;              /*序号*/
    unsigned long ulType;            /*类型*/
    unsigned long ulCH;              /*通道*/
    unsigned long ulTIme;            /*返回自从 Unix 纪元（格林威治时间 1970 年 1 月 1 日 00:00:00）到当前时间的秒数.*/
    unsigned char ucMes[100];        /*事件描述*/
}LOG_EVENT_INFO;

/*录像过滤条件*/
typedef enum ARG_PLAYBACK_TYPE{
    ARG_PLAYBACK_ALL    = 0x07,     /*所有录像*/
    ARG_PLAYBACK_ALARM  = 0x01,     /*报警录像*/
    ARG_PLAYBACK_TIME   = 0x02,     /*定时录像*/
    ARG_PLAYBACK_MANUAL = 0x04,     /*人工录像*/
}ARG_PLAYBACK_TYPE;

typedef struct PLAYBACK_GREP_INFO{
    unsigned char  ucCH;              /*通道*/
    unsigned char  ucRes[1];          /*保留*/
    unsigned short ulType;            /*类型*/

    unsigned long  ulStartNo;         /*开始序号*/
    unsigned long  ulNum;             /*数目*/
    unsigned long  ulStartTime;       /*录像开始时间*/
    unsigned long  ulStopTime;        /*录像结束时间*/
    unsigned long  ulMaxFileLen;      /*最大文件长度*/
    unsigned long  ulMinFileLen;      /*最小文件长度*/

    unsigned long  ulRes2[4];         /*保留2*/
}PLAYBACK_GREP_INFO;

typedef struct PLAYBACK_INFO{
    unsigned long  totleNum;          /*录像文件总数*/
    unsigned long  suitableNum;       /*匹配的录像文件总数*/

    unsigned long  ulRes2[4];         /*保留2*/
}PLAYBACK_INFO;

typedef struct PLAYBACK_FILE_INFO{
    unsigned char  ucCh;              /*通道*/
    unsigned char  ucRes[1];          /*保留*/
    unsigned short ulType;            /*类型*/

    unsigned long  ulNo;              /*文件序号*/
    unsigned long  ulStartTime;       /*开始时间*/
    unsigned long  ulStopTime;        /*结束时间*/
    unsigned long  ulFileLen;         /*文件大小: 单位字节*/

    unsigned long  ulRes2[4];         /*保留2*/
    unsigned char  ucFileName[128];   /*文件名*/
}PLAYBACK_FILE_INFO;
//ALARM-chn-starttime-endtime
//NORMAL-chn-starttime-endtime

/*回放操作类型*/
typedef enum PLAYBACK_OPTION_TYPE{
    PLAYBACK_OPTION_STAR            = 0x00,       /*开始*/
    PLAYBACK_OPTION_STOP,                         /*结束*/
    PLAYBACK_OPTION_FAST_REVERSE,                 /*快进*/
    PLAYBACK_OPTION_FAST_FORWARD,                 /*快退*/
    PLAYBACK_OPTION_FRAME_REVERSE,                /*帧进*/
    PLAYBACK_OPTION_FRAME_FORWARD,                /*帧腿*/
    PLAYBACK_OPTION_SEEK,                         /*拖动*/
    PLAYBACK_OPTION_SUSPEND,                      /*暂停*/
    PLAYBACK_OPTION_CONTINUE,                     /*继续播放*/
}PLAYBACK_OPTION_TYPE;

typedef struct PLAYBACK_START_INFO{
    unsigned long   ulFileNo;          /*文件序号*/
    unsigned char   ucCh;              /*通道号*/
    unsigned char   ucRes1[1];         /*保留1*/
    unsigned short  usFileType;        /*文件类型*/

    unsigned long   ulStartTime;       /*录像开始时间*/
    unsigned long   ulStopTime;        /*录像结束时间*/
}PLAYBACK_START_INFO;

typedef struct PLAYBACK_SEEK_INFO{
    long   lSeekTime;         /*拖动时间: 单位为毫秒*/
}PLAYBACK_SEEK_INFO;

typedef struct PLAYBACK_MULTIPLY_INFO{
    unsigned char   ucMultiplying;     /*填写 ARG_MULTIPLY_TYPE的(P)部分*/
    unsigned char   ucRes1[3];         /*保留1*/
}PLAYBACK_MULTIPLY_INFO;

typedef struct PLAYBACK_DOWNLOAD_INFO{
    unsigned long   ulFileNo;          /*文件序号*/
    unsigned char   ucCh;              /*通道号*/
    unsigned char   ucRes1[1];         /*保留1*/
    unsigned short  usFileType;        /*文件类型*/

    unsigned long   ulStartTime;       /*录像开始时间*/
    unsigned long   ulStopTime;        /*录像结束时间*/
}PLAYBACK_DOWNLOAD_INFO;

typedef struct PLAYBACK_OPTION{
    unsigned short  usOptType;         /*操作类型*/
    unsigned char   ucRes[18];         /*保留*/
    union{
        PLAYBACK_START_INFO      start;          /*开始*/
        PLAYBACK_SEEK_INFO       seek;           /*拖动*/
        PLAYBACK_MULTIPLY_INFO   multiply;       /*倍率播放*/
        unsigned long  ulPlaceholder[8];         /*占位*/
    }info;
}PLAYBACK_OPTION;

typedef struct WIFI_INFO{
	unsigned char ucSSID[128];        /*wifi 名称*/
	unsigned char ucKey[128];         /*连接秘钥*/
	unsigned long ulStrength;         /*0-10 10最强*/
    unsigned long ulIsScane;          /*0扫描得到,1手动添加*/
}WIFI_INFO;

typedef struct TIME_PROPERTY{
    unsigned short usYear;                 /*年*/
    unsigned char  ucMonth;                /*月*/
    unsigned char  ucDay;                  /*日*/
    unsigned char  ucHour;                 /*时*/
    unsigned char  ucMinute;               /*分*/
    unsigned char  ucSecond;               /*秒*/
    unsigned char  ucModel;                /*时间模式, 0手动设置, 1与电脑同步, 2NTP服务*/
    unsigned char  ucTimeZoon;             /*时区 请参照本文件中的时区对应规则*/
    unsigned char  ucRes[3];               /*保留*/
    unsigned char  ucSNTPServerAddr[200];  /*NTP服务器地址*/
}TIME_PROPERTY;

/*电子快门对应表*/
typedef enum ARG_SHUTTERSPEED_TYPE{
    ARG_SHUTTERSPEED_1_1 = 0,
    ARG_SHUTTERSPEED_1_4,
    ARG_SHUTTERSPEED_1_8,
    ARG_SHUTTERSPEED_1_15,
    ARG_SHUTTERSPEED_1_30,
    ARG_SHUTTERSPEED_1_60,
    ARG_SHUTTERSPEED_1_90,
    ARG_SHUTTERSPEED_1_100,
    ARG_SHUTTERSPEED_1_125,
    ARG_SHUTTERSPEED_1_180,
    ARG_SHUTTERSPEED_1_250,
    ARG_SHUTTERSPEED_1_350,
    ARG_SHUTTERSPEED_1_500,
    ARG_SHUTTERSPEED_1_725,
    ARG_SHUTTERSPEED_1_1000,
    ARG_SHUTTERSPEED_1_1500,
    ARG_SHUTTERSPEED_1_2000,
    ARG_SHUTTERSPEED_1_3000,
    ARG_SHUTTERSPEED_1_4000,
    ARG_SHUTTERSPEED_1_6000,
    ARG_SHUTTERSPEED_1_10000,
}ARG_SHUTTERSPEED_TYPE;

typedef struct DISPLAY_PROPERTY{
    unsigned char ucCH;                 /*通道号*/
    unsigned char ucMirrorMode;         /*是否镜像:  0无镜像, 1水平镜像 2垂直镜像 3水平垂直镜像*/
    unsigned char ulElectronicShutter;  /*电子快门: 填写 ARG_SHUTTERSPEED_TYPE*/
    unsigned char ucIRLEDEnable;        /*0:IRLED关闭 1:IRLED打开 */
    unsigned char ucIRCutMode;          /*0:LDR自动 1:视频自动 2:彩色模式 3:黑白模式*/
    unsigned char ucIRCutSwitchTime;    /*ircut 切换时间, 3-120 秒*/
    unsigned char ucRes[1];             /*保留1*/

    unsigned char ucOSDVisiable;        /*0不显示osd, 1显示osd*/
    unsigned char ucOSDPostion;         /*填写 ARG_POSITON_TYPE, 在osd 显示时有效*/
    unsigned char ucDateVisiable;       /*0显示日期  1不显示日期*/
    unsigned char ucDateFormat;         /*0:y/m/d 1:m/d/y 2:d/m/y*/
    unsigned char ucIPCNameVisiable;    /*0显示ipcname 1不显示ipcname*/
    unsigned char ucOSDText[200];       /*osd 显示文本*/

    unsigned char ucRotateAngle;        /*旋转角度: 填写ARG_ROTATE_TYPE*/
    unsigned char ulRes[15];            /*保留*/
}DISPLAY_PROPERTY;

/*(DNR means DigitalNoiseReduction)*/
typedef enum ARG_DNRMODE_TYPE{
    ARG_DNRMODE_CLOSE   = 0x00,          /*关闭DNR*/
    ARG_DNRMODE_2D      = 0x01,          /*2D 去噪*/
    ARG_DNRMODE_3D      = 0x02,          /*3D 去噪*/
    ARG_DNRMODE_2D_3D   = 0x03,          /*2D 3D 去噪*/
}ARG_DNRMODE_TYPE;

typedef struct IMAGE_PROPERTY{
    unsigned char ucCH;                  /*通道号*/
    unsigned char ucStreamType;          /*码流类型: 填写(ARG_STREAM_TYPE)视频部分*/

    unsigned char ucBrightness;          /*亮度*/
    unsigned char ucContrast;            /*对比度*/
    unsigned char ucSaturation;          /*饱和度*/
    unsigned char ucHue;                 /*色度*/

    unsigned char ucGainCtrl;            /*增益调节 0-255*/
    unsigned char ucGainRCtrl;           /*红色增益调节 0-255*/
    unsigned char ucGainGCtrl;           /*绿色增益调节 0-255*/
    unsigned char ucGainBCtrl;           /*蓝色增益调节 0-255*/

    unsigned char ucWhiteBalance;        /*白平衡: 0自动 1手动*/
    unsigned char ucBLCEable;            /*背光补偿: 0关闭 1启动*/
    unsigned char ucBLCStrength;         /*背光强度: 0低 1中 2高. 背光启动是有效*/
    unsigned char ucExposureCtrl;        /*自动曝光控制:  0:关闭 1:打开*/

    unsigned char ucWideDynamicEnable;   /*宽动态 0:关闭 1:打开*/
    unsigned char ucWideDynamicLevel;    /*宽动态级别 0:自动 1:弱 2:中 3:强 4:超强*/ 
    unsigned char ucFlickerCtrlEnable;   /*闪烁控制 0:关闭 1打开*/
    unsigned char ucFlickerCtrlModel;    /*闪烁控制模式: 0:室内NTSC 1:室内PAL 2室外*/

    unsigned char ucDNRModel;            /*数字降噪模式 ARG_DNRMODE_TYPE*/
    unsigned char ucDNR2DStrength;       /*数字降噪2d强度: 0-255 */
    unsigned char ucRes1[1];             /*保留1*/
    unsigned char ucDNR3DStrength;       /*数字降噪3d强度: 0-255 */

    unsigned char ucDefogEnable;         /*去雾开关: 0:关闭 1:启动 2:自动*/
    unsigned char ucDefogStrength;       /*去雾强度: 0-255*/

    unsigned char ucSLInhibition;        /*强光抑制开关：　0:关闭 1:启动*/
    unsigned char ucSLInhibitionStreangth;/*强光抑制强度: 0-255*/

    unsigned char ucRes[10];             /*保留*/
}IMAGE_PROPERTY;

/*视频编码属性*/
typedef struct VIDEO_ENCODING_PROPERTY{
    unsigned char  ucEncodingType;      /* 填写 (ARG_ENCODING_TYPE)的视频部分*/
    unsigned char  ucPicQuality;        /*图象质量 0-最好 1-次好 2-较好 3-一般 4-较差 5-差*/

    unsigned short usWidth;             /*宽度*/
    unsigned short usHeight;            /*高度*/
    unsigned short ucFrameRate;         /*帧率*/
    unsigned short ucBitRate;           /*码率 单位k byte(大B)*/

    unsigned char  ucKeyFrameInterval;  /* I帧间隔,  秒数*10 (ucEncodingType为h264时有效) */
    unsigned char  ucBitrateType;       /*码率类型 0:定码率，1:变码率*/

    unsigned char  ucRes[16];
}VIDEO_ENCODING_PROPERTY;

/*采样率枚举*/
typedef enum ARG_SAMPLERATE_TYPE{
    ARG_SAMPLERATE_8K = 0,
    ARG_SAMPLERATE_16K = 10,
    ARG_SAMPLERATE_32K = 20,
    ARG_SAMPLERATE_64K = 30
}ARG_SAMPLERATE_TYPE;
/*采样宽度枚举*/
typedef enum ARG_BITWIDTH_TYPE{
    ARG_BITWIDTH_8 = 0,
    ARG_BITWIDTH_16,
    ARG_BITWIDTH_32,
}ARG_BITWIDTH_TYPE;
/*音频编码属性*/
typedef struct AUDIO_ENCODING_PROPERTY{
    unsigned char  ucEncodingType;     /*编码格式:  填写(ARG_ENCODING_TYPE)的音频部分*/
    unsigned char  ucSampleRate;       /*采样率:   填写(ARG_SAMPLERATE_TYPE)*/
    unsigned char  ucBitWidth;         /*采样宽度:  填写(ARG_BITWIDTH_TYPE)*/
    unsigned char  ucSoundMode;        /*模式: 0单声道(momo) 1立体声(steror)*/
    unsigned char  ucRes[24];          /*保留*/
}AUDIO_ENCODING_PROPERTY;

typedef struct ENCODING_PROPERTY{
    unsigned char  ucCh;              /*通道号*/
    unsigned char  ucStreamType;      /*填写 ARG_STREAM_TYPE*/

    union{
        VIDEO_ENCODING_PROPERTY  video;
        AUDIO_ENCODING_PROPERTY  audio;
    }property;

    unsigned char  ucEnable;          /*编码通道是否使能: 0关闭, 1打开 */
    unsigned char  ucRes[1];          /*保留*/
}ENCODING_PROPERTY;

typedef struct ENCODING_SCOPE{
    unsigned char  ucCh;
    unsigned char  ucMaxFrameRate;

    unsigned char  ucRes[1];                   /*保留*/
    unsigned char  ucResolutions[1024];
}ENCODING_SCOPE;

typedef struct CHANNEL_SCOPE{
    unsigned char ucCHScope[64];            /*通道可选范围*/
    unsigned long ulCHValidNum;             /*(ucCHScope)有效个数  0-64*/
}CHANNEL_SCOPE;

typedef struct AUDIO_PROPERTY{
    unsigned char ucCh;                /*通道*/
    unsigned char ucEnable;            /*是否使能 0关闭,1开启*/
    unsigned char ucOutputVolume;      /*输出音量 0-100 100最大*/
    unsigned char ucInputVolume;       /*输入音量 0-100 100最大*/
    unsigned char ucInputModel;        /*输入模式 0mic输入 1有源输入*/

    unsigned char ucRes[19];
}AUDIO_PROPERTY;


/********************************************************************************************/
/*******************************  视频遮盖,移动侦测,ROI ***************************************/
/********************************************************************************************/
#define ALARMTYPE_MOTIONDETECTION 0
typedef struct MOTIONDETECTION_INFO{
    unsigned char ucCH;               /*报警通道*/
    unsigned char ucAlarmType;        /*报警类型 0移动报警 */
    unsigned char ucStatus;           /*报警状态 0开始报警, 1,结束报警*/
    unsigned char ucLevel;            /*报警等级 0高级 1中级 2低级*/
    unsigned long ulTime;             /*报警时间 1970年1月1日00:00:00所经过的秒数*/
}MOTIONDETECTION_INFO;

/*感兴趣属性*/
typedef enum ROI_QP_TYPE{
    ROI_QP_ABSOLUTE = 0,              /*绝对 QP 值*/
    RIO_QP_RELATIVE = 1               /*相对 QP 值*/
}ROI_QP_TYPE;

#if (defined ARG_SDK_VERSION_1_1)
/*移动侦测属性*/
typedef struct MOTIONDETECTION_PROPERTY{
	unsigned char ucCH;               /*通道*/
    unsigned char ucEnable;           /*0关闭移动侦测, 1开启移动侦测*/
	unsigned char ucType;             /*配置方式 0(64切分方式 8*8) 1(鼠标拖动方式)*/
	unsigned char ucSensitivity;      /*0-10等级 0最低,10最高*/   
	unsigned char ucBlock[8];         /*ucBlock[0]的第0位为1 则64切分的第1行左边第1块区域移动侦测为打开,0为关闭*/
    ARG_RECT      ucArea[12];         /*12组百分比区域*/
}MOTIONDETECTION_PROPERTY;


/*视频遮盖属性*/
typedef struct VIDEOCOVER_PROPERTY{
	unsigned char ucCH;               /*通道*/
    unsigned char ucEnable;           /*0关闭视频遮盖, 1开启视频遮盖*/
	unsigned char ucType;             /*配置方式 0(64切分方式 8*8) 1(鼠标拖动方式)*/
	unsigned char ucRes;              /*保留*/   
	unsigned char ucBlock[8];         /*ucBlock[0]的第0位为1 则64切分的第1行左边第1块区域移动侦测为打开,0为关闭*/
    ARG_RECT      ucArea[12];         /*12组百分比区域*/
}VIDEOCOVER_PROPERTY;

/*视频遮盖属性2*/
typedef struct VIDEOCOVER_EX_PROPERTY{
    unsigned char    ucCH;            /*通道*/
    unsigned char    ucEnable;        /*0关闭视频遮盖, 1开启视频遮盖*/
    unsigned char    ucRes1[2];       /*保留1*/
    TRANSPORT_INFO   transportInfo;   /*传输信息*/
    unsigned char    ucRes2[16];      /*保留2*/
}VIDEOCOVER_EX_PROPERTY;


typedef struct ROI_REGION{
    ARG_RECT      area;               /*百分比区域*/
    unsigned char ucQpType;           /*QP类型: 填写 ROI_QP_TYPE*/
             char cQpVal;             /*QP数值: 相对模式下填[-51,51] 绝对模式下填[0,51]*/
    unsigned char ucEnbale;           /*使能: 0关闭 1打开*/
    unsigned char ucRes[1];           /*保留*/
}ROI_REGION;

typedef struct ROI_PROPERTY{
    unsigned char ucCh;               /*通道*/
    unsigned char ucEnable;           /*0关闭RIO, 1开启ROI*/
    unsigned char ucBgFrameRate;      /*背景帧率: 0关闭背景帧率, 1-255数值为背景帧率.并开启背景帧率*/
    unsigned char ucRes1[1];          /*保留1*/
    ROI_REGION    region[8];
    unsigned char ucRes2[16];         /*保留2*/
}ROI_PROPERTY;

#elif (defined ARG_SDK_VERSION_1_2)

/*移动侦测属性*/
typedef struct MOTIONDETECTION_PROPERTY{
    unsigned char ucCH;               /*通道*/
    unsigned char ucEnable;           /*填写 ARG_SWITCH_TYPE*/
    unsigned char ucRes1[1];          /*保留*/
    unsigned char ucSensitivity;      /*0-10灵敏度 0最低,10最高*/
    AREA_INFO     region;             /*区域*/
    unsigned char ucRes2[16];         /*保留*/
}MOTIONDETECTION_PROPERTY;

/*视频遮盖属性*/
typedef struct VIDEOCOVER_PROPERTY{
    unsigned char ucCH;               /*通道*/
    unsigned char ucEnable;           /*填写 ARG_SWITCH_TYPE*/
    unsigned char ucRes1[2];          /*保留*/
    AREA_INFO     region;             /*区域*/
    unsigned char ucRes2[16];         /*保留*/
}VIDEOCOVER_PROPERTY;
#endif

/********************************************************************************************/
/*******************************          ptz         ***************************************/
/********************************************************************************************/
#define PTZ_PRESET_POINT_MAXNUM          128             /*最多支持 128 个预置点*/
#define PTZ_CRUISE_LINE_MAXNUM           4               /*最多支持 4 条巡航线*/
#define PTZ_CRUISE_POINT_MAXNUM          32              /*每条巡航线最多有32个预置点*/

typedef struct PRESET_POINT_PROPERTY{
    unsigned char      ucCh;              /*通道*/
    unsigned char      ucIndex;           /*预置点 序号*/
    unsigned char      ucEnable;          /*是否使能, 0不使能 1使能,只读*/
    unsigned char      ucStep;            /*速度，范围1~8，8控制效果最明显,从这个点移动到下个点的速度*/
    unsigned long      ulDwelltime;       /*停留时间: 单位秒数, 最大30分钟*/
    unsigned char      ucNodeName[64];    /*节点名称*/
    unsigned char      ucRes1[16];        /*保留*/
}PRESET_POINT_PROPERTY;

typedef struct CRUISE_LINE_PROPERTY{
    unsigned char      ucCh;              /*通道*/
    unsigned char      ucIndex;           /*巡航线序号*/
    unsigned char      ucEnable;          /*是否启用, 0启用, 1不启用, 只读*/
    unsigned char      ucLineName[64];    /*巡航线名称*/
    unsigned char      ucPoints[32];      /*巡航线下标, 最多 32 个预置点*/
}CRUISE_LINE_PROPERTY;

typedef struct PTZ_PROPERTY{
    unsigned char      ucCh;              /*通道号*/
    unsigned char      ucStep;            /*速度，范围1~8，8控制效果最明显,移动速度*/
    unsigned char      ucDuration;        /*动作持续时间: 1-100, 单位 100ms*/
    unsigned char      ucRes1[1];         /*保留*/
    /*光圈*/
    unsigned char      ucApertureMode;    /*光圈模式  0自动  1控制*/
    unsigned char      ucApertureVal;     /*光圈打开量 0-100 (在光圈模式为控制的情况下有效)*/

    unsigned char      ucRes[18];         /*保留*/
}PTZ_PROPERTY;

typedef struct PTZ_INFO{


}PTZ_INFO;

typedef enum PTZ_OPTION_TYPE{
    PTZ_OPTION_STOP = 0,                 /*停止*/
    PTZ_OPTION_CONTINUE,                 /*继续运行*/
    
    PTZ_OPTION_UP      = 10,             /*向上*/
    PTZ_OPTION_DOWN,                     /*向下*/
    PTZ_OPTION_LEFT,                     /*向左*/
    PTZ_OPTION_RIGHT,                    /*向右*/
    PTZ_OPTION_LEFTUP,                   /*向左上*/
    PTZ_OPTION_LEFTDOWN,                 /*向左下*/
    PTZ_OPTION_RIGHTUP,                  /*向右上*/
    PTZ_OPTION_RIGHTDOWN,                /*向右下*/

    PTZ_OPTION_ADDZOOM = 20,             /*变倍+*/
    PTZ_OPTION_DECZOOM,                  /*变倍-*/
    PTZ_OPTION_ADDFOCUS,                 /*变焦+*/
    PTZ_OPTION_DECFOCUS,                 /*变焦-*/
    PTZ_OPTION_ADDAPERTURE,              /*光圈+*/
    PTZ_OPTION_DECAPERTURE,              /*光圈-*/

    PTZ_OPTION_LAMP = 30,                /*灯光雨刷*/
    PTZ_OPTION_ONEKEYFOCUS,              /*一键聚焦*/
    PTZ_OPTION_INITLENS,                 /*初始化镜头*/

    PTZ_OPTION_MOVEPOINT = 40,           /*移动到预置点*/
    PTZ_OPTION_STARTLOOP,                /*开始巡航*/
    PTZ_OPTION_STARTPANCRUISE,           /*开始水平旋转*/
    PTZ_OPTION_STARTLINESCAN,            /*开始线扫*/
}PTZ_OPTION_TYPE;

typedef struct PTZ_OPTION{
    unsigned char ucCh;                 /*通道*/
    unsigned char ucOptType;            /*操作类型: PTZ_OPTION_TYPE*/
	unsigned char ucRes1[2];            /*保留*/
    union{
        unsigned char ucPresetPointIndex;   /*预置点序号*/
        unsigned char ucCruiseLineIndex;    /*巡航线序号*/
        unsigned long ulPlaceholder[4];     /*占位*/
    }opt;
}PTZ_OPTION;

/********************************************************************************************/
/*******************************        存储配置       ***************************************/
/********************************************************************************************/
/*存储介质枚举*/
typedef enum STORAGE_ID_TYPE{
    STORAGE_ID_1    =   0x01,          /*id1*/
    STORAGE_ID_2    =   0x02,          /*id2*/
    STORAGE_ID_3    =   0x04,          /*id3*/
    STORAGE_ID_4    =   0x08,          /*id4*/

    STORAGE_ID_5    =   0x10,          /*id5*/
    STORAGE_ID_6    =   0x20,          /*id6*/
    STORAGE_ID_7    =   0x40,          /*id7*/
    STORAGE_ID_8    =   0x80,          /*id8*/

    STORAGE_ID_NULL =   0x00,          /*无*/
    STORAGE_ID_ALL  =   0xff,          /*所有*/
}STORAGE_ID_TYPE;

/*存储属性配置*/
typedef struct STORAGE_PROPERTY{
    unsigned char ucCh;                /*通道*/
    unsigned char ucStorageType;       /*存储模式: 填写 ARG_STORAGE_TYPE*/

    unsigned char ucPackingModel;      /*打包模式 0按时间 1按大小 2无*/
    union{
    unsigned char ucPackingTime;       /*打包时间 0-255 单位分钟(按时间打包模式下有效)*/
    unsigned char ucPackingSize;       /*打包大小 0-125M, 1-256M, 2-512M, 3-1G, 4-2G, 5-4G (按大小打包模式下有效)*/
    }ucPackingValue;

    unsigned char ucLackSpaceModel;    /*0停止存储 1循环覆盖, 2按有效期覆盖, (NVR,DVR)0为不覆盖 1为覆盖*/
    unsigned char ucId;                /*id: 填写 STORAGE_ID_TYPE*/
    unsigned char ucRes[18];
}STORAGE_PROPERTY;

/*存储信息*/
typedef struct STORAGE_INFO{
    unsigned char ucCh;                /*通道*/
    unsigned char ucStorageType;       /*存储模式: 填写 ARG_STORAGE_TYPE*/

    unsigned char ucState;             /*状态 0可用 1不可用 2格式化中 */
    unsigned char ucPercent;           /*格式化进度 0-100*/

    unsigned long ulTotle;             /*总容量: 单位兆(M)*/
    unsigned long ulFree;              /*可用空间: 单位兆(M)*/

    unsigned char ucId;                /*id: 填写 STORAGE_ID_TYPE*/
    unsigned char ucRes[15];           /*保留*/
}STORAGE_INFO;

/*存储操作枚举*/
typedef enum STORAGE_OPTION_TYPE{
    STORAGE_OPTION_FORMAT = 0,         /*格式*/
    STORAGE_OPTION_CLEAN,              /*清除数据*/
}STORAGE_OPTION_TYPE;

/*存储操作*/
typedef struct STORAGE_OPTION{
    unsigned char ucCh;                /*通道*/
    unsigned char ucStorageType;       /*存储模式: 填写 ARG_STORAGE_TYPE*/
    unsigned char ucOptType;           /*操作类型: 填写 STORAGE_OPTION_TYPE*/
    unsigned char ucId;                /*id: 填写 STORAGE_ID_TYPE*/
    unsigned char ucRes[16];           /*保留*/
}STORAGE_OPTION;
/********************************************************************************************/
/*******************************        网络配置       ***************************************/
/********************************************************************************************/
/*net  配置*/
typedef struct NET_PROPERTY{
    unsigned char ucCh;                              /*通道号*/
    unsigned char ucNetCardNum;                      /*网卡号*/
    unsigned char ucEnDHCP;                          /*使能dhcp, 0开启 1关闭*/
    unsigned char ucRes1[1];                         /*保留*/
    unsigned char ucIP[ARG_IPSTR_LEN];               /*IP地址*/
    unsigned char ucSubNetMask[ARG_IPSTR_LEN];       /*子网掩码*/
    unsigned char ucGateway[ARG_IPSTR_LEN];          /*网关*/
    unsigned char ucDNS1Ip[ARG_IPSTR_LEN];           /*dns 服务器1*/
    unsigned char ucDNS2Ip[ARG_IPSTR_LEN];           /*dns 服务器2*/
    unsigned char ucMacIp[18];                       /*Mac服务器地址: 填写字符串例如 "A1:B2:C3:D4:E5:F6"*/
    unsigned char ucRes2[16];                        /*保留*/
}NET_PROPERTY;

/*smtp 配置*/
typedef struct SMTP_PROPERTY{
    unsigned char ucCh;                              /*通道*/
    unsigned char ucEnable;                          /*是否打开 0不需要, 1启动,*/
    unsigned char ucRes1[2];                         /*保留1*/
    unsigned long ulPort;                            /*端口: 填写 25 或大于 1024*/
    unsigned long ulAnonymity;                       /*匿名 0匿名,1不匿名*/
    unsigned long ulEncryptionType;                  /*加密类型 0不加密,1SSL*/
    unsigned long ulTime;                            /*发送间隔时间 1-999*/
    unsigned long ulAccessory;                       /*是否加附件, 0加, 1不加*/
    unsigned long ulAccessoryType;                   /*附件类型(暂时没用)*/
    unsigned char ucUserName[64];                    /*用户名*/
    unsigned char ucPasswd[64];                      /*密码*/

    unsigned char ucServerAddr[200];                 /*服务器地址*/
    unsigned char ucSenderAddr[200];                 /*发送地址*/
    unsigned char ucRecverAddr1[200];                /*收件地址1*/
    unsigned char ucRecverAddr2[200];                /*收件地址2*/
    unsigned char ucRecverAddr3[200];                /*收件地址3*/
    unsigned char ucTheme[200];                      /*主题*/
    unsigned char ucRes2[16];                        /*保留2*/
}SMTP_PROPERTY;
/*ftp 配置*/
typedef struct FTP_PROPERTY{
    unsigned char ucCh;                              /*通道*/
    unsigned char ucEnable;                          /*是否打开 0不需要, 1启动,*/
    unsigned char ucRes1[2];                         /*保留1*/
    unsigned long ulPort;                            /*端口: 填写 21 或大于 1024*/
    unsigned char ucUserName[ARG_USERNAME_LEN];      /*用户名*/
    unsigned char ucPasswd[ARG_PASSWD_LEN];          /*密码*/
    unsigned char ucServerAddr[200];                 /*服务器地址*/
    unsigned char ucPath[200];                       /*路径*/
    unsigned char ucRes2[16];                        /*保留2*/
}FTP_PROPERTY;

/*DDNS 服务器类型枚举*/
typedef enum DDNS_SERVER_TYPE{
    DDNS_SERVER_ORAY           = 0x00,               /*花生壳*/
    DDNS_SERVER_NO_IP,                               /*noip*/
    DDNS_SERVER_DYN,                                 /*dyn*/
    DDNS_SERVER_CHANGEIP,                            /*changeip*/
    DDNS_SERVER_A_PRESS,                             /*a-press*/
}DDNS_SERVER_TYPE;

/*ddns 配置*/
typedef struct DDNS_PROPERTY{
    unsigned char ucCh;                              /*通道*/
    unsigned char ucEnable;                          /*是否启动,0关闭 ,1启动*/
    unsigned char ucServerType;                      /*服务器类型: 填写 DDNS_SERVER_TYPE*/
    unsigned char ucState;                           /*状态: 0-登录成功, 1-登录中*/

    unsigned char ucServerIp[32];                    /*服务器ip*/
    unsigned long ulServerPort;                      /*服务器端口*/
    unsigned char ucDomainName[32];                  /*动态域名*/
    unsigned char ucDefaultDomainName[32];           /*默认域名*/
    unsigned char ucUserName[ARG_USERNAME_LEN];      /*用户名*/
    unsigned char ucPasswd[ARG_PASSWD_LEN];          /*密码*/
    unsigned char ucAlias[ARG_USERNAME_LEN];         /*别名*/

    unsigned long ulAlivePeriod;                     /*保活时间: 60-999999 单位秒*/
    unsigned long ulServiceType;                     /*服务类型: 0-普通用户 1-管理员*/
}DDNSSERVER_PROPERTY;

/*p2p 配置*/
typedef struct P2P_PROPERTY{
    unsigned char ucFreeIpEnable;                    /*是否启动 freeip*/
    unsigned char ucFreeIpSerialNumber[100];         /*freeip 序列号*/
    unsigned char ucFreeIpIOSAppAddr[100];           /*freeip 苹果app地址*/
    unsigned char ucFreeIpAndroidAppAddr[100];       /*freeip 安卓app地址*/

    unsigned char ucDanaleEnable;                    /*是否启动 dannale*/
    unsigned char ucDanaleSerialNumber[100];         /*dnnale 序列号*/
    unsigned char ucDanaleIOSAppAddr[100];           /*Danale 苹果app地址*/
    unsigned char ucDanaleAndroidAppAddr[100];       /*Danale 安卓app地址*/
    unsigned char ucRes[18];
}P2P_PROPERTY;

/*p2p info*/
typedef struct P2P_INFO{
    unsigned char ucFreeIpIsOnline;                  /*freeIp 是否可用:  0不可用 1可用*/
    unsigned char ucRes1[19];                        /*保留*/
    unsigned char ucDanaleIsOnline;                  /*danale 是否可用:  0不可用 1可用*/
    unsigned char ucRes2[19];                        /*保留*/
}P2P_INFO;
/********************************************************************************************/
/*******************************        协议配置       ***************************************/
/********************************************************************************************/
/*rtsp 属性配置*/
typedef struct RTSP_AGREEMENT_PROPERTY{
    unsigned short usPort;                          /*协议端口*/
    unsigned char  ucAuthentication;                /*是否开启身份验证: 0关闭 1开启*/
    unsigned char  ucRes[1];
}RTSP_AGREEMENT_PROPERTY;

/*xm 属性配置*/
typedef struct XM_AGREEMENT_PROPERTY{
    unsigned short usPort;                          /*协议端口: 需填写大于 1024 或 80*/
    unsigned char  ucRes[2];                        /*保留*/
}XM_AGREEMENT_PROPERTY;

/*私有协议属性配置*/
typedef struct PRIVATE_AGREEMENT_PROPERTY{
    unsigned short usPort;                          /*协议端口: 需填写大于 1024 或 80*/
    unsigned char  ucRes[2];                        /*保留*/
}PRIVATE_AGREEMENT_PROPERTY;

/*onvif 属性配置*/
typedef struct ONVIF_AGREEMENT_PROPERTY{
    unsigned short usPort;                          /*协议端口 需填写大于 1024 或 80*/
    unsigned char  ucAuthentication;                /*是否开启身份验证: 0关闭 1开启*/
    unsigned char  ucRes[1];                        /*保留*/
}ONVIF_AGREEMENT_PROPERTY;

/*http 属性配置*/
typedef struct HTTP_AGREEMENT_PROPERTY{
    unsigned short usPort;                          /*协议端口: 需填写大于 1024 或 80*/
    unsigned char  ucRes[2];                        /*保留*/
}HTTP_AGREEMENT_PROPERTY;

/*tcp 属性配置*/
typedef struct TCP_AGREEMENT_PROPERTY{
    unsigned short usPort;                          /*协议端口: 需填写大于 1024*/
    unsigned char  ucRes[2];                        /*保留*/
}TCP_AGREEMENT_PROPERTY;

/*协议属性配置*/
typedef struct AGREEMENT_PROPERTY{
    unsigned char     ucCh;                         /*通道*/
    unsigned char     ucEnable;                     /*是否使能 0关闭 1使能*/
    unsigned char     ucArgType;                    /*协议类型: 填写 ARG_AGREEMENT_TYPE */
    unsigned char     ucRes1[1];                    /*保留*/

    union{
        RTSP_AGREEMENT_PROPERTY      rtspPro;       /*rtsp协议*/
        XM_AGREEMENT_PROPERTY        xmPro;         /*xm协议*/
        PRIVATE_AGREEMENT_PROPERTY   privatePro;    /*私有协议*/
        ONVIF_AGREEMENT_PROPERTY     onvifPro;      /*onvif 协议*/
        HTTP_AGREEMENT_PROPERTY      httpPro;       /*http 协议*/
        TCP_AGREEMENT_PROPERTY       tcpPro;        /*回放 tcp*/
        unsigned char ucPlaceHolder[128];           /*占位符号*/
    }property;
    unsigned char     ucRes2[16];                   /*保留*/
}AGREEMENT_PROPERTY;

/*协议配置属性范围*/
typedef struct  AGREEMENT_SCOPE{


}AGREEMENT_SCOPE;


/********************************************************************************************/
/*******************************      定时录像,报警联动  ***************************************/
/********************************************************************************************/
/*报警持续时间枚举*/
typedef enum ALARM_DURATION_TYPE{
    ALARM_DURATION_1S     = 0,               /*1  秒*/
    ALARM_DURATION_3S,                       /*3  秒*/
    ALARM_DURATION_5S,                       /*5  秒*/
    ALARM_DURATION_10S,                      /*10 秒*/
    ALARM_DURATION_15S,                      /*15 秒*/
    ALARM_DURATION_30S,                      /*30 秒*/
    ALARM_DURATION_45S,                      /*45 秒*/


    ALARM_DURATION_1MIN   = 10,              /*1  分钟*/
    ALARM_DURATION_3MIN,                     /*3  分钟*/
    ALARM_DURATION_5MIN,                     /*5  分钟*/
    ALARM_DURATION_10MIN,                    /*10 分钟*/
    ALARM_DURATION_15MIN,                    /*15 分钟*/
    ALARM_DURATION_30MIN,                    /*30 分钟*/

    ALARM_DURATION_LIVE   = 20,              /*持续*/
}ALARM_DURATION_TYPE;

/*录像配置属性*/
typedef struct STREAM_RECORD_PROPERTY{
    unsigned char     ucEnable;               /*是否启动录像: 0关闭 1启动*/
    unsigned char     ucStorageType;          /*存储模式: 填写 ARG_STORAGE_TYPE*/

    unsigned char     ucVideoStreamType;      /*视频流类型:  填写 ARG_STREAM_TYPE(视频部分)*/
    unsigned char     ucAudioStreamType;      /*音频流类型:  填写 ARG_STREAM_TYPE(音频部分)*/
    unsigned char     ucValidWeek;            /*有效周数: 0永久有效, 1-255 具体周数*/
    unsigned char     ucDuration;             /*持续时间: 填写 ALARM_DURATION_TYPE*/

    unsigned char     ucRes[18];              /*保留*/
}STREAM_RECORD_PROPERTY;

/*抓拍配置属性*/
typedef struct STREAM_SNAPSHOOT_PROPERTY{
    unsigned char     ucEnable;               /*是否启动录像: 0关闭 1启动*/
    unsigned char     ucStorageType;          /*存储模式: 填写 ARG_STORAGE_TYPE*/

    unsigned char     ucVideoStreamType;     /*视频流类型:  填写 ARG_STREAM_TYPE(视频部分)*/
    unsigned char     ucValidWeek;            /*有效周数: 0永久有效, 1-255 具体周数*/

    unsigned char     ucShootNum;             /*抓拍张数: 1-10, 单位一张*/
    unsigned char     ucSingalShootInterval;  /*抓拍间隔: 1-60, 单位秒(s)*/
    unsigned char     ucDuration;             /*持续时间: 填写 ALARM_DURATION_TYPE*/

    unsigned char     ucRes[17];              /*保留*/
}STREAM_SNAPSHOOT_PROPERTY;

/*文本消息配置属性*/
typedef struct TEXT_RECORD_PROPERTY{
    unsigned char     ucEnable;               /*是否开启文字记录: 0关闭 1开启*/
    unsigned char     ucStorageType;          /*存储模式: 填写 ARG_STORAGE_TYPE*/
    unsigned char     ucRes1[2];              /*保留*/
    unsigned char     ucText[32];             /*发送内容*/
    unsigned char     ucRes2[16];             /*保留*/
}TEXT_RECORD_PROPERTY;

/*蜂鸣器属性配置*/
typedef struct BUZZER_PROPERTY{
    unsigned char     ucEnable;               /*是否开启蜂鸣器: 0关闭 1开启*/
    unsigned char     ucDuration;             /*持续时间: 填写 ALARM_DURATION_TYPE*/
    unsigned char     ucRes1[18];             /*保留*/
}BUZZER_PROPERTY;

/*定时录像*/
typedef struct REGULAR_RECORD_PROPERTY{
    unsigned char                 ucCh;                   /*通道号*/
    unsigned char                 ucRes1[3];              /*保留1*/
    STREAM_RECORD_PROPERTY        videoRecord;            /*录像*/
    TIME_HOUR_SCOPE               timeSocpeTab[7][6];     /*时间表 7天每天6个时段*/
    unsigned char                 ucRes2[16];             /*保留2*/
}REGULAR_RECORD_PROPERTY;
/*定时抓拍*/
typedef struct REGULAR_SNAP_PROPERTY{
    unsigned char                 ucCh;                   /*通道号*/
    unsigned char                 ucShootInterval;        /*抓拍组时间间隔: 1-255, 单位分钟(min)*/
    unsigned char                 ucRes1[2];              /*保留1*/
    STREAM_SNAPSHOOT_PROPERTY     snapshoot;              /*抓拍*/
    TIME_HOUR_SCOPE               timeSocpeTab[7][6];     /*时间表 7天每天6个时段*/
    unsigned char                 ucRes2[16];             /*保留2*/
}REGULAR_SNAP_PROPERTY;

/*报警属性配置*/
typedef struct ALARM_PROPERTY{
    unsigned char                ucCh;                   /*通道*/
    unsigned char                ucAlarmType;            /*报警类型: 填写 ARG_ALARM_TYPE*/
    unsigned char                ucDuration;             /*持续时间 : 填写 ALARM_DURATION_TYPE*/
    unsigned char                ucEnable;               /*报警是否打开: 0关闭 1打开*/
    unsigned char                ucRes1[4];              /*保留1*/

    STREAM_RECORD_PROPERTY       videoRecord;            /*录像*/
    STREAM_SNAPSHOOT_PROPERTY    snapshoot;              /*抓拍*/
    TEXT_RECORD_PROPERTY         message;                /*消息*/
    BUZZER_PROPERTY              buzzer;                 /*蜂鸣器*/

    unsigned char                ucRes2[3];              /*保留2*/
    unsigned char                ucTimeScopeMode;        /*时间段: 0全部时间  1时间表内*/
    TIME_HOUR_SCOPE              timeSocpeTab[7][6];     /*时间表: 7天每天6个时段*/

    unsigned char                ucRes3[16];             /*保留3*/
}ALARM_PROPERTY;

/*报警属性范围*/
typedef struct ALARM_SCOPE{
    unsigned char  ucCh;                        /*通道*/
    unsigned char  ucAlarmTypeScope;            /*支持的报警类型: ARG_ALARM_TYPE 只读*/

    unsigned char  ucStreamRecordEnable;        /*是否支持录像: 0不支持 1支持 只读*/
    unsigned char  ucStreamRecordStorageScope;  /*视频录像支持的存储类型: ARG_STORAGE_TYPE 只读.*/

    unsigned char  ucSnapshootEnable;           /*是否支持抓拍: 0不支持 1支持 只读*/
    unsigned char  ucSnapshootScope;            /*抓拍支持的存储类型: ARG_STORAGE_TYPE 只读*/

    unsigned char  ucMessageEnable;             /*是否支持message: 0不支持 1支持 只读*/
    unsigned char  ucMessageScope;              /*message 支持的存储类型: ARG_STORAGE_TYPE 只读 */

    unsigned char  ucRes[16];                   /*保留*/
}ALARM_SCOPE;

/********************************************************************************************/
/*******************************      镜头畸变矫正      ***************************************/
/********************************************************************************************/

/*畸变矫正属性*/
typedef struct LDC_PROPERTY{
    unsigned char  ucCh;               /*通道*/
    unsigned char  ucViewType;         /*畸变矫正类型: 0裁剪 1全模式*/
    unsigned char  ucRes1[2];          /*保留*/
    long  lCenterXOffset;		       /*畸变中心点相对图象中心点水平偏移: -75到75*/
    long  lCenterYOffset;		       /*畸变中心点相对图象中心点垂直偏移: -75到75*/
    unsigned long  ulRatio;            /*畸变程度: 0-511*/
    unsigned char  ucRes2[16];         /*保留*/
}LDC_PROPERTY;

/********************************************************************************************/
/*******************************      视频输出属性      ***************************************/
/********************************************************************************************/

/*视频制式类型*/
typedef enum VIDEO_FORMAT_TYPE{
	VIDEO_FORMAT_PAL   =   0x00,        /*PAL*/
    VIDEO_FORMAT_NTSC,                  /*NTSC*/
}VIDEO_FORMAT_TYPE;

/*视频输出属性*/
typedef struct VIDEO_OUTPUT_PROPERTY{
    unsigned char   ucCh;              /*通道号*/
    unsigned char   ucFormat;          /*视频制式:       VIDEO_FORMAT_TYPE*/
    unsigned char   ucResolutionType;  /*视频输出分辨率:  ARG_RESOLUTION_TYPE*/
    unsigned char   ucRes1[17];        /*保留*/
}VIDEO_OUTPUT_PROPERTY;

/********************************************************************************************/
/*******************************        通道配置       ***************************************/
/********************************************************************************************/
/*搜索设备信息*/
typedef struct SEARCH_DEV_INFO{
    unsigned char   ucNum;                             /*序号*/
    unsigned char   ucArgType;                         /*协议类型: 填写 ARG_AGREEMENT_ONVIF 或 ARG_AGREEMENT_PRIVATE */
    unsigned char   ucDevType;                         /*设备类型: 填写 ARG_DEV_TYPE*/
    unsigned char   ucRes1[1];                         /*保留*/

    unsigned char   ucDevIP[ARG_IPSTR_LEN];            /*IP地址*/
    unsigned short  usDevPort;                         /*设备端口号*/
    unsigned char   ucRes2[18];                        /*保留*/
}SEARCH_DEV_INFO;

/*通道设备*/
typedef struct  CHANNEL_PROPERTY{
    unsigned char   ucCh;                              /*通道号: 0-254代表通道号, 255代表所有通道*/
    unsigned char   ucArgType;                         /*协议类型: 填写 ARG_AGREEMENT_ONVIF 或 ARG_AGREEMENT_PRIVATE */
    unsigned char   ucDevType;                         /*设备类型: 填写 ARG_DEV_TYPE*/
    unsigned char   ucPreviewStream;                   /*预览码流: 填写 ARG_STREAM_TYPE 视频部分*/
    unsigned char   ucTransportPro;                    /*传输协议: 0tcp 1udp*/
    unsigned char   ucEnable;                          /*是否启用: 0关闭, 1启动*/
    unsigned char   ucRes1[2];                         /*保留*/

    unsigned char   ucUserName[ARG_USERNAME_LEN];      /*用户名*/
    unsigned char   ucPasswd[ARG_PASSWD_LEN];          /*密码*/
    unsigned char   ucDevIP[ARG_IPSTR_LEN];            /*IP地址*/
    unsigned short  usDevPort;                         /*设备端口号*/
    unsigned char   ucRes2[18];                        /*保留*/
}CHANNEL_PROPERTY;

/********************************************************************************************/
/*******************************           系统        ***************************************/
/********************************************************************************************/
/*系统重启属性配置*/
typedef struct SYSREBOOT_PROPERTY{
    unsigned char ucEnable;                    /*是否开启定时重启, 0关闭, 1开启*/
    unsigned char ucHour;                      /*重启时间, 单位小时*/
    unsigned char ucWeekData;                  /*0-6 分别为星期天到星期六, 10为每天*/
}SYSREBOOT_PROPERTY;

/*通用系统配置*/
typedef struct COMMON_SYS_PROPERTY{
    unsigned char ucCh;                        /*通道*/
    unsigned char ucLanguageType;              /*默认语言类型: 填写 ARG_LANGUAGE_TYPE*/
    unsigned char ucStandbyTime;               /*待机时间: 1-120 单位分钟*/
    unsigned char ucBootGuide;                 /*开机向导: 0开启 1关闭*/
    unsigned char ucDevName[32];               /*设备名称*/
    unsigned char ucRes2[16];                  /*保留2*/
}COMMON_SYS_PROPERTY;

#if (defined ARG_SDK_VERSION_1_2)
/*恢复默认类型*/
typedef enum RESET_SYS_TYPE{
    RESET_SYS_COMMON_PROPERTY      =  (1<<1),     /*系统普通配置*/
    RESET_SYS_USER_MANAGEMENT      =  (1<<2),     /*用户管理*/

    RESET_SYS_NET                  =  (1<<8),     /*网络属性配置*/
    RESET_SYS_SMTP                 =  (1<<9),     /*smtp  配置*/
    RESET_SYS_FTP                  =  (1<<10),    /*ftp   配置*/
    RESET_SYS_DDNS                 =  (1<<11),    /*ddns  配置*/

    RESET_SYS_ENCODING_PROPERTY    =  (1<<16),    /*编码配置*/
    RESET_SYS_MOTIONDETECTION      =  (1<<17),    /*移动侦测*/
    RESET_SYS_VIDEO_RECORDIG       =  (1<<18),    /*定时录像*/
    RESET_SYS_CHANNEL_PROPERTY     =  (1<<19),    /*通道配置*/
}RESET_SYS_TYPE;

/*恢复默认系统配置*/
typedef struct RESET_SYS_INFO{
	unsigned char ucCh;                       /*通道号*/
	unsigned char ucRes1[3];                  /*保留*/
    unsigned long resetType;                  /*恢复默认类型*/
    unsigned char ucRes2[16];                 /*保留*/
}RESET_SYS_INFO;
#endif

typedef struct DEV_INFO{
    unsigned char ucDevType;                  /*设备类型 填写 ARG_DEV_TYPE*/
    unsigned char ucPlatformType;             /*平台类型 填写 ARG_PLATFORM_TYPE*/
    unsigned char ucRes1[2];                  /*保留1*/
    unsigned char ucRes2[64];                 /*保留2*/
    unsigned char ucDEVID[100];               /*设备id号(序列号)*/
    unsigned char ucManufacturers[100];       /*厂家*/
    unsigned char ucSoftwareVersion[100];     /*软件版本*/
    unsigned char ucFirmwareVersion[100];     /*固件版本*/
}DEV_INFO;

#ifdef __cplusplus
}
#endif
#endif
