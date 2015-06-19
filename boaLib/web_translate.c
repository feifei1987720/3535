
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#include "file_list.h"
#include "web_translate.h"
#include "sys_env_type.h"
#include "net_config.h"
#include "sdk_struct.h"
#include "main.h"
#include "databaseLib.h"
#include "commonLib.h"
#include "appLib.h"


#define REL_UNIQUE_ID (026000)

 static sdk_eth_cfg_t  g_EthInfo;
static sdk_net_mng_cfg_t g_NetManageInfo={0};
//static sdk_channel_t        g_channelInfo[MAX_CHANNEL_NUM];
static sdk_channel_t        g_channelInfo;
static sdk_sys_cfg_t	g_sysInfo;
//static sdk_record_cfg_t  g_recordInfo;
//static sdk_preview_t	g_previewInfo;
static sdk_comm_cfg_t	g_commInfo;
//static sdk_disk_t		g_diskInfo;
//static sdk_user_right_t	g_userRightInfo;
static sdk_version_t		g_versionInfo;

ARG_HASH_TABLE *arg_hash;

//extern  HiBoaMutex;

int readAllSystemdb(void)
{	
	//pthread_mutex_lock(&HiBoaMutex);
	//databaseOperate(int type, int subType, void *data, int len, void *pRecvData, int recvDataLen)
	//databaseOperate(SDK_MAIN_MSG_SYS_CFG,0x02,NULL,0,&g_sysInfo,sizeof(g_sysInfo));
	int ret=0;
	int channel =1;

	ret=databaseOperate(SDK_MAIN_MSG_NET_MNG_CFG,0x02,&channel,sizeof(int),&g_NetManageInfo,sizeof(g_NetManageInfo));
	if(ret<0)
	{
		Printf("databaseOperate fail \n");
	}
	
	databaseOperate(SDK_MAIN_MSG_NET_LINK_CFG,0x02,NULL,0,&g_EthInfo,sizeof(g_EthInfo));

	databaseOperate(SDK_MAIN_MSG_COMM_CFG,0x02,NULL,0,&g_commInfo,sizeof(g_commInfo));
	
	databaseOperate(SDK_MAIN_MSG_CHANN_CFG,0x02,&channel,sizeof(int),&g_channelInfo,sizeof(g_channelInfo));

	databaseOperate(SDK_MAIN_MSG_SYS_CFG,0x02,NULL,0,&g_sysInfo,sizeof(g_sysInfo));

		//int cmdOperate(int type, int subType, void *data, int len, void *pRecvData, int recvDataLen)
	cmdOperate(SDK_MAIN_MSG_VERSION, 0, NULL, 0,&g_versionInfo ,sizeof(g_versionInfo));
	//pthread_mutex_unlock(&HiBoaMutex);
	return  0;
}



/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_netIp(char *data, char *arg)
{
	//fprintf(stderr,"  %s :%d\n",__FILE__,__LINE__);
	return sprintf(data, "%s",g_EthInfo.ip_info.ip_addr);
}

int para_netMask(char *data, char *arg)
{
	//fprintf(stderr,"  %s :%d\n",__FILE__,__LINE__);
	return sprintf(data, "%s",g_EthInfo.ip_info.mask);
}

#if 1

int para_netGetWay(char *data, char *arg)
{
	//fprintf(stderr,"%s   %s :%d\n",g_EthInfo.ip_info.gateway,__FILE__,__LINE__);
	return sprintf(data, "%s",g_EthInfo.ip_info.gateway);
}

int para_dns1(char *data, char *arg)
{
	//fprintf(stderr,"%s    %s :%d\n",g_EthInfo.ip_info.dns1,__FILE__,__LINE__);
	return sprintf(data, "%s",g_EthInfo.ip_info.dns1);
}

int para_dns2(char *data, char *arg)
{
	//fprintf(stderr,"%s      %s :%d\n",g_EthInfo.ip_info.dns2,__FILE__,__LINE__);
	return sprintf(data, "%s",g_EthInfo.ip_info.dns2);
}

int para_mac(char *data, char *arg)
{
	//fprintf(stderr,"%s    %s :%d\n",g_EthInfo.ip_info.mac,__FILE__,__LINE__);
	return sprintf(data, "%s",g_EthInfo.ip_info.mac);
}

int para_dhcp(char *data, char *arg)
{
	//fprintf(stderr,"%d    %s :%d\n",g_EthInfo.ip_info.enable_dhcp,__FILE__,__LINE__);
	return sprintf(data, "%d",g_EthInfo.ip_info.enable_dhcp);
}

int para_p2pEnable(char *data, char *arg)
{
	//fprintf(stderr,"%d   %s :%d\n",g_EthInfo.ip_info.dns_auto_en,__FILE__,__LINE__);
	return sprintf(data, "%d",g_EthInfo.ip_info.enable_p2p);
	/**/
}



