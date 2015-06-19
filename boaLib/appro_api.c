#include <time.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "boa.h"
#include "appro_api.h"
#include "net_config.h"
#include "file_list.h"
#include "sys_env_type.h"
#include "sdk_struct.h"
#include "actvcon_list.h"
#include "databaseLib.h"
#include "main.h"
#include "appLib.h"
#include "hi3531_rtcLib.h"
#include "recordLib.h"
#include "commonLib.h"
#include "midWare.h"
#include "appFunTable.h"
#include "onvif_client.h"
#include "DbSystem.h"
#include "appFunTable.h"
#include "configLib.h"

#define PTZ_ALL_PRE_SET 0        // PTZ All preset预置点设置
#define PTZ_ALL_PRE_RECALL 1	// PTZ All preset预置点转动
/*

运动监测相关字段：
motiondetchan：通道号
motiondeten：运动监测使能
motiondetbeepen：蜂鸣器使能
motiondetbeeptime：蜂鸣器报警持续时间
motiondetrecorden：录像使能
motiondetrecordtime：录像时间
motiondetsmtpen：SmTP使能
motiondetftpen：FTP使能
motiondetsen：灵敏度
motiondetres：保留

*/

static sdk_eth_cfg_t  g_EthInfo;
static sdk_net_mng_cfg_t g_NetManageInfo={0};
static sdk_channel_t        g_channelInfo[MAX_DISPLAY_NUM];
//static sdk_channel_t        g_channelInfo;
//static sdk_sys_cfg_t	g_sysInfo;
//static sdk_record_cfg_t  g_recordInfo[MAX_RECORD_NUM_WEB];
static sdk_record_cfg_t  g_recordInfo[MAX_DISPLAY_NUM];
static sdk_preview_t	g_previewInfo;
static sdk_comm_cfg_t	g_commInfo;
//static sdk_disk_t		g_diskInfo;
static sdk_user_right_t	g_userRightInfo[MAX_USER_NUM + 1];
static sdk_default_param_t	g_defaultInfo={0};
static upgrad_pkg_file_t	g_updateInfo;
static sdk_time_t			g_timeInfo={0};
static record_search_file_t	g_recordSearchFile={0};
static sdk_ntp_conf_t      g_ntpInfo;
static sdk_email_conf_t   g_smtp_info;
static sdk_ddns_cfg_t   g_ddns_info;
static sdk_ftp_config g_ftp_info;
static sdk_p2p_conf_t g_p2p_info[MAX_P2P_TYPE];
static sdk_autoMaintain_attr_t g_AutoMaintainAttrCfg;

//static record_search_file_t	g_recordItem[1024]={0};
//static unsigned int  g_temporary=0;
static unsigned int g_channel=0;
static unsigned int g_setChannel=0;
static unsigned int g_queryNum=0;
//static unsigned int g_defaultMask=0;
static unsigned int g_setPreviewChannel=0;
static unsigned int g_getPreviewChannel=0;
static unsigned int g_setRecordChannel=0;
static unsigned int g_getRecordChannel=0;
//static unsigned int g_searchRecordFieNum=0;
static unsigned int g_is_setAllChannel=0;
static unsigned int g_is_setAllchannelRecord=0;
static int AllMotionDetChn = 0;
unsigned int event_location = 0;
static char g_usr[32] = {0};
static char gf_Restarting ;
static char gf_CmdBlocking = 0;
//static char g_updateProcessNum=0;
//编码参数
static int g_currentEncodeNo = 0;
static int AllEbcideChn = 0;
// 运动监测
static sdk_motion_cfg_v2_t   g_MotionInfo[MAX_DISPLAY_NUM];  
static unsigned int g_setMotionDetChannel=0;
static unsigned int g_getMotionDetChannel=0;
// 日志
sdk_log_cond_t g_LogCond;

// PTZ
// 如果重新定义结构体可能要修改很多，先暂时用着些
static sdk_ptz_ctl_t ptz;
static int g_ptzChannel = 0;
static sdk_cruise_param_t g_ptzCruise[MAX_DISPLAY_NUM][MAX_CRUISE_LINE_NUM];
static unsigned int g_PtzCruCh = 0;
static unsigned int g_PtzCruLineNum = 0;
static sdk_preset_all_t g_ptzAllPreset[MAX_DISPLAY_NUM];
static unsigned int g_PtzAllPreCh = 0;
static unsigned int g_PtzAllPreNum = 0;

char *g_camlistWPoint = NULL;

unsigned char lower_ascii[256] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
	0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
	0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
	0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
	0x78, 0x79, 0x7a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
	0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
	0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
	0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
	0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
	0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
	0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
	0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
	0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
	0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
	0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
	0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
	0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
	0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
	0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
	0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

void strtolower(unsigned char *str)
{
	while (*str) {
		*str = lower_ascii[*str];
		str++;
	}
}

