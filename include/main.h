/*
 * =====================================================================================
 *
 *       Filename:  main.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  20110523 005641
 *       Revision:  none
 *       Compiler:  gcc
 *
 * =====================================================================================
 */
#ifndef __main_h__
#define __main_h__

#if defined (__cplusplus)
extern "C" {
#endif

/*
 * 全局数据类型定义
 */
#include "sdk_global.h"

/*
 * 参数、控制 数据结构定义
 */
#include "sdk_struct.h"

/*
 * 网络协议及接口定义
 */
//#include "sdk_netlib.h"

/*----------- 主控消息ID定义 -----------*/

/*
 * 消息ID遵循"只允许追加修改，不允许改变原序列
 * 参数模块使用消息ID作为参数存储单元ID，修改消息ID会影响参数配置文件.
 */
typedef enum _SDK_MAIN_MSG_ID{
      SDK_MAIN_MSG_VERSION = 0x001    //    	版本信息    sdk_version_t
    , SDK_MAIN_MSG_STATUS             //		系统状态    sdk_status_t
    , SDK_MAIN_MSG_PARAM_DEFAULT      //		默认参数    sdk_default_param_t, SDK_PARAM_MASK_ID_E
    , SDK_MAIN_MSG_UPGRAD             //		升级        SDK_UPGRAD_OP_E
    , SDK_MAIN_MSG_USER               //        用户管理    SDK_USER_OP_E
    , SDK_MAIN_MSG_LOG                //		日志管理    SDK_LOG_OP_E
    , SDK_MAIN_MSG_SYS_CFG            //		系统配置    sdk_sys_cfg_t
    , SDK_MAIN_MSG_NET_MNG_CFG        //		网络配置    sdk_net_mng_cfg_t
    , SDK_MAIN_MSG_PTZ_CFG            //     	云台配置    sdk_ptz_param_t
    , SDK_MAIN_MSG_PTZ_CONTROL        //     	云台控制    SDK_PTZ_MSG_ID_E
    , SDK_MAIN_MSG_CRUISE_CFG         //     	巡航配置    sdk_cruise_param_t
    , SDK_MAIN_MSG_PREVIEW_CFG        //		本地预览配置 sdk_preview_t
    , SDK_MAIN_MSG_VO_BINDING         //		视频输出绑定(预览/回放) sdk_vo_binding_t
    , SDK_MAIN_MSG_PB_GROUP_CREATE    //		创建本地回放组 sdk_pb_group_t 
    , SDK_MAIN_MSG_PB_GROUP_DESTORY   //		销毁本地回放组 
    , SDK_MAIN_MSG_PB_GROUP_CONTROL   //		控制本地回放组 SDK_PB_GROUP_CONTROL_E
    , SDK_MAIN_MSG_CHANN_CFG          //		通道配置       sdk_channel_t
    , SDK_MAIN_MSG_ENCODE_CFG         //		编码配置       sdk_encode_t 
    , SDK_MAIN_MSG_RECORD_CFG         //		录像配置       sdk_record_cfg_t
    , SDK_MAIN_MSG_RECORD_QUERY       //		录像查询       sdk_record_item_t, sdk_record_cond_t
    , SDK_MAIN_MSG_MOTION_CFG         //    	视频移动配置   sdk_motion_cfg_v2_t
    , SDK_MAIN_MSG_ALARM_IN_CFG       //		外部报警输入配置sdk_alarm_in_cfg_t
    , SDK_MAIN_MSG_HIDE_CFG           //		视频遮蔽报警配置sdk_hide_cfg_t
    , SDK_MAIN_MSG_LOST_CFG           //		视频丢失配置   sdk_lost_cfg_t
    , SDK_MAIN_MSG_OSD_CFG            //		OSD配置       sdk_osd_cfg_t
    , SDK_MAIN_MSG_COMM_CFG			  //    	常规配置       sdk_comm_cfg_t
    , SDK_MAIN_MSG_OVERLAY_CFG		  //    	遮挡区域配置   sdk_overlay_cfg_t
    , SDK_MAIN_MSG_DISK		  		  //		磁盘管理	pmsg->args:SDK_DISK_OP_E
    , SDK_MAIN_MSG_SYSTIME		  	  //		系统时间配置	  sdk_time_t
    , SDK_MAIN_MSG_SERIAL_CFG		  //		串口参数配置	  sdk_serial_func_cfg_t
    , SDK_MAIN_MSG_IMAGE_ATTR_CFG     //		图像属性配置   sdk_image_attr_t
    , SDK_MAIN_MSG_TV_CFG         	  //		TV设置   sdk_vo_sideSize_t
	, SDK_MAIN_MSG_NOTIFY             //        通知事件发生/停止 args:SDK_EVENT_TYPE_E, chann:通道, extend:发生/停止.
    , SDK_MAIN_MSG_DEVLIST            //        GUI 查询设备列表 sdk_device_t
    , SDK_MAIN_MSG_MANUAL_REC		  //		手动打开(关闭)录像 	sdk_manual_record_t
    , SDK_MAIN_MSG_MANUAL_ALMIN		  //		手动打开(关闭)报警输入 	sdk_manual_alarmin_t
    , SDK_MAIN_MSG_MANUAL_ALMOUT	  //		手动打开(关闭)报警输出  sdk_manual_alarmout_t
    , SDK_MAIN_MSG_RECORD_BACKUP	  //		录像备份  pmsg->args:SDK_BACKUP_OP_E
    , SDK_MAIN_MSG_CLOSE_SYSTEM		  // 		关闭系统  pmsg->args:SDK_CLOSE_SYS_OP_E
    , SDK_MAIN_MSG_AUDIO_CONTROL	  // 		音频控制  args:SDK_AUDIO_CONTROL_OP_E; extend:开启/关闭
    , SDK_MAIN_MSG_PLATFORM_CFG       //        平台信息配置
    , SDK_MAIN_MSG_CHANGE_REMOTE_IP   //        改变远程设备的ip sdk_device_t
	, SDK_MAIN_MSG_DDNS_CFG			  //		DDNS配置 sdk_ddns_cfg_t
	, SDK_MAIN_MSG_CAPABILITY		  //		设备能力集信息 sdk_device_capability_t
	, SDK_MAIN_MSG_NET_LINK_CFG		  //		网络链接配置 pmsg->chann = SDK_NET_IDX_E;
	, SDK_MAIN_MSG_UPNP_CFG           //        UPNP 配置
	, SDK_MAIN_MSG_SCREEN_CFG         //        screen配置 sdk_screen_t
	, SDK_MAIN_MSG_CRUISE_CTL         //        开启巡航     无对应结构体，只认ID
	, SDK_MAIN_MSG_PRESET_SET         //        预置点集合 sdk_preset_param_t
	, SDK_MAIN_MSG_PREVIEW_TOUR       //        预览巡回配置    sdk_tour_t
	, SDK_MAIN_MSG_28181				// 国标28181
	, SDK_MAIN_MSG_NTP				//ntp时间服务
	, SDK_MAIN_MSG_SMTP				//SMTP服务
	, SDK_MAIN_MSG_ZOOM				//
	,SDK_MAIN_MSG_GET_CHANNEL_BITRATE
	,SDK_MAIN_MSG_P2P                           
	,SDK_MAIN_MSG_LOCAT_IMAGE_ATTR_CFG     //	本地图像属性配置   sdk_image_attr_t
	,SDK_MAIN_MSG_ROTATE                             //配置通道的旋转    0:0  1:90  2:180  3:270
	, SDK_MAIN_MSG_AUTO_BIND	 
	,SDK_MAIN_MSG_PRESET
	,SDK_MAIN_MSG_ALLPRESETS


	,SDK_MAIN_MSG_CRUISECFG                                      //设置某个通道某条巡航路线所有的信息 
	, SDK_MAIN_MSG_PRI_SEARCH
	, SDK_MAIN_MSG_CHL_SWITCH
	 ,SDK_MAIN_MSG_IPC_NETWORK_CFG             //配置IPC的IP地址
	 ,SDK_MAIN_MSG_IPC_DEVICE_INFO
	  ,SDK_MAIN_MSG_AUTO_MAINTAIN               //自组网
	  ,SDK_MAIN_MSG_RECORD_QUERY_STATUS  //录像查询状态       sdk_record_item_t, sdk_record_cond_t
	  ,SDK_MAIN_MSG_GET_P2P_STATUS		//获取p2p的在线状态
	  ,SDK_MAIN_MSG_SMTP_TEST_STATUS		//获取emil的在线状态
	  ,SDK_MAIN_MSG_POLL
	  ,SDK_MAIN_MSG_PB_WIN_SWITCH
	  ,SDK_MAIN_MSG_FTP
	  ,SDK_MAIN_MSG_BUTT
	
}SDK_MAIN_MSG_ID_E;

typedef int(*sdk_msg_dispatch_cb)(sdk_msg_t *pmsg);

int net_dispatch(sdk_msg_t *pmsg);
int msg_ptz_dispatch(sdk_msg_t *pmsg);
char *net_msg2main_msg_str(int msg_id);
int mcu_operate(int oper_type, int arg, void *p_arg);
int dm_msg_send(int fd, int id, int arg, int ch, void *data, int size, int timeout);
int main_free_system_resources();

#if defined (__cplusplus)
}
#endif

#endif //__main_h__