int para_httpPort(char *data, char *arg)
{
	//fprintf(stderr,"%d    %s :%d\n",g_NetManageInfo.http_port,__FILE__,__LINE__);
	return sprintf(data, "%d",g_NetManageInfo.http_port);
}

int para_rtspServerPort(char *data, char *arg)
{
	//fprintf(stderr,"%d      %s :%d\n",g_NetManageInfo.dvr_data_port,__FILE__,__LINE__);
	return sprintf(data, "%d",g_NetManageInfo.dvr_data_port);
}


int para_tcpPort(char *data, char *arg)
{
	//fprintf(stderr,"%d    %s :%d\n",g_NetManageInfo.dvr_cmd_port,__FILE__,__LINE__);
	return sprintf(data, "%d",g_NetManageInfo.dvr_cmd_port);
}

int para_multiCast(char *data, char *arg)
{
	//fprintf(stderr,"%s     %s :%d\n",g_NetManageInfo.multicast,__FILE__,__LINE__);
	return sprintf(data, "%s",g_NetManageInfo.multicast);
	/**/
}


/**/
int para_chanEnable(char *data, char *arg)
{
	//fprintf(stderr," %d      %s :%d\n",g_channelInfo.enable,__FILE__,__LINE__);
	return sprintf(data, "%d",g_channelInfo.enable);
}

int para_chanName(char *data, char *arg)
{
	//fprintf(stderr," %s    %s :%d\n",g_channelInfo.name,__FILE__,__LINE__);
	return sprintf(data, "%s",g_channelInfo.name);
}

int para_channelNo(char *data, char *arg)
{
	//fprintf(stderr," %d   %s    :%d\n",g_channelInfo.chann_no,__FILE__,__LINE__);
	return sprintf(data, "%d",g_channelInfo.chann_no);
}

int para_chanStreamNo(char *data, char *arg)
{
	//fprintf(stderr," %d      %s :%d\n",g_channelInfo.chann_win_mode,__FILE__,__LINE__);
	return sprintf(data, "%d",g_channelInfo.chann_win_mode);
}

int para_chanDeviceIpAddr(char *data, char *arg)
{
	//fprintf(stderr,"%s     %s :%d\n",g_channelInfo.device.ip_addr,__FILE__,__LINE__);
	return sprintf(data, "%s",g_channelInfo.ip_addr);
}

int para_chanDevicePort(char *data, char *arg)
{
	//fprintf(stderr,"%d    %s :%d\n",g_channelInfo.device.port,__FILE__,__LINE__);
	return sprintf(data, "%d",g_channelInfo.port);
}

int para_chanDevideUserName(char *data, char *arg)
{
	//fprintf(stderr," %s      %s :%d\n",g_channelInfo.user.user_name,__FILE__,__LINE__);
	return sprintf(data, "%s",g_channelInfo.user.user_name);
}

int para_chanDeviceUserPwd(char *data, char *arg)
{
	//fprintf(stderr,"  %s    %s :%d\n",g_channelInfo.user.user_pwd,__FILE__,__LINE__);
	return sprintf(data, "%s",g_channelInfo.user.user_pwd);
}

int para_chanDeviceProtocol(char *data, char *arg)
{
	//fprintf(stderr,"  %d     %s :%d\n",g_channelInfo.chann_protocol,__FILE__,__LINE__);
	return sprintf(data, "%d",g_channelInfo.chann_protocol);

	/**/
}

/**/
int para_language(char *data, char *arg)
{
	//fprintf(stderr," %d   %s :%d\n",g_commInfo.language,__FILE__,__LINE__);
	return sprintf(data, "%d",g_commInfo.language);
}

int para_recycleRecord(char *data, char *arg)
{
	//fprintf(stderr," %d    %s :%d\n",g_commInfo.recycle_record,__FILE__,__LINE__);
	return sprintf(data, "%d",g_commInfo.recycle_record);
}

int para_DevName(char *data, char *arg)
{
	//fprintf(stderr," %d      %s :%d\n",g_commInfo.dvr_id,__FILE__,__LINE__);
	return sprintf(data, "%s",g_commInfo.dvr_name);
}

int para_drvId(char *data, char *arg)
{
	//fprintf(stderr," %d      %s :%d\n",g_commInfo.dvr_id,__FILE__,__LINE__);
	return sprintf(data, "%d",g_commInfo.dvr_id);
}

int para_outDevice(char *data, char *arg)
{
	//fprintf(stderr," %d      %s :%d\n",g_commInfo.out_device,__FILE__,__LINE__);
	return sprintf(data, "%d",g_commInfo.out_device);
}