#if 0
typedef union __NET_IPV4 {
	__u32	int32;
	__u8	str[4];
} NET_IPV4;
#endif

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
static int netsplit( char *pAddress, void *ip )
{
	unsigned int ret;
	NET_IPV4 *ipaddr = (NET_IPV4 *)ip;

	if ((ret = atoi(pAddress + 9)) > 255)
		return FALSE;
	ipaddr->str[3] = ret;

	*( pAddress + 9 ) = '\x0';
	if ((ret = atoi(pAddress + 6)) > 255)
		return FALSE;
	ipaddr->str[2] = ret;

	*( pAddress + 6 ) = '\x0';
	if ((ret = atoi(pAddress + 3)) > 255)
		return FALSE;
	ipaddr->str[1] = ret;

	*( pAddress + 3 ) = '\x0';
	if ((ret = atoi(pAddress + 0)) > 255)
		return FALSE;
	ipaddr->str[0] = ret;

	return TRUE;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int ipv4_str_to_num(char *data, struct in_addr *ipaddr)
{
	if ( strchr(data, '.') == NULL )
		return netsplit(data, ipaddr);
	return inet_aton(data, ipaddr);
}


/***************************************************************************
 *                                                                         *
 ***************************************************************************/
char *req_bufptr(request * req)
{
	return (req->buffer + req->buffer_end);
}

int LogInsert(int PriType,int MinType,char *usr)
{
	int ret = 0;
	sdk_log_item_t g_LogItem;
	g_LogItem.logId = ((PriType << 8) | MinType);
	strcpy((char *)g_LogItem.user,usr);
	ret= databaseOperate(SDK_MAIN_MSG_LOG,SDK_LOG_INSERT,NULL,0,&g_LogItem,sizeof(sdk_log_item_t));
	if(ret<0)
	{
		Printf("databaseOperate  SDK_MAIN_MSG_LOG write  fail \n");
	}

	return 0;
}


static int getDiskInfo(char *buf)
{
	char temp[4096];
	int ret, i, diskNum, strLen=0;
	sdk_disk_t *pDisk = NULL;
	
	ret = DiskQuery(temp), strLen=0;
	diskNum = ret / sizeof(sdk_disk_t);
	pDisk = (sdk_disk_t * )temp;

	for ( i = 0; i < diskNum; i++)
	{
		strLen += sprintf(buf+strLen, "diskNo%d=%d,status=%d,capability=%d,freesapce=%d,type=%d<br>",\
			i, pDisk->disk_no, pDisk->status, pDisk->total_size, pDisk->free_size, pDisk->disk_type);

		pDisk++;
	}
	
	Printf("Disk info: %s\n", buf);
	return 0;
}

#if 0
int writeLogData(int PriType,int MinType)	///char *usr, char *ipAddr, char *describe)
{
	if(LOG_STATUS_HDD == MinType)
	{
		char describe[512];
		getDiskInfo(describe);
		return LogInsert(PriType, MinType, g_usr, NULL, describe);
	}
	
	return LogInsert(PriType, MinType, g_usr, NULL, NULL);
}
#endif

void set_netIp(request *req,COMMAND_ARGUMENT*argm)
{
	memset(g_EthInfo.ip_info.ip_addr, 0, sizeof(g_EthInfo.ip_info.ip_addr));// 在使用memcpy之前最好先清空原有的数据，以免有数据残留
	memcpy(g_EthInfo.ip_info.ip_addr,argm->value,strlen(argm->value));
	//fprintf(stderr,"%s   %s   %s :%d\n",g_EthInfo.ip_info.ip_addr,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void set_netMask(request *req,COMMAND_ARGUMENT*argm)
{
	memset(g_EthInfo.ip_info.mask, 0, sizeof(g_EthInfo.ip_info.mask));
	memcpy(g_EthInfo.ip_info.mask,argm->value,strlen(argm->value));
	//fprintf(stderr,"%s   %s   %s :%d\n",g_EthInfo.ip_info.mask,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void set_netGetWay(request *req,COMMAND_ARGUMENT*argm)
{
	memset(g_EthInfo.ip_info.gateway, 0, sizeof(g_EthInfo.ip_info.gateway));
	memcpy(g_EthInfo.ip_info.gateway,argm->value,strlen(argm->value));
	//fprintf(stderr,"%s   %s   %s :%d\n",g_EthInfo.ip_info.gateway,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void set_dns1(request *req,COMMAND_ARGUMENT*argm)
{
	memset(g_EthInfo.ip_info.dns1, 0, sizeof(g_EthInfo.ip_info.dns1));
	memcpy(g_EthInfo.ip_info.dns1,argm->value,strlen(argm->value));
	//fprintf(stderr,"%s   %s   %s :%d\n",g_EthInfo.ip_info.dns1,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void set_dns2(request *req,COMMAND_ARGUMENT*argm)
{
	memset(g_EthInfo.ip_info.dns2, 0, sizeof(g_EthInfo.ip_info.dns2));
	memcpy(g_EthInfo.ip_info.dns2,argm->value,strlen(argm->value));
	//fprintf(stderr,"%s   %s   %s :%d\n",g_EthInfo.ip_info.dns2,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void set_mac(request *req, COMMAND_ARGUMENT *argm)
{	
	memset(g_EthInfo.ip_info.mac, 0, sizeof(g_EthInfo.ip_info.mac));
	memcpy(g_EthInfo.ip_info.mac,argm->value,strlen(argm->value));
	//fprintf(stderr,"%s   %s   %s :%d\n",g_EthInfo.ip_info.mac,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void set_dhcp(request *req, COMMAND_ARGUMENT *argm)
{
	__u8  value= atoi(argm->value);
	g_EthInfo.ip_info.enable_dhcp =value;
	//fprintf(stderr,"%d  %s   %s :%d\n",g_EthInfo.ip_info.enable_dhcp,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
	if(value == 1)
	{
		// 插入日志---DHCP开启
		LogInsert(LOG_STATUS_WARNING,LOG_STATUS_DHCP_ON,g_usr);
	}
	else
	{
		// 插入日志---DHCP开启
		LogInsert(LOG_STATUS_WARNING,LOG_STATUS_NETWORK_MANUAL_SETUP,g_usr);
	}
}



void set_httpPort(request *req, COMMAND_ARGUMENT *argm)
{
	int value=atoi(argm->value);
	g_NetManageInfo.http_port=value;
	//fprintf(stderr,"%d   %s   %s :%d\n",g_NetManageInfo.http_port,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void set_rtspServerPort(request *req, COMMAND_ARGUMENT *argm)
{
	int value=atoi(argm->value);
	g_NetManageInfo.dvr_data_port=value;
	//fprintf(stderr,"%d   %s   %s :%d\n",g_NetManageInfo.dvr_data_port,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}


void set_tcpPort(request *req, COMMAND_ARGUMENT *argm)
{
	int value=atoi(argm->value);
	//g_NetManageInfo.net_manager_port=value;
	//fprintf(stderr,"%d   %s   %s :%d\n",g_NetManageInfo.net_manager_port,argm->value,__FILE__,__LINE__);
	g_NetManageInfo.dvr_cmd_port=value;
	//fprintf(stderr,"%d   %s   %s :%d\n",g_NetManageInfo.dvr_cmd_port,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}


#if 0
static void * reset_netport_thread(void *args)
{
	int ch = 0;
	pthread_detach(pthread_self());
	ch = 1;
	databaseOperate(SDK_MAIN_MSG_NET_MNG_CFG,SDK_PARAM_SET,&ch,sizeof(int),&g_NetManageInfo,sizeof(g_NetManageInfo));
	return NULL;
}

static void create_reset_netport_thread()
{
	pthread_t reset_t;
	if(pthread_create(&reset_t, NULL, (void *)reset_netport_thread, NULL) < 0)
	{
		fprintf(stderr,"[%s:%s:%d]:create reset netport thread failed \n",__FILE__,__FUNCTION__,__LINE__);
	}
}
#endif
void * NetDefaultThread(void *args)
{
	pthread_detach(pthread_self());
	int ret=0;
	int channel=CHANN_TYPE_ALL;
	int temp = 0;
	stream_alarm_t alarmInfo;
	unsigned char isRecord = (g_defaultInfo.param_mask & (0x01 << SDK_PARAM_MASK_RECORD));
	unsigned char isChl= (g_defaultInfo.param_mask & (0x01 << SDK_PARAM_MASK_CHANNEL));
	temp = (g_defaultInfo.param_mask & (0x01 << SDK_PARAM_MASK_COMM)); 
	memset(&alarmInfo, 0, sizeof(stream_alarm_t));
	ret=databaseOperate(SDK_MAIN_MSG_PARAM_DEFAULT,SDK_PARAM_SET,&channel,sizeof(channel),&g_defaultInfo,sizeof(g_defaultInfo));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_PARAM_DEFAULT write  fail %s:%d\n",__FILE__,__LINE__);
	}
	g_defaultInfo.param_mask = 0;
	if(isRecord == 1)
	{
		alarmInfo.type = SDK_EVENT_TIMER_REC;
		alarmInfo.channelNo = MAX_DISPLAY_NUM;
		alarmInfo.streamNo = 0;
		alarmInfo.status = 0;
		streamInsertAlarmToAlarmQueue(NULL, &alarmInfo);

	}
	else if(isChl == 1)
	{
		alarmInfo.type = SDK_CHL_RESET;
		alarmInfo.channelNo = MAX_DISPLAY_NUM;
		alarmInfo.streamNo = 0;
		alarmInfo.status = 0;
		streamInsertAlarmToAlarmQueue(NULL, &alarmInfo);
	}
	if(temp > 0)// 当恢复setdefaultcommond为默认配置，则重启NVR。
	{
		// 插入日志---系统重启
		LogInsert(LOG_SYS_MANAGE, LOG_SYS_REBOOT,g_usr);
		system("reboot");
	}
	return NULL;
}
static void CreateNetDefaultThread()
{
	pthread_t NetDefault;
	if(pthread_create(&NetDefault, NULL, (void *)NetDefaultThread, NULL) < 0)
	{
		fprintf(stderr,"[%s:%s:%d]:create reset netport thread failed \n",__FILE__,__FUNCTION__,__LINE__);
	}
}
void set_multiCast(request *req, COMMAND_ARGUMENT *argm)
{
	int ret=0;
	int channel =1;
	memset(g_NetManageInfo.multicast, 0, sizeof(g_NetManageInfo.multicast));
	memcpy(g_NetManageInfo.multicast,argm->value,strlen(argm->value));
	//fprintf(stderr,"%s   %s   %s :%d\n",g_NetManageInfo.multicast,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
	/**/
	// 插入日志---网络配置
	LogInsert(LOG_PARAMS_SETUP,LOG_NETWORK_SETUP,g_usr);
	ret=databaseOperate(SDK_MAIN_MSG_NET_LINK_CFG,SDK_PARAM_SET,NULL,0,&g_EthInfo,sizeof(g_EthInfo));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_NET_LINK_CFG write  fail %s:%d\n",__FILE__,__LINE__);
	}
	else
	{
		netSetIp("eth0", inet_addr((char *)g_EthInfo.ip_info.ip_addr));
		netSetMask("eth0", inet_addr((char *)g_EthInfo.ip_info.mask));
		netSetGw("eth0", inet_addr((char *)g_EthInfo.ip_info.gateway));
		netSetRoute("eth0", inet_addr((char *)g_EthInfo.ip_info.gateway));
		#ifdef ENABLE_MODIFY_MAC
		netSetMac("eth0", &g_EthInfo.ip_info.mac);
		#endif
		netSetDns(inet_addr((char *)g_EthInfo.ip_info.dns1), inet_addr((char *)g_EthInfo.ip_info.dns2));
	}
	//create_reset_netport_thread();
#if 1
	ret=databaseOperate(SDK_MAIN_MSG_NET_MNG_CFG,SDK_PARAM_SET,&channel,sizeof(int),&g_NetManageInfo,sizeof(g_NetManageInfo));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_NET_LINK_CFG write  fail %s:%d\n",__FILE__,__LINE__);
	}
#endif
}


/**/
void set_channelNum(request *req, COMMAND_ARGUMENT *argm)
{

	int value=atoi(argm->value);
	if(value==CHANN_TYPE_ALL)
	{
		g_setChannel=0;
		g_is_setAllChannel=1;
		//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name,g_setChannel,argm->value,__FILE__,__LINE__);

	}
	else
	{
		g_setChannel=value;
	}
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name,g_setChannel,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void set_chanEnable(request *req, COMMAND_ARGUMENT *argm)
{
	int value=atoi(argm->value);
	g_channelInfo[g_setChannel].enable=value;
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name,g_channelInfo[g_setChannel].enable,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void set_chanName(request *req, COMMAND_ARGUMENT *argm)
{
	memset(g_channelInfo[g_setChannel].name, 0, sizeof(g_channelInfo[g_setChannel].name));
	memcpy(g_channelInfo[g_setChannel].name,argm->value,strlen(argm->value));
	//fprintf(stderr,"%s  %s   %s   %s :%d\n",argm->name,g_channelInfo[g_setChannel].name,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,g_channelInfo[g_setChannel].name);
}

void set_channelNo(request *req, COMMAND_ARGUMENT *argm)
{
	int value=atoi(argm->value);
	g_channelInfo[g_setChannel].chann_no=value;
	//fprintf(stderr,"%s  %d   %s   %s :%d\n",argm->name,g_channelInfo[g_setChannel].chann_no,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void set_chanStreamNo(request *req, COMMAND_ARGUMENT *argm)
{
	int value=atoi(argm->value);
	g_channelInfo[g_setChannel].chann_win_mode=value;
	//fprintf(stderr,"%s  %d   %s   %s :%d\n",argm->name,g_channelInfo[g_setChannel].chann_win_mode,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void set_chanDeviceIpAddr(request *req, COMMAND_ARGUMENT *argm)
{
	memset(g_channelInfo[g_setChannel].ip_addr, 0, sizeof(g_channelInfo[g_setChannel].ip_addr));
	memcpy(g_channelInfo[g_setChannel].ip_addr,argm->value,strlen(argm->value));
	//fprintf(stderr,"%s  %s   %s   %s :%d\n",argm->name,g_channelInfo[g_setChannel].device.ip_addr,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,g_channelInfo[g_setChannel].ip_addr);
}

void set_chanDevicePort(request *req, COMMAND_ARGUMENT *argm)
{
	int value=atoi(argm->value);
	g_channelInfo[g_setChannel].port=value;
	//fprintf(stderr,"%s  %d   %s   %s :%d\n",argm->name,g_channelInfo[g_setChannel].device.port,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void set_chanDevideUserName(request *req, COMMAND_ARGUMENT *argm)
{ 	
	memset(g_channelInfo[g_setChannel].user.user_name, 0, sizeof(g_channelInfo[g_setChannel].user.user_name));
	memcpy(g_channelInfo[g_setChannel].user.user_name,argm->value,strlen(argm->value));
	//fprintf(stderr,"%s  %s   %s   %s :%d\n",argm->name,g_channelInfo[g_setChannel].user.user_name,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,g_channelInfo[g_setChannel].user.user_name);
}

void set_chanDeviceUserPwd(request *req, COMMAND_ARGUMENT *argm)
{
	memset(g_channelInfo[g_setChannel].user.user_pwd, 0, sizeof(g_channelInfo[g_setChannel].user.user_pwd));
	memcpy(g_channelInfo[g_setChannel].user.user_pwd,argm->value,strlen(argm->value));
	//fprintf(stderr,"%s  %s   %s   %s :%d\n",argm->name,g_channelInfo[g_setChannel].user.user_pwd,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,g_channelInfo[g_setChannel].user.user_pwd);
}
void set_chanManProType(request *req, COMMAND_ARGUMENT *argm)
{
	int value=atoi(argm->value);
	g_channelInfo[g_setChannel].manufacturer=value;
	//fprintf(stderr,"%s  %d   %s   %s :%d\n",argm->name,g_channelInfo[g_setChannel].chann_no,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}
void set_chanRtspMainUrl(request *req, COMMAND_ARGUMENT *argm)
{
	memset(g_channelInfo[g_setChannel].rtsp_main_url, 0, sizeof(g_channelInfo[g_setChannel].rtsp_main_url));
	memcpy(g_channelInfo[g_setChannel].rtsp_main_url,argm->value,strlen(argm->value));
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,g_channelInfo[g_setChannel].rtsp_main_url);
}
void set_chanRtspSubUrl(request *req, COMMAND_ARGUMENT *argm)
{
	memset(g_channelInfo[g_setChannel].rtsp_sub_url, 0, sizeof(g_channelInfo[g_setChannel].rtsp_sub_url));
	memcpy(g_channelInfo[g_setChannel].rtsp_sub_url,argm->value,strlen(argm->value));
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,g_channelInfo[g_setChannel].rtsp_sub_url);
}
void set_chanDeviceProtocol(request *req, COMMAND_ARGUMENT *argm)
{
	int value=atoi(argm->value);
	int ret=0;
	stream_alarm_t alarmInfo;
	memset(&alarmInfo,0,sizeof(stream_alarm_t));
	// 插入日志---通道配置
	LogInsert(LOG_PARAMS_SETUP,LOG_CHN_SETUP,g_usr);
	g_channelInfo[g_setChannel].chann_protocol=value;
	//fprintf(stderr,"%s  %d   %s   %s :%d\n",argm->name,g_channelInfo[g_setChannel].chann_protocol,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);

	/**/
	if(g_is_setAllChannel==1)
	{
		g_is_setAllChannel=0;
		g_setChannel=CHANN_TYPE_ALL;
		ret=databaseOperate(SDK_MAIN_MSG_CHANN_CFG,SDK_PARAM_SET,&g_setChannel,sizeof(int),&g_channelInfo[0],sizeof(g_channelInfo[0]));
		if(ret<0)
		{
			fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_CHANN_CFG write  fail %s:%d\n",__FILE__,__LINE__);
		}
	}
	else
	{
		ret=databaseOperate(SDK_MAIN_MSG_CHANN_CFG,SDK_PARAM_SET,&g_setChannel,sizeof(int),&g_channelInfo[g_setChannel],sizeof(g_channelInfo[g_setChannel]));
		if(ret<0)
		{
			fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_CHANN_CFG write  fail %s:%d\n",__FILE__,__LINE__);
		}
	}
	alarmInfo.type = SDK_CHL_RESET;
	alarmInfo.channelNo = MAX_DISPLAY_NUM;
	alarmInfo.streamNo = 0;
	alarmInfo.status = 0;
	streamInsertAlarmToAlarmQueue(NULL, &alarmInfo);

}


/*   通道设置*/
void get_channelNum(request *req, COMMAND_ARGUMENT *argm)
{

	int value=atoi(argm->value);
	int ret=0;
	if(value==CHANN_TYPE_ALL)
	{
		g_channel=0;
		//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name,g_channel,argm->value,__FILE__,__LINE__);
	}
	else
	{
		g_channel=value;
	}
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name,g_channel,argm->value,__FILE__,__LINE__);
	ret=databaseOperate(SDK_MAIN_MSG_CHANN_CFG,SDK_PARAM_GET,&g_channel,sizeof(int),&g_channelInfo[g_channel],sizeof(g_channelInfo[g_channel]));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_CHANN_CFG fail %s:%d\n",__FILE__,__LINE__);
	}
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name,g_channel,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void get_chanEnable(request *req, COMMAND_ARGUMENT *argm)
{
	int value=0;
	value=g_channelInfo[g_channel].enable;
	//fprintf(stderr," %s %d    %s :%d\n",argm->name,g_channelInfo[g_channel].enable,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void get_chanName(request *req, COMMAND_ARGUMENT *argm)
{
	//fprintf(stderr,"%s  %s  %s :%d\n",argm->name,g_channelInfo[g_channel].name,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,g_channelInfo[g_channel].name);
}

void get_channelNo(request *req, COMMAND_ARGUMENT *argm)
{
	int value=0;
	value=g_channelInfo[g_channel].chann_no;
	//fprintf(stderr," %s %d    %s :%d\n",argm->name,g_channelInfo[g_channel].chann_no,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void get_chanStreamNo(request *req, COMMAND_ARGUMENT *argm)
{
	int value=0;
	value=g_channelInfo[g_channel].chann_win_mode;
	//fprintf(stderr," %s %d    %s :%d\n",argm->name,g_channelInfo[g_channel].chann_win_mode,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void get_chanDeviceIpAddr(request *req, COMMAND_ARGUMENT *argm)
{
	//fprintf(stderr,"%s  %s  %s :%d\n",argm->name,g_channelInfo[g_channel].device.ip_addr,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,g_channelInfo[g_channel].ip_addr);
}

void get_chanDevicePort(request *req, COMMAND_ARGUMENT *argm)
{
	int value=0;
	value=g_channelInfo[g_channel].port;
	//fprintf(stderr," %s %d    %s :%d\n",argm->name,g_channelInfo[g_channel].device.port,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void get_chanDevideUserName(request *req, COMMAND_ARGUMENT *argm)
{
	//fprintf(stderr,"%s  %s  %s :%d\n",argm->name,g_channelInfo[g_channel].user.user_name,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,g_channelInfo[g_channel].user.user_name);
}

void get_chanDeviceUserPwd(request *req, COMMAND_ARGUMENT *argm)
{
	//fprintf(stderr,"%s  %s  %s :%d\n",argm->name,g_channelInfo[g_channel].user.user_pwd,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,g_channelInfo[g_channel].user.user_pwd);
}

void get_chanDeviceProtocol(request *req, COMMAND_ARGUMENT *argm)
{
	int value=0;
	value=g_channelInfo[g_channel].chann_protocol;
	//fprintf(stderr," %s %d    %s :%d\n",argm->name,g_channelInfo[g_channel].chann_protocol,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
	/**/
}

void get_chanManProType(request *req, COMMAND_ARGUMENT *argm)
{
	//int value=0;
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,g_channelInfo[g_channel].manufacturer);
}
void get_chanRtspMainUrl(request *req, COMMAND_ARGUMENT *argm)
{
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,g_channelInfo[g_channel].rtsp_main_url);
}
void get_chanRtspSubUrl(request *req, COMMAND_ARGUMENT *argm)
{
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,g_channelInfo[g_channel].rtsp_sub_url);
}
/**/
void set_DevName(request *req, COMMAND_ARGUMENT *argm)
{
	int ret = 0;
	memset(&g_commInfo,0,sizeof(g_commInfo));
	ret=databaseOperate(SDK_MAIN_MSG_COMM_CFG,SDK_PARAM_GET,NULL,0,&g_commInfo,sizeof(g_commInfo));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_COMM_CFG write  fail %s:%d\n",__FILE__,__LINE__);
	}
	strcpy((char *)g_commInfo.dvr_name,argm->value);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s = %s\n", argm->name,argm->value);
}

void set_language(request *req, COMMAND_ARGUMENT *argm)
{
	int value=atoi(argm->value);
	g_commInfo.language=value;
	//fprintf(stderr,"%s  %d   %s   %s :%d\n",argm->name,g_commInfo.language,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void set_recycleRecord(request *req, COMMAND_ARGUMENT *argm)
{
	int value=atoi(argm->value);
	g_commInfo.recycle_record=value;
	//fprintf(stderr,"%s  %d   %s   %s :%d\n",argm->name,g_commInfo.recycle_record,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void set_drvId(request *req, COMMAND_ARGUMENT *argm)
{
	int value=atoi(argm->value);
	g_commInfo.dvr_id=value;
	//fprintf(stderr,"%s  %d   %s   %s :%d\n",argm->name,g_commInfo.dvr_id,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void set_outDevice(request *req, COMMAND_ARGUMENT *argm)
{
	int value=atoi(argm->value);
	g_commInfo.out_device=value;
	//fprintf(stderr,"%s  %d   %s   %s :%d\n",argm->name,g_commInfo.out_device,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void set_videoMode(request *req, COMMAND_ARGUMENT *argm)
{
	int value=atoi(argm->value);
	g_commInfo.video_mode=value;
	//fprintf(stderr,"%s  %d   %s   %s :%d\n",argm->name,g_commInfo.video_mode,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void set_resolution(request *req, COMMAND_ARGUMENT *argm)
{
	int value=atoi(argm->value);
	g_commInfo.resolution=value;
	//fprintf(stderr,"%s  %d   %s   %s :%d\n",argm->name,g_commInfo.resolution,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void set_standbyTime(request *req, COMMAND_ARGUMENT *argm)
{
	int value=atoi(argm->value);
	int ret =0;
	// 插入日志---普通配置
	LogInsert(LOG_PARAMS_SETUP, LOG_COMMON_SETUP,g_usr);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
	g_commInfo.standby_time=value;
	//fprintf(stderr,"%s  %d   %s   %s :%d\n",argm->name,g_commInfo.standby_time,argm->value,__FILE__,__LINE__);

	/**/
	ret=databaseOperate(SDK_MAIN_MSG_COMM_CFG,SDK_PARAM_SET,NULL,0,&g_commInfo,sizeof(g_commInfo));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_COMM_CFG write  fail %s:%d\n",__FILE__,__LINE__);
	}
}

void select_all_user_Name(request *req, COMMAND_ARGUMENT *argm)
{
	int ret=0;
	char buf[256];
	int value=atoi(argm->value);
	int i=0;
	char *r=NULL;
	g_queryNum=value;
	ret=databaseOperate(SDK_MAIN_MSG_USER,SDK_USER_QUERY,NULL,0,g_userRightInfo,sizeof(g_userRightInfo));
	if(ret<0)
	{
		fprintf(stderr,"  databaseOperate SDK_MAIN_MSG_USER  SDK_USER_QUERY fail  %s:%d\n",__FILE__,__LINE__);
	}
	r=buf;
	for(i=0;i<17;i++)
	{
		if(strlen((char *)g_userRightInfo[i].user.user_name)==0)
		{
			break;
		}
		else
		{
			r+=sprintf(r,"%s;",g_userRightInfo[i].user.user_name);	
		}
	}
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,buf);
}

/**/
void query_userNum(request *req, COMMAND_ARGUMENT *argm)
{
	int ret=0;
	int value=atoi(argm->value);
	memset(g_userRightInfo,0,sizeof(g_userRightInfo));
	g_queryNum=value;
	ret=databaseOperate(SDK_MAIN_MSG_USER,SDK_USER_QUERY,NULL,0,&g_userRightInfo,sizeof(g_userRightInfo));
	if(ret<0)
	{
		fprintf(stderr,"  databaseOperate SDK_MAIN_MSG_USER  SDK_USER_QUERY fail  %s:%d\n",__FILE__,__LINE__);
	}
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);

}

void query_userName(request *req, COMMAND_ARGUMENT *argm)
{
	//fprintf(stderr,"%s  %s  %s :%d\n",argm->name,g_userRightInfo[g_queryNum].user.user_name,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,g_userRightInfo[g_queryNum].user.user_name);
}

void query_userPwd(request *req, COMMAND_ARGUMENT *argm)
{
	//fprintf(stderr,"%s  %s  %s :%d\n",argm->name,g_userRightInfo[g_queryNum].user.user_pwd,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,g_userRightInfo[g_queryNum].user.user_pwd);
}

void query_localRight(request *req, COMMAND_ARGUMENT *argm)
{
	int value=0;
	value=g_userRightInfo[g_queryNum].local_right;
	//fprintf(stderr," %s %d    %s :%d\n",argm->name,g_userRightInfo[g_queryNum].local_right,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void query_remoteRight(request *req, COMMAND_ARGUMENT *argm)
{
	int value=0;
	value=g_userRightInfo[g_queryNum].remote_right;
	//fprintf(stderr," %s %d    %s :%d\n",argm->name,g_userRightInfo[g_queryNum].remote_right,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

/*add user cfg*/
void add_userName(request *req, COMMAND_ARGUMENT *argm)
{
	memset(g_userRightInfo[MAX_USER_NUM].user.user_name, 0, sizeof(g_userRightInfo[MAX_USER_NUM].user.user_name));
	memcpy(g_userRightInfo[MAX_USER_NUM].user.user_name,argm->value,strlen(argm->value));
	//fprintf(stderr,"%s  %s   %s   %s :%d\n",argm->name,g_userRightInfo[MAX_USER_NUM].user.user_name,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void add_userPwd(request *req, COMMAND_ARGUMENT *argm)
{
	memset(g_userRightInfo[MAX_USER_NUM].user.user_pwd, 0, sizeof(g_userRightInfo[MAX_USER_NUM].user.user_pwd));
	memcpy(g_userRightInfo[MAX_USER_NUM].user.user_pwd,argm->value,strlen(argm->value));
	//fprintf(stderr,"%s  %s   %s   %s :%d\n",argm->name,g_userRightInfo[MAX_USER_NUM].user.user_pwd,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void add_localRight(request *req, COMMAND_ARGUMENT *argm)
{
	int value=atoi(argm->value);
	g_userRightInfo[MAX_USER_NUM].local_right=value;
	//fprintf(stderr," %s %d    %s :%d\n",argm->name,g_userRightInfo[MAX_USER_NUM].local_right,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}


void add_remoteRight(request *req, COMMAND_ARGUMENT *argm)
{
	int ret=0;
	int value=atoi(argm->value);
	// 插入日志---添加用户
	LogInsert(LOG_USER_MANAGE, LOG_USER_ADD,g_usr);
	g_userRightInfo[MAX_USER_NUM].remote_right=value;
	//fprintf(stderr," %s %d    %s :%d\n",argm->name,g_userRightInfo[MAX_USER_NUM].remote_right,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
	ret=databaseOperate(SDK_MAIN_MSG_USER,SDK_USER_ADD,&g_userRightInfo[MAX_USER_NUM],sizeof(g_userRightInfo[MAX_USER_NUM]),NULL,0);
	if(ret<0)
	{
		fprintf(stderr,"  databaseOperate SDK_MAIN_MSG_USER  SDK_USER_ADD fail  %s:%d\n",__FILE__,__LINE__);
	}
	/**/
	memset(&g_userRightInfo,0,sizeof(g_userRightInfo));
	ret=databaseOperate(SDK_MAIN_MSG_USER,SDK_USER_QUERY,NULL,0,&g_userRightInfo,sizeof(g_userRightInfo));
	if(ret<0)
	{
		fprintf(stderr,"  databaseOperate SDK_MAIN_MSG_USER  SDK_USER_QUERY fail  %s:%d\n",__FILE__,__LINE__);
	}
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

/* user mode cfg*/
void mode_userName(request *req, COMMAND_ARGUMENT *argm)
{
	memset(g_userRightInfo[MAX_USER_NUM].user.user_name, 0, sizeof(g_userRightInfo[MAX_USER_NUM].user.user_name));
	memcpy(g_userRightInfo[MAX_USER_NUM].user.user_name,argm->value,strlen(argm->value));
	//fprintf(stderr,"%s  %s   %s   %s :%d\n",argm->name,g_userRightInfo[MAX_USER_NUM].user.user_name,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void mode_userPwd(request *req, COMMAND_ARGUMENT *argm)
{
	int ret = 0 ;
	memset(g_userRightInfo[MAX_USER_NUM].user.user_pwd, 0, sizeof(g_userRightInfo[MAX_USER_NUM].user.user_pwd));
	memcpy(g_userRightInfo[MAX_USER_NUM].user.user_pwd,argm->value,strlen(argm->value));

	//fprintf(stderr,"%s  %s   %s   %s :%d\n",argm->name,g_userRightInfo[MAX_USER_NUM].user.user_pwd,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
	fprintf(stderr,"%s  %s   %s   %s :%d\n",argm->name,g_userRightInfo[MAX_USER_NUM].user.user_name,argm->value,__FILE__,__LINE__);
	// 修改admin用户密码时，要关闭P2P程序，以便用户重新登录P2P客户端的预览界面

	memset(g_p2p_info, 0, sizeof(g_p2p_info));
	ret=databaseOperate(SDK_MAIN_MSG_P2P, SDK_PARAM_GET, NULL, 0, g_p2p_info, sizeof(sdk_p2p_conf_t));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_DDNS_CFG  read  fail %s:%d\n",__FILE__,__LINE__);
	}
	if((g_p2p_info[0].P2pEn != 0) || (g_p2p_info[1].P2pEn != 0))
	{
		if(!strcmp((char *)g_userRightInfo[MAX_USER_NUM].user.user_name,"admin"))
		{
			// 插入日志，P2P关闭
			LogInsert(LOG_STATUS_WARNING,LOG_STATUS_P2P_OFF,g_usr);
		}
		g_p2p_info[0].P2pEn = 0;// FreeIP的P2P不使能
		g_p2p_info[1].P2pEn = 0;// Danale的P2P不使能
		ret=databaseOperate(SDK_MAIN_MSG_P2P, SDK_PARAM_SET, NULL, 0, g_p2p_info, sizeof(sdk_p2p_conf_t));
		if(ret<0)
		{
			fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_DDNS_CFG  read  fail %s:%d\n",__FILE__,__LINE__);
		}
	}

}

void mode_localRight(request *req, COMMAND_ARGUMENT *argm)
{
	int value=atoi(argm->value);
	g_userRightInfo[MAX_USER_NUM].local_right=value;
	//fprintf(stderr," %s %d    %s :%d\n",argm->name,g_userRightInfo[MAX_USER_NUM].local_right,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}


void mode_remoteRight(request *req, COMMAND_ARGUMENT *argm)
{
	int ret=0;
	int value=atoi(argm->value);
	// 插入日志---修改用户
	LogInsert(LOG_USER_MANAGE, LOG_USER_MODIFY,g_usr);
	g_userRightInfo[MAX_USER_NUM].remote_right=value;
	//fprintf(stderr," %s %d    %s :%d\n",argm->name,g_userRightInfo[MAX_USER_NUM].remote_right,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
	ret=databaseOperate(SDK_MAIN_MSG_USER,SDK_PARAM_SET,&g_userRightInfo[MAX_USER_NUM],sizeof(g_userRightInfo[MAX_USER_NUM]),NULL,0);
	if(ret<0)
	{
		fprintf(stderr,"  databaseOperate SDK_MAIN_MSG_USER  SDK_USER_ADD fail  %s:%d\n",__FILE__,__LINE__);
	}
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

/*del user cfg*/
void del_userName(request *req, COMMAND_ARGUMENT *argm)
{
	int ret=0;
	// 插入日志---删除用户
	LogInsert(LOG_USER_MANAGE, LOG_USER_DEL,g_usr);
	memset(&g_userRightInfo[MAX_USER_NUM],0,sizeof(g_userRightInfo[MAX_USER_NUM]));
	memcpy(g_userRightInfo[MAX_USER_NUM].user.user_name,argm->value,strlen(argm->value));
	//fprintf(stderr,"%s  %s   %s   %s :%d\n",argm->name,g_userRightInfo[MAX_USER_NUM].user.user_name,argm->value,__FILE__,__LINE__);
	ret=databaseOperate(SDK_MAIN_MSG_USER,SDK_USER_DEL,&g_userRightInfo[MAX_USER_NUM],sizeof(g_userRightInfo[MAX_USER_NUM]),NULL,0);
	if(ret<0)
	{
		fprintf(stderr,"  databaseOperate SDK_MAIN_MSG_USER  SDK_USER_DEL fail  %s:%d\n",__FILE__,__LINE__);
	}

	/**/
	memset(&g_userRightInfo,0,sizeof(g_userRightInfo));
	ret=databaseOperate(SDK_MAIN_MSG_USER,SDK_USER_QUERY,NULL,0,&g_userRightInfo,sizeof(g_userRightInfo));
	if(ret<0)
	{
		fprintf(stderr,"  databaseOperate SDK_MAIN_MSG_USER  SDK_USER_QUERY fail  %s:%d\n",__FILE__,__LINE__);
	}
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

/*system default  cfg */
void set_encodeDefault(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	// 插入日志--- 编码参数恢复为默认
	//LogInsert(LOG_PARAMS_SETUP, LOG_ENCODE_DEFAULT,g_usr);
	//fprintf(stderr,"%s  %s   %s :%d\n",argm->name,argm->value,__FILE__,__LINE__);
	if(value>0)
	{
		value=0x01;
	}
	else
	{
		value=0x0;
	}
	g_defaultInfo.param_mask=g_defaultInfo.param_mask | (value<<SDK_PARAM_MASK_ENCODE);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void set_defaultPtz(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	// 插入日志--- 巡航参数恢复为默认
	//LogInsert(LOG_PARAMS_SETUP, LOG_PTZ_DEFAULT,g_usr);

	//fprintf(stderr,"%s   %s   %s :%d\n",argm->name,argm->value,__FILE__,__LINE__);
	if(value>0)
	{
		value=0x01;
	}
	else
	{
		value=0x0;
	}
	g_defaultInfo.param_mask=g_defaultInfo.param_mask | (value<<SDK_PARAM_MASK_PTZ);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void set_defaultRecord(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	// 插入日志--- 录像参数恢复为默认
	LogInsert(LOG_PARAMS_SETUP, LOG_RECODE_DEFAULT,g_usr);
	//fprintf(stderr,"%s   %s   %s :%d\n",argm->name,argm->value,__FILE__,__LINE__);
	if(value>0)
	{
		value=0x01;
	}
	else
	{
		value=0x0;
	}
	g_defaultInfo.param_mask=g_defaultInfo.param_mask | (value<<SDK_PARAM_MASK_RECORD);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void set_defaultNet(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	// 插入日志--- 网络参数恢复为默认
	LogInsert(LOG_PARAMS_SETUP, LOG_NETWORK_DEFAULT,g_usr);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
	//fprintf(stderr,"%s   %s   %s :%d\n",argm->name,argm->value,__FILE__,__LINE__);
	if(value>0)
	{
		value=0x01;
	}
	else
	{
		value=0x0;
	}
	g_defaultInfo.param_mask=g_defaultInfo.param_mask | (value<<SDK_PARAM_MASK_NET);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void set_defaultChannel(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	// 插入日志--- 通道参数恢复为默认
	LogInsert(LOG_PARAMS_SETUP, LOG_CHN_DEFAULT,g_usr);
	//fprintf(stderr,"%s   %s   %s :%d\n",argm->name,argm->value,__FILE__,__LINE__);
	if(value>0)
	{
		value=0x01;
	}
	else
	{
		value=0x0;
	}
	g_defaultInfo.param_mask=g_defaultInfo.param_mask | (value<<SDK_PARAM_MASK_CHANNEL);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void set_defaultImageAttr(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	// 插入日志---图像参数恢复为默认
	//LogInsert(LOG_PARAMS_SETUP, LOG_IMAGE_COLOR_DEFAULT,g_usr);

	//fprintf(stderr,"%s   %s   %s :%d\n",argm->name,argm->value,__FILE__,__LINE__);
	if(value>0)
	{
		value=0x01;
	}
	else
	{
		value=0x0;
	}
	g_defaultInfo.param_mask=g_defaultInfo.param_mask | (value<<SDK_PARAM_MASK_IMAGE_ATTR);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);

}

void set_defaultSerial(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	// 插入日志---串口参数恢复为默认
	//LogInsert(LOG_PARAMS_SETUP, LOG_SERIAL_DEFAULT,g_usr);

	//fprintf(stderr,"%s   %s   %s :%d\n",argm->name,argm->value,__FILE__,__LINE__);
	if(value>0)
	{
		value=0x01;
	}
	else
	{
		value=0x0;
	}
	g_defaultInfo.param_mask=g_defaultInfo.param_mask | (value<<SDK_PARAM_MASK_SERIAL);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);

}

void set_defaultOverlay(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	// 插入日志---区域遮蔽参数恢复为默认
	//LogInsert(LOG_PARAMS_SETUP, LOG_SERIAL_DEFAULT,g_usr);

	//fprintf(stderr,"%s   %s   %s :%d\n",argm->name,argm->value,__FILE__,__LINE__);
	if(value>0)
	{
		value=0x01;
	}
	else
	{
		value=0x0;
	}
	g_defaultInfo.param_mask=g_defaultInfo.param_mask | (value<<SDK_PARAM_MASK_OVERLAY);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void set_defaultMotion(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	// 插入日志---移动侦测参数恢复为默认
	LogInsert(LOG_PARAMS_SETUP, LOG_MOTION_DEFAULT,g_usr);

	fprintf(stderr,"%s   %s   %s :%d\n",argm->name,argm->value,__FILE__,__LINE__);
	if(value>0)
	{
		value=0x01;
	}
	else
	{
		value=0x0;
	}
	g_defaultInfo.param_mask=g_defaultInfo.param_mask | (value<<SDK_PARAM_MASK_MOTION);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void set_defaultCommond(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	// 插入日志---普通参数恢复为默认
	LogInsert(LOG_PARAMS_SETUP, LOG_COMMON_DEFAULT,g_usr);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
	//fprintf(stderr,"%s   %s   %s :%d\n",argm->name,argm->value,__FILE__,__LINE__);
	if(value>0)
	{
		value=0x01;
	}
	else
	{
		value=0x0;
	}
	g_defaultInfo.param_mask=g_defaultInfo.param_mask | (value<<SDK_PARAM_MASK_COMM);
}

void set_defaultOsd(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	// 插入日志---图像参数恢复为默认
	LogInsert(LOG_PARAMS_SETUP, LOG_IMAGE_COLOR_DEFAULT,g_usr);
	//fprintf(stderr,"%s   %s   %s :%d\n",argm->name,argm->value,__FILE__,__LINE__);
	if(value>0)
	{
		value=0x01;
	}
	else
	{
		value=0x0;
	}
	g_defaultInfo.param_mask=(value<<SDK_PARAM_MASK_OSD);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);

}

void set_defaultUser(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	// 插入日志---用户参数恢复为默认
	LogInsert(LOG_PARAMS_SETUP, LOG_USER_PARAM_DEFAULT,g_usr);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
	//fprintf(stderr,"%s   %s   %s :%d\n",argm->name,argm->value,__FILE__,__LINE__);
	if(value>0)
	{
		value=0x01;
	}
	else
	{
		value=0x0;
	}
	g_defaultInfo.param_mask=g_defaultInfo.param_mask | (value<<SDK_PARAM_MASK_USER);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void set_defaultAlarmIn(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	// 插入日志---告警输入参数恢复为默认
	LogInsert(LOG_PARAMS_SETUP, LOG_ALARM_DEFAULT,g_usr);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);

	//fprintf(stderr,"%s   %s   %s :%d\n",argm->name,argm->value,__FILE__,__LINE__);
	if(value>0)
	{
		value=0x01;
	}
	else
	{
		value=0x0;
	}
	g_defaultInfo.param_mask=g_defaultInfo.param_mask | (value<<SDK_PARAM_MASK_ALARM_IN);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}
void set_defaultDdns(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);

	if(value>0)
	{
		value=0x01;
	}
	else
	{
		value=0x0;
	}
	g_defaultInfo.param_mask=g_defaultInfo.param_mask | (value<<SDK_PARAM_MASK_DDNS);
}
void set_defaultSmtp(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
	if(value>0)
	{
		value=0x01;
	}
	else
	{
		value=0x0;
	}
	g_defaultInfo.param_mask=g_defaultInfo.param_mask | (value<<SDK_PARAM_MASK_EMAIL);
}
void set_defaultFactory(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
	if(value>0)
	{
		g_defaultInfo.param_mask= 0xFFFFFFFF;
	}
}
void set_defaultHide(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	//fprintf(stderr,"%s   %s   %s :%d\n",argm->name,argm->value,__FILE__,__LINE__);
	if(value>0)
	{
		value=0x01;
	}
	else
	{
		value=0x0;
	}
	g_defaultInfo.param_mask=g_defaultInfo.param_mask | (value<<SDK_PARAM_MASK_HIDE);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);

}

void set_defaultVideoLost(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	//fprintf(stderr,"%s   %s   %s :%d\n",argm->name,argm->value,__FILE__,__LINE__);
	if(value>0)
	{
		value=0x01;
	}
	else
	{
		value=0x0;
	}
	g_defaultInfo.param_mask=g_defaultInfo.param_mask | (value<<SDK_PARAM_MASK_VIDEO_LOST);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);

}

void set_defaultTv(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	//fprintf(stderr,"%s   %s   %s :%d\n",argm->name,argm->value,__FILE__,__LINE__);
	if(value>0)
	{
		value=0x01;
	}
	else
	{
		value=0x0;
	}
	g_defaultInfo.param_mask=g_defaultInfo.param_mask | (value<<SDK_PARAM_MASK_TV);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void set_defaultPreview(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	// 插入日志---预览参数恢复为默认
	//LogInsert(LOG_PARAMS_SETUP, LOG_PREVIEW_DEFAULT,g_usr);

	//fprintf(stderr,"%s   %s   %s :%d\n",argm->name,argm->value,__FILE__,__LINE__);
	if(value>0)
	{
		value=0x01;
	}
	else
	{
		value=0x0;
	}
	g_defaultInfo.param_mask=g_defaultInfo.param_mask | (value<<SDK_PARAM_MASK_PREVIEW);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);

}

void set_defaultTour(request *req, COMMAND_ARGUMENT *argm)
{
	int ret=0;
	unsigned int value=atoi(argm->value);
	//fprintf(stderr,"%s   %s   %s :%d\n",argm->name,argm->value,__FILE__,__LINE__);
	if(value>0)
	{
		value=0x01;
	}
	else
	{
		value=0x0;
	}
	g_defaultInfo.param_mask=g_defaultInfo.param_mask | (value<<SDK_PARAM_MASK_TOUR);
	//fprintf(stderr,"g_defaultInfo.param_mask =%d   %s:%d\n",g_defaultInfo.param_mask,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
	/**/
#if 1
	ret=databaseOperate(SDK_MAIN_MSG_PARAM_DEFAULT,SDK_PARAM_SET,NULL,0,&g_commInfo,sizeof(g_commInfo));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_PARAM_DEFAULT write  fail %s:%d\n",__FILE__,__LINE__);
	}
#endif
}

void set_submitDefaultCfg(request *req, COMMAND_ARGUMENT *argm)
{
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
	CreateNetDefaultThread();
}

/*set  preview cfg*/
void set_previewChanNum(request *req, COMMAND_ARGUMENT *argm)
{
	int ret=0;
	unsigned int value=atoi(argm->value);
	g_setPreviewChannel=value;
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name,g_setPreviewChannel,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
	ret=databaseOperate(SDK_MAIN_MSG_PREVIEW_CFG,SDK_PARAM_GET,NULL,0,&g_previewInfo,sizeof(g_previewInfo));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_PREVIEW_CFG  read  fail %s:%d\n",__FILE__,__LINE__);
	}
}

void set_previewDisplayNum(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_previewInfo.init_mode=value;
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name,g_previewInfo.init_mode,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void set_previewEnableViceScreen(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_previewInfo.enable=value;
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name,g_previewInfo.enable,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void set_previewPos(request *req, COMMAND_ARGUMENT *argm)
{
	int ret=0;
	unsigned int value=atoi(argm->value);
	g_previewInfo.windows[0][g_setPreviewChannel]=value;
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name,g_previewInfo.windows[0][g_setPreviewChannel],argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
	ret=databaseOperate(SDK_MAIN_MSG_PREVIEW_CFG,SDK_PARAM_SET,NULL,0,&g_previewInfo,sizeof(g_previewInfo));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_PREVIEW_CFG write  fail %s:%d\n",__FILE__,__LINE__);
	}

}

/*get preview cfg*/
void get_previewChanNum(request *req, COMMAND_ARGUMENT *argm)
{
	int ret=0;
	unsigned int value=atoi(argm->value);
	g_getPreviewChannel=value;
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name,g_getPreviewChannel,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
	ret=databaseOperate(SDK_MAIN_MSG_PREVIEW_CFG,SDK_PARAM_GET,NULL,0,&g_previewInfo,sizeof(g_previewInfo));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_PREVIEW_CFG  read  fail %s:%d\n",__FILE__,__LINE__);
	}
}

void get_previewDisplayNum(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=0;
	value=g_previewInfo.init_mode;
	//fprintf(stderr," %s %d    %s :%d\n",argm->name,g_previewInfo.init_mode,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void get_previewEnableViceScreen(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	value=g_previewInfo.enable;
	//fprintf(stderr," %s %d    %s :%d\n",argm->name,g_previewInfo.enable,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void get_previewPos(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=0;
	value=g_previewInfo.windows[0][g_getPreviewChannel];
	//fprintf(stderr," %s %d    %s :%d\n",argm->name,g_previewInfo.windows[0][g_getPreviewChannel],__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

/*set record cfg*/
void set_recChanNum(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
	//memset(g_recordInfo,0,sizeof(g_recordInfo));
	if(value==CHANN_TYPE_ALL)
	{
		g_setRecordChannel=0;
		g_is_setAllchannelRecord=1;
	}
	else
	{
		g_setRecordChannel=value;
	}
}

// 符号之间不能有空格
int SetRecStrTok(char *str)
{
	int SchedTimeNum = 0, WordSegNum = 0;
	int i = 0, ret = 0;
	int Weekday;
	char *p1 = NULL,*p2 = NULL;
	char RecSegStr[2 + MAX_REC_SEG][512];
	p1 = strtok(str,";");
	while(p1 != NULL)// 截取这天的某个时间段
	{
		strcpy(RecSegStr[SchedTimeNum],p1);// 保存到数组中，总共有6个时间段
		//Printf("RecSegStr[%d]:%s\n",i,RecSegStr[i++]);
		p1 = strtok(NULL,";");
		if(SchedTimeNum++ == MAX_REC_SEG+1)
		{
			break;
		}
	}
	Weekday = atoi(RecSegStr[0]);// 星期几
	p2 = strstr(RecSegStr[1],":");// 第一个数组保存的是某天录像时间的总段数。
	p2++;
	g_recordInfo[g_setRecordChannel].record_sched[Weekday].record_time_num = atoi(p2);
	for(SchedTimeNum = 2; SchedTimeNum < 2 + MAX_REC_SEG; SchedTimeNum++)
	{
		WordSegNum = 0;
		p1 = strtok(RecSegStr[SchedTimeNum],",");
		while(p1!= NULL)
		{
			p2 = strstr(p1,":");
			p2++;
			switch(WordSegNum)
			{
				case 0:
					g_recordInfo[g_setRecordChannel].record_sched[Weekday].sched_time[SchedTimeNum - 2].record_type = atoi(p2);
					break;
				case 1:
					g_recordInfo[g_setRecordChannel].record_sched[Weekday].sched_time[SchedTimeNum - 2].start_hour = atoi(p2);
					break;
				case 2:
					g_recordInfo[g_setRecordChannel].record_sched[Weekday].sched_time[SchedTimeNum - 2].start_min= atoi(p2);
					break;
				case 3:
					g_recordInfo[g_setRecordChannel].record_sched[Weekday].sched_time[SchedTimeNum - 2].stop_hour= atoi(p2);
					break;
				case 4:
					g_recordInfo[g_setRecordChannel].record_sched[Weekday].sched_time[SchedTimeNum - 2].stop_min= atoi(p2);
					break;
				default:
					break;
			}
			p1 = strtok(NULL,",");
			WordSegNum++;
		}
	}
	if(Weekday == 7)// 每天都录像
	{
		for(i = 0; i < 7; i++)
		{
			memcpy(&g_recordInfo[g_setRecordChannel].record_sched[i], &g_recordInfo[g_setRecordChannel].record_sched[Weekday], sizeof(g_recordInfo[g_setRecordChannel].record_sched[Weekday]));
		}
	}

	if(g_is_setAllchannelRecord==1)// 全部通道
	{
		g_is_setAllchannelRecord=0;
		g_setRecordChannel=CHANN_TYPE_ALL;
		ret=databaseOperate(SDK_MAIN_MSG_RECORD_CFG,SDK_PARAM_SET,&g_setRecordChannel,sizeof(int),&g_recordInfo[0],sizeof(g_recordInfo[0]));
		if(ret<0)
		{
			fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_RECORD_CFG  write  fail %s:%d\n\n",__FILE__,__LINE__);

		}

	}
	else  // 单个通道
	{
		ret=databaseOperate(SDK_MAIN_MSG_RECORD_CFG,SDK_PARAM_SET,&g_setRecordChannel,sizeof(int),&g_recordInfo[g_setRecordChannel],sizeof(g_recordInfo[g_setRecordChannel]));
		if(ret<0)
		{
			fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_RECORD_CFG  write  fail %s:%d\n",__FILE__,__LINE__);
		}
	}
	return 0;
}

void set_weeday(request *req, COMMAND_ARGUMENT *argm)
{
	char *value = argm->value;
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n",argm->name,argm->value);
	SetRecStrTok(value);
}

void get_recChanNum(request *req, COMMAND_ARGUMENT *argm)
{
	int ret=0;
	unsigned int value=atoi(argm->value);
	if(value==CHANN_TYPE_ALL)
	{
		g_getRecordChannel=0;
	}
	else
	{
		g_getRecordChannel=value;
	}
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
	memset(&g_recordInfo[g_getRecordChannel],0,sizeof(sdk_record_cfg_t));
	ret=databaseOperate(SDK_MAIN_MSG_RECORD_CFG,SDK_PARAM_GET,&g_getRecordChannel,sizeof(int),&g_recordInfo[g_getRecordChannel],sizeof(g_recordInfo[g_getRecordChannel]));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_RECORD_CFG  read  fail %s:%d\n",__FILE__,__LINE__);
	}
}

int GetRecStrCat(int Weekday,char *RecStr)
{
	int SchedTimeNum ;
	char IntString[64] = {0};
	char SegStr[128] = {0};
	memset(SegStr,0,sizeof(SegStr));
	sprintf(IntString,"getrectotalseg:%d",g_recordInfo[g_getRecordChannel].record_sched[Weekday].record_time_num);
	strcat(SegStr,IntString);    
	strcat(SegStr,",");
	for(SchedTimeNum = 0; SchedTimeNum < MAX_REC_SEG; SchedTimeNum++)
	{	
		memset(IntString,0,sizeof(IntString));
		sprintf(IntString,"getrectype%d:%d",SchedTimeNum,g_recordInfo[g_getRecordChannel].record_sched[Weekday].sched_time[SchedTimeNum].record_type);
		strcat(SegStr,IntString);    
		strcat(SegStr,",");
		memset(IntString,0,sizeof(IntString));
		sprintf(IntString,"getrecstarthour%d:%d",SchedTimeNum,g_recordInfo[g_getRecordChannel].record_sched[Weekday].sched_time[SchedTimeNum].start_hour);
		strcat(SegStr,IntString);    
		strcat(SegStr,",");
		memset(IntString,0,sizeof(IntString));
		sprintf(IntString,"getrecstartmin%d:%d",SchedTimeNum,g_recordInfo[g_getRecordChannel].record_sched[Weekday].sched_time[SchedTimeNum].start_min);
		strcat(SegStr,IntString);    
		strcat(SegStr,",");
		memset(IntString,0,sizeof(IntString));
		sprintf(IntString,"getrecstophour%d:%d",SchedTimeNum,g_recordInfo[g_getRecordChannel].record_sched[Weekday].sched_time[SchedTimeNum].stop_hour);
		strcat(SegStr,IntString);    
		strcat(SegStr,",");
		memset(IntString,0,sizeof(IntString));
		sprintf(IntString,"getrecstopmin%d:%d",SchedTimeNum,g_recordInfo[g_getRecordChannel].record_sched[Weekday].sched_time[SchedTimeNum].stop_min);
		strcat(SegStr,IntString);
		strcat(SegStr,";");
		//Printf("SegStr%d:%s   sizeof:%d\n",SchedTimeNum,SegStr,strlen(SegStr));
		strcat(RecStr,SegStr);
		memset(SegStr,0,sizeof(SegStr));
	}
	return 0;
}
void get_weekday(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	char RecStr[1024] = {0};
	GetRecStrCat(value, RecStr);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d;%s\n", argm->name,value,RecStr);
}

/* update info*/
void query_updateName(request *req, COMMAND_ARGUMENT *argm)
{
	int ret=0;
	//fprintf(stderr,"%s  %s  %s :%d\n",argm->name,g_updateInfo.name,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,g_updateInfo.name);
	ret=cmdOperate(SDK_MAIN_MSG_UPGRAD, SDK_UPGRAD_FIND_FILE, NULL, 0,&g_updateInfo ,sizeof(g_updateInfo));
	if(ret<0)
	{
		fprintf(stderr,"SDK_MAIN_MSG_UPGRAD SDK_UPGRAD_FIND_FILE faile  %s:%d\n",__FILE__,__LINE__);
	}
}

void query_updateFilePath(request *req, COMMAND_ARGUMENT *argm)
{
	//fprintf(stderr,"%s  %s  %s :%d\n",argm->name,g_updateInfo.file_path,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,g_updateInfo.file_path);
}

void query_updateFileSize(request *req, COMMAND_ARGUMENT *argm)
{
	//fprintf(stderr,"%s  %d  %s :%d\n",argm->name,g_updateInfo.size,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,g_updateInfo.size);
}

void query_updateFileDate(request *req, COMMAND_ARGUMENT *argm)
{
	//fprintf(stderr,"%s  %s  %s :%d\n",argm->name,g_updateInfo.date,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,g_updateInfo.date);
}

/*get update progress*/
void get_updateProgress(request *req, COMMAND_ARGUMENT *argm)
{
	int ret=0;
	int progress=0;
	ret=cmdOperate(SDK_MAIN_MSG_UPGRAD, SDK_UPGRAD_PROGRESS, NULL, 0,&progress ,sizeof(int));
	if(ret<0)
	{
		//fprintf(stderr,"SDK_MAIN_MSG_UPGRAD SDK_UPGRAD_FIND_FILE faile  %s:%d\n",__FILE__,__LINE__);
		req->buffer_end += sprintf(req_bufptr(req), OPTION_UW "%s\n", argm->name);
	}
	else
	{
		//fprintf(stderr,"%s  %s :%d\n",argm->name,__FILE__,__LINE__);
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,progress);
	}
}

/*close system*/
void set_systemShutDown(request *req, COMMAND_ARGUMENT *argm)
{
	int ret=0;
	// 插入日志---关机
	LogInsert(LOG_SYS_MANAGE, LOG_SYS_SHUTDOWN,g_usr);
	ret=cmdOperate(SDK_MAIN_MSG_CLOSE_SYSTEM, SDK_CLOSE_SYS_SHUTDOWN, NULL, 0,NULL ,0);
	if(ret<0)
	{
		//fprintf(stderr,"SDK_MAIN_MSG_CLOSE_SYSTEM SDK_CLOSE_SYS_SHUTDOWN faile  %s:%d\n",__FILE__,__LINE__);
		req->buffer_end += sprintf(req_bufptr(req), OPTION_UW "%s\n", argm->name);
	}
	else
	{
		//fprintf(stderr,"%s  %s :%d\n",argm->name,__FILE__,__LINE__);
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
	}
}

void set_systemReboot(request *req, COMMAND_ARGUMENT *argm)
{
	int ret=0;
	// 插入日志---系统重启
	LogInsert(LOG_SYS_MANAGE, LOG_SYS_REBOOT,g_usr);
	ret=cmdOperate(SDK_MAIN_MSG_CLOSE_SYSTEM, SDK_CLOSE_SYS_REBOOT, NULL, 0,NULL,0);
	if(ret<0)
	{
		//fprintf(stderr,"SDK_MAIN_MSG_CLOSE_SYSTEM SDK_CLOSE_SYS_REBOOT faile  %s:%d\n",__FILE__,__LINE__);
		req->buffer_end += sprintf(req_bufptr(req), OPTION_UW "%s\n", argm->name);
	}
	else
	{
		//fprintf(stderr,"%s  %s :%d\n",argm->name,__FILE__,__LINE__);
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
	}
}

/*date clock cfg*/
int systemTime(sdk_time_t *time,int flag)
{
	int ret = 0;
	HI_Rtc_Time_t  hi_rtc_time={0};
	sdk_time_t *p_cfg = (sdk_time_t*)time;

	if(flag == SDK_PARAM_SET)
	{
		memset(&hi_rtc_time,0,sizeof(hi_rtc_time));
		//fprintf(stderr,"p_cfg->year=%d\n",p_cfg->year);
		if(p_cfg->year<=2000)
		{
			hi_rtc_time.year=2000-2000;
		}
		else
		{
			hi_rtc_time.year=p_cfg->year-2000;
		}

		if((p_cfg->mon>12)&&(p_cfg->mon<=0))
		{
			hi_rtc_time.month=1;
		}
		else
		{
			hi_rtc_time.month=p_cfg->mon;
		}

		if((p_cfg->day<=0)&&(p_cfg->day>31))
		{
			hi_rtc_time.day=1;
		}
		else
		{
			hi_rtc_time.day=p_cfg->day;
		}

		if((hi_rtc_time.hour>=24)&&(hi_rtc_time.hour<0))
		{
			hi_rtc_time.hour=0;
		}
		else
		{
			hi_rtc_time.hour=p_cfg->hour;
		}

		if((hi_rtc_time.minutes>=60)&&(hi_rtc_time.minutes<0))
		{
			hi_rtc_time.minutes=0;
		}
		else
		{
			hi_rtc_time.minutes=p_cfg->min;
		}

		if((hi_rtc_time.seconds>=60)&&(hi_rtc_time.seconds<0))
		{
			hi_rtc_time.seconds=0;
		}
		else
		{
			hi_rtc_time.seconds=p_cfg->sec;
		}
		hi_rtc_time.week=5;
		hi_rtc_time.Hour_Mode=0;
		ret=write_rtc(& hi_rtc_time);
		if(ret<0)
		{
			fprintf(stderr,"write_rtc  fail \n");
		}
		hi_rtc_time.year = hi_rtc_time.year+2000;
		ret=setTimeTo_system(&hi_rtc_time);
		if(ret<0)
		{
			fprintf(stderr,"setTimeTo_system  fail \n");
		}
	}
	else
	{
		memset(&hi_rtc_time,0,sizeof(hi_rtc_time));
		ret=read_rtc(&hi_rtc_time);
		if(ret<0)
		{
			fprintf(stderr,"read_rtc fail\n");
		}
		hi_rtc_time.year=hi_rtc_time.year+2000;
		//fprintf(stderr,"hi_rtc_time.year=%d\n",hi_rtc_time.year);
		//fprintf(stderr,"hi_rtc_time.month=%d\n",hi_rtc_time.month);
		//fprintf(stderr,"hi_rtc_time.day=%d\n",hi_rtc_time.day);
		//fprintf(stderr,"hi_rtc_time.hour=%d\n",hi_rtc_time.hour);
		//fprintf(stderr,"hi_rtc_time.minutes=%d\n",hi_rtc_time.minutes);
		//fprintf(stderr,"hi_rtc_time.seconds=%d\n",hi_rtc_time.seconds);
		ret=setTimeTo_system(&hi_rtc_time);
		if(ret<0)
		{
			fprintf(stderr,"setTimeTo_system  fail %s:%d \n",__FILE__,__LINE__);
		}
		p_cfg->year=hi_rtc_time.year;
		p_cfg->mon=hi_rtc_time.month;
		p_cfg->day=hi_rtc_time.day;
		p_cfg->hour=hi_rtc_time.hour;
		p_cfg->min=hi_rtc_time.minutes;
		p_cfg->sec=hi_rtc_time.seconds;
	}
	return 0;
}
void  set_systemDateYear(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_timeInfo.year=value;
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name,g_timeInfo.year,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void  set_systemDateMonth(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_timeInfo.mon=value;
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name,g_timeInfo.mon,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void  set_systemDateDay(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_timeInfo.day=value;
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name,g_timeInfo.day,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void  set_systemDateHour(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_timeInfo.hour=value;
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name,g_timeInfo.hour,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void  set_systemDateMinutes(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_timeInfo.min=value;
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name,g_timeInfo.min,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void  set_systemDateSeconds(request *req, COMMAND_ARGUMENT *argm)
{
	int ret=0;
	int flag=0x01;
	unsigned int value=atoi(argm->value);
	g_timeInfo.sec=value;
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name,g_timeInfo.sec,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
	ret=systemTime(&g_timeInfo, flag);
	if(ret<0)
	{
		fprintf(stderr,"set systemTime fail %s:%d\n",__FILE__,__LINE__);
	}
}

/*set system time*/
void  get_systemDateYear(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=0;
	int flag=0x02;
	int ret=0;
	ret=systemTime(&g_timeInfo, flag);
	if(ret<0)
	{
		fprintf(stderr,"get systemTime fail %s:%d\n",__FILE__,__LINE__);
	}
	value=g_timeInfo.year;
	//fprintf(stderr," %s %d    %s :%d\n",argm->name,g_timeInfo.year,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void  get_systemDateMonth(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=0;
	value=g_timeInfo.mon;
	//fprintf(stderr," %s %d  %s :%d\n",argm->name,g_timeInfo.mon,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void  get_systemDateDay(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=0;
	value=g_timeInfo.day;
	//fprintf(stderr," %s %d  %s :%d\n",argm->name,g_timeInfo.day,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void  get_systemDateHour(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=0;
	value=g_timeInfo.hour;
	//fprintf(stderr," %s %d  %s :%d\n",argm->name,g_timeInfo.hour,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void  get_systemDateMinutes(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=0;
	value=g_timeInfo.min;
	//fprintf(stderr," %s %d  %s :%d\n",argm->name,g_timeInfo.min,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void  get_systemDateSeconds(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=0;
	value=g_timeInfo.sec;
	//fprintf(stderr," %s %d  %s :%d\n",argm->name,g_timeInfo.sec,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}



void play_backChannel(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_recordSearchFile.diskNo=value;
	//fprintf(stderr," %s %d  %s :%d\n",argm->name,g_recordSearchFile.diskNo,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void play_backStartTime(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_recordSearchFile.startTime=value;
	//fprintf(stderr," %s %d  %s :%d\n",argm->name,g_recordSearchFile.startTime,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void play_backEndTime(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_recordSearchFile.endTime=value;
	//fprintf(stderr," %s %d  %s :%d\n",argm->name,g_recordSearchFile.endTime,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void play_backFileType(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	int itemNum=0;
	int i=0;
	char *r=NULL;
	char buf[20*1024]={0};
	record_search_file_t recordItem[1024];
	memset(&recordItem,0,sizeof(recordItem));
	g_recordSearchFile.fileType=value;
	//fprintf(stderr," %s %d  %s :%d\n",argm->name,g_recordSearchFile.fileType,__FILE__,__LINE__);

	itemNum = recordFindDevChannelRecord(g_recordSearchFile.diskNo, 0,g_recordSearchFile.fileType,g_recordSearchFile.startTime, g_recordSearchFile.endTime, (record_search_file_t *)&recordItem);

	//fprintf(stderr,"itemNum =%d  %s:%d\n",itemNum,__FILE__,__LINE__);
	memset(&buf,0,sizeof(buf));
	r=buf;
	if(itemNum<=0)
	{
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d have no playback file\n", argm->name,value);
	}
	else
	{
#if 0
		for(i=0;i<itemNum;i++)
		{
			fprintf(stderr,"recordNo=%d\n",recordItem[i].recordNo);
		}
#endif
#if 1
		for(i=0;i<itemNum;i++)
		{
			r+=sprintf(r,"item=%d",i);
			r+=sprintf(r,"diskNo=%u",recordItem[i].diskNo);
			r+=sprintf(r,"recordNo=%u",recordItem[i].recordNo);
			r+=sprintf(r,"fileType=%u",recordItem[i].fileType);
			r+=sprintf(r,"startTime=%u",recordItem[i].startTime);
			r+=sprintf(r,"endTime=%u",recordItem[i].endTime);
			r+=sprintf(r,"startAddr=%u",recordItem[i].startAddr);
			r+=sprintf(r,"endAddr=%u",recordItem[i].endAddr);
			r+=sprintf(r,"recordNo=%u",recordItem[i].recordNo);
		}
#endif

		//fprintf(stderr,"buf=%s  %s:%d\n",buf,__FILE__,__LINE__);
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d=%s\n", argm->name,value,buf);
	}
}




void get_audioenable(request *req, COMMAND_ARGUMENT *argm)
{
	do {
		//SysInfo* pSysInfo = GetSysInfo();
		//if(pSysInfo == NULL)
		//	break;
		//req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name, pSysInfo->audio_config.audioenable);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

int CheckAudioStatus(void)
{
	//SysInfo* pSysInfo = GetSysInfo();
	//return pSysInfo->audio_config.audioenable;
	return 0;
}


void get_date(request *req, COMMAND_ARGUMENT *argm)
{
	static char wday_name[7][3] = {
		"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	time_t tnow;
	struct tm *tmnow;
	int string_len;

	time(&tnow);
	tmnow = localtime(&tnow);

	string_len = sprintf(req_bufptr(req), OPTION_OK "%s=%04d-%02d-%02d %.3s\n", argm->name,
			tmnow->tm_year+1900, tmnow->tm_mon+1, tmnow->tm_mday, wday_name[tmnow->tm_wday]);
	req->buffer_end += string_len;
}

void get_time(request *req, COMMAND_ARGUMENT *argm)
{
	time_t tnow;
	struct tm *tmnow;
	int string_len;

	time(&tnow);
	tmnow = localtime(&tnow);

	string_len = sprintf(req_bufptr(req), OPTION_OK "%s=%02d:%02d:%02d\n", argm->name,
			tmnow->tm_hour, tmnow->tm_min, tmnow->tm_sec);
	req->buffer_end += string_len;
}

int sys_set_date(int year, int month, int day)
{
	struct tm *tm;
	time_t now;
	now = time(NULL);
	tm = localtime(&now);

	year = (year>1900) ? year-1900 : 0;
	tm->tm_year = year;
	month = (month>0) ? month-1 : 0;
	tm->tm_mon = month;
	tm->tm_mday = day;

	if ((now = mktime(tm)) < 0)
		return -1;
	stime(&now);
	if(system("hwclock -uw")<0)
	{
		fprintf(stderr,"system fail  %s:%d\n",__FILE__,__LINE__);
	}

	return 0;
}

int sys_set_time(int hour, int min, int sec)
{
	struct tm *tm;
	time_t now;
	//unsigned char v2;

	now = time(NULL);
	tm = localtime(&now);

	tm->tm_hour = hour;
	tm->tm_min = min;
	tm->tm_sec = sec + 3;

	if ((now = mktime(tm)) < 0)
		return -1;
	//ControlSystemData(SFIELD_SCHEDULE_STOP, (void *)&v2, sizeof(v2));
	stime(&now);
	if(system("hwclock -uw")<0)
	{
		fprintf(stderr,"system fail %s:%d\n",__FILE__,__LINE__);
	}

	return 0;
}

void set_system_date(request *req, COMMAND_ARGUMENT *argm)
{
	int year, month, day;

	do {
		if (sscanf(argm->value, "%d/%d/%d", &year, &month, &day) != 3)
			break;

		if (sys_set_date(year, month, day) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_system_time(request *req, COMMAND_ARGUMENT *argm)
{
	int hour, min, sec;

	do {
		if (sscanf(argm->value, "%d:%d:%d", &hour, &min, &sec) != 3)
			break;

		if (sys_set_time(hour, min, sec) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_gioouttype(request *req, COMMAND_ARGUMENT *argm)
{
	//__u8 value = atoi(argm->value);
	do {
		//if(ControlSystemData(SFIELD_SET_GIOOUTTYPE, &value, sizeof(value)) < 0)
		//break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}


//--------------------------------encode param

static sdk_encode_t g_encode[MAX_DISPLAY_NUM];


void get_encodeCfg(request *req, COMMAND_ARGUMENT *argm)
{
	int chno = atoi(argm->value);

	if (chno < 0 || chno >= MAX_DISPLAY_NUM )
	{
		req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
		return;
	}

	memset(&g_encode[chno], 0, sizeof(sdk_encode_t));
	if (Remote_GetEncodeCfg(chno, &g_encode[chno] )<0)
	{
		fprintf(stderr, "%s", "Remote_GetEncodeCfg fail\n");
		req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
		return 0;
	}	

	sdk_av_enc_t *pEn = NULL;

	char tmp[4096] = { 0 };
	char *p = tmp;

#if 0
	//	p += sprintf(p, "%s", "encodingList=H264,MPEG4,JPEG<br>");
#else
	p += sprintf(p, "%s", "encodingList=H264<br>");
#endif
	p += sprintf(p, "%s", "profilelevelList=baseline,main,high<br>");	
	p += sprintf(p, "%s", "audioList=G.711,G.726<br>");

	int i = 0;
	for (i = 0; i < 2; i++) {

		if ( 0 == i ) {
			pEn = &g_encode[chno].main;
		} else {
			pEn = &g_encode[chno].second;
		}

#if 0
		if (0 == pEn->video_enc) {					
			p += sprintf(p, "encoding%d=%d<br>", i, 0);
		} else if ( 1 == pEn->video_enc ) {		
			p += sprintf(p, "encoding%d=%d<br>", i, 1);
		} else {		
			p += sprintf(p, "encoding%d=%d<br>", i, 2);	
		}
#else
		p += sprintf(p, "encoding%d=%d<br>", i, 0);
#endif
		if ( 0 == i ) 
			{
			    int j ;
                               p += sprintf(p, "resolutionList%d=",i);
					for(j=0;j!=pEn->res_num -1;++j)
					{
					 p += sprintf(p, "%dx%d,",pEn->resolution_all[j].width,pEn->resolution_all[j].height);

					}
					 p += sprintf(p, "%dx%d<br>",pEn->resolution_all[pEn->res_num-1].width,pEn->resolution_all[pEn->res_num-1].height );


			p += sprintf(p, "resolution%d=%d<br>", i, pEn->resolution_current);
		} 
		else {
                          int k;
 			    p += sprintf(p, "resolutionList%d=",i);
					for(k=0;k!=pEn->res_num -1;++k)
					{
					 p += sprintf(p, "%dx%d,",pEn->resolution_all[k].width,pEn->resolution_all[k].height);

					}
					 p += sprintf(p, "%dx%d<br>",pEn->resolution_all[pEn->res_num-1].width,pEn->resolution_all[pEn->res_num-1].height );


			p += sprintf(p, "resolution%d=%d<br>", i, pEn->resolution_current);
		}

		p += sprintf(p, "bitrate%d=%d<br>", i, pEn->bitrate);
		p += sprintf(p, "framerate%d=%d<br>", i, pEn->frame_rate);
		p += sprintf(p, "quality%d=%d<br>", i, pEn->pic_quilty);			
		p += sprintf(p, "profilelevel%d=%d<br>", i, pEn->level);
		p += sprintf(p, "audioformat%d=%d<br>", i, 0);

	}	

	p = tmp;

	req->buffer_end += sprintf(req_bufptr(req), "%s\n", p);
}

#if 0
uint8_t resolution;     //SDK_VIDEO_RESOLUTION_E
uint8_t bitrate_type;   // 0--;   1-- SDK_CODEC_BR_TYPE_E
uint8_t pic_quilty;     //0-1-2-3-4-5-  
uint8_t frame_rate;     //1-25(PAL)/30
uint8_t gop;            //I 1-200
uint8_t video_enc;      //0--h264 1--MJPEG 2--JPEG  SDK_CODEC_ENC_TYPE_E
uint8_t audio_enc;      //0:G711A
uint8_t mix_type;       //: 0, : 1.
uint16_t bitrate;       // 32kbps-16000kbps
uint8_t level;          //h264: 0--baseline, 1--main, 2--high;
uint8_t h264_ref_mod; 
#endif

//encoding0=0&resolution0=1&bitrate0=3072&framerate0=25&quality0=100&profilelevel0=0&audio_enc0=0&encoding1=0&resolution1=1&bitrate1=3072&framerate1=25&quality1=100&profilelevel1=0&audio_enc1=0

void GetEncodeItem( char *pData, sdk_encode_t *pEncode )
{
	char *p1 = pData;
	char *p2 = pData;

	int streamId = 0;
	char tmp;
	sdk_av_enc_t *pEn = NULL;

	while ( *p2 != '\0' ) 
	{
		if (' ' == *p2) {
			*p2 = '\0';
		}

		if ( 0 == streamId) {
			pEn = &pEncode->main;			
		} else {
			pEn = &pEncode->second;
		}

		if ( '\0' == *p2 ) {

			if (' ' == *p1 ) {
				p1++;
			}

			tmp = *p1;
			p1 = strstr(p1, "=");	
			if ( p1 ) {
				p1++;				
			}

			if ( 'e' == tmp ) {	
				printf("=========== e%d = %d \n", streamId, atoi(p1));

				pEn->video_enc = 2;

			} else if ( 'r' == tmp ) {			
				pEn->resolution = atoi(p1);
			} else if ( 'b' == tmp ) {
				pEn->bitrate = atoi(p1);
			} else if ( 'f' == tmp ) {			
				pEn->frame_rate = atoi(p1);
			} else if ( 'p' == tmp ) {			
				pEn->level = atoi(p1);
			} else if ( 'q' == tmp ) {			
				pEn->pic_quilty = atoi(p1);
			} else if ( 'a' == tmp ) {	
				pEn->audio_enc = atoi(p1);	

				streamId++;			
			}

			//fprintf(stderr, "=====================tmp %c p1 %s \n", tmp, p1);
			p2++;
			p1 = p2;
		}
		p2++;
	}
}

//i;encoding0 H264,resolution0 1280x720,bitrate0 3072,framerate0 25,profilelevel 0;
//encoding1 H264,resolution1 704x576,bitrate1 1024,framerate1 25,profilelevel 0;
void set_encodeCfg(request *req, COMMAND_ARGUMENT *argm)
{	
	//fprintf(stderr, "set encodeCfg %s \n", argm->value);

	int chno = 0;

	char *pBegin = argm->value;
	char *pEnd = pBegin;
	//sdk_av_enc_t *pEn = NULL;

	int len = strlen(argm->value);
	if ( len <= 0) {
		return;
	}	

	sdk_encode_t *pEncode = (sdk_encode_t *)malloc(sizeof(sdk_encode_t));
	if (!pEncode) {
		return;
	}

	memset(pEncode, 0, sizeof(sdk_encode_t));

	//fprintf(stderr, "==================== pBegin %s \n", pBegin);

	while ( pEnd < argm->value + len)
	{
		if (';' == *pEnd) {
			*pEnd = '\0';			

			chno = atoi(pBegin);
			if (Remote_GetEncodeCfg(chno, pEncode ) ) {
				fprintf(stderr, "%s", "Remote_GetEncodeCfg success\n");
			}			

			pEnd++;
			pBegin = pEnd;	

			//fprintf(stderr, "==================== pBegin %s \n", pBegin);

			GetEncodeItem(pBegin, pEncode);
			break;					
		}
		pEnd++;
	}

	Remote_SetEncodeCfg(chno, pEncode );

	if (pEncode) {
		free(pEncode);
		pEncode = NULL;
	}

	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

// -----------------------------image param

static sdk_image_attr_t g_img[MAX_DISPLAY_NUM];
int g_currentImageNo = 0;

void get_image( request *req, COMMAND_ARGUMENT *argm)
{

	int chno = atoi(argm->value);

	//fprintf(stderr, "get image %s \n", argm->value);

	if (chno < 0 || chno >= MAX_DISPLAY_NUM ) {
		req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
		return;
	}	

	memset(&g_img[chno], 0, sizeof(sdk_image_attr_t));
	Remote_GetImageParam(chno, &g_img[chno]);

	req->buffer_end += sprintf(req_bufptr(req), "bright=%d<br>contrast=%d<br>saturation=%d<br>sharpness=%d\n", 
			g_img[chno].brightness, g_img[chno].contrast, g_img[chno].saturation, g_img[chno].sharpness );


}






void Get_IPCInfomation( request *req, COMMAND_ARGUMENT *argm)
{
	int chno = atoi(argm->value);

	//fprintf(stderr, "get image %s \n", argm->value);

	if (chno < 0 || chno >= MAX_DISPLAY_NUM ) {
		req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
		return;
	}	
       sdk_ipc_devinfo_t sdk_ipc_devpara;
       Remote_GetIPCDevInfo( chno , &sdk_ipc_devpara);
	req->buffer_end += sprintf(req_bufptr(req), "Manufacturer=%s<br>Model=%s<br>FirmwareVersion=%s<br>SerialNumber=%s<br>HardwareId=%s\n", 
			sdk_ipc_devpara.Manufacturer, sdk_ipc_devpara.Model, sdk_ipc_devpara.FirmwareVersion, sdk_ipc_devpara.SerialNumber,sdk_ipc_devpara.HardwareId );

}



 static  sdk_modify_ip g_sdk_modify_ip;
void Set_old_ip_addr( request *req, COMMAND_ARGUMENT *argm)
{
        strncpy(g_sdk_modify_ip.old_ip_addr, argm->value,MAX_IP_ADDR_LEN);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);

}

void Set_new_ip_addr( request *req, COMMAND_ARGUMENT *argm)
{
        strncpy(g_sdk_modify_ip.new_ip_addr, argm->value,MAX_IP_ADDR_LEN);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);

}
void Set_mask( request *req, COMMAND_ARGUMENT *argm)
{
        strncpy(g_sdk_modify_ip.mask, argm->value,MAX_IP_ADDR_LEN);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void Set_mac( request *req, COMMAND_ARGUMENT *argm)
{
	strncpy(g_sdk_modify_ip.macaddr, argm->value,MAX_IP_ADDR_LEN);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}


void Set_port( request *req, COMMAND_ARGUMENT *argm)
{
      g_sdk_modify_ip.port= atoi(argm->value),
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);

}


void Set_user_name( request *req, COMMAND_ARGUMENT *argm)
{
        strncpy(g_sdk_modify_ip.user_name, argm->value,MAX_IP_ADDR_LEN);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);

}

void Set_user_pwd( request *req, COMMAND_ARGUMENT *argm)
{
        strncpy(g_sdk_modify_ip.user_pwd, argm->value,MAX_IP_ADDR_LEN);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);

}

void Set_IPC_network_CFG( request *req, COMMAND_ARGUMENT *argm)
{
	int devTpye = atoi(argm->value);
	if(MANUFACT_ONVIF==devTpye)
	{
		Remote_SetIPCNetworkPara(1, &g_sdk_modify_ip);
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
	}
	else if(MANUFACT_PRIVATE==devTpye)
	{
		Remote_SetIPCNetworkParaByPrivate(1, &g_sdk_modify_ip);
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
	}
	return NULL;
}

void set_image(request *req, COMMAND_ARGUMENT *argm)
{
	int chno = atoi(argm->value);

	//fprintf(stderr, "set image %s \n", argm->value);

	if (chno < 0 || chno >= MAX_DISPLAY_NUM ) {
		req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
		return;
	}	

	g_currentImageNo = chno;

}

void set_imageBright(request *req, COMMAND_ARGUMENT *argm)
{	
	int bright = 0;
	bright = atoi(argm->value);

	bright = bright < 0 ? 0 : bright ;
	bright = bright > 255 ? 255 : bright;
	g_img[g_currentImageNo].brightness = bright;	

}

void set_imageContrast(request *req, COMMAND_ARGUMENT *argm)
{
	int contrast = 0;	
	contrast = atoi(argm->value);	

	contrast = contrast < 0 ? 0 : contrast ;
	contrast = contrast > 255 ? 255 : contrast;
	g_img[g_currentImageNo].contrast = contrast;

}

void set_imageSaturation(request *req, COMMAND_ARGUMENT *argm)
{

	int saturation = 0;
	saturation = atoi(argm->value);	

	saturation = saturation < 0 ? 0 : saturation ;
	saturation = saturation > 255 ? 255 : saturation;

	g_img[g_currentImageNo].saturation = saturation;

}

void set_imageSharpness(request *req, COMMAND_ARGUMENT *argm)
{

	int sharpness = 0;
	sharpness = atoi(argm->value);
	sharpness = sharpness < 0 ? 0 : sharpness ;
	sharpness = sharpness > 255 ? 255 : sharpness;		

	g_img[g_currentImageNo].sharpness = sharpness;	
}

void submit_image(request *req, COMMAND_ARGUMENT *argm)
{
	Remote_SetImageParam(g_currentImageNo, &g_img[g_currentImageNo]);	

	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK"\n");
}

void setup_ptz_channel(request *req, COMMAND_ARGUMENT *argm)
{
	g_ptzChannel=atoi(argm->value);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s = %d\n", argm->name,g_ptzChannel);
}

void setup_ptz_speed(request *req, COMMAND_ARGUMENT *argm)
{
	ptz.speed=atoi(argm->value);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s = %d\n", argm->name,ptz.speed);
}

void setup_ptz_preset(request *req, COMMAND_ARGUMENT *argm)
{ 
	ptz.val=atoi(argm->value);
	ptz.val =1;//此功能还没有实现，先暂时固定为1，2014.8.20 by hejunying
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s = %d\n", argm->name,ptz.val);
}	

void setup_ptz_opera(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int opera = 0;
	unsigned int extend = 0; 

	opera=atoi(argm->value);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s = %d\n", argm->name,opera);
	if (g_ptzChannel > 0 || g_ptzChannel < MAX_DISPLAY_NUM ) 
	{
		extend = (ptz.speed << 16) & 0xFFFF0000;
		Remote_RunPtzCmd(g_ptzChannel, opera, &ptz, extend);
	}	
}
void SetPtzCruCh(request *req, COMMAND_ARGUMENT *argm)
{
	//
	g_PtzCruCh=atoi(argm->value);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s = %d\n", argm->name,g_PtzCruCh);
	//fprintf(stderr,"setptzcruch:%d       %s   %s    %d\n",g_PtzCruCh,__FILE__, __FUNCTION__,__LINE__);
}
void SetPtzCruLineNum(request *req, COMMAND_ARGUMENT *argm)
{
	g_PtzCruLineNum =atoi(argm->value);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s = %d\n", argm->name,g_PtzCruLineNum);
	g_ptzCruise[g_PtzCruCh][g_PtzCruLineNum].line = g_PtzCruLineNum;
	//fprintf(stderr,"setptzcrulinenum:%d     %s   %s   %d\n",g_PtzCruLineNum,__FILE__, __FUNCTION__,__LINE__);
}
void SetPtzCruLineEn(request *req, COMMAND_ARGUMENT *argm)
{
	 int PtzCruLineEn=atoi(argm->value);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s = %d\n", argm->name,PtzCruLineEn);
	//fprintf(stderr,"setptzcrulineen:%d     %s   %s   %d\n",PtzCruLineEn,__FILE__, __FUNCTION__,__LINE__);
	g_ptzCruise[g_PtzCruCh][g_PtzCruLineNum].enable = PtzCruLineEn;
}
#define MAX_CRU_SET_PRE_NUM  8   // 设置循环线时，下发字段中最多包含了MAX_CRU_PRE_NUM个预置点的数据
int SetPtzCruStrTok(char *StrSource, int StartPreNum, int StopPreNum)
{
	char *p1 = NULL;
	char *p2 = NULL;
	int i = 0,j = 0;         
	char strArr[MAX_CRU_SET_PRE_NUM][128] = {{0}}; 
	if(StrSource == NULL)
	{
		return -1;
	}
	p1 = strtok(StrSource,";");
	while(p1 != NULL)
	{   
		strcpy(strArr[i],p1);
		p1 = strtok(NULL,";");
		 //fprintf(stderr,"len = %d,  %s \n",sizeof(strArr[i]),strArr[i]);
		i++;
	}
	for(i = 0; i < StopPreNum - StartPreNum +1; i++)
	{

	      g_ptzCruise[g_PtzCruCh][g_PtzCruLineNum].cruise_preset[StartPreNum + i].num = StartPreNum + i +1;
		//fprintf(stderr,"i= %d   %s\n",i,strArr[i]);
		p1 = strtok(strArr[i],",");
		j = 0;
		while(p1 != NULL)
		{
			p2 = strstr(p1,":");
			p2++;
			switch(j)
			{
				case 0:
					g_ptzCruise[g_PtzCruCh][g_PtzCruLineNum].cruise_preset[StartPreNum + i].enable = atoi(p2);
					//fprintf(stderr,"cru_en:%d\n",g_ptzCruise[g_PtzCruCh][g_PtzCruLineNum].cruise_preset[StartPreNum + i].enable );
				break;
				case 1:
					g_ptzCruise[g_PtzCruCh][g_PtzCruLineNum].cruise_preset[StartPreNum + i].speed= atoi(p2);
					//fprintf(stderr,"cru_sp:%d\n",g_ptzCruise[g_PtzCruCh][g_PtzCruLineNum].cruise_preset[StartPreNum + i].speed);
				break;
				case 2:
					g_ptzCruise[g_PtzCruCh][g_PtzCruLineNum].cruise_preset[StartPreNum + i].dwell_time= atoi(p2);
					//fprintf(stderr,"cru_dt:%d\n",g_ptzCruise[g_PtzCruCh][g_PtzCruLineNum].cruise_preset[StartPreNum + i].dwell_time );
				break;
				case 3:
					g_ptzCruise[g_PtzCruCh][g_PtzCruLineNum].cruise_preset[StartPreNum + i].preset.num= atoi(p2);
					//fprintf(stderr,"pre_en:%d\n",g_ptzCruise[g_PtzCruCh][g_PtzCruLineNum].cruise_preset[StartPreNum + i].preset.num);
				break;
				case 4:
					g_ptzCruise[g_PtzCruCh][g_PtzCruLineNum].cruise_preset[StartPreNum + i].preset.enable= atoi(p2);
					//fprintf(stderr,"pre_num:%d\n",g_ptzCruise[g_PtzCruCh][g_PtzCruLineNum].cruise_preset[StartPreNum + i].preset.enable);
				break;
				default:
				break;
			}
			p1 = strtok(NULL,",");
			j++;
		}
		
	}
	return 0;

}
// 由于巡航预置点的字符串长度已经超过1024个，所以只能分段下发这些数据
// 预置点0~7
void SetPtzCruPreset0_7(request *req, COMMAND_ARGUMENT *argm)
{
	char *StrSource = argm->value;	
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,argm->value);
	SetPtzCruStrTok(StrSource,0,7);
}
// 预置点8~15
void SetPtzCruPreset8_15(request *req, COMMAND_ARGUMENT *argm)
{
	char *StrSource = argm->value;	
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,argm->value);
	SetPtzCruStrTok(StrSource,8,15);
}
// 预置点16~23
void SetPtzCruPreset16_23(request *req, COMMAND_ARGUMENT *argm)
{
	char *StrSource = argm->value;	
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,argm->value);
	SetPtzCruStrTok(StrSource,16,23);
}
// 预置点24~31
void SetPtzCruPreset24_31(request *req, COMMAND_ARGUMENT *argm)
{
	char *StrSource = argm->value;
	int ret = 0;
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,argm->value);
	SetPtzCruStrTok(StrSource,24,31);
	ret = SetPtzCruiseAllPreset(g_PtzCruCh, &g_ptzCruise[g_PtzCruCh][g_PtzCruLineNum] );
	if(ret < 0)
	{
		fprintf(stderr,"Set PtzCruPreset Error!!\n");
	}
	memset(g_ptzCruise,0,sizeof(g_ptzCruise));
}

void GetPtzCruCh(request *req, COMMAND_ARGUMENT *argm)
{
	int ret = 0;
	memset(g_ptzCruise,0,sizeof(g_ptzCruise));
	g_PtzCruCh=atoi(argm->value);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,g_PtzCruCh);
	//fprintf(stderr,"setptzcruch:%d       %s   %s    %d\n",g_PtzCruCh,__FILE__, __FUNCTION__,__LINE__);
}
void GetPtzCruLineNum(request *req, COMMAND_ARGUMENT *argm)
{
	g_PtzCruLineNum =atoi(argm->value);
	g_ptzCruise[g_PtzCruCh][g_PtzCruLineNum].line = g_PtzCruLineNum;
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,g_PtzCruLineNum);
	GetPtzCruiseAllPreset( g_PtzCruCh , &g_ptzCruise[g_PtzCruCh][g_PtzCruLineNum]);
	//fprintf(stderr,"setptzcrulinenum:%d     %s   %s   %d\n",g_PtzCruLineNum,__FILE__, __FUNCTION__,__LINE__);
}
void GetPtzCruLineEn(request *req, COMMAND_ARGUMENT *argm)
{
	int PtzCruLineEn = g_ptzCruise[g_PtzCruCh][g_PtzCruLineNum].enable ;
	 req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,PtzCruLineEn);
	//fprintf(stderr,"setptzcrulineen:%d     %s   %s   %d\n",PtzCruLineEn,__FILE__, __FUNCTION__,__LINE__);
}
int GetPtzCruStrCat(char *StrSource, int StartPreNum, int StopPreNum)
{
	int i = 0;
	char IntString[32] = {0};
	char PtzCruStr[5012] = {0};
	for(i = 0; i < StopPreNum - StartPreNum + 1; i++)
	{
		memset(IntString,0,sizeof(IntString));
		sprintf(IntString,"get_cru_en%d:%d",StartPreNum + i,g_ptzCruise[g_PtzCruCh][g_PtzCruLineNum].cruise_preset[StartPreNum + i].enable );
		strcat(PtzCruStr,IntString);
		strcat(PtzCruStr,",");
		memset(IntString,0,sizeof(IntString));
		sprintf(IntString,"get_cru_sp%d:%d",StartPreNum + i,g_ptzCruise[g_PtzCruCh][g_PtzCruLineNum].cruise_preset[StartPreNum + i].speed);
		strcat(PtzCruStr,IntString);
		strcat(PtzCruStr,",");
		memset(IntString,0,sizeof(IntString));
		sprintf(IntString,"get_cru_dt%d:%d",StartPreNum + i,g_ptzCruise[g_PtzCruCh][g_PtzCruLineNum].cruise_preset[StartPreNum + i].dwell_time);
		strcat(PtzCruStr,IntString);
		strcat(PtzCruStr,",");
		memset(IntString,0,sizeof(IntString));
		sprintf(IntString,"get_cru_pre%d_num:%d",StartPreNum + i,g_ptzCruise[g_PtzCruCh][g_PtzCruLineNum].cruise_preset[StartPreNum + i].preset.num);
		strcat(PtzCruStr,IntString);
		strcat(PtzCruStr,",");
		memset(IntString,0,sizeof(IntString));
		sprintf(IntString,"get_cru_pre%d_en:%d",StartPreNum + i,g_ptzCruise[g_PtzCruCh][g_PtzCruLineNum].cruise_preset[StartPreNum + i].preset.enable);
		strcat(PtzCruStr,IntString);
		strcat(PtzCruStr,";");
	}
	strcpy(StrSource,PtzCruStr);
	//fprintf(stderr,"GetPtzCruStrCat:%s\n",StrSource);
	return 0;
}
void GetPtzCruPreset0_15(request *req, COMMAND_ARGUMENT *argm)
{
	char PtzCruStr[5012] = {0};
//	fprintf(stderr,"GetPtzCruStrCat\n\n\n");
	GetPtzCruStrCat(PtzCruStr, 0, 15);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,PtzCruStr);

}
void GetPtzCruPreset16_31(request *req, COMMAND_ARGUMENT *argm)
{
	char PtzCruStr[5012] = {0};
	//fprintf(stderr,"GetPtzCruStrCat\n\n\n");
	GetPtzCruStrCat(PtzCruStr, 16, 31);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,PtzCruStr);

}
void SetPtzAllPreCh(request *req, COMMAND_ARGUMENT *argm)
{
	g_PtzAllPreCh=atoi(argm->value);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,g_PtzAllPreCh);
	//fprintf(stderr,"setptzcruch:%d       %s   %s    %d\n",g_PtzCruCh,__FILE__, __FUNCTION__,__LINE__);
}
void SetPtzAllPreNum(request *req, COMMAND_ARGUMENT *argm)
{
	g_PtzAllPreNum = atoi(argm->value);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,g_PtzAllPreNum);
	g_ptzAllPreset[g_PtzAllPreCh].preset[g_PtzAllPreNum].num = g_PtzAllPreNum;
	//fprintf(stderr,"setptzcruch:%d       %s   %s    %d\n",g_PtzCruCh,__FILE__, __FUNCTION__,__LINE__);
}
void SetPtzAllPreEn(request *req, COMMAND_ARGUMENT *argm)
{
	int PtzPreEn = atoi(argm->value);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,PtzPreEn);
	g_ptzAllPreset[g_PtzAllPreCh].preset[g_PtzAllPreNum].enable= PtzPreEn;
}
void SetPtzAllPreOpera(request *req, COMMAND_ARGUMENT *argm)
{
	int opera = atoi(argm->value);
	//fprintf(stderr, "opera:%d\n",opera);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,opera);
	if(opera == PTZ_ALL_PRE_SET)
	{
		SetPreset( g_PtzAllPreCh , &g_ptzAllPreset[g_PtzAllPreCh].preset[g_PtzAllPreNum]);
	}
	else if(opera == PTZ_ALL_PRE_RECALL)
	{
		RecallPreset(g_PtzAllPreCh, &g_ptzAllPreset[g_PtzAllPreCh].preset[g_PtzAllPreNum]);
	}
}
void GetPtzAllPreCh(request *req, COMMAND_ARGUMENT *argm)
{
	int ret = 0;
	g_PtzAllPreCh=atoi(argm->value);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,g_PtzAllPreCh);
	ret = databaseOperate(SDK_MAIN_MSG_ALLPRESETS, SDK_PARAM_GET, &g_PtzAllPreCh, sizeof(g_PtzAllPreCh), &g_ptzAllPreset[g_PtzAllPreCh], sizeof(sdk_preset_all_t) );
	if(ret < 0)
	{
		fprintf(stderr,"Set PtzAllPreset Error!!\n");
	}
}

int GetPtzAllPreNumStrCat(char *StrSource, int StartPreNum, int StopPreNum)
{
	int i = 0;
	char Str[32] = {0};
	char AllPreStr[5012] = {0};
	for(i = 0; i < StopPreNum - StartPreNum + 1; i++)
	{
		memset(Str,0,sizeof(Str));
		sprintf(Str,"getpreset%d_num:%d", StartPreNum + i,g_ptzAllPreset[g_PtzAllPreCh].preset[StartPreNum + i].num);
		strcat(AllPreStr,Str);
		if(i != MAX_PRESET_NUM -1)
		{
			strcat(AllPreStr,";");
		}
	}
	strcpy(StrSource, AllPreStr);
	//Printf("Len:%d    StrSource:%s\n\n",strlen(AllPreStr),AllPreStr);
	return 0;
}
void GetPtzAllPreNum(request *req, COMMAND_ARGUMENT *argm)
{
	char PtzAllPreNumStr0_63[5012] = {0};
	GetPtzAllPreNumStrCat(PtzAllPreNumStr0_63, 0, 127);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,PtzAllPreNumStr0_63);
}
int GetPtzAllPreEnStrCat(char *StrSource, int StartPreNum, int StopPreNum)
{
	int i = 0;
	char Str[32] = {0};
	char AllPreEnStr[5012] = {0};
	for(i = 0; i < StopPreNum - StartPreNum + 1; i++)
	{
		memset(Str,0,sizeof(Str));
		sprintf(Str,"getpreset%d_en:%d",StartPreNum + i,g_ptzAllPreset[g_PtzAllPreCh].preset[StartPreNum + i].enable);
		strcat(AllPreEnStr,Str);
		if(i != MAX_PRESET_NUM -1)
		{
			strcat(AllPreEnStr,";");
		}
	}
	strcpy(StrSource, AllPreEnStr);
	//fprintf(stderr,"len:%d    StrSource:%s\n",strlen(AllPreEnStr),AllPreEnStr);
	return 0;
}
void GetPtzAllPreEn(request *req, COMMAND_ARGUMENT *argm)
{
	char PtzAllPreEnStr0_63[5012] = {0};
	GetPtzAllPreEnStrCat(PtzAllPreEnStr0_63, 0, 127);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,PtzAllPreEnStr0_63);	
}

void start_ptz_cruise(request *req, COMMAND_ARGUMENT *argm) 
{

	int chno = 0; 	
	// 插入日志---开启PTZ巡航
	LogInsert(LOG_STATUS_WARNING, LOG_STATUS_PTZ_CHN_ON,g_usr);
	chno = atoi(argm->value);

	if (chno < 0 || chno >= MAX_DISPLAY_NUM ) {
		return;
	}

	ptzStartCruise(chno);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

void stop_ptz_cruise(request *req, COMMAND_ARGUMENT *argm) 
{

	int chno = 0; 	
	// 插入日志---关闭PTZ巡航
	LogInsert(LOG_STATUS_WARNING, LOG_STATUS_PTZ_CHN_OFF,g_usr);
	chno = atoi(argm->value);

	if (chno < 0 || chno >= MAX_DISPLAY_NUM ) {
		return;
	}

	ptzStopCruise(chno);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}



void set_ipcencode(request *req, COMMAND_ARGUMENT *argm) 
{

	int chno = atoi(argm->value);
	if (chno < 0 || chno >= MAX_DISPLAY_NUM ) {
		req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
		return;
	}
	else if (chno == 0) 
	{		
		AllEbcideChn = 1;
		g_currentEncodeNo = 0;
	}
	else
	{
		g_currentEncodeNo =chno - 1; 
	}
	memset(&g_encode[g_currentEncodeNo], 0, sizeof(sdk_encode_t));
	//fprintf(stderr, " get_encoding g_currentEncodeNo %d \n", g_currentEncodeNo);
	if (Remote_GetEncodeCfg(g_currentEncodeNo, &g_encode[g_currentEncodeNo] ) ) {
		fprintf(stderr, "Get channel:%d enconde configure success!!\n\n",g_currentEncodeNo);
	}	
	else
	{
		fprintf(stderr, "Get channel:%d enconde configure failed!!\n\n",g_currentEncodeNo);
	}

}

void set_encoding0(request *req, COMMAND_ARGUMENT *argm)
{
	g_encode[g_currentEncodeNo].main.video_enc = atoi(argm->value);
}

void set_resolution0(request *req, COMMAND_ARGUMENT *argm)
{
	int res = atoi(argm->value);
	if ( 0 == res ) {
		res = SDK_VIDEO_RESOLUTION_D1;
	} else if ( 1 == res ) {
		res = SDK_VIDEO_RESOLUTION_720p;
	} else if ( 2 == res ) {
		res = SDK_VIDEO_RESOLUTION_1080p;
	} else {
		res = SDK_VIDEO_RESOLUTION_720p;
	}

	g_encode[g_currentEncodeNo].main.resolution = res;
}

void set_framerate0(request *req, COMMAND_ARGUMENT *argm)
{
	g_encode[g_currentEncodeNo].main.frame_rate = atoi(argm->value);
} 

void set_streamtype0(request *req, COMMAND_ARGUMENT *argm)
{

}

void set_audioformat0(request *req, COMMAND_ARGUMENT *argm)
{
	g_encode[g_currentEncodeNo].main.audio_enc = atoi(argm->value);
} 

void set_bitrate0(request *req, COMMAND_ARGUMENT *argm) 
{
	g_encode[g_currentEncodeNo].main.bitrate = atoi(argm->value);

	//	int b = atoi(argm->value);
	//fprintf(stderr, "bittttttttttttt %s %d\n", argm->value, b);

}

void set_profilelevel0(request *req, COMMAND_ARGUMENT *argm)
{
	g_encode[g_currentEncodeNo].main.level = atoi(argm->value);
}

void set_encoding1(request *req, COMMAND_ARGUMENT *argm)
{
	g_encode[g_currentEncodeNo].second.video_enc = atoi(argm->value);
} 

void set_resolution1(request *req, COMMAND_ARGUMENT *argm) 
{
	int res = atoi(argm->value);
	if ( 0 == res ) {
		res = SDK_VIDEO_RESOLUTION_QCIF;
	} else if ( 1 == res ) {
		res = SDK_VIDEO_RESOLUTION_CIF;
	} else if ( 2 == res ) {
		res = SDK_VIDEO_RESOLUTION_D1;
	} else {
		res = SDK_VIDEO_RESOLUTION_D1;
	}

	g_encode[g_currentEncodeNo].second.resolution = res;
}

void set_framerate1(request *req, COMMAND_ARGUMENT *argm) 
{
	g_encode[g_currentEncodeNo].second.frame_rate = atoi(argm->value);
}

void set_streamtype1(request *req, COMMAND_ARGUMENT *argm) 
{

}

void set_audioformat1(request *req, COMMAND_ARGUMENT *argm) 
{
	g_encode[g_currentEncodeNo].second.audio_enc = atoi(argm->value);
}

void set_bitrate1(request *req, COMMAND_ARGUMENT *argm) 
{
	g_encode[g_currentEncodeNo].second.bitrate = atoi(argm->value);

}

void set_profilelevel1(request *req, COMMAND_ARGUMENT *argm) 
{
	g_encode[g_currentEncodeNo].second.level = atoi(argm->value);
}

void submitcodecfg(request *req, COMMAND_ARGUMENT *argm ) 
{
	int j = 0;
	int ret = 0;
	if(AllEbcideChn == 1)
	{
		AllEbcideChn = 0;

		for(j = 0; (j < MAX_DISPLAY_NUM) && (j < configGetDisplayNum()); j++)
		{	
			req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name, j);
			ret = Remote_SetEncodeCfg(j, &g_encode[j] );
			if(ret<0)
			{
				fprintf(stderr, "Remote_SetEncodeCfg  fail\n");
			}
		}
	}
	else
	{
		ret = Remote_SetEncodeCfg(g_currentEncodeNo, &g_encode[g_currentEncodeNo] );
		if(ret<0)
		{
			fprintf(stderr, "Remote_SetEncodeCfg  fail\n");
		}
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s = %d\n", argm->name,g_currentEncodeNo);
	}
	//req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);

}


static void onvifDeviceInfo(char *pDeviceIp, unsigned short devicePort, char *pDeviceUuid)
{
	if ( g_camlistWPoint )	
		g_camlistWPoint += sprintf(g_camlistWPoint, "ip=%s, port=%d, type=%d, manu=0, name=admin, pwd=admin, mac=, devType=ONVIF<br>", pDeviceIp, devicePort, MANUFACT_ONVIF);

}

void search_camera(request *req, COMMAND_ARGUMENT *argm)
{
	int i=0;
	int devType=atoi(argm->value);
	char *camlistbuf = malloc(218 * 256 * sizeof(char));
	g_camlistWPoint = camlistbuf;

	int	privatedevNum; 
	sdk_device_t *privatePtr;

	switch(devType)
	{
		case MANUFACT_ONVIF:
			StartDiscovery(onvifDeviceInfo,false);
			sleep(2);
			req->buffer_end += sprintf(req_bufptr(req), "%s\n", camlistbuf);
			free(camlistbuf);
			break;
			
		case MANUFACT_PRIVATE:
			SearchPrivateDevInfo(&privatedevNum, &privatePtr);
			for(i=0; i<privatedevNum; i++)
			{
				////要添加MAC地址
				g_camlistWPoint += sprintf(g_camlistWPoint, "ip=%s, port=%d, type=%d, manu=0, name=admin, pwd=admin, mac=%s, devType=Private<br>",\
				(privatePtr+i)->ip_addr, (privatePtr+i)->port, (privatePtr+i)->manufacturer, (privatePtr+i)->macaddr);
			}

			req->buffer_end += sprintf(req_bufptr(req), "%s\n", camlistbuf);
			free(camlistbuf);
			break;
			
		case MANUFACT_ONVIF_PRIVATE:
			SearchPrivateDevInfo(&privatedevNum, &privatePtr);
			for(i=0; i<privatedevNum; i++)
			{
				////要添加MAC地址
				g_camlistWPoint += sprintf(g_camlistWPoint, "ip=%s, port=%d, type=%d, manu=0, name=admin, pwd=admin, mac=%s, devType=Private<br>",\
					(privatePtr+i)->ip_addr, (privatePtr+i)->port, (privatePtr+i)->manufacturer, (privatePtr+i)->macaddr);
			}

			StartDiscovery(onvifDeviceInfo,false);
			sleep(2);
			req->buffer_end += sprintf(req_bufptr(req), "%s\n", camlistbuf);
			free(camlistbuf);
			break;

		default:
			break;

	}

	
}

#if 0
void get_disk(request *req, COMMAND_ARGUMENT *argm)
{
	sdk_disk_t  *pDisk = NULL;
	char temp[4096];
	int ret = databaseOperate(SDK_MAIN_MSG_DISK, SDK_DISK_QUERY, NULL, 0, temp, 4096);

	int diskNum = ret / sizeof(sdk_disk_t);
	pDisk = (sdk_disk_t * )temp;

	int i = 0;

	for ( i = 0; i < diskNum; i++) {
		req->buffer_end += sprintf(req_bufptr(req), "diskNo%d=%d,status=%d,capability=%d,freesapce=%d,type=%d<br>", 
				i, pDisk->disk_no, pDisk->status, pDisk->total_size, pDisk->free_size, pDisk->disk_type);
	}

}
#else
void get_disk(request *req, COMMAND_ARGUMENT *argm)
{
	sdk_disk_t  *pDisk = NULL;
	char temp[4096];
	int ret = DiskQuery(temp);
	int diskNum = ret / sizeof(sdk_disk_t);
	pDisk = (sdk_disk_t * )temp;

	int i = 0;

	for ( i = 0; i < diskNum; i++)
	{
		req->buffer_end += sprintf(req_bufptr(req), "diskNo%d=%d,status=%d,capability=%d,freesapce=%d,type=%d<br>", i, pDisk->disk_no, pDisk->status, pDisk->total_size, pDisk->free_size, pDisk->disk_type);
		pDisk++;
	}

}
#endif

void format_disk(request *req, COMMAND_ARGUMENT *argm)
{

	int diskNo = atoi(argm->value);
	stream_alarm_t alarmFormatInfo;

	memset(&alarmFormatInfo, 0, sizeof(stream_alarm_t));
	alarmFormatInfo.type = SDK_DISKFORMAT;
	alarmFormatInfo.status = 1;
	streamInsertAlarmToAlarmQueue(NULL, &alarmFormatInfo);
	//fprintf(stderr, "========= diskNo %d \n", diskNo);
	// 插入日志---格式化硬盘
	LogInsert(LOG_FILE_OPERATE, LOG_FORMAT_DISK,g_usr);
	cmdOperate(SDK_MAIN_MSG_DISK, SDK_DISK_FORMAT, &diskNo, sizeof(diskNo), NULL, 0);
	req->buffer_end += sprintf(req_bufptr(req), "%s\n", "OK");
}

void format_process(request *req, COMMAND_ARGUMENT *argm)
{

	int process = 0;
	cmdOperate(SDK_MAIN_MSG_DISK, SDK_DISK_PROGRESS, &process, sizeof(process), NULL, 0);

	req->buffer_end += sprintf(req_bufptr(req), "process=%d\n", process);
}





int processUpgrade(int sock)
{

	char buffer[4096];
	int status = 0;
	int  packetLen = 0;
	int bin_size = 0;
	int recvLen = 0;
	int nNetTimeout = 20000;//20SECS
	MYUpdate_Info update_info_head;
	MYUpdate_FileInfo *fileInfo = NULL;
	int file_head = 0;
	int file_num = 0;
	int file_size = 0;
	int total_len = 0;
	//FILE *fp = NULL;
	int fp = -1;
	char file_path[128];

	stream_alarm_t alarmUpgradeInfo;
	memset(&alarmUpgradeInfo, 0, sizeof(stream_alarm_t));
	alarmUpgradeInfo.type = SDK_UPDATE;
	alarmUpgradeInfo.status = 1;
	streamInsertAlarmToAlarmQueue(NULL, &alarmUpgradeInfo);
	recordStopAllDevChannel();
	decodeStopAllDecode();
	
	memset(&update_info_head,0,sizeof(update_info_head));
	set_block_fd(sock);
	setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&nNetTimeout, sizeof(int));
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));


	send(sock, "CMD:UPGRADE MCTP/1.0 SC\n@OK@\nEND\n", 33, 0); //ACK
	//fprintf(stderr, "RECV NOW~~~\n");
	mkdir("/tmp/update", S_IRWXU);

	/*
	   char *filename = "/tmp/update/update_raysharp.tgz";
	   FILE *fp = NULL;
	   if ((fp = fopen(filename, "r+")) == NULL) {			
	   if ((fp = fopen(filename, "w")) == NULL) {				
	   return 0;
	   }					
	   }	

	   fseek(fp, 0L, SEEK_SET);
	 */

	//fprintf(stderr,"[%s:%s:%d]:debug \n",__FILE__,__FUNCTION__,__LINE__);
	if ( recv(sock, buffer, sizeof(int), 0) != sizeof(int ) ) {
		fprintf(stderr, "recv head error!\n");
		send(sock, "CMD:UPGRADE MCTP/1.0 SC\n@11@\nEND\n", 32, 0);
		//		fclose(fp);
		return -1;
	}

	bin_size = packetLen = *(int*)buffer;	
	//fprintf(stderr,"[%s:%s:%d]:packetLen=%d \n",__FILE__,__FUNCTION__,__LINE__,packetLen);

	while (packetLen > 0 && file_num < FILE_NUM) {
		recvLen = recv(sock, buffer, 1024, 0);
		total_len += recvLen;
		if ( recvLen <= 0 ) {
			//fprintf(stderr,"[%s:%s:%d]:recv update file error \n",__FILE__,__FUNCTION__,__LINE__);
			send(sock, "CMD:UPGRADE MCTP/1.0 SC\n@12@\nEND\n", 32, 0);
			//fclose(fp);
			return -1;	
		}	
		if(recvLen > sizeof(update_info_head) && file_head == 0)
		{
			//int i = 0;
			file_head = 1;
			memcpy((char*)&update_info_head,buffer,sizeof(update_info_head));
			fileInfo = &update_info_head.files[file_num];
			/*
			   for(i=0;i<FILE_NUM;i++)
			   {
			   fprintf(stderr,"[%s:%s:%d]:filename =%s,filesize=%d \n",__FILE__,__FUNCTION__,__LINE__,update_info_head.files[i].filename,
			   update_info_head.files[i].size);	
			   }
			 */

			memset(file_path,0,128);
			sprintf(file_path,"%s%s","/tmp/update/",fileInfo->filename);
			//fprintf(stderr,"[%s:%s:%d]:start file=%s,size=%d \n",__FILE__,__FUNCTION__,__LINE__,file_path,update_info_head.files[file_num].size);
			fp = open(file_path,O_CREAT|O_RDWR|O_APPEND,0666);
			if(fp == -1)
			{
				fprintf(stderr,"[%s:%s:%d]:create file %s failed \n",__FILE__,__FUNCTION__,__LINE__,file_path);
				return 0;
			}
			file_size = recvLen-sizeof(update_info_head);
			//fwrite(buffer+sizeof(update_info_head), file_size, 1, fp);
			write(fp,buffer+sizeof(update_info_head),file_size);
		}
		else if(fileInfo != NULL)
		{
			if((file_size+recvLen) >= fileInfo->size)
			{
				int left_len = fileInfo->size-file_size;
				//fprintf(stderr,"[%s:%s:%d]:total_len=%d ,left_len=%d \n",__FILE__,__FUNCTION__,__LINE__,total_len,left_len);
				//fwrite(buffer, left_len, 1, fp);
				write(fp,buffer,left_len);
				//fclose(fp);
				close(fp);
				fp = -1;
				//fprintf(stderr,"[%s:%s:%d]:recvd file =%s ,fsz=%d,recvd_fsz=%d ,recvLen=%d\n",__FILE__,__FUNCTION__,__LINE__,
				//		fileInfo->filename,fileInfo->size,file_size,recvLen);
				file_num++;
				//fprintf(stderr,"[%s:%s:%d]:file_num=%d \n",__FILE__,__FUNCTION__,__LINE__,file_num);
				if(file_num >= FILE_NUM)
				{
					fprintf(stderr,"[%s:%s:%d]:file_num=%d \n",__FILE__,__FUNCTION__,__LINE__,file_num);
					break;
				}
				fileInfo = &update_info_head.files[file_num];
				memset(file_path,0,128);
				sprintf(file_path,"%s%s","/tmp/update/",fileInfo->filename);
				fp = open(file_path,O_CREAT|O_RDWR|O_APPEND,0666);
				//fprintf(stderr,"[%s:%s:%d]:start file=%s,size=%d \n",__FILE__,__FUNCTION__,__LINE__,file_path,update_info_head.files[file_num].size);
				if(fp == -1)
				{
					fprintf(stderr,"[%s:%s:%d]:create file %s failed \n",__FILE__,__FUNCTION__,__LINE__,file_path);
					return 0;
				}
				//fwrite(buffer+left_len, recvLen-left_len, 1, fp);
				if((recvLen-left_len) > fileInfo->size)
				{
					//fprintf(stderr,"[%s:%s:%d]:write %s,%d \n",__FILE__,__FUNCTION__,__LINE__,file_path,fileInfo->size);
					write(fp,buffer+left_len,fileInfo->size);
					close(fp);
					fp = -1;
					file_num++;
					file_size = recvLen-left_len-fileInfo->size;
					if(file_num >= FILE_NUM)
					{
						fprintf(stderr,"[%s:%s:%d]:file_num=%d \n",__FILE__,__FUNCTION__,__LINE__,file_num);
						break;
					}
					
				}
				else
				{
					write(fp,buffer+left_len,recvLen-left_len);
					file_size = recvLen-left_len;
					if(file_size == 0 && (packetLen - recvLen) == 0)
					{
						close(fp);
						file_num++;
						break;
					}
				}
			}
			else
			{
				//fwrite(buffer, recvLen, 1, fp);
				write(fp,buffer,recvLen);
				file_size += recvLen;
			}
		}	
		packetLen -= recvLen;		
	}
	if(total_len < bin_size)
	{
		memset(file_path,0,128);
		#ifdef HI3520D
			sprintf(file_path,"%s%s","/tmp/update/","HI3520DUBOOT.bin");
		#elif HI3535					
			sprintf(file_path,"%s%s","/tmp/update/","HI3535UBOOT.bin");
		#elif HI3521
			sprintf(file_path,"%s%s","/tmp/update/","HI3521UBOOT.bin");
		#elif HI3531
			sprintf(file_path,"%s%s","/tmp/update/","HI3531UBOOT.bin");
		#endif

		fp = open(file_path,O_CREAT|O_RDWR|O_APPEND,0666);
		if(fp == -1)
		{
			fprintf(stderr,"[%s:%s:%d]:create file %s failed \n",__FILE__,__FUNCTION__,__LINE__,file_path);
			return 0;
		}
		write(fp,buffer+recvLen-file_size,file_size);
		while(total_len < bin_size)
		{
			recvLen = recv(sock, buffer, 1024, 0);
			total_len += recvLen;
			if ( recvLen <= 0 ) {
				//fprintf(stderr,"[%s:%s:%d]:recv update file error \n",__FILE__,__FUNCTION__,__LINE__);
				send(sock, "CMD:UPGRADE MCTP/1.0 SC\n@12@\nEND\n", 32, 0);
				//fclose(fp);
				return -1;	
			}
			write(fp,buffer,recvLen);
		}
		if(fp != -1)
		{
			close(fp);
			//file_num++;
			//fp = NULL;
		}
	}
	
	//exit(0);
	//fprintf(stderr,"[%s:%s:%d]:total_len=%d \n",__FILE__,__FUNCTION__,__LINE__,total_len);
	if(file_num != FILE_NUM)
	{
		fprintf(stderr,"[%s:%s:%d]:recvd %d file ,error \n",__FILE__,__FUNCTION__,__LINE__,file_num);
		send(sock, "CMD:UPGRADE MCTP/1.0 SC\n@12@\nEND\n", 32, 0);
		return -1;	
	}

	//fclose(fp);
	upgrad_pkg_file_t pkg;
	memset(&pkg, 0, sizeof(upgrad_pkg_file_t));
	strcpy((char *)pkg.name, "update_raysharp.tgz");
	//strcpy((char *)pkg.file_path, "/tmp/update");	
	strcpy((char *)pkg.file_path,"OCXUpdate");

	status = cmdOperate(SDK_MAIN_MSG_UPGRAD, SDK_UPGRAD_DISK_FILE, NULL, 0, &pkg, sizeof(upgrad_pkg_file_t));

	if(status == 0)
		send(sock, "CMD:UPGRADE MCTP/1.0 SC\n@0@\nEND\n", 32, 0);
	else
		send(sock, "CMD:UPGRADE MCTP/1.0 SC\n@3@\nEND\n", 32, 0);

	return 0;

}

void upgrade_process(request *req, COMMAND_ARGUMENT *argm)
{
	int process = 0;
	static char temp = 0;
	cmdOperate(SDK_MAIN_MSG_UPGRAD, SDK_UPGRAD_PROGRESS, &process, sizeof(process), NULL, 0);
	req->buffer_end += sprintf(req_bufptr(req), "process=%d\n", process);
	// 当系统升级时，会发送多条查询升级进度的命令，但要确保只产生一条升级日志。
	if(temp == 0)
	{
		temp = 1;
		// 插入日志--- 升级
		LogInsert(LOG_SYS_MANAGE, LOG_SYS_UPGRADE,g_usr);
	}
	if(process == 100)
	{
		temp = 0;
	}
}



// 用户登录
void SetLogin(request *req, COMMAND_ARGUMENT *argm)
{
	char *value = argm->value;
	strcpy(g_usr,value);
	// 插入日志---移动侦测配置
	LogInsert(LOG_USER_MANAGE,LOG_USER_LOGIN,g_usr);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n",argm->name,value);

}
// 用户登录
void SetLogOut(request *req, COMMAND_ARGUMENT *argm)
{
	char *value = argm->value;
	strcpy(g_usr,value);
	// 插入日志---移动侦测配置
	LogInsert(LOG_USER_MANAGE,LOG_USER_LOGOUT,g_usr);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n",argm->name,value);

}

//用户名
void SetUser(request *req, COMMAND_ARGUMENT *argm)
{
	char *value = argm->value;
	strcpy(g_usr,value);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n",argm->name,g_usr);
}

/***************************************************************************
  运动监测(MotionDetect) add by he 2014.5.29
 ****************************************************************************/
// SET
// 通道号
void SetMotionDetChan(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	int i;
	int ret = 0;
	if(value == 0)// 当通道号为ALL时，接收的数据是0，表示将所有的通道都设置成当前的数据
	{
		AllMotionDetChn = 1;
		g_setMotionDetChannel  = 0; // 将数据临时保存在通道0中。
		//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name,g_setMotionDetChannel,argm->value,__FILE__,__LINE__);
		for(i = 0;i<MAX_DISPLAY_NUM;i++)// 使用前先清零
		{
			memset(&g_MotionInfo[i], 0, sizeof(sdk_motion_cfg_v2_t));
		}
	}
	else
	{
		g_setMotionDetChannel= value -1;
		//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name,g_setMotionDetChannel,argm->value,__FILE__,__LINE__);
		memset(&g_MotionInfo[g_setMotionDetChannel], 0, sizeof(sdk_motion_cfg_v2_t));
	}
	ret=databaseOperate(SDK_MAIN_MSG_MOTION_CFG,SDK_PARAM_GET,&g_getMotionDetChannel,sizeof(int),&g_MotionInfo[g_getMotionDetChannel],sizeof(g_MotionInfo[g_getMotionDetChannel]));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_MOTION_CFG fail %s:%d\n",__FILE__,__LINE__);
	}
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);

}

// 运动监测使能
void SetMotionDetEn(request *req, COMMAND_ARGUMENT *argm)
{	
	unsigned int value=atoi(argm->value);
	if(value == 1)
	{
		// 插入日志---开启移动侦测
		LogInsert(LOG_STATUS_WARNING, LOG_STATUS_MOTION_DETECT_ON,g_usr);
	}
	else
	{
		// 插入日志---关闭移动侦测
		LogInsert(LOG_STATUS_WARNING,LOG_STATUS_MOTION_DETECT_OFF,g_usr);
	}
	g_MotionInfo[g_setMotionDetChannel].enable=value;
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name, g_MotionInfo[g_setMotionDetChannel].enable,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}

// 蜂鸣器使能
void SetMotionDetBeepEn(request *req, COMMAND_ARGUMENT *argm)
{	
	unsigned int value=atoi(argm->value);
	if(value == 1)
	{
		// 插入日志---联动蜂鸣器
		LogInsert(LOG_STATUS_WARNING, LOG_STATUS_LINK_BUZZER_ON,g_usr);
	}
	else
	{
		// 插入日志---关闭联动蜂鸣器
		LogInsert(LOG_STATUS_WARNING,LOG_STATUS_LINK_BUZZER_OFF,g_usr);
	}
	g_MotionInfo[g_setMotionDetChannel].beep_enable=value;
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name, g_MotionInfo[g_setMotionDetChannel].beep_enable,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);	
}
// 蜂鸣器开启时间
void SetMotionDetBeepTime(request *req, COMMAND_ARGUMENT *argm)
{	
	unsigned int value=atoi(argm->value);
	g_MotionInfo[g_setMotionDetChannel].beep_time=value;
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name, g_MotionInfo[g_setMotionDetChannel].beep_time,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);	
}
// 录像使能	   
void SetMotionDetRecordEn(request *req, COMMAND_ARGUMENT *argm)
{	
	unsigned int value=atoi(argm->value);
	if(value == 1)
	{
		// 插入日志---联动蜂鸣器
		LogInsert(LOG_STATUS_WARNING, LOG_STATUS_LINK_RECODE_CH_ON,g_usr);
	}
	else
	{
		// 插入日志---关闭联动蜂鸣器
		LogInsert(LOG_STATUS_WARNING,LOG_STATUS_LINK_RECODE_CH_OFF,g_usr);
	}
	g_MotionInfo[g_setMotionDetChannel].record_enable=value;
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name, g_MotionInfo[g_setMotionDetChannel].record_enable,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}
// 录像时间
void SetMotionDetRecordTime(request *req, COMMAND_ARGUMENT *argm)
{	
	unsigned int value=atoi(argm->value);
	g_MotionInfo[g_setMotionDetChannel].record_time=value;
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name, g_MotionInfo[g_setMotionDetChannel].record_time,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);	
}
// SMTP邮件告警使能
void SetMotionDetSmtpEn(request *req, COMMAND_ARGUMENT *argm)
{	
	unsigned int value=atoi(argm->value);
	g_MotionInfo[g_setMotionDetChannel].smtp_enable=value;
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name, g_MotionInfo[g_setMotionDetChannel].smtp_enable,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
}
// FTP使能
void SetMotionDetFtpEn(request *req, COMMAND_ARGUMENT *argm)
{	
	unsigned int value=atoi(argm->value);
	g_MotionInfo[g_setMotionDetChannel].ftp_enable=value;
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name, g_MotionInfo[g_setMotionDetChannel].ftp_enable,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);	
}
void SetMotionDetBlock(request *req, COMMAND_ARGUMENT *argm)
{	
	int i = 0, j = 0;
	char *p =NULL;
	char *value = argm->value;
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,argm->value);
	//fprintf(stderr," %s  %s   %s :%d\n",argm->name,argm->value,__FILE__,__LINE__);
	if(AllMotionDetChn)
	{
		for(i = 0;i < configGetDisplayNum();i++)
		{
			if(strlen(value)  != 0)
			{
				p = strtok(value,";");
				while(p != NULL)
				{
					g_MotionInfo[i].blockstatus[j++] = atoi(p);
					p = strtok(NULL,";");
				}
			}
		}

	}
	else
	{
		if(strlen(value)  != 0)
		{
			p = strtok(value,";");
			while(p != NULL)
			{
				//fprintf(stderr,"p:%s\n",p);
				g_MotionInfo[g_setMotionDetChannel].blockstatus[j++] = atoi(p);
				p = strtok(NULL,";");
			}

		}
	}

}
// 灵敏度等级
void SetMotionDetSen(request *req, COMMAND_ARGUMENT *argm)
{	
	int ret=0;
	unsigned int value=atoi(argm->value);
	g_MotionInfo[g_setMotionDetChannel].sensitivity=value;
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name, g_MotionInfo[g_setMotionDetChannel].sensitivity,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
	// 插入日志---移动侦测配置
	LogInsert(LOG_PARAMS_SETUP,LOG_MOTION_SETUP,g_usr);
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name, g_MotionInfo[g_setMotionDetChannel].reserve,argm->value,__FILE__,__LINE__);
	if(AllMotionDetChn)
	{
		AllMotionDetChn= 0;
		g_setMotionDetChannel = CHANN_TYPE_ALL; // 注意数组的长度不能超过MAX_DISPLAY_NUM(32)
		ret=databaseOperate(SDK_MAIN_MSG_MOTION_CFG, SDK_PARAM_SET, &g_setMotionDetChannel, sizeof(int), &g_MotionInfo[0], sizeof(g_MotionInfo[0]));
		if(ret<0)
		{
			fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_RECORD_CFG  read  fail %s:%d\n",__FILE__,__LINE__);
		}
	}
	else
	{
		ret=databaseOperate(SDK_MAIN_MSG_MOTION_CFG, SDK_PARAM_SET, &g_setMotionDetChannel, sizeof(int), &g_MotionInfo[g_setMotionDetChannel], sizeof(g_MotionInfo[g_setMotionDetChannel]));
		if(ret<0)
		{
			fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_RECORD_CFG  read  fail %s:%d\n",__FILE__,__LINE__);
		}
	}
}
// GET
void GetMotionDetChan(request *req, COMMAND_ARGUMENT *argm)
{
	int value=atoi(argm->value);
	int ret=0;
	if(value==0)
	{
		g_getMotionDetChannel=0;
		//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name,g_getMotionDetChannel,argm->value,__FILE__,__LINE__);

	}
	else
	{
		g_getMotionDetChannel=value -1;
	}
	memset(g_MotionInfo,0,sizeof(g_MotionInfo));
	ret=databaseOperate(SDK_MAIN_MSG_MOTION_CFG,SDK_PARAM_GET,&g_getMotionDetChannel,sizeof(int),&g_MotionInfo[g_getMotionDetChannel],sizeof(g_MotionInfo[g_getMotionDetChannel]));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_MOTION_CFG fail %s:%d\n",__FILE__,__LINE__);
	}
	//fprintf(stderr," %s %d   %s   %s :%d\n",argm->name,g_getMotionDetChannel,argm->value,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void GetMotionDetEn(request *req, COMMAND_ARGUMENT *argm)
{
	int value=0;
	value=g_MotionInfo[g_getMotionDetChannel].enable;
	//fprintf(stderr," %s %d    %s :%d\n",argm->name,g_MotionInfo[g_getMotionDetChannel].enable,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void GetMotionDetBeepEn(request *req, COMMAND_ARGUMENT *argm)
{
	int value=0;
	value=g_MotionInfo[g_getMotionDetChannel].beep_enable;
	//fprintf(stderr," %s %d    %s :%d\n",argm->name,g_MotionInfo[g_getMotionDetChannel].beep_enable,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void GetMotionDetBeepTime(request *req, COMMAND_ARGUMENT *argm)
{
	int value=0;
	value=g_MotionInfo[g_getMotionDetChannel].beep_time;
	//fprintf(stderr," %s %d    %s :%d\n",argm->name,g_MotionInfo[g_getMotionDetChannel].beep_time,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void GetMotionDetRecordEn(request *req, COMMAND_ARGUMENT *argm)
{
	int value=0;
	value=g_MotionInfo[g_getMotionDetChannel].record_enable;
	//fprintf(stderr," %s %d    %s :%d\n",argm->name,g_MotionInfo[g_getMotionDetChannel].record_enable,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void GetMotionDetRecordTime(request *req, COMMAND_ARGUMENT *argm)
{
	int value=0;
	value=g_MotionInfo[g_getMotionDetChannel].record_time;
	//fprintf(stderr," %s %d    %s :%d\n",argm->name,g_MotionInfo[g_getMotionDetChannel].record_time,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void GetMotionDetSmtpEn(request *req, COMMAND_ARGUMENT *argm)
{
	int value=0;
	value=g_MotionInfo[g_getMotionDetChannel].smtp_enable;
	//fprintf(stderr," %s %d    %s :%d\n",argm->name,g_MotionInfo[g_getMotionDetChannel].smtp_enable,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void GetMotionDetFtpEn(request *req, COMMAND_ARGUMENT *argm)
{
	int value=0;
	value=g_MotionInfo[g_getMotionDetChannel].ftp_enable;
	//fprintf(stderr," %s %d    %s :%d\n",argm->name,g_MotionInfo[g_getMotionDetChannel].ftp_enable,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void GetMotionDetBlock(request *req, COMMAND_ARGUMENT *argm)
{
	int i = 0;
	char IntString[8] = {0};
	char BlockArr[750] = {0};
	for(i = 0;i < MAX_BLOCK_ROW;i++)
	{
		// 将blockstatus[MAX_BLOCK_ROW]数组内的数据保存为字符串
		// 字符串中每个数据之间使用分号;隔开
		memset(IntString,0,sizeof(IntString));
		sprintf(IntString,"%d",g_MotionInfo[g_getMotionDetChannel].blockstatus[i]);
		strcat(BlockArr,IntString);    
		strcat(BlockArr,";");
	}
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,BlockArr);
}
void GetMotionDetSen(request *req, COMMAND_ARGUMENT *argm)
{
	int value=0;
	value=g_MotionInfo[g_getMotionDetChannel].sensitivity;
	//fprintf(stderr," %s %d    %s :%d\n",argm->name,g_MotionInfo[g_getMotionDetChannel].sensitivity,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

/***************************************************************************
  日志(Log) add by he 2014.6.20
  日志的相关字段:
  1）日志参数设置
  setLogType        日志类型
  setquerylog       查询
  setdeletelog      删除     
  setlogbeginyear   起始时间：年
  setlogbeginmon    起始时间：月
  setlogbeginday    起始时间：日
  setlogbeginhour   起始时间：时
  setlogbeginmin    起始时间：分
  setlogbeginsec    起始时间：秒
  setlogendyear     结束时间：年
  setlogendmon      结束时间：月
  setlogendday      结束时间：日
  setlogendhour     结束时间：时
  setlogendmin      结束时间：分
  setlogendsec      结束时间：秒
  2）日志信息
  getlognum            日志信息的序列号
  getlogtime           日志产生的时间
  getlogid           日志类型

 ****************************************************************************/

// 查询获取日志信息
// 首先要将需要查询的日志类型、起始时间、结束时间等信息保存到相关结构体中
void SetLogType(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	memset(&g_LogCond, 0, sizeof(sdk_log_cond_t));// 使用之前最好先清零

	// 现在日志类型还没做区分2014.6.23
	g_LogCond.type = value;
	//fprintf(stderr," WWWWWWWSetLogType= %d  %s :%d\n",value, __FILE__,__LINE__);
	//req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void SetLogBeginYear(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_LogCond.begin_time.year = value;
	//fprintf(stderr," 22222222SetLogBeginYear=%d  %s :%d\n",value, __FILE__,__LINE__);
	//req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}
void SetLogBeginMon(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_LogCond.begin_time.mon= value;
	//fprintf(stderr," 3333333SetLogBeginMon=%d  %s :%d\n",value, __FILE__,__LINE__);
	//req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}
void SetLogBeginDay(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_LogCond.begin_time.day= value;
	//fprintf(stderr," 4444444444SetLogBeginDay= %d  %s :%d\n",value, __FILE__,__LINE__);
	//req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}
void SetLogBeginHour(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_LogCond.begin_time.hour= value;
	//fprintf(stderr," 555555555555SetLogBeginHour  = %d  %s :%d\n",value, __FILE__,__LINE__);
	//req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}
void SetLogBeginMin(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_LogCond.begin_time.min= value;
	//fprintf(stderr," 6666666666SetLogBeginMin  = %d  %s :%d\n",value, __FILE__,__LINE__);
	//req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}
void SetLogBeginSec(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_LogCond.begin_time.sec= value;
	//fprintf(stderr," 7777777777777SetLogBeginSec  = %d  %s :%d\n",value, __FILE__,__LINE__);
	//req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}
void SetLogEndYear(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_LogCond.end_time.year =value;
	//fprintf(stderr," 888888888SetLogEndYear  = %d  %s :%d\n",value, __FILE__,__LINE__);
	//req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}
void SetLogEndMon(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_LogCond.end_time.mon=value;
	//fprintf(stderr," 99999999999SetLogEndMon  = %d  %s :%d\n",value, __FILE__,__LINE__);
	//req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}
void SetLogEndDay(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_LogCond.end_time.day=value;
	//fprintf(stderr," AAAAAAAAAAAAAAASetLogEndDay  = %d  %s :%d\n",value, __FILE__,__LINE__);
	//req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}
void SetLogEndHour(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_LogCond.end_time.hour=value;
	//fprintf(stderr," BBBBBBBBBBBBBSetLogEndHour  = %d  %s :%d\n",value, __FILE__,__LINE__);
	//req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}
void SetLogEndMin(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_LogCond.end_time.min=value;
	//fprintf(stderr," CCCCCCCCCCCCCSetLogEndMin  = %d  %s :%d\n",value, __FILE__,__LINE__);
	//req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}
void SetLogEndSec(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_LogCond.end_time.sec=value;
	//fprintf(stderr," DDDDDDDDDDDDDSetLogEndSec  = %d  %s :%d\n",value, __FILE__,__LINE__);
	//req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}
void SetQueryLog(request *req, COMMAND_ARGUMENT *argm)// 这个函数要放在查询数据的后面，用于调用数据库接口函数
{
	unsigned int value=atoi(argm->value);
	int ret = 0;
	char *temp= malloc(60 * sizeof(sdk_log_item_t));// 要注意不能溢出
	int LogItemNum = 0;
	int ReqNum = 0; // 查询的次数，每次最多查询60条日志信息
	sdk_log_cond_t *pCond = NULL;
	pCond = (sdk_log_cond_t *)temp;
	sdk_log_item_t *pItem = NULL;
	int i = 0,j = 0;
	char logtime[32] = {0};
	// req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d", argm->name,value);
	if(value == 1)
	{
		memcpy(pCond,&g_LogCond,sizeof(g_LogCond));
		ret=databaseOperate(SDK_MAIN_MSG_LOG,SDK_LOG_QUERY,SDK_LOG_QUERY_NUM,0,pCond,0);
		if(ret<0)
		{
			fprintf(stderr,"databaseOperate  Query Log fail %s:%d\n",__FILE__,__LINE__);
		}

		LogItemNum = ret;
		ReqNum = LogItemNum/60;
		//fprintf(stderr," LogItemNum:%d,  ReqNum:%d \n",LogItemNum,ReqNum);
		for(i = 0; i <= ReqNum; i++)
		{
			memset(temp,0,(60 * sizeof(sdk_log_item_t)));
			pItem = (sdk_log_item_t *)temp;//
			databaseOperate(SDK_MAIN_MSG_LOG,SDK_LOG_QUERY,0,SDK_LOG_QUERY_CONTENT,pItem,0);
			if(i == ReqNum)
			{
				for(j = 0; j < (LogItemNum%60); j++)
				{

					sprintf(logtime,"%4d/%02d/%02d-%02d:%02d:%02d",pItem->time.year, pItem->time.mon, pItem->time.day,pItem->time.hour,pItem->time.min,pItem->time.sec);
					req->buffer_end += sprintf(req_bufptr(req), "getlognum=%d,getlogtime=%s,getlogid=0x%x,getlogusr=%s<br>", (j + (60 * i)),logtime,pItem->logId,pItem->user);
					pItem++;
				}
			}
			else
			{
				for(j = 0; j < 60; j++)
				{
					sprintf(logtime,"%4d/%02d/%02d-%02d:%02d:%02d",pItem->time.year, pItem->time.mon, pItem->time.day,pItem->time.hour,pItem->time.min,pItem->time.sec);
					req->buffer_end += sprintf(req_bufptr(req), "getlognum=%d,getlogtime=%s,getlogid=0x%x,getlogusr=%s<br>",( j + (60 * i)),logtime,pItem->logId,pItem->user);
					pItem++;
				}
			}
		}
	}
	free(temp);
}


// 删除日志
void SetDeleteLog(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	int ret;
	//fprintf(stderr," #########SetDeleteLog %d  %s :%d\n",value, __FILE__,__LINE__);
	if(value == 1)
	{
		ret=databaseOperate(SDK_MAIN_MSG_LOG,SDK_LOG_CLEAR,NULL,0,NULL,0);
		if(ret<0)
		{
			fprintf(stderr,"#####databaseOperate  Query Log fail %s:%d\n",__FILE__,__LINE__);
		}
	}
	// 插入日志---删除日志
	LogInsert(LOG_ALARM_EVENT,LOG_WARNING_CLEAR_LOG,g_usr);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

//设置 NTP时间服务器
void SetNtpServer(request *req, COMMAND_ARGUMENT *argm)
{
	char *value = argm->value;
	memset(&g_ntpInfo, 0, sizeof(sdk_ntp_conf_t));
	//fprintf(stderr," #########NTP Server %s %s :%d\n",argm->value, __FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,value);
	memcpy(g_ntpInfo.ntp_server, value, strlen(value));

}
// 设置NTP时区
void SetNtpTimeZone(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned char value=atoi(argm->value);
	// 插入日志---NTP配置
	LogInsert(LOG_STATUS_WARNING,LOG_STATUS_SYNC_NTP,g_usr);
	//fprintf(stderr," #########NTP Time zone %d %s :%d\n",value, __FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
	g_ntpInfo.time_zone = value;
}


void SetNtpcontrol(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned char value=atoi(argm->value);
	// 插入日志---NTP配置
	LogInsert(LOG_STATUS_WARNING,LOG_STATUS_SYNC_NTP,g_usr);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
	g_ntpInfo.enable = value;
}


void SetNtpPort(request *req, COMMAND_ARGUMENT *argm)
{
	int ret= 0;
	unsigned char value=atoi(argm->value);
	// 插入日志---NTP配置
	LogInsert(LOG_STATUS_WARNING,LOG_STATUS_SYNC_NTP,g_usr);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
	g_ntpInfo.port = value;
	ret=databaseOperate(SDK_MAIN_MSG_NTP, SDK_PARAM_SET, NULL, 0, &g_ntpInfo, sizeof(sdk_ntp_conf_t));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_NTP  read  fail %s:%d\n",__FILE__,__LINE__);
	}
}


void SetNtpInterval(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned char value=atoi(argm->value);
	// 插入日志---NTP配置
	LogInsert(LOG_STATUS_WARNING,LOG_STATUS_SYNC_NTP,g_usr);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
	g_ntpInfo.interval = value;
}


void SetNtpDateFormat(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned char value=atoi(argm->value);
	// 插入日志---NTP配置
	LogInsert(LOG_STATUS_WARNING,LOG_STATUS_SYNC_NTP,g_usr);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
	g_ntpInfo.date_format = value;
}


void SetNtpDateSeparator(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned char value=atoi(argm->value);
	// 插入日志---NTP配置
	LogInsert(LOG_STATUS_WARNING,LOG_STATUS_SYNC_NTP,g_usr);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
	g_ntpInfo.date_separator = value;
}


void SetNtpTimeFormat(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned char value=atoi(argm->value);
	// 插入日志---NTP配置
	LogInsert(LOG_STATUS_WARNING,LOG_STATUS_SYNC_NTP,g_usr);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
	g_ntpInfo.time_format = value;
}


//获取 NTP时间服务器
void GetNtpServer(request *req, COMMAND_ARGUMENT *argm)
{
	int ret= 0;
	memset(&g_ntpInfo, 0, sizeof(sdk_ntp_conf_t));
	ret=databaseOperate(SDK_MAIN_MSG_NTP, SDK_PARAM_GET, NULL, 0, &g_ntpInfo, sizeof(sdk_ntp_conf_t));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_NTP  read  fail %s:%d\n",__FILE__,__LINE__);
	}
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n",argm->name,g_ntpInfo.ntp_server);
}
// 获取NTP时区
void GetNtpTimeZone(request *req, COMMAND_ARGUMENT *argm)
{
	signed char value = 0; //必须是有符号
	//fprintf(stderr," TTTTTimezone:  %d   %s:%d\n",g_ntpInfo.time_zone,__FILE__,__LINE__);
	value = g_ntpInfo.time_zone;

	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,value);
}


void GetNtpcontrol(request *req, COMMAND_ARGUMENT *argm)
{
	signed char value = 0; //必须是有符号
	//fprintf(stderr," TTTTTimezone:  %d   %s:%d\n",g_ntpInfo.time_zone,__FILE__,__LINE__);
	value = g_ntpInfo.enable;

	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,value);
}


void GetNtpPort(request *req, COMMAND_ARGUMENT *argm)
{
	signed char value = 0;
	value = g_ntpInfo.port;

	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,value);
}


void GetNtpInterval(request *req, COMMAND_ARGUMENT *argm)
{
	signed char value = 0;
	value = g_ntpInfo.interval;

	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,value);
}


void GetNtpDateFormat(request *req, COMMAND_ARGUMENT *argm)
{
	signed char value = 0;
	value = g_ntpInfo.date_format;

	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,value);
}


void GetNtpDateSeparator(request *req, COMMAND_ARGUMENT *argm)
{
	signed char value = 0;
	value = g_ntpInfo.date_separator;

	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,value);
}


void GetNtpTimeFormat(request *req, COMMAND_ARGUMENT *argm)
{
	signed char value = 0;
	value = g_ntpInfo.time_format;

	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,value);
}


//------------FTP
void SetFtpEnable(request *req, COMMAND_ARGUMENT *argm)
{
	char value = atoi(argm->value);
	memset(&g_ftp_info, 0, sizeof(sdk_ftp_config));
	g_ftp_info.bFtpEnable = value;
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void SetFtpServierIp(request *req, COMMAND_ARGUMENT *argm)
{
	char *value = argm->value;
	memcpy(g_ftp_info.servier_ip, value, strlen(value));
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,value);
}
	
void SetFtpUserName(request *req, COMMAND_ARGUMENT *argm)
{
	char *value = argm->value;
	memcpy(g_ftp_info.username, value, strlen(value));
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,value);
}

void SetFtpPassword(request *req, COMMAND_ARGUMENT *argm)
{
	char *value = argm->value;
	memcpy(g_ftp_info.password, value, strlen(value));
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,value);
}

void SetFtpFolderName(request *req, COMMAND_ARGUMENT *argm)
{
	char *value = argm->value;
	memcpy(g_ftp_info.foldername, value, strlen(value));
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,value);
}


void SetFtpPort(request *req, COMMAND_ARGUMENT *argm)
{
	int ret;
	unsigned int value = atoi(argm->value);
	g_ftp_info.port= value;
	ret=databaseOperate(SDK_MAIN_MSG_FTP, SDK_PARAM_SET, NULL, 0, &g_ftp_info, sizeof(sdk_ftp_config));

	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_NTP  read  fail %s:%d\n",__FILE__,__LINE__);
	}
	
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void GetFtpEnable(request *req, COMMAND_ARGUMENT *argm)
{
	int ret= 0;
	memset(&g_ftp_info, 0, sizeof(sdk_ftp_config));
	ret=databaseOperate(SDK_MAIN_MSG_FTP, SDK_PARAM_GET, NULL, 0, &g_ftp_info, sizeof(sdk_ftp_config));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_NTP  read  fail %s:%d\n",__FILE__,__LINE__);
	}
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,g_ftp_info.bFtpEnable);

}

void GetFtpServierIp(request *req, COMMAND_ARGUMENT *argm)
{
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n",argm->name,g_ftp_info.servier_ip);
}

void GetFtpUserName(request *req, COMMAND_ARGUMENT *argm)
{
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n",argm->name,g_ftp_info.username);
}

void GetFtpPassword(request *req, COMMAND_ARGUMENT *argm)
{
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n",argm->name,g_ftp_info.password);
}

void GetFtpFolderName(request *req, COMMAND_ARGUMENT *argm)
{
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n",argm->name,g_ftp_info.foldername);
}

void GetFtpPort(request *req, COMMAND_ARGUMENT *argm)
{
	signed char value = 0; 
	value = g_ftp_info.port;

	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,value);
}
	


//------------SMTP
//SMTP SET
void SetSmtpServer(request *req, COMMAND_ARGUMENT *argm)
{
	char *value = argm->value;
	memset(&g_smtp_info, 0, sizeof(g_smtp_info));
	memcpy(g_smtp_info.smtp_server, value, strlen(value));
	//fprintf(stderr," #########SetSmtpServer %s  %s :%d\n",value, __FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,value);
}
void SetSmtpUser(request *req, COMMAND_ARGUMENT *argm)
{
	char *value = argm->value;
	memcpy(g_smtp_info.user, value, strlen(value));
	//fprintf(stderr," #########SetSmtpUser %s  %s :%d\n",value, __FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,value);
}
void SetSmtpPasswd(request *req, COMMAND_ARGUMENT *argm)
{
	char *value = argm->value;
	memcpy(g_smtp_info.passwd, value, strlen(value));
	//fprintf(stderr," #########SetSmtpPasswd %s  %s :%d\n",value, __FILE__,__LINE__);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,value);
}
void SetSmtpReceiver(request *req, COMMAND_ARGUMENT *argm)
{
	char *value = argm->value;
	memcpy(g_smtp_info.receiver, value, strlen(value));
	//fprintf(stderr," #########SetSmtpReceiver %s  %s :%d\n",value, __FILE__,__LINE__);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,value);
}
void SetSmtpSender(request *req, COMMAND_ARGUMENT *argm)
{
	char *value = argm->value;
	memcpy(g_smtp_info.sender, value, strlen(value));
	//fprintf(stderr," #########SetSmtpSender %s  %s :%d\n",value, __FILE__,__LINE__);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,value);
}
void SetSmtpTheme(request *req, COMMAND_ARGUMENT *argm)
{
	char *value = argm->value;
	memcpy(g_smtp_info.theme, value, strlen(value));
	//fprintf(stderr," #########SetSmtpTheme %s  %s :%d\n",value, __FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,value);
}

void SetSmtpAnoEn(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_smtp_info.anonymity = value;
	//fprintf(stderr," #########SetSmtpAnoEn %d  %s :%d\n",value, __FILE__,__LINE__);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}
void SetSmtpFileEn(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_smtp_info.file_enable= value;
	//fprintf(stderr," #########SetSmtpFileEn %d  %s :%d\n",value, __FILE__,__LINE__);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}
void SetSmtpPort(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_smtp_info.port= value;
	//fprintf(stderr," #########SetSmtpPort %d  %s :%d\n",value, __FILE__,__LINE__);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}
void SetSmtpEn(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	if(value == 1)
	{
		// 插入日志---SMTP使能
		LogInsert(LOG_STATUS_WARNING,LOG_STATUS_SMTP_ON,g_usr);
	}
	else
	{
		// 插入日志---SMTP不使能
		LogInsert(LOG_STATUS_WARNING,LOG_STATUS_SMTP_OFF,g_usr);
	}
	g_smtp_info.email_enable= value;
	//fprintf(stderr," #########SetSmtpEn %d  %s :%d\n",value, __FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);

}

void SetSmtpEncType(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_smtp_info.encryption_type= value;
	//fprintf(stderr," #########SetSmtpEncType %d  %s :%d\n",value, __FILE__,__LINE__);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}
void SetSmtpInterval(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_smtp_info.interval= value;
	//fprintf(stderr," #########SetSmtpInterval %d  %s :%d\n",value, __FILE__,__LINE__);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}
void SetSmtpHealthEn(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_smtp_info.health_enable= value;
	//fprintf(stderr," #########SetSmtpHealthEn %d  %s :%d\n",value, __FILE__,__LINE__);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}
void SetSmtpHealthInterval(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	int ret;
	// 插入日志---SMTP配置
	LogInsert(LOG_PARAMS_SETUP,LOG_SMTP_SETUP,g_usr);
	g_smtp_info.health_interval= value;
	ret=databaseOperate(SDK_MAIN_MSG_SMTP, SDK_PARAM_SET, NULL, 0, &g_smtp_info, sizeof(sdk_email_conf_t));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_NTP  read  fail %s:%d\n",__FILE__,__LINE__);
	}
	//fprintf(stderr," #########SetSmtpHealthInterval %d  %s :%d\n",value, __FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}
// SMTP GET
void GetSmtpServer(request *req, COMMAND_ARGUMENT *argm)
{
	int ret;
	memset(&g_smtp_info, 0, sizeof(g_smtp_info));
	ret=databaseOperate(SDK_MAIN_MSG_SMTP, SDK_PARAM_GET, NULL, 0, &g_smtp_info, sizeof(sdk_email_conf_t));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_NTP  read  fail %s:%d\n",__FILE__,__LINE__);
	}
	//fprintf(stderr," GetSmtpServer:  %s   %s:%d\n",g_smtp_info.smtp_server,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n",argm->name,g_smtp_info.smtp_server);
}
void GetSmtpUser(request *req, COMMAND_ARGUMENT *argm)
{
	//fprintf(stderr," GetSmtpUser:  %s   %s:%d\n",g_smtp_info.user,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n",argm->name,g_smtp_info.user);
}
void GetSmtpPasswd(request *req, COMMAND_ARGUMENT *argm)
{
	//fprintf(stderr," GetSmtpPasswd:  %s   %s:%d\n",g_smtp_info.passwd,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n",argm->name,g_smtp_info.passwd);
}
void GetSmtpRreceiver(request *req, COMMAND_ARGUMENT *argm)
{
	//fprintf(stderr," GetSmtpRreceiver:  %s   %s:%d\n",g_smtp_info.receiver,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n",argm->name,g_smtp_info.receiver);
}
void GetSmtpSender(request *req, COMMAND_ARGUMENT *argm)
{
	//fprintf(stderr," GetSmtpSender:  %s   %s:%d\n",g_smtp_info.sender,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n",argm->name,g_smtp_info.sender);
}
void GetSmtpTheme(request *req, COMMAND_ARGUMENT *argm)
{
	//fprintf(stderr," GetSmtpTheme:  %s   %s:%d\n",g_smtp_info.theme,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n",argm->name,g_smtp_info.theme);
}
void GetSmtpAnoEn(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned char value = 0;
	//fprintf(stderr," GetSmtpAnoEn:  %d   %s:%d\n",g_smtp_info.anonymity,__FILE__,__LINE__);
	value = g_smtp_info.anonymity;
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,value);
}
void GetSmtpFileEn(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned char value = 0;
	//fprintf(stderr," GetSmtpFileEn:  %d   %s:%d\n",g_smtp_info.file_enable,__FILE__,__LINE__);
	value = g_smtp_info.file_enable;
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,value);
}
void GetSmtpPort(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = 0;
	//fprintf(stderr," GetSmtpPort:  %d   %s:%d\n",g_smtp_info.port,__FILE__,__LINE__);
	value = g_smtp_info.port;
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,value);
}
void GetSmtpEncType(request *req, COMMAND_ARGUMENT *argm)
{
	signed char value = 0;
	//fprintf(stderr," GetSmtpEncType:  %d   %s:%d\n",g_smtp_info.encryption_type,__FILE__,__LINE__);
	value = g_smtp_info.encryption_type;
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,value);
}
void GetSmtpInterval(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = 0;
	//fprintf(stderr," GetSmtpInterval:  %d   %s:%d\n",g_smtp_info.interval,__FILE__,__LINE__);
	value = g_smtp_info.interval;
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,value);
}
void GetSmtpHealthEn(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned char value = 0;
	//fprintf(stderr," GetSmtpHealthEn:  %d   %s:%d\n",g_smtp_info.health_enable,__FILE__,__LINE__);
	value = g_smtp_info.health_enable;
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,value);
}
void GetSmtpHealthInterval(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = 0;
	//fprintf(stderr," GetSmtpHealthInterval:  %d   %s:%d\n",g_smtp_info.health_interval,__FILE__,__LINE__);
	value = g_smtp_info.health_interval;
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,value);
}
void GetSmtpEn(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned char value = 0;
	//fprintf(stderr," GetSmtpEn:  %d   %s:%d\n",g_smtp_info.email_enable,__FILE__,__LINE__);
	value = g_smtp_info.email_enable;
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,value);
}

//-------------- DDNS

void SetDdnsEn(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	if(value == 1)
	{
		// 插入日志---DDNS使能
		LogInsert(LOG_STATUS_WARNING,LOG_STATUS_DDNS_ON,g_usr);
	}

	memset(&g_ddns_info, 0, sizeof(g_ddns_info));
	g_ddns_info.enable= value;
	//fprintf(stderr," #########SetDdnsEn %d  %s :%d\n",value, __FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}
void SetDdnstype(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_ddns_info.type= value;
	//fprintf(stderr," #########SetDdnstype %d  %s :%d\n",value, __FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}
void SetDdnsInterTime(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_ddns_info.interval= value;
	//fprintf(stderr," #########SetDdnsInterTime %d  %s :%d\n",value, __FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}

void SetDdnsUserType(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	g_ddns_info.user_type= value;
	//fprintf(stderr," #########SetDdnsUserType %d  %s :%d\n",value, __FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
}
void SetDdnsDomainName(request *req, COMMAND_ARGUMENT *argm)
{
	char *value = argm->value;
	memcpy(g_ddns_info.domain, value, strlen(value));
	//fprintf(stderr," #########SetDdnsDomainName %s  %s :%d\n",value, __FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,value);
}

void SetDdnsUserName(request *req, COMMAND_ARGUMENT *argm)
{
	char *value = argm->value;
	memcpy(g_ddns_info.user, value, strlen(value));
	//fprintf(stderr," #########SetDdnsUserName %s  %s :%d\n",value, __FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,value);
}
void SetDdnsUserPwd(request *req, COMMAND_ARGUMENT *argm)
{
	char *value = argm->value;
	int ret;
	// 插入日志---DDNS配置
	LogInsert(LOG_PARAMS_SETUP,LOG_DDNS_SETUP,g_usr);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name,value);
	memcpy(g_ddns_info.pwd, value, strlen(value));
	//fprintf(stderr," #########SetDdnsUserPwd %s  %s :%d\n",value, __FILE__,__LINE__);
	ret=databaseOperate(SDK_MAIN_MSG_DDNS_CFG, SDK_PARAM_SET, &g_ddns_info.type, 0, &g_ddns_info, sizeof(sdk_ddns_cfg_t));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_DDNS_CFG  read  fail %s:%d\n",__FILE__,__LINE__);
	}
}

void GetDdnstype(request *req, COMMAND_ARGUMENT *argm)
{
	int ret;
	memset(&g_ddns_info, 0, sizeof(g_ddns_info));
	g_ddns_info.type = atoi(argm->value) ;
	ret=databaseOperate(SDK_MAIN_MSG_DDNS_CFG, SDK_PARAM_GET, &g_ddns_info.type, 0, &g_ddns_info, sizeof(sdk_ddns_cfg_t));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_DDNS_CFG  read  fail %s:%d\n",__FILE__,__LINE__);
	}
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,atoi(argm->value));
}

void GetDdnsEn(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned char value = 0;

	value = g_ddns_info.enable;
	//fprintf(stderr," GetDdnsEn:  %d   %s:%d\n",g_ddns_info.enable,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,value);

}

void GetDdnsInterTime(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = 0;
	//fprintf(stderr," GetDdnsInterTime:  %d   %s:%d\n",g_ddns_info.interval,__FILE__,__LINE__);
	value = g_ddns_info.interval;
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,value);
}

void GetDdnsUserType(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned char value = 0;
	//fprintf(stderr," GetDdnsUserType:  %d   %s:%d\n",g_ddns_info.user_type,__FILE__,__LINE__);
	value = g_ddns_info.user_type;
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,value);
}
void GetDdnsDomainName(request *req, COMMAND_ARGUMENT *argm)
{
	//fprintf(stderr," GetDdnsUserName:  %s   %s:%d\n",g_ddns_info.domain,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n",argm->name,g_ddns_info.domain);
}
void GetDdnsUserName(request *req, COMMAND_ARGUMENT *argm)
{
	//fprintf(stderr," GetDdnsUserName:  %s   %s:%d\n",g_ddns_info.user,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n",argm->name,g_ddns_info.user);
}

void GetDdnsUserPwd(request *req, COMMAND_ARGUMENT *argm)
{
	//fprintf(stderr," GetDdnsUserPwd:  %s   %s:%d\n",g_ddns_info.pwd,__FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n",argm->name,g_ddns_info.pwd);
}

void SetFreeIpEn(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value=atoi(argm->value);
	int ret;
	memset(g_p2p_info,0,sizeof(g_p2p_info));
	ret=databaseOperate(SDK_MAIN_MSG_P2P, SDK_PARAM_GET, NULL, 0, g_p2p_info, sizeof(sdk_p2p_conf_t));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_DDNS_CFG  read  fail %s:%d\n",__FILE__,__LINE__);
	}
	g_p2p_info[0].P2pEn= value;
	fprintf(stderr," #########SetFreeIpEn %d  %s :%d\n",value, __FILE__,__LINE__);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);

}	

void SetDanaleEn(request *req, COMMAND_ARGUMENT *argm)
{
	int ret;
	unsigned int value=atoi(argm->value);
	if((value == 0)&&(!g_p2p_info[0].P2pEn))
	{
		// 插入日志，P2P关闭
		LogInsert(LOG_STATUS_WARNING,LOG_STATUS_P2P_OFF,g_usr);
	}
	else
	{
		// 插入日志，P2P开启
		LogInsert(LOG_STATUS_WARNING,LOG_STATUS_P2P_ON,g_usr);
	}
	g_p2p_info[1].P2pEn= value;
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name,value);
	ret=databaseOperate(SDK_MAIN_MSG_P2P, SDK_PARAM_SET, NULL, 0, g_p2p_info, sizeof(sdk_p2p_conf_t));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_P2P  write  fail %s:%d\n",__FILE__,__LINE__);
	}
}	
void SetFreeIpSn(request *req, COMMAND_ARGUMENT *argm)
{
	int ret=0;
	char buf[64];
	memset(&buf,0,sizeof(buf));
	memcpy(buf,argm->value,strlen(argm->value));
	FILE *fp = fopen(SN_FILE_FREEIP, "w");
	if (!fp ) 
	{
		return ;
	}
	ret=fwrite(&buf, sizeof(buf), 1, fp);
	if(ret<0)
	{
		fprintf(stderr,"fwrite fail  %s:%d\n",__FILE__,__LINE__);
		fclose(fp);
		return ;
	}
	fclose(fp);

	memset(g_p2p_info, 0, sizeof(g_p2p_info));
	ret=databaseOperate(SDK_MAIN_MSG_P2P, SDK_PARAM_GET, NULL, 0, g_p2p_info, sizeof(sdk_p2p_conf_t));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_DDNS_CFG  read  fail %s:%d\n",__FILE__,__LINE__);
	}
	memset(&g_p2p_info[0].SerialNum,0,sizeof(g_p2p_info[0].SerialNum));
	memcpy(&g_p2p_info[0].SerialNum,argm->value,strlen(argm->value));
	ret=databaseOperate(SDK_MAIN_MSG_P2P, SDK_PARAM_SET, NULL, 0, g_p2p_info, sizeof(sdk_p2p_conf_t));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_DDNS_CFG  read  fail %s:%d\n",__FILE__,__LINE__);
	}
	req->buffer_end += sprintf(req_bufptr(req), "%s\n", "OK");
}	

// 序列号单独字段设置
void serialnum(request *req, COMMAND_ARGUMENT *argm)
{
	int ret=0;
	char buf[64];
	memset(&buf,0,sizeof(buf));
	memcpy(buf,argm->value,strlen(argm->value));
	FILE *fp = fopen(SN_FILE_DANALE, "w");

	if (!fp ) {
		return ;
	}

	ret=fwrite(&buf, sizeof(buf), 1, fp);
	if(ret<0)
	{
		fprintf(stderr,"fwrite fail  %s:%d\n",__FILE__,__LINE__);
	}

	if(fp>0)
	{
		fclose(fp);
	}
	memset(g_p2p_info, 0, sizeof(g_p2p_info));
	ret=databaseOperate(SDK_MAIN_MSG_P2P, SDK_PARAM_GET, NULL, 0, g_p2p_info, sizeof(sdk_p2p_conf_t));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_DDNS_CFG  read  fail %s:%d\n",__FILE__,__LINE__);
	}
	memset(&g_p2p_info[1].SerialNum,0,sizeof(g_p2p_info[1].SerialNum));
	memcpy(g_p2p_info[1].SerialNum,argm->value,strlen(argm->value));
	ret=databaseOperate(SDK_MAIN_MSG_P2P, SDK_PARAM_SET, NULL, 0, g_p2p_info, sizeof(sdk_p2p_conf_t));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_DDNS_CFG  read  fail %s:%d\n",__FILE__,__LINE__);
	}
	req->buffer_end += sprintf(req_bufptr(req), "%s\n", "OK");
}