int para_videoMode(char *data, char *arg)
{
	//fprintf(stderr,"  %d     %s :%d\n",g_commInfo.video_mode,__FILE__,__LINE__);
	return sprintf(data, "%d",g_commInfo.video_mode);
}

int para_resolution(char *data, char *arg)
{
	//fprintf(stderr," %d   %s :%d\n",g_commInfo.resolution,__FILE__,__LINE__);
	return sprintf(data, "%d",g_commInfo.resolution);
}

int para_standbyTime(char *data, char *arg)
{
	//fprintf(stderr,"  %d      %s :%d\n",g_commInfo.standby_time,__FILE__,__LINE__);
	return sprintf(data, "%d",g_commInfo.standby_time);
}

#endif

/*get system info*/
int para_devType(char *data, char *arg)
{
	//fprintf(stderr,"  %d      %s :%d\n",g_sysInfo.dev_type,__FILE__,__LINE__);
	return sprintf(data, "%d",g_sysInfo.dev_type);
}

int para_sysChanNum(char *data, char *arg)
{
	//fprintf(stderr,"  %d      %s :%d\n",g_sysInfo.ip_chan_num,__FILE__,__LINE__);
	return sprintf(data, "%d",g_sysInfo.ip_chan_num);
}

/*get version info*/
int para_deviceType(char *data, char *arg)
{
	//fprintf(stderr,"  %s     %s :%d\n",g_versionInfo.device_type,__FILE__,__LINE__);
	return sprintf(data, "%s",g_versionInfo.device_type);
}

int para_softwareVersion(char *data, char *arg)
{
	//fprintf(stderr,"  %s      %s :%d\n",g_versionInfo.software_ver,__FILE__,__LINE__);
	return sprintf(data, "%s",g_versionInfo.software_ver);
}