// FreeIp的序列号单独从数据库获取
void GetFreeIpSn(request *req, COMMAND_ARGUMENT *argm)
{
	int ret;
	memset(g_p2p_info, 0, sizeof(g_p2p_info));
	ret=databaseOperate(SDK_MAIN_MSG_P2P, SDK_PARAM_GET, NULL, 0, g_p2p_info, sizeof(sdk_p2p_conf_t));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_DDNS_CFG  read  fail %s:%d\n",__FILE__,__LINE__);
	}
	fprintf(stderr,"FreeIp SN:%s   %s:%d\n",g_p2p_info[0].SerialNum,__FILE__,__LINE__);

	req->buffer_end += sprintf(req_bufptr(req),OPTION_OK "%s\n", g_p2p_info[0].SerialNum);
}

void GetFreeIpEn(request *req, COMMAND_ARGUMENT *argm)
{
	int ret;
	memset(g_p2p_info, 0, sizeof(g_p2p_info));
	ret=databaseOperate(SDK_MAIN_MSG_P2P, SDK_PARAM_GET, NULL, 0, g_p2p_info, sizeof(sdk_p2p_conf_t));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_DDNS_CFG  read  fail %s:%d\n",__FILE__,__LINE__);
	}
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name, g_p2p_info[0].P2pEn);
}


void GetFreeIpAndUrl(request *req, COMMAND_ARGUMENT *argm)
{
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n",argm->name,g_p2p_info[0].AndroidAppUrl);
}

void GetFreeIpIosUrl(request *req, COMMAND_ARGUMENT *argm)
{
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n",argm->name,g_p2p_info[0].IosAppUrl);
}

void GetDanaleEn(request *req, COMMAND_ARGUMENT *argm)
{
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,g_p2p_info[1].P2pEn);
}

void getserialnum(request *req, COMMAND_ARGUMENT *argm)
{
	int ret=0;
	char buf[64];
	memset(&buf,0,sizeof(buf));
	memcpy(buf,argm->value,strlen(argm->value));

	FILE *fp = fopen(SN_FILE_DANALE, "r");

	if (!fp ) {
		return ;
	}

	ret=fread(&buf, sizeof(buf), 1, fp);
	if(ret<0)
	{
		fprintf(stderr,"read fail  %s:%d\n",__FILE__,__LINE__);
	}

	if(fp>0)
	{
		fclose(fp);
	}
	req->buffer_end += sprintf(req_bufptr(req), "%s\n", buf);
}

void GetDanaleAndUrl(request *req, COMMAND_ARGUMENT *argm)
{
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n",argm->name,g_p2p_info[1].AndroidAppUrl);
}