int para_softwareDate(char *data, char *arg)
{
	//fprintf(stderr,"  %s      %s :%d\n",g_versionInfo.software_date,__FILE__,__LINE__);
	return sprintf(data, "%s",g_versionInfo.software_date);
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
#define HASH_TABLE_SIZE	(sizeof(HttpArgument)/sizeof(HTML_ARGUMENT))

HTML_ARGUMENT HttpArgument [] =
{
	


	
	
	{"netip",para_netIp,AUTHORITY_VIEWER, NULL},
	{"netmask",para_netMask,AUTHORITY_VIEWER, NULL},
	#if 1
	{"gateway",para_netGetWay,AUTHORITY_OPERATOR,  NULL},
	{"dns1",para_dns1,AUTHORITY_VIEWER,  NULL},
	{"dns2",para_dns2,AUTHORITY_VIEWER,  NULL},
	{"mac",para_mac,AUTHORITY_VIEWER,  NULL},
	{"dhcpenable",para_dhcp,AUTHORITY_VIEWER, NULL},
	{"p2penable",para_p2pEnable,AUTHORITY_VIEWER, NULL},

	/*net manage */
	{"httpport",para_httpPort,AUTHORITY_VIEWER,  NULL},
	{"rtspserverport",para_rtspServerPort,AUTHORITY_VIEWER,  NULL},
	{"tcpport",para_tcpPort,AUTHORITY_VIEWER, NULL},
	{"multicast",para_multiCast,AUTHORITY_VIEWER, NULL},

	#if 0
	/*chan cfs*/
	{"chanenable",para_chanEnable,AUTHORITY_VIEWER, NULL},
	{"channame",para_chanName,AUTHORITY_VIEWER,NULL},
	{"channo",para_channelNo,AUTHORITY_VIEWER,NULL},
	{"chanstream",para_chanStreamNo,AUTHORITY_VIEWER,  NULL},
	{"chandeviceip",para_chanDeviceIpAddr,AUTHORITY_VIEWER, NULL},
	{"chandeviceport",para_chanDevicePort,AUTHORITY_VIEWER, NULL},
	{"chanusername",para_chanDevideUserName,AUTHORITY_VIEWER, NULL},
	{"chanuserpwd",para_chanDeviceUserPwd,AUTHORITY_VIEWER, NULL},
	{"chandeviceprotocol",para_chanDeviceProtocol,AUTHORITY_VIEWER, NULL},
	#endif

	/*general cfg*/
	{"getlanguage",para_language,AUTHORITY_VIEWER,  NULL},
	{"getrecyclerecord",para_recycleRecord,AUTHORITY_VIEWER,NULL},
	{"getdrvid",para_drvId,AUTHORITY_VIEWER,NULL},
	{"getoutdevice",para_outDevice,AUTHORITY_VIEWER, NULL},
	{"getvideomode",para_videoMode,AUTHORITY_VIEWER,NULL},
	{"getresolution",para_resolution,AUTHORITY_VIEWER, NULL},
	{"getdevname",para_DevName,AUTHORITY_VIEWER, NULL},
	{"getstandbytime",para_standbyTime,AUTHORITY_VIEWER, NULL},
	#endif

	/*system info*/
	{"getsysdevtype",para_devType,AUTHORITY_VIEWER, NULL},
	{"getsyschannum",para_sysChanNum,AUTHORITY_VIEWER, NULL},

	/*version info*/
	{"getdevicetype",para_deviceType,AUTHORITY_VIEWER,NULL},
	{"getsoftwareversion",para_softwareVersion,AUTHORITY_VIEWER, NULL},
	{"getsoftwaredate",para_softwareDate,AUTHORITY_VIEWER, NULL},
	

};

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
unsigned int arg_hash_cal_value(char *name)
{
	unsigned int value = 0;

	while (*name)
		value = value * 37 + (unsigned int)(*name++);
	return value;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void arg_hash_insert_entry(ARG_HASH_TABLE *table, HTML_ARGUMENT *arg)
{
	if (table->harg) {
		arg->next = table->harg;
	}
	table->harg = arg;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int arg_hash_table_init(void)
{
	int i;

	if ( (arg_hash = (ARG_HASH_TABLE *)calloc(sizeof(ARG_HASH_TABLE), MAX_ARG_HASH_SIZE)) == NULL) {
		return -1;
	}
	for (i=0; i<HASH_TABLE_SIZE; i++) {
		arg_hash_insert_entry(arg_hash+(arg_hash_cal_value(HttpArgument[i].name)%MAX_ARG_HASH_SIZE), HttpArgument+i);
	}
	return 0;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int TranslateWebPara(AUTHORITY authority, char *target, char *para, char *subpara)
{
	HTML_ARGUMENT *htmp;
	htmp = arg_hash[arg_hash_cal_value(para)%MAX_ARG_HASH_SIZE].harg;

	while (htmp) {
		if ( strcasecmp(htmp->name, para) == 0 ) {
			if (authority > htmp->authority) {
				boa_dbg("[%s.%s] permission denied!!!\n", para, subpara);
				return -1;
			}
			return (*htmp->handler) (target, subpara);
		}
		htmp = htmp->next;
	}
	boa_dbg("[%s.%s] not found\n", para, subpara);
	return -1;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/

void arg_hash_table_cleanup()
{
	free(arg_hash);
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/

int ShowAllWebValue(char *data, int max_size, AUTHORITY authority)
{
	HTML_ARGUMENT *htmp;
	int i, total_size = 0, size;
	char buf[1024];
	readAllSystemdb();
	for(i = 0;i < HASH_TABLE_SIZE; i++){
		htmp = &HttpArgument[i];
		if (authority > htmp->authority)
			continue;
		if(htmp ->handler == NULL)
			continue;
		size = sprintf(buf, "%s=", htmp->name);
		if(total_size + size + 1 > max_size){
			total_size = sprintf(data, "Not enogh size to show");
			break;
		}
		total_size += sprintf(data+total_size, "%s", buf);
		size = (*htmp->handler) (buf, "");
		if(size < 0){
			size = sprintf(buf, "Error return");
		}
		if(total_size + size + 1 > max_size){
			total_size = sprintf(data, "Not enogh size to show");
			break;
		}
		total_size += sprintf(data+total_size, "%s<br>", buf);
	}
	return total_size;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/

static int ShowPara(char* buf, int index, char* name, AUTHORITY authority)
{
	char strAuthority[5][20] = {"ADMINISTRATOR","OPERATOR","VIEWER","NONE","ERROR"};
	int a_index;
	switch(authority){
		case AUTHORITY_ADMIN:
			a_index = 0;
			break;
		case AUTHORITY_OPERATOR:
			a_index = 1;
			break;
		case AUTHORITY_VIEWER:
			a_index = 2;
			break;
		case AUTHORITY_NONE:
			a_index = 3;
			break;
		default:
			a_index = 4;
			break;
	}
	return sprintf(buf, "%3d.%-25s%s\n", index, name, strAuthority[a_index]);
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/

int ShowAllPara(char *data, int max_size, AUTHORITY authority)
{
	HTML_ARGUMENT *htmp;
	int i, total_size = 0, size,count = 0;
	char buf[1024];
	for(i = 0;i < HASH_TABLE_SIZE; i++){
		htmp = &HttpArgument[i];
		if(htmp ->handler == NULL)
			continue;
		size = ShowPara(buf, ++count, htmp->name, htmp->authority);
		if(total_size + size + 1 > max_size){
			total_size = sprintf(data, "Not enogh size to show");
			break;
		}
		total_size += sprintf(data+total_size, "%s", buf);
	}
	return total_size;
}