void GetDanaleIosUrl(request *req, COMMAND_ARGUMENT *argm)
{
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n",argm->name,g_p2p_info[1].IosAppUrl);
}

void GetRcvSendRate(request *req, COMMAND_ARGUMENT *argm)
{
	double rate_rcv,rate_send;
	GetNetworkFlow("eth0",&rate_rcv,&rate_send);
	rate_rcv = rate_rcv/(1024*1024)*8;
	rate_send = rate_send/(1024*1024)*8;
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "rate_rcv=%.2f,rate_send = %.2f\n",rate_rcv,rate_send);
}

void GetP2pConnectStatus(request *req, COMMAND_ARGUMENT *argm)
{
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,GetP2PConnectStatus());
}

void TestEmailStatus(request *req, COMMAND_ARGUMENT *argm)
{
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,testSmtp());
}

void GetAutoMaintainInterval(request *req, COMMAND_ARGUMENT *argm)
{
	int ret;
	int value = 0;
	memset(&g_AutoMaintainAttrCfg, 0, sizeof(g_AutoMaintainAttrCfg));
	ret=databaseOperate(SDK_MAIN_MSG_AUTO_MAINTAIN, SDK_PARAM_GET, NULL, 0, &g_AutoMaintainAttrCfg, sizeof(sdk_autoMaintain_attr_t));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_AUTO_MAINTAIN  read  fail %s:%d\n",__FILE__,__LINE__);
	}
	value = g_AutoMaintainAttrCfg.interval;
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,value);
}

void GetAutoMaintainHour(request *req, COMMAND_ARGUMENT *argm)
{
	int ret;
	int value = 0;
	value = g_AutoMaintainAttrCfg.hour;
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n",argm->name,value);
}


void SetAutoMaintainInterval(request *req, COMMAND_ARGUMENT *argm)
{
	g_AutoMaintainAttrCfg.interval=atoi(argm->value);
	req->buffer_end += sprintf(req_bufptr(req), "%s\n", "OK");
}

void SetAutoMaintainHour(request *req, COMMAND_ARGUMENT *argm)
{
	int ret;
	g_AutoMaintainAttrCfg.hour=atoi(argm->value);
	ret=databaseOperate(SDK_MAIN_MSG_AUTO_MAINTAIN, SDK_PARAM_SET, NULL, 0, &g_AutoMaintainAttrCfg, sizeof(sdk_autoMaintain_attr_t));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_AUTO_MAINTAIN  read  fail %s:%d\n",__FILE__,__LINE__);
	}
	req->buffer_end += sprintf(req_bufptr(req), "%s\n", "OK");
}


/***************************************************************************
 *                                                                         *
 ***************************************************************************/
#define HASH_TABLE_SIZE	(sizeof(HttpOptionTable)/sizeof(HTTP_OPTION))
HTTP_OPTION HttpOptionTable [] =
{

	/*net eth*/
	{"netip",set_netIp,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"netmask",set_netMask,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"gateway",set_netGetWay,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"dns1",set_dns1,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"dns2",set_dns2,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"mac",set_mac,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"dhcpenable",set_dhcp,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	//{"p2penable",set_p2pEnable,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL}, // 注意，这个p2p使能是保存在g_EthInfo.ip_info.dns_auto_en变量中的

	/*net manage */
	{"httpport",set_httpPort,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"rtspserverport",set_rtspServerPort,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"tcpport",set_tcpPort,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"multicast",set_multiCast,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	/*set channel cfg*/
	{"setchannelnum",set_channelNum,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setchanenable",set_chanEnable,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setchanname",set_chanName,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setchanno",set_channelNo,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setchanstream",set_chanStreamNo,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setchandeviceip",set_chanDeviceIpAddr,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setchandeviceport",set_chanDevicePort,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setchanusername",set_chanDevideUserName,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setchanuserpwd",set_chanDeviceUserPwd,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setchanmanprotocoltype",set_chanManProType,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setchanrtspmainurl",set_chanRtspMainUrl,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setchanrtspsuburl",set_chanRtspSubUrl,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setchandeviceprotocol",set_chanDeviceProtocol,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	/*get channel cfg*/
	{"getchannelnum",get_channelNum,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getchanenable",get_chanEnable,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getchanname",get_chanName,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getchanno",get_channelNo,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getchanstream",get_chanStreamNo,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getchandeviceip",get_chanDeviceIpAddr,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getchandeviceport",get_chanDevicePort,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getchanusername",get_chanDevideUserName,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getchanuserpwd",get_chanDeviceUserPwd,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getchanmanprotocoltype",get_chanManProType,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getchanrtspmainurl",get_chanRtspMainUrl,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getchanrtspsuburl",get_chanRtspSubUrl,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getchandeviceprotocol",get_chanDeviceProtocol,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	/*general cfg*/
	{"setdevname",set_DevName,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setlanguage",set_language,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setrecyclerecord",set_recycleRecord,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setdrvid",set_drvId,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setoutdevice",set_outDevice,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setvideomode",set_videoMode,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setresolution",set_resolution,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setstandbytime",set_standbyTime,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	{"selectallusername",select_all_user_Name,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	/*user  query cfg*/
	{"queryusernum",query_userNum,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"queryusername",query_userName,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"queryuserpwd",query_userPwd,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"querylocalright",query_localRight,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"queryremoteright",query_remoteRight,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	/*add user cfg*/
	{"addusername",add_userName,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"adduserpwd",add_userPwd,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"addlocalright",add_localRight,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"addremoteright",add_remoteRight,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	/*user mode cfg*/
	{"modeusername",mode_userName,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"modeuserpwd",mode_userPwd,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"modelocalright",mode_localRight,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"moderemoteright",mode_remoteRight,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},


	/*del user cfg*/
	{"delusername",del_userName,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	// encode cfg
	/// new config
	{"setipcencode", set_ipcencode, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"encoding0", set_encoding0, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"resolution0", set_resolution0, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"framerate0", set_framerate0, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"streamtype0", set_streamtype0, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"audioformat0", set_audioformat0, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"bitrate0", 	set_bitrate0, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"profilelevel0", set_profilelevel0, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"encoding1", set_encoding1, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"resolution1", set_resolution1, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"framerate1", set_framerate1, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"streamtype1", set_streamtype1, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"audioformat1", set_audioformat1, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"bitrate1", 	set_bitrate1, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"profilelevel1", set_profilelevel1, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"submitcodecfg", submitcodecfg, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	{"getencodecfg", get_encodeCfg, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	//	{"setencodecfg", set_encodeCfg, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	// image	
	{"setimage", set_image, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},	
	{"bright", set_imageBright, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"contrast", set_imageContrast, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"saturation", set_imageSaturation, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"sharpness", set_imageSharpness, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"submit_image", submit_image, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	{"getimage", get_image, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},


	// ptz
	{ "setptzchannel"  , setup_ptz_channel, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "setptzspeed"  , setup_ptz_speed, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "setptzpreset"  , setup_ptz_preset, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "setptzopera"	 , setup_ptz_opera, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	// cruise
	{ "setptzcruch"  , SetPtzCruCh, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "setptzcrulinenum"  , SetPtzCruLineNum, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "setptzcrulineen"  , SetPtzCruLineEn, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "setptzcrupreset0_7"  , SetPtzCruPreset0_7, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "setptzcrupreset8_15"  , SetPtzCruPreset8_15, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "setptzcrupreset16_23"  , SetPtzCruPreset16_23, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "setptzcrupreset24_31"  , SetPtzCruPreset24_31, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	{ "getptzcruch"  , GetPtzCruCh, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "getptzcrulinenum"  , GetPtzCruLineNum, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "getptzcrulineen"  , GetPtzCruLineEn, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "getptzcrupreset0_15"  , GetPtzCruPreset0_15, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "getptzcrupreset16_31"  , GetPtzCruPreset16_31, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	// all presets
	{ "setptzallprech"  , SetPtzAllPreCh, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "setptzallprenum"  , SetPtzAllPreNum, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "setptzallpreen"  , SetPtzAllPreEn, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "setptzallpreopera"  , SetPtzAllPreOpera, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	{ "getptzallprech"  , GetPtzAllPreCh, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "getptzallprenum"  , GetPtzAllPreNum, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "getptzallpreen"  , GetPtzAllPreEn, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	
	{ "searchcamera"	 , search_camera, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "getdisk"	 , get_disk, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "formatdisk"	 , format_disk, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "formatprocess"	 , format_process, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "upgradeprocess"	 , upgrade_process, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/*default cfg*/
	//{"setencodedefault",set_encodeDefault,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	//{"setdefaultptz",set_defaultPtz,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setdefaultrecord",set_defaultRecord,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setdefaultnet",set_defaultNet,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setdefaultchannel",set_defaultChannel,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	//{"setdefaultimageattr",set_defaultImageAttr,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	//{"setdefaultserial",set_defaultSerial,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	//{"setdafaultaverlay",set_defaultOverlay,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setdefaultmotion",set_defaultMotion,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setdefaultcommond",set_defaultCommond,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	//{"setdefaultosd",set_defaultOsd,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setdefaultuser",set_defaultUser,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setdefaultalarmin",set_defaultAlarmIn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setdefaultddns",set_defaultDdns,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setdefaultsmtp",set_defaultSmtp,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setdefaultfactory",set_defaultFactory,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	//{"setdefaulthide",set_defaultHide,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	//{"setdefaultvideolost",set_defaultVideoLost,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	//{"setdefaulttv",set_defaultTv,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	//{"setdefaultpreview",set_defaultPreview,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	//{"setdefaulttour",set_defaultTour,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"submitdefaultcfg",set_submitDefaultCfg,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	/*set preview cfg*/
	{"setpreviewchannum",set_previewChanNum,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setpreviewdisplaynum",set_previewDisplayNum,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setpreviewenablevicescreen",set_previewEnableViceScreen,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setpreviewpos",set_previewPos,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	/*get preview cfg*/
	{"getpreviewchannum",get_previewChanNum,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getpreviewdisplaynum",get_previewDisplayNum,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getpreviewenablevicescreen",get_previewEnableViceScreen,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getpreviewpos",get_previewPos,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	/*set record cfg*/
	{"setrecchannum",set_recChanNum,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},// 当前录像的通道号
	{"setrecweekday",set_weeday,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},	
	/* get rec cfg*/
	{"getrecchannum",get_recChanNum,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},// 当前录像的通道号
	{"getrecweekday",get_weekday,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	/*query update info*/
	{"queryupdatefilename",query_updateName,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"queryupdatefilepath",query_updateFilePath,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"queryupdatefilesize",query_updateFileSize,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"queryupdatefiledate",query_updateFileDate,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	/*get update progress*/
	{"getupdateprogress",get_updateProgress,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	/*close system */
	{"setsystemshutdown",set_systemShutDown,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setsystemreboot",set_systemReboot,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	/*set system time*/
	{"setsystemtimeyear",set_systemDateYear,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setsystemtimemonth",set_systemDateMonth,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setsystemtimeday",set_systemDateDay,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setsystemtimehour",set_systemDateHour,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setsystemtimeminutes",set_systemDateMinutes,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setsystemtimeseconds",set_systemDateSeconds,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	/*get system time*/
	{"getsystemtimeyear",get_systemDateYear,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getsystemtimemonth",get_systemDateMonth,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getsystemtimeday",get_systemDateDay,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getsystemtimehour",get_systemDateHour,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getsystemtimeminutes",get_systemDateMinutes,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getsystemtimeseconds",get_systemDateSeconds,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	/*query playback item*/
	{"pkchannel",play_backChannel,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"pkstarttime",play_backStartTime,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"pkendtime",play_backEndTime,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"pkfiletype",play_backFileType,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},


	/*setlogin*/
	{"setlogin", SetLogin,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setlogout", SetLogOut,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	/*usr*/
	{"setuser", SetUser,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	//-------------- MotionDetect,运动监测,add by he 2014.5.29
	// 从网页设置SET，将网页的数据设置到系统，并保存到数据库中
	{"setmotiondetchan",SetMotionDetChan,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setmotiondeten",SetMotionDetEn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setmotiondetbeepen",SetMotionDetBeepEn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setmotiondetbeeptime",SetMotionDetBeepTime,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setmotiondetrecorden",SetMotionDetRecordEn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setmotiondetrecordtime",SetMotionDetRecordTime,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setmotiondetsmtpen",SetMotionDetSmtpEn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setmotiondetftpen",SetMotionDetFtpEn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setmotiondetblock",SetMotionDetBlock,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setmotiondetsen",SetMotionDetSen,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	// 网页获取GET，将底层保存在数据库中的数据上传到网页
	{"getmotiondetchan",GetMotionDetChan,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getmotiondeten",GetMotionDetEn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getmotiondetbeepen",GetMotionDetBeepEn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getmotiondetbeeptime",GetMotionDetBeepTime,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getmotiondetrecorden",GetMotionDetRecordEn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getmotiondetrecordtime",GetMotionDetRecordTime,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getmotiondetsmtpen",GetMotionDetSmtpEn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getmotiondetftpen",GetMotionDetFtpEn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getmotiondetblock",GetMotionDetBlock,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getmotiondetsen",GetMotionDetSen,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	//-------------Log日志add by he 2014.6.20
	{"setlogtype",SetLogType,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},// 日志查询使能标识
	{"setlogbeginyear",SetLogBeginYear,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},// 起始时间
	{"setlogbeginmon",SetLogBeginMon,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setlogbeginday",SetLogBeginDay,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setlogbeginhour",SetLogBeginHour,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setlogbeginmin",SetLogBeginMin,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setlogbeginsec",SetLogBeginSec,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setlogendyear",SetLogEndYear,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},// 结束时间
	{"setlogendmon",SetLogEndMon,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setlogendday",SetLogEndDay,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setlogendhour",SetLogEndHour,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},	
	{"setlogendmin",SetLogEndMin,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setlogendsec",SetLogEndSec,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},	
	{"setquerylog",SetQueryLog,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},// 日志查询使能标识
	{"setdeletelog",SetDeleteLog,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},//日志删除标志
	//-------------NTP时间更新
	{"setntpserver",SetNtpServer,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setntptimezone",SetNtpTimeZone,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},	
	{"setntpcontrol",SetNtpcontrol,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setntinterval",SetNtpInterval,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setntpport",SetNtpPort,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setntpdateformat",SetNtpDateFormat,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setntpdateseparator",SetNtpDateSeparator,AUTHORITY_OPERATOR, FALSE,	TRUE, NULL},
	{"setntptimeformat",SetNtpTimeFormat,AUTHORITY_OPERATOR, FALSE,	TRUE, NULL},

	{"getntpserver",GetNtpServer,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getntptimezone",GetNtpTimeZone,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},	
	{"getntpcontrol",GetNtpcontrol,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getntinterval",GetNtpInterval,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getntpport",GetNtpPort,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getntpdateformat",GetNtpDateFormat,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getntpdateseparator",GetNtpDateSeparator,AUTHORITY_OPERATOR, FALSE,	TRUE, NULL},
	{"getntptimeformat",GetNtpTimeFormat,AUTHORITY_OPERATOR, FALSE, TRUE, NULL},

	//-------------FTP设置
	{"setftpenable",SetFtpEnable,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setftpservierip",SetFtpServierIp,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setftpusername",SetFtpUserName,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setftppassword",SetFtpPassword,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setftpfoldername",SetFtpFolderName,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setftpport",SetFtpPort,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	{"getftpenable",GetFtpEnable,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getftpservierip",GetFtpServierIp,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getftpusername",GetFtpUserName,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getftppassword",GetFtpPassword,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getftpfoldername",GetFtpFolderName,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getftpport",GetFtpPort,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},


	//-------------SMTP 邮件设置
	{"setsmtpen",SetSmtpEn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setsmtpserver",SetSmtpServer,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setsmtpuser",SetSmtpUser,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setsmtppasswd",SetSmtpPasswd,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setsmtpreceiver",SetSmtpReceiver,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setsmtpsender",SetSmtpSender,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setsmtptheme",SetSmtpTheme,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setsmtpanoen",SetSmtpAnoEn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setsmtpfileen",SetSmtpFileEn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setsmtpport",SetSmtpPort,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setsmtpenctype",SetSmtpEncType,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setsmtpinterval",SetSmtpInterval,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setsmtphealthen",SetSmtpHealthEn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setsmtphealthinterval",SetSmtpHealthInterval,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	{"getsmtpserver",GetSmtpServer,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getsmtpuser",GetSmtpUser,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getsmtppasswd",GetSmtpPasswd,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getsmtpreceiver",GetSmtpRreceiver,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getsmtpsender",GetSmtpSender,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getsmtptheme",GetSmtpTheme,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getsmtpanoen",GetSmtpAnoEn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getsmtpfileen",GetSmtpFileEn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getsmtpport",GetSmtpPort,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getsmtpenctype",GetSmtpEncType,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getsmtpinterval",GetSmtpInterval,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getsmtphealthen",GetSmtpHealthEn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getsmtphealthinterval",GetSmtpHealthInterval,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getsmtpen",GetSmtpEn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	//-----------DDNS配置 add by he 2014.7.7
	{"setddnsen",SetDdnsEn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setddnstype",SetDdnstype,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setddnsintertime",SetDdnsInterTime,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setddnsusertype",SetDdnsUserType,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setddnsdomainname",SetDdnsDomainName,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setddnsusername",SetDdnsUserName,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setddnsuserpwd",SetDdnsUserPwd,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	{"getddnsen",GetDdnsEn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getddnstype",GetDdnstype,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getddnsintertime",GetDdnsInterTime,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getddnsusertype",GetDdnsUserType,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getddnsdomainname",GetDdnsDomainName,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getddnsusername",GetDdnsUserName,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getddnsuserpwd",GetDdnsUserPwd,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	/*sn*/

	// -----------P2P配置  add by he 2014.10.13
	{"setfreeipen",SetFreeIpEn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setlongsesn",SetFreeIpSn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"serialnum",serialnum,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setdanaleen",SetDanaleEn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getfreeipen",GetFreeIpEn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getlongsesn",GetFreeIpSn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getfreeipandurl",GetFreeIpAndUrl,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getfreeipiosurl",GetFreeIpIosUrl,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getdanaleen",GetDanaleEn,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getserialnum",getserialnum,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getdanalandurl",GetDanaleAndUrl,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getdanaliosurl",GetDanaleIosUrl,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	/*自动维护*/
	{"setautomaintaininterval",SetAutoMaintainInterval,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"setautomaintainhour",SetAutoMaintainHour,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getautomaintaininterval",GetAutoMaintainInterval,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	{"getautomaintainhour",GetAutoMaintainHour,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	//  get p2p status
	{"getp2psconnecttatus",GetP2pConnectStatus,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},
	//test email status
	{"testemailstatus",TestEmailStatus,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	// ---------------TOTAL RATE-----------
	{"getrcvsendrate",GetRcvSendRate,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

      // ipc infomation
	{"getipcinfomation",Get_IPCInfomation,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL},

	//set up  ipc  network parameter
	{"setoldipaddr",Set_old_ip_addr,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL} ,
	{"setnewipaddr",Set_new_ip_addr,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL} ,
	{"setmask",Set_mask,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL} ,
	{"setmac",Set_mac,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL} ,
	{"setport",Set_port,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL} ,
	{"setusername",Set_user_name,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL} ,
	{"setuserpwd",Set_user_pwd,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL} ,
	{"setipcnetworkcfg",Set_IPC_network_CFG,AUTHORITY_OPERATOR, FALSE,  TRUE, NULL} 	
};


unsigned int hash_cal_value(char *name)
{
	unsigned int value = 0;

	while (*name)
		value = value * 37 + (unsigned int)(*name++);
	return value;
}


void hash_insert_entry(CMD_HASH_TABLE *table, HTTP_OPTION *op)
{
	if (table->entry) {
		op->next = table->entry;
	}
	table->entry = op;
}

CMD_HASH_TABLE *cmd_hash;
int hash_table_init(void)
{
	int i;

	if ( (cmd_hash = (CMD_HASH_TABLE *)calloc(sizeof(CMD_HASH_TABLE), MAX_CMD_HASH_SIZE)) == NULL) {
		return -1;
	}
	for (i=0; i<HASH_TABLE_SIZE; i++) {
		hash_insert_entry(cmd_hash+(hash_cal_value(HttpOptionTable[i].name)%MAX_CMD_HASH_SIZE), HttpOptionTable+i);
	}
	boa_dbg("HASH_TABLE_SIZE = %d\n", HASH_TABLE_SIZE);
	return 0;
}

HTTP_OPTION *http_option_search(char *arg)
{
	HTTP_OPTION *opt;

	opt = cmd_hash[hash_cal_value(arg)%MAX_CMD_HASH_SIZE].entry;

	while (opt) {
		if ( strcmp(opt->name, arg) == 0 )
			return opt;
		opt = opt->next;
	}
	return NULL;
}

void http_run_command(request *req, COMMAND_ARGUMENT *arg, int num)
{
	AUTHORITY authority = req->authority;
	HTTP_OPTION *option;
	int i;

	send_request_ok_api(req);     /* All's well */
	boa_dbg("argument count = %d\n", num);
	for (i=0; i<num; i++) {
		strtolower((unsigned char *)arg[i].name);  // convert the command argument to lowcase
#ifdef DEBUG
		boa_dbg("arg[%d].name=%s", i, arg[i].name);
		if (arg[i].value[0])
			boa_dbg(", value=%s\n", arg[i].value);
		else
			boa_dbg("\n");
#endif

		option = http_option_search(arg[i].name);
		if (option) {
			if ((authority <= option->authority || !strcmp(arg[i].name, "getweblanguage")) && gf_CmdBlocking == 0) {			
				arg[i].flags = 0;
				(*option->handler) (req, &arg[i]);
			}
			else {
				req_write(req, OPTION_UA);
				req_write(req, arg[i].name);
				boa_dbg("http_run_command: Permission denied!!!\n");
			}
		}
		else {
			req_write(req, OPTION_UW);
			req_write(req, arg[i].name);
		}
	}
	if ((gf_CmdBlocking = gf_Restarting) > 0)
		boa_dbg("+++ Command Block +++++++++++\n");
}

extern int ShowAllWebValue(char *data, int max_size, AUTHORITY authority);
extern int ShowAllPara(char *data, int max_size, AUTHORITY authority);
int html_ini_cmd(AUTHORITY authority, char *addr, int max_size)
{
	int ret;
	ret = ShowAllWebValue(addr, max_size, authority);
	return ret;
}

int html_keyword_cmd(AUTHORITY authority, char *addr, int max_size)
{
	int ret;
	ret = ShowAllPara(addr, max_size, authority);
	return ret;
}

int html_cmdlist_cmd(AUTHORITY authority, char *addr, int max_size)
{
	int ret = 0,i,count = 0,size;
	HTTP_OPTION *option;
	char buf[80];
	for (i=0; i<HASH_TABLE_SIZE; i++) {
		option = &HttpOptionTable[i];
		if (authority > option->authority)
			continue;
		size = sprintf(buf, "%03d.%-25s%d\n", ++count, option->name, option->authority);
		if(ret + size + 1 > max_size){
			ret = sprintf(addr, "Not enogh size to show");
			break;
		}
		ret += sprintf(addr + ret, "%s", buf);
	}
	return ret;
}
#define MMC_NODE "/dev/mmcblk0p1"
#define MMC_PATH "/mnt/mmc/ipnc/"

int http_sdget_cmd(request *req, COMMAND_ARGUMENT *argm, char *addr, int max_size, char *sdpath)
{
#if 0
	int ret = 0;

	if (argm->value != "\0")
	{
		if( CheckFileExist(MMC_PATH,argm->value) == 0 )
		{
			strcpy(sdpath, MMC_PATH);
			return 0;
		}
	}

	MEM_List_files_To_html( MMC_PATH, MMC_PATH, addr, max_size);
	ret = strlen(addr);

	return ret;
#endif
	return 0;
}

extern char html_sdget_basic_hdr[];

void http_sddel_cmd(request *req, COMMAND_ARGUMENT *argm)
{
#if 0
	char http_ip_addr[100];

	GetIP_Addr(http_ip_addr);

	send_request_ok_api(req);

	if (strcmp(argm->name, "FILE") == 0)
	{
		if ( DeleteFile(MMC_PATH,argm->value) == 0 ) {
			req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		}
		else {
			req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
		}
		req->buffer_end += sprintf(req_bufptr(req), "<HR>HTTP Server at <A HREF=\"http://%s\">ipnc</A><BR></PRE></BODY></HTML> %s ", http_ip_addr, html_sdget_basic_hdr);
	}
	else {
		req_write(req, OPTION_UW);
		req_write(req, argm->name);
	}
#endif
}

int http_sysget_cmd(char *addr, int max_size)
{
	int ret = 0;
	//SysLog_List_To_html(addr, max_size);
	ret = strlen(addr);
	return ret;
}

int http_accessget_cmd(char *addr, int max_size)
{
	int ret = 0;
	//AccessLog_List_To_html(addr, max_size);
	ret = strlen(addr);
	return ret;
}

int http_actvconlistget_cmd(char *addr, int max_size)
{
	int ret = 0;
	ActvCon_List_To_html(addr, max_size);
	ret = strlen(addr);
	return ret;
}

int http_paraget_cmd(char *addr, int max_size)
{
	// int ret = 0;
	//Curr_Param_List_To_html(addr, max_size);
	// ret = strlen(addr);
	//return ret;
	return 0;
}

