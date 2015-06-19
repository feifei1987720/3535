
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "global.h"
#include "param.h"
#include "guiServerLib.h"
#include "sdk_struct.h"
#include "main.h"
#include "appFunTable.h"
#include "commonLib.h"
#include "hi3531_rtcLib.h"
#include "decodeLib.h"
#include "playbackLib.h"
#include "databaseLib.h"
#include "DbSystem.h"
#include "onvif_client.h"

#include "sqlite3.h"

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "diskLib.h"
#include "appLib.h"
#include "configLib.h"
#include "midWare.h"
#include "ntp.h"
#include <pthread.h>
#include "ping.h"
#include "discovery.h"

#define CHANN_TYPE_ALL		0xFA

#define MAX_LOG 256
#define LOG_FILE "/config/log.txt"

static pthread_mutex_t g_discoveryMutex = PTHREAD_MUTEX_INITIALIZER ;

static int _msg_dispatch(sdk_msg_t *pmsg);

static int devNum = 0;
static sdk_device_t onvifDev[256];
int privatedevNum ;
sdk_device_t privateDev[256];

typedef struct _alldisplayDev_
{
	char ip_addr[MAX_IP_ADDR_LEN];
}alldisplayDev;

static  alldisplayDev  allDev[32]; 



NetConfigResponse g_netConfigResponse;
void onvifDeviceInfo(char *pDeviceIp, unsigned short devicePort, char *pDeviceUuid,int pIndex)
{
	//fprintf(stderr,"pDeviceIp = (%s), devicePort %d, pDeviceUuid = (%s)\r\n", pDeviceIp, devicePort, pDeviceUuid);
	strcpy((char *)onvifDev[devNum].ip_addr, pDeviceIp);
	onvifDev[devNum].port = devicePort;
	onvifDev[devNum].device_type = 0;
	onvifDev[devNum].manufacturer = MANUFACT_ONVIF;
	strcpy((char *)onvifDev[devNum].user.user_name, "admin");
	strcpy((char *)onvifDev[devNum].user.user_pwd, "admin");
	onvifDev[devNum].index = pIndex;
	devNum++;
}

int SearchPrivateDevInfo(int *num, sdk_device_t **ptr)
{
	memset(&privateDev, 0, sizeof(privateDev));
	privatedevNum = 0;
	if(DD_SearchDev()!=0)
	{
		Printf("start private discovery error\r\n");
	}
	sleep(3);
	pthread_mutex_lock(&g_discoveryMutex);
	*num = privatedevNum;
	*ptr = privateDev;
	pthread_mutex_unlock(&g_discoveryMutex);

	return 0;
}



int main_msg_version(sdk_msg_t *pmsg)
{
	int ret = 0;

	ret = cmdOperate(pmsg->msg_id, pmsg->args, NULL, 0, pmsg->data, pmsg->size);
	pmsg->size = ret;

	return 0;
}

int main_msg_param_def(sdk_msg_t *pmsg)
{
	int ret = databaseOperate(pmsg->msg_id, pmsg->args, &pmsg->chann, sizeof(pmsg->chann), pmsg->data, pmsg->size);
	if(ret == -1)
	{
		pmsg->size = 0;
		pmsg->ack = -1;
	}
	else
	{
		pmsg->size = ret;
	}

	return 0;
}

int main_msg_upgrad(sdk_msg_t *pmsg)
{
	int ret = 0;

	ret = cmdOperate(pmsg->msg_id, pmsg->args, &pmsg->chann, sizeof(pmsg->chann), pmsg->data, pmsg->size);
	pmsg->extend = pmsg->chann;
	pmsg->size = ret;

	return 0;
}

int main_msg_user(sdk_msg_t *pmsg)
{
	int ret = databaseOperate(pmsg->msg_id, pmsg->args, pmsg->data, pmsg->size, pmsg->data, pmsg->size);
	if(ret == -1)
	{
		pmsg->size = 0;
		pmsg->ack = -1;
	}
	else
	{
		pmsg->size = ret;
	}

	return 0;
}

int main_msg_sys_cfg(sdk_msg_t *pmsg)
{
	int ret = databaseOperate(pmsg->msg_id, pmsg->args, pmsg->data, pmsg->size, pmsg->data, pmsg->size);
	if(ret == -1)
	{
		pmsg->size = 0;
		pmsg->ack = -1;
	}
	else
	{
		pmsg->size = ret;
	}

	return 0;
}

static int main_msg_net_link_cfg( sdk_msg_t *pmsg )
{
	int ret = databaseOperate(SDK_MAIN_MSG_NET_LINK_CFG, pmsg->args, &pmsg->chann, sizeof(pmsg->chann), pmsg->data, pmsg->size);
	if(ret == -1)
	{
		pmsg->size = 0;
		pmsg->ack = -1;
	}
	else
	{
		pmsg->size = ret;
	}

	return 0;
}

static int main_msg_record_backup(sdk_msg_t *pmsg)
{
	int ret = 0;
	if(pmsg->args == SDK_BACKUP_START)
	{
		ret = playbackStartBackupRecord((sdk_record_backup_t *)pmsg->data);
	}
	else if(pmsg->args == SDK_BACKUP_PROGRESS)
	{
		pmsg->chann = playbackGetProcess();
	}
	else
	{
		Printf("not support backup cmd\r\n");
	}

	pmsg->size = 0;

	return ret;
}

static int main_msg_close_system( sdk_msg_t *pmsg )
{
	int ret = 0;

	ret = cmdOperate(pmsg->msg_id, pmsg->args, NULL, 0, pmsg->data, pmsg->size);
	pmsg->size = ret;

	return 0;
}

#if 0
static sdk_msg_t g_NetMngPar;
static void * NetMngDefaultThread(void *para)
{

	pthread_detach(pthread_self());
	sdk_msg_t *pmsg = (sdk_msg_t *)para;
	int ret = databaseOperate(SDK_MAIN_MSG_NET_MNG_CFG, pmsg->args, &pmsg->chann, sizeof(pmsg->chann), pmsg->data, pmsg->size);
	if(ret == -1)
	{
		pmsg->size = 0;
		pmsg->ack = -1;
	}
	else
	{
		pmsg->size = ret;
	}
	return NULL;
}

static void CreateNetMngThread(void *para )
{
	pthread_t NetMngID;
	if(pthread_create(&NetMngID, NULL, (void *)NetMngDefaultThread, para) < 0)
	{
		fprintf(stderr,"[%s:%s:%d]:create reset netport thread failed \n",__FILE__,__FUNCTION__,__LINE__);
	}
}
#endif

int main_msg_net_mng_cfg( sdk_msg_t *pmsg )
{
	if(pmsg->args == SDK_PARAM_GET)
	{
		int ret = databaseOperate(SDK_MAIN_MSG_NET_MNG_CFG, pmsg->args, &pmsg->chann, sizeof(pmsg->chann), pmsg->data, pmsg->size);
		if(ret == -1)
		{
			pmsg->size = 0;
			pmsg->ack = -1;
		}
		else
		{
			pmsg->size = ret;
		}
	}
	else if(pmsg->args == SDK_PARAM_SET)// UI设置端口的时候有可能导致数据库读写失败。故加入线程处理。
	{
		//CreateNetMngThread(pmsg);
		//usleep(500*1000);
		//sdk_msg_t *pmsg = (sdk_msg_t *)para;
		int ret = databaseOperate(SDK_MAIN_MSG_NET_MNG_CFG, pmsg->args, &pmsg->chann, sizeof(pmsg->chann), pmsg->data, pmsg->size);
		if(ret == -1)
		{
			pmsg->size = 0;
			pmsg->ack = -1;
		}
		else
		{
			pmsg->size = ret;
		}
	}
	return 0;
}

static int main_msg_record_cfg( sdk_msg_t *pmsg )
{
	int ret = 0;

	ret = databaseOperate(pmsg->msg_id, pmsg->args, &pmsg->chann, sizeof(pmsg->chann), pmsg->data, pmsg->size);
	if(ret == -1)
	{
		pmsg->size = 0;
		pmsg->ack = -1;
	}
	else
	{
		pmsg->size = ret;
	}

	return 0;
}

static int main_msg_motion_cfg(sdk_msg_t *pmsg)
{
	int ret = 0;

	ret = databaseOperate(pmsg->msg_id, pmsg->args, &pmsg->chann, sizeof(pmsg->chann), pmsg->data, pmsg->size);
	if(ret == -1)
	{
		pmsg->size = 0;
		pmsg->ack = -1;
	}
	else
	{
		pmsg->size = ret;
	}

	return 0;
}


static int main_msg_disk(sdk_msg_t *pmsg )
{
	int opera = pmsg->args;

	int ret = 0;

	if (SDK_DISK_QUERY == opera )
	{	
		ret = DiskQuery(pmsg->data);
		pmsg->size = ret;
		return 0;
	} 

	ret = cmdOperate(SDK_MAIN_MSG_DISK, pmsg->args, &pmsg->chann, sizeof(pmsg->chann), pmsg->data, pmsg->size);
	pmsg->size = 0;

	return 0;

}

static int parseResolution(const int width, const int height)
{

	if ( 176 == width && 144 == height ) {
		return SDK_VIDEO_RESOLUTION_QCIF;

	} else if (  (352 == width && 288 == height) || (320 == width && 240 == height)  ) {
		return SDK_VIDEO_RESOLUTION_CIF;

	} else if ( 704 == width && 288 == height ) {
		return SDK_VIDEO_RESOLUTION_HD1;

	} else if (  (704 == width && 576 == height) || (720 == width && 576 == height) || (720 == width && 480 == height)) {
		return SDK_VIDEO_RESOLUTION_D1;

	} else if ( 960 == width && 576 == height ) {
		return SDK_VIDEO_RESOLUTION_960H;

	} else if ( 320 == width && 240 == height ) {
		return SDK_VIDEO_RESOLUTION_QVGA;

	} else if ( 640 == width && 480 == height ) {
		return SDK_VIDEO_RESOLUTION_VGA;

	} else if ( 1024 == width && 768 == height ) {
		return SDK_VIDEO_RESOLUTION_XGA;

	} else if ( 1400 == width && 1050 == height ) {
		return SDK_VIDEO_RESOLUTION_SXGA;

	} else if ( 704 == width && 288 == height ) {
		return SDK_VIDEO_RESOLUTION_UXGA;

	} else if ( 1600 == width && 1536 == height ) {
		return SDK_VIDEO_RESOLUTION_QXGA;

	} else if ( 854 == width && 480 == height ) {
		return SDK_VIDEO_RESOLUTION_WVGA;

	} else if ( 1680 == width && 1050 == height ) {
		return SDK_VIDEO_RESOLUTION_WSXGA;

	} else if ( 1920 == width && 1200 == height ) {
		return SDK_VIDEO_RESOLUTION_WUXGA;

	} else if ( 704 == width && 288 == height ) {
		return SDK_VIDEO_RESOLUTION_WQXGA;

	} else if ( 1280 == width && 720 == height ) {
		return SDK_VIDEO_RESOLUTION_720p;
	}
	else if ( 1280 == width && 1024 == height ) {
		return SDK_VIDEO_RESOLUTION_1024p;

	} else if ( 1920 == width && 1080 == height ) {
		return SDK_VIDEO_RESOLUTION_1080p;

	} else if ( 1280 == width && 960 == height ) {
		return SDK_VIDEO_RESOLUTION_960p;

	} else if ( 800 == width && 600 == height ) {
		return SDK_VIDEO_RESOLUTION_SVGA;

	}

	return SDK_VIDEO_RESOLUTION_BUTT;

}


static int GetResolution(int index, int *width, int *height)
{

	if ( SDK_VIDEO_RESOLUTION_QCIF == index ) {
		*width = 176;
		*height = 144;

	} else if ( SDK_VIDEO_RESOLUTION_CIF == index ) {
		*width = 352;
		*height = 288;

	} else if ( SDK_VIDEO_RESOLUTION_HD1 == index ) {
		*width = 704;
		*height = 288;

	} else if ( SDK_VIDEO_RESOLUTION_D1 == index ) {
		*width = 704;
		*height = 576;

	} else if ( SDK_VIDEO_RESOLUTION_960H == index ) {
		*width = 960;
		*height = 576;

	} else if ( SDK_VIDEO_RESOLUTION_QVGA == index ) {
		*width = 320;
		*height = 240;

	} else if ( SDK_VIDEO_RESOLUTION_VGA == index ) {
		*width = 640;
		*height = 480;

	} else if ( SDK_VIDEO_RESOLUTION_XGA == index ) {	
		*width = 1024;
		*height = 768;

	} else if ( SDK_VIDEO_RESOLUTION_SXGA == index ) {
		*width = 1400;
		*height = 1050;    

	} else if ( SDK_VIDEO_RESOLUTION_UXGA  == index ) {
		*width = 1600;
		*height = 1200;

	} else if ( SDK_VIDEO_RESOLUTION_QXGA == index ) {
		*width = 2048;
		*height = 1536;

	} else if ( SDK_VIDEO_RESOLUTION_WVGA == index ) {
		*width = 854;
		*height = 480;

	} else if ( SDK_VIDEO_RESOLUTION_WSXGA == index ) {
		*width = 1680;
		*height = 1050;

	} else if ( SDK_VIDEO_RESOLUTION_WUXGA == index ) {
		*width = 1920;
		*height = 1200;

	} else if ( SDK_VIDEO_RESOLUTION_WQXGA  == index ) {
		*width = 2560;
		*height = 1600;

	} else if ( SDK_VIDEO_RESOLUTION_720p == index ) {
		*width = 1280;
		*height = 720;

	} else if ( SDK_VIDEO_RESOLUTION_1080p == index ) {
		*width = 1920;
		*height = 1080;

	} else if ( SDK_VIDEO_RESOLUTION_960p == index  ) {
		*width = 1280;
		*height = 960;

	} else if ( SDK_VIDEO_RESOLUTION_SVGA == index ) {
		*width = 800;
		*height = 600;
	}

	return 0;
}

#if  0
//  0 : ok    -2：  获取范围失败  -3： 获取具体配置失败  -1： 其他失败
int Remote_GetEncodeMainResolution(int channel, sdk_resolution_t *pResolution )
{
	int ret = 0;

	if ( channel < 0 || channel >= MAX_DISPLAY_NUM || !pEncode )
	{
		return -1;
	}

	//struct  timeval    tv0;
	// struct  timezone   tz0;
	//gettimeofday(&tv0,&tz0);

	sdk_av_enc_t *pEnc = NULL;
	Remote_Device_Info remoteIpc;
	ret = DbMainMsgChannelInfo(channel,remoteIpc.ip,&remoteIpc.port,remoteIpc.userName,remoteIpc.password);
	if(-1==ret)
	{
		Printf( " DbMainMsgChannelInfo    get channel failed\n");
		return -1;
	}

	ONVIF_AddRemoteDevice(channel,&remoteIpc);
	Onvif_Profiles getProfilesRespone = { 0 };
	//ret = ONVIF_GetProfiles(&remoteIpc, &getProfilesRespone);
	ret = ONVIF_GetProfilesFromCache(&remoteIpc, &getProfilesRespone);

	int sizeProfiles = getProfilesRespone.sizeOfProfiles;
	Onvif_Video_Config pGetVideoEncoderResponse;
	Onvif_VideoEncoderConfigurationOptions  pGetVideoEncoderOptionsResponse;
	if ( sizeProfiles < 1  )
	{
		return -1;
	}
	int k = 0;
	for ( k = 0; k < 0; k++ )
	{

		if ( 2 == k )
		{
			break;
		}
		if ( 0 == k ) {
			pEnc = &pEncode->main;
		}

		if ( 1 == k ) {
			pEnc = &pEncode->second;
		}


		//   fprintf(stderr, "%s:%d :token=%s k=%d \n", __func__, __LINE__ , getProfilesRespone.Onvif_Profiles[k].videoConfig.token,k);
		ret = ONVIF_GetVideoEncoderConfiguration(&remoteIpc , getProfilesRespone.Onvif_Profiles[k].profileToken,
				getProfilesRespone.Onvif_Profiles[k].videoConfig.token, &pGetVideoEncoderResponse ,&pGetVideoEncoderOptionsResponse);
		if(1!=ret)
		{
			return -1;
		}


		pEnc->res_num = pGetVideoEncoderOptionsResponse.H264Options.__sizeResolutionsAvailable;
		int j = 0;
		for(j = 0 ;j!=pEnc->res_num;++j)
		{
			pEnc->resolution_all[j].width= pGetVideoEncoderOptionsResponse.H264Options.ResolutionsAvailable[j].Width;
			pEnc->resolution_all[j].height=pGetVideoEncoderOptionsResponse.H264Options.ResolutionsAvailable[j].Height;
			if(pGetVideoEncoderResponse.height== pGetVideoEncoderOptionsResponse.H264Options.ResolutionsAvailable[j].Height
					&& pGetVideoEncoderResponse.width ==pGetVideoEncoderOptionsResponse.H264Options.ResolutionsAvailable[j].Width)
			{
				pEnc->resolution_current = j;
				pResolution->height = pGetVideoEncoderOptionsResponse.H264Options.ResolutionsAvailable[j].Height;
				pResolution->width = pGetVideoEncoderOptionsResponse.H264Options.ResolutionsAvailable[j].Width;
			}			
			//fprintf(stderr,"pEnc->res_num= %d ,height=%d,widgth =%d\n",pEnc->res_num , pEnc->resolution_all[j].height, pEnc->resolution_all[j].width);

		}
		//  pEnc->resolution_current.height = pGetVideoEncoderResponse.height;
		//  pEnc->resolution_current.width = pGetVideoEncoderResponse.width;

		// fprintf(stderr,"current resolution is  height=%d,widgth =%d\n",  pEnc->resolution_current.height, pEnc->resolution_current.width);




		//fprintf(stderr, "newfunc:%s bitrate %d frame_rate %d level %d gop %d encoding %d quality %lf width %d height %d \n", __FILE__,
		//     pEnc->bitrate, pEnc->frame_rate, level, pEnc->gop, pEnc->video_enc,pGetVideoEncoderResponse.Quality,pGetVideoEncoderResponse.width,pGetVideoEncoderResponse.height);
	}

	return sizeof(sdk_resolution_t) ;
}
#endif


//  0 : ok    -2：  获取范围失败  -3： 获取具体配置失败  -1： 其他失败
int Remote_GetEncodeCfg(int channel, sdk_encode_t *pEncode )
{
	int ret = 0;

	if ( channel < 0 || channel >= MAX_DISPLAY_NUM || !pEncode )
	{
		return -1;
	}

	//struct  timeval    tv0;
	// struct  timezone   tz0;
	//gettimeofday(&tv0,&tz0);

	sdk_av_enc_t *pEnc = NULL;
	Remote_Device_Info remoteIpc;
	ret = DbMainMsgChannelInfo(channel,remoteIpc.ip,&remoteIpc.port,remoteIpc.userName,remoteIpc.password);
	if(-1==ret)
	{
		Printf( " DbMainMsgChannelInfo    get channel failed\n");
		return -1;
	}

	ONVIF_AddRemoteDevice(channel,&remoteIpc);
	Onvif_Profiles getProfilesRespone = { 0 };
	//ret = ONVIF_GetProfiles(&remoteIpc, &getProfilesRespone);
	ret = ONVIF_GetProfilesFromCache(&remoteIpc, &getProfilesRespone);

	int sizeProfiles = getProfilesRespone.sizeOfProfiles;
	Onvif_Video_Config pGetVideoEncoderResponse;
	Onvif_VideoEncoderConfigurationOptions  pGetVideoEncoderOptionsResponse;
	if ( sizeProfiles < 1  )
	{
		return -1;
	}
	int k = 0;
	for ( k = 0; k < sizeProfiles; k++ )
	{

		if ( 2 == k )
		{
			break;
		}
		if ( 0 == k ) {
			pEnc = &pEncode->main;
		}

		if ( 1 == k ) {
			pEnc = &pEncode->second;
		}


		//   fprintf(stderr, "%s:%d :token=%s k=%d \n", __func__, __LINE__ , getProfilesRespone.Onvif_Profiles[k].videoConfig.token,k);
		ret = ONVIF_GetVideoEncoderConfiguration(&remoteIpc , getProfilesRespone.Onvif_Profiles[k].profileToken,
				getProfilesRespone.Onvif_Profiles[k].videoConfig.token, &pGetVideoEncoderResponse ,&pGetVideoEncoderOptionsResponse);
		if(1!=ret)
		{
			return -1;
		}


		pEnc->res_num = pGetVideoEncoderOptionsResponse.H264Options.__sizeResolutionsAvailable;
		int j = 0;
		for(j = 0 ;j!=pEnc->res_num;++j)
		{
			pEnc->resolution_all[j].width= pGetVideoEncoderOptionsResponse.H264Options.ResolutionsAvailable[j].Width;
			pEnc->resolution_all[j].height=pGetVideoEncoderOptionsResponse.H264Options.ResolutionsAvailable[j].Height;
			if(pGetVideoEncoderResponse.height== pGetVideoEncoderOptionsResponse.H264Options.ResolutionsAvailable[j].Height
					&& pGetVideoEncoderResponse.width ==pGetVideoEncoderOptionsResponse.H264Options.ResolutionsAvailable[j].Width)
			{
				pEnc->resolution_current = j;
			}			
			//fprintf(stderr,"pEnc->res_num= %d ,height=%d,widgth =%d\n",pEnc->res_num , pEnc->resolution_all[j].height, pEnc->resolution_all[j].width);

		}
		//  pEnc->resolution_current.height = pGetVideoEncoderResponse.height;
		//  pEnc->resolution_current.width = pGetVideoEncoderResponse.width;

		// fprintf(stderr,"current resolution is  height=%d,widgth =%d\n",  pEnc->resolution_current.height, pEnc->resolution_current.width);
		pEnc->bitrate_type = 0;
		pEnc->pic_quilty = 100;
		pEnc->frame_rate = pGetVideoEncoderResponse.FrameRateLimit;
		pEnc->bitrate = pGetVideoEncoderResponse.BitrateLimit;

		int level = 0;
		level = pGetVideoEncoderResponse.h264.H264Profile;
		switch(pGetVideoEncoderResponse.videoEncoding)
		{
			case 0:
				pEnc->video_enc = 2;
				break;
			case 1:
				pEnc->video_enc = 1;
				break;
			case 2:
				pEnc->video_enc = 0;
				break;
			default:
				pEnc->video_enc = 0;
				break;
		}
		pEnc->level = level;
		//pEnc->video_enc = 0;
		pEnc->gop = pGetVideoEncoderResponse.h264.GovLength;
		pEnc->h264_ref_mod = 0;
		pEnc->audio_enc = 0;
		pEnc->mix_type = 1;
		//fprintf(stderr, "newfunc:%s bitrate %d frame_rate %d level %d gop %d encoding %d quality %lf width %d height %d \n", __FILE__,
		//     pEnc->bitrate, pEnc->frame_rate, level, pEnc->gop, pEnc->video_enc,pGetVideoEncoderResponse.Quality,pGetVideoEncoderResponse.width,pGetVideoEncoderResponse.height);
	}

	return sizeof(sdk_encode_t) ;
}

int Remote_SetEncodeCfg(int channel, sdk_encode_t *pEncode )
{
	int ret = 0;

	if ( channel < 0 || channel >= MAX_DISPLAY_NUM || !pEncode )
	{
		return -1;
	}

	sdk_av_enc_t *pEnc = NULL;
	Remote_Device_Info remoteIpc;
	ret = DbMainMsgChannelInfo(channel,remoteIpc.ip,&remoteIpc.port,remoteIpc.userName,remoteIpc.password);
	if(-1==ret)
	{
		Printf("DbMainMsgChannelInfo  get channel failed\n");
		return  -1;
	}

	ONVIF_AddRemoteDevice(channel,&remoteIpc);
	Onvif_Profiles getProfilesRespone = { 0 };
	//ret = ONVIF_GetProfiles(&remoteIpc, &getProfilesRespone);			
	ret = ONVIF_GetProfilesFromCache(&remoteIpc, &getProfilesRespone);	

	int sizeProfiles = getProfilesRespone.sizeOfProfiles;	

	if ( sizeProfiles < 1  ) 
	{
		return -1;
	}

	int k = 0;

	for ( k = 0; k < sizeProfiles; k++ )
	{

		if ( 2 == k ) 
		{
			break;
		}

		Onvif_Video_Config videoConfig;
		memset(&videoConfig, 0, sizeof(Onvif_Video_Config));

		if ( 0 == k ) {
			pEnc = &pEncode->main;
		}

		if ( 1 == k ) {
			pEnc = &pEncode->second;
		}	

		strncpy(videoConfig.Name, getProfilesRespone.Onvif_Profiles[k].videoConfig.Name, 
				sizeof(videoConfig.Name) - 1 );
		strncpy(videoConfig.token, getProfilesRespone.Onvif_Profiles[k].videoConfig.token,
				sizeof(videoConfig.token)  - 1 );

		videoConfig.videoEncoding = getProfilesRespone.Onvif_Profiles[k].videoConfig.videoEncoding;
		videoConfig.EncodingInterval = getProfilesRespone.Onvif_Profiles[k].videoConfig.EncodingInterval;
		videoConfig.Quality = getProfilesRespone.Onvif_Profiles[k].videoConfig.Quality;
		//fprintf(stderr,"encode %d,quality %lf,interval %d \n",videoConfig.videoEncoding,videoConfig.Quality, videoConfig.EncodingInterval);
		//GetResolution(pEnc->resolution, &videoConfig.width, &videoConfig.height);
		videoConfig.width = pEnc->resolution_all[pEnc->resolution_current].width;
		videoConfig.height= pEnc->resolution_all[pEnc->resolution_current].height;
		videoConfig.FrameRateLimit = pEnc->frame_rate ;
		videoConfig.BitrateLimit = pEnc->bitrate;
		videoConfig.h264.H264Profile = pEnc->level;
		videoConfig.h264.GovLength =  pEnc->frame_rate;

		ret = ONVIF_SetVideoEncoderConfiguration(&remoteIpc, &videoConfig);	
		if(1!=ret)
		{
			return  -1;
		}

	}

	return sizeof(sdk_encode_t);
}

static int main_msg_encode_cfg( sdk_msg_t *pmsg )
{
	int opera = pmsg->args;
	int chno =  pmsg->chann;

	sdk_encode_t *pEncode = (sdk_encode_t *)pmsg->data;

	if ( CHANN_TYPE_ALL == chno ) {
		pmsg->size = 0;
		return 0;
	}

	if ( SDK_PARAM_GET == opera ) {	 
		pmsg->size = Remote_GetEncodeCfg(chno, pEncode );
		if(-1==pmsg->size)
		{
			pmsg->ack=-1;
		}
	} else if ( SDK_PARAM_SET == opera ) {
		pmsg->size = Remote_SetEncodeCfg(chno, pEncode );
		if(-1==pmsg->size)
		{
			pmsg->ack=-1;
		}
	}

	return 0;
}

int Remote_GetImageParam(int channel, sdk_image_attr_t * pImg)
{
	if (channel < 0 || channel > MAX_DISPLAY_NUM|| !pImg) 
	{
		return 0;
	}
	int ret ;

	Remote_Device_Info remoteIpc;
	pImg->brightness = (uint8_t)128;
	pImg->contrast = (uint8_t)128;
	pImg->saturation = (uint8_t)128;
	pImg->sharpness = (uint8_t)128;
	pImg->hue = pImg->saturation;
	ret = DbMainMsgChannelInfo(channel,remoteIpc.ip,&remoteIpc.port,remoteIpc.userName,remoteIpc.password);
	if(-1==ret)
	{
		Printf( "DbMainMsgChannelInfo   get channel failed\n");
		return 0;
	}


	ONVIF_AddRemoteDevice(channel,&remoteIpc);
	Onvif_Profiles getProfilesRespone = { 0 };
	//	ret = ONVIF_GetProfiles(&remoteIpc, &getProfilesRespone);
	ret = ONVIF_GetProfilesFromCache(&remoteIpc, &getProfilesRespone);

	int sizeProfiles = getProfilesRespone.sizeOfProfiles;	

	if ( sizeProfiles < 1  )
	{
		Printf( "DbMainMsgChannelInfo   get channel failed\n");
		return sizeof(sdk_image_attr_t);
	}
	Onvif_Img_Config onvifImgConfig;
	memset(&onvifImgConfig, 0, sizeof( Onvif_Img_Config ) );
	ret = ONVIF_GetImageConfiguration(&remoteIpc, getProfilesRespone.Onvif_Profiles[0].videoSourceToken, &onvifImgConfig);
	if ( -1 == ret )
	{
		Printf("@@@@@@@@@@@@@@@:err: ONVIF_GetImageConfiguration err bright %d contrast %d statuation %d sharpness %d \n",
				pImg->brightness, pImg->contrast, pImg->saturation, pImg->sharpness );
		return sizeof(sdk_image_attr_t);
	}

	pImg->brightness = (int)onvifImgConfig.brightness;
	pImg->contrast = (int)onvifImgConfig.Contrast;
	pImg->saturation = (int)onvifImgConfig.ColorSaturation;
	pImg->sharpness = (int)onvifImgConfig.Sharpness;	
	pImg->hue = pImg->sharpness;

	//Printf("@@@@@@@@@@@@@@@:get successfully: ONVIF_GetImageConfiguration  bright %d contrast %d statuation %d sharpness %d \n",
	//     pImg->brightness, pImg->contrast, pImg->saturation, pImg->sharpness );

	return sizeof(sdk_image_attr_t);

}

int Remote_SetImageParam(int channel, sdk_image_attr_t * pImg)
{

	if (channel < 0 || channel >= MAX_DISPLAY_NUM || !pImg) {
		return 0;
	}
	int ret ;
	Remote_Device_Info remoteIpc;
	ret = DbMainMsgChannelInfo(channel,remoteIpc.ip,&remoteIpc.port,remoteIpc.userName,remoteIpc.password);
	if(-1==ret)
	{
		Printf( "DbMainMsgChannelInfo  get channel failed\n");
		return 0;
	}

	ONVIF_AddRemoteDevice(channel,&remoteIpc);
	Onvif_Profiles getProfilesRespone = { 0 };
	//ret = ONVIF_GetProfiles(&remoteIpc, &getProfilesRespone);			
	ret = ONVIF_GetProfilesFromCache(&remoteIpc, &getProfilesRespone);	

	int sizeProfiles = getProfilesRespone.sizeOfProfiles;	

	if ( sizeProfiles < 1  ) {		
		return 0;
	}

	Onvif_Img_Config onvifImgConfig;
	//memset(&onvifImgConfig, 0, sizeof( Onvif_Img_Config ) );

	onvifImgConfig.brightness = pImg->brightness;
	onvifImgConfig.ColorSaturation = pImg->saturation;
	onvifImgConfig.Contrast = pImg->contrast;
	onvifImgConfig.Sharpness = pImg->hue;

	ret = ONVIF_SetImageConfiguration(&remoteIpc, getProfilesRespone.Onvif_Profiles[0].videoSourceToken, &onvifImgConfig);
	if(ret!=1)
	{
		Printf("ONVIF_SetImageConfiguration err!\n");
	}
	return 0;
}


int Remote_SetDefaultImageParam(int channel, sdk_image_attr_t * pImg)
{
	if (channel < 0 || channel >= MAX_DISPLAY_NUM || !pImg)
	{
		return 0;
	}
	int ret ;
	Remote_Device_Info remoteIpc;
	ret = DbMainMsgChannelInfo(channel,remoteIpc.ip,&remoteIpc.port,remoteIpc.userName,remoteIpc.password);
	if(-1==ret)
	{
		Printf( "DbMainMsgChannelInfo get channel failed\n");
		return 0;
	}
	ONVIF_AddRemoteDevice(channel,&remoteIpc);
	Onvif_Profiles getProfilesRespone = { 0 };
	ret = ONVIF_GetProfilesFromCache(&remoteIpc, &getProfilesRespone);
	int sizeProfiles = getProfilesRespone.sizeOfProfiles;
	if ( sizeProfiles < 1  )
	{
		return 0;
	}
	Onvif_Img_Options pGetImgOptionsResponse;
	ret= ONVIF_GetImageOptions(&remoteIpc, getProfilesRespone.Onvif_Profiles[0].videoSourceToken, &pGetImgOptionsResponse);
	if(ret!=1)
	{
		Printf("ONVIF_GetImageOptions  err!\n");
	}

	Onvif_Img_Config onvifImgConfig;
	onvifImgConfig.brightness = (pGetImgOptionsResponse.brightness.Max + 1)/2;
	onvifImgConfig.ColorSaturation = (pGetImgOptionsResponse.ColorSaturation.Max + 1 )/2;
	onvifImgConfig.Contrast = (pGetImgOptionsResponse.Contrast.Max + 1 )/2;
	onvifImgConfig.Sharpness = (pGetImgOptionsResponse.Sharpness.Max + 1 )/2;

	ret = ONVIF_SetImageConfiguration(&remoteIpc, getProfilesRespone.Onvif_Profiles[0].videoSourceToken, &onvifImgConfig);
	if(ret!=1)
	{
		Printf("ONVIF_SetImageConfiguration err!\n");
	}
	return 0;
}


int main_msg_image_attr_cfg( sdk_msg_t *pmsg )
{

	int opera = pmsg->args;
	int chno =  pmsg->chann;	

	if ( CHANN_TYPE_ALL == chno ) {
		pmsg->size = 0;
		return 0;
	}

	sdk_image_attr_t * 	pImg = (sdk_image_attr_t *)pmsg->data;

	if (SDK_PARAM_GET == opera ) {

		pmsg->size = Remote_GetImageParam(chno, pImg);

	} else if ( SDK_PARAM_SET == opera ) {

		pmsg->size = Remote_SetImageParam(chno, pImg);
	}

	return 0;

}


int Remote_RunPtzCmd(int channel, int opera, sdk_ptz_ctl_t *pPtz, unsigned int extend )
{
	if (channel < 0 || channel >= configGetDisplayNum() || !pPtz ) {
		return 0;
	}
	char preSetName[16];
	char preSetToken[16];

	Remote_Device_Info remoteIpc;
	int ret = DbMainMsgChannelInfo(channel,remoteIpc.ip,&remoteIpc.port,remoteIpc.userName,remoteIpc.password);
	if(-1==ret)
	{
		Printf("DbMainMsgChannelInfo  get channel failed\n");
		return 0;
	}


	ONVIF_AddRemoteDevice(channel,&remoteIpc);

	Onvif_Profiles getProfilesRespone;
	//memset(&getProfilesRespone, 0, sizeof(getProfilesRespone));
	ONVIF_GetProfilesFromCache(&remoteIpc, &getProfilesRespone);

	ONVIF_PTZ__ContinuousMove PtzContinuousMove;
	//memset(&PtzContinuousMove, 0, sizeof( PtzContinuousMove ));

	strncpy(PtzContinuousMove.ProfileToken, getProfilesRespone.Onvif_Profiles[0].profileToken,
			sizeof( PtzContinuousMove.ProfileToken ) - 1);

	if ( 0 == PtzContinuousMove.Speed )
	{
		PtzContinuousMove.Speed = pPtz->speed;
	}

	PtzContinuousMove.Speed = (extend >> 16) & 0xFFFF;

	PtzContinuousMove.Timeout = 10000;
	PtzContinuousMove.isPan = 1;

	switch ( opera )
	{
		case SDK_PTZ_MSG_UP_STOP:
		case SDK_PTZ_MSG_DOWN_STOP:
		case SDK_PTZ_MSG_LEFT_STOP:
		case SDK_PTZ_MSG_RIGHT_STOP:
		case SDK_PTZ_MSG_LEFT_UP_STOP:
		case SDK_PTZ_MSG_RIGHT_UP_STOP:
		case SDK_PTZ_MSG_LEFT_DOWN_STOP:
		case SDK_PTZ_MSG_RITHT_DOWN_STOP:
		case SDK_PTZ_MSG_ZOOM_ADD_STOP:
		case SDK_PTZ_MSG_ZOOM_SUB_STOP:
		case SDK_PTZ_MSG_FOCUS_ADD_STOP:
		case SDK_PTZ_MSG_FOCUS_SUB_STOP:
		case SDK_PTZ_MSG_IRIS_ADD_STOP:
		case SDK_PTZ_MSG_IRIS_SUB_STOP:
		case SDK_PTZ_MSG_SET_PATTERN_STOP:
			//case SDK_PTZ_MSG_AUTO_SCAN_STOP:
		case SDK_PTZ_MSG_RANDOM_SCAN_STOP:
			{
				ONVIF_PtzStop(&remoteIpc, PtzContinuousMove.ProfileToken );
				break;
			}

		case SDK_PTZ_MSG_UP_START:
			{
				// fprintf(stderr, "================up  opera= %d,iszoom =%d \n",opera,PtzContinuousMove.isZoom);
				PtzContinuousMove.direction = PTZ_UP;
				PtzContinuousMove.isZoom = 0;
				ret = ONVIF_PtzContinueMove(&remoteIpc, &PtzContinuousMove );
				break;
			}

		case SDK_PTZ_MSG_DOWN_START:
			{
				PtzContinuousMove.direction = PTZ_DOWN;
				PtzContinuousMove.isZoom = 0;
				ret = ONVIF_PtzContinueMove(&remoteIpc, &PtzContinuousMove );
				break;
			}

		case SDK_PTZ_MSG_LEFT_START:
			{
				PtzContinuousMove.direction = PTZ_LEFT;
				PtzContinuousMove.isZoom = 0;
				ret = ONVIF_PtzContinueMove(&remoteIpc, &PtzContinuousMove );
				break;
			}

		case SDK_PTZ_MSG_RIGHT_START:
			{
				PtzContinuousMove.direction = PTZ_RIGHT;
				PtzContinuousMove.isZoom = 0;
				ret = ONVIF_PtzContinueMove(&remoteIpc, &PtzContinuousMove );
				break;
			}

		case SDK_PTZ_MSG_LEFT_UP_START:
			{
				PtzContinuousMove.direction = PTZ_LEFT_UP;
				PtzContinuousMove.isZoom = 0;
				ret = ONVIF_PtzContinueMove(&remoteIpc, &PtzContinuousMove );
				break;
			}

		case SDK_PTZ_MSG_RIGHT_UP_START:
			{
				PtzContinuousMove.direction = PTZ_RIGHT_UP;
				PtzContinuousMove.isZoom = 0;
				ret = ONVIF_PtzContinueMove(&remoteIpc, &PtzContinuousMove );
				break;
			}

		case SDK_PTZ_MSG_LEFT_DOWN_START:
			{
				PtzContinuousMove.direction = PTZ_LEFT_DOWN;
				PtzContinuousMove.isZoom = 0;
				ret = ONVIF_PtzContinueMove(&remoteIpc, &PtzContinuousMove );
				break;
			}

		case SDK_PTZ_MSG_RIGHT_DOWN_START:
			{
				PtzContinuousMove.direction = PTZ_RIGHT_DOWN;
				PtzContinuousMove.isZoom = 0;
				ret = ONVIF_PtzContinueMove(&remoteIpc, &PtzContinuousMove );
				break;
			}

		case SDK_PTZ_MSG_PRESET_SET:
			{
				sprintf(preSetName, "%d", extend & 0xFFFF);
				sprintf(preSetToken, "%d", extend & 0xFFFF);

				// fprintf(stderr, "================SDK_PTZ_MSG_PRESET_SET name %s token %s \n", preSetName, preSetToken);
				//ret = ONVIF_PtzSetPreset(&remoteIpc,PtzContinuousMove.ProfileToken, preSetName, preSetToken );
				//ONVIF_PtzSetPreset(&remoteIpc,PtzContinuousMove.ProfileToken, preSetName, preSetToken );

				break;
			}

		case SDK_PTZ_MSG_PRESET_CALL:
			{
				sprintf(preSetToken, "%d", extend & 0xFFFF);
				//ret = ONVIF_PtzGotoPreset(&remoteIpc, PtzContinuousMove.ProfileToken, preSetToken);
				break;
			}

		case SDK_PTZ_MSG_PRESET_DEL:
			{
				sprintf(preSetToken, "%d", extend & 0xFFFF);
				//ret = ONVIF_PtzRemovePreset(&remoteIpc, PtzContinuousMove.ProfileToken, preSetToken);

				break;
			}

		case SDK_PTZ_MSG_ZOOM_ADD_START:
			{
				PtzContinuousMove.isZoom = 1;
				PtzContinuousMove.isPan = 0;
				PtzContinuousMove.direction = PTZ_ZOOM_IN ;
				ret = ONVIF_PtzContinueMove(&remoteIpc, &PtzContinuousMove );

				break;
			}

		case SDK_PTZ_MSG_ZOOM_SUB_START:
			{
				PtzContinuousMove.isZoom = 1;
				PtzContinuousMove.isPan = 0;
				PtzContinuousMove.direction = PTZ_ZOOM_OUT;
				PtzContinuousMove.Speed *= -1;
				ret = ONVIF_PtzContinueMove(&remoteIpc, &PtzContinuousMove );
				break;
			}

		case SDK_PTZ_MSG_FOCUS_ADD_START:
			{
				break;
			}

		case SDK_PTZ_MSG_FOCUS_SUB_START:
			{
				break;
			}


		case SDK_PTZ_MSG_IRIS_ADD_START:
			{
				break;
			}

		case SDK_PTZ_MSG_IRIS_SUB_START:
			{
				break;
			}

		case SDK_PTZ_MSG_GOTO_ZERO_PAN:
			{
				break;
			}

		case SDK_PTZ_MSG_FLIP_180:
			{
				break;
			}

		case SDK_PTZ_MSG_SET_PATTERN_START:
			{
				break;
			}

		case SDK_PTZ_MSG_RUN_PATTERN:
			{
				break;
			}

		case SDK_PTZ_MSG_SET_AUXILIARY:
			{
				break;
			}

		case SDK_PTZ_MSG_CLEAR_AUXILIARY:
			{
				break;
			}

		case SDK_PTZ_MSG_AUTO_SCAN_START:
			{
				ptzStartCruise(channel);

				break;
			}

		case SDK_PTZ_MSG_AUTO_SCAN_STOP:
			{
				ptzStopCruise(channel);
				break;
			}

		case SDK_PTZ_MSG_RANDOM_SCAN_START:
			{
				break;
			}

		case SDK_PTZ_MSG_LEFT_OFFSET:
			{
				break;
			}

		case SDK_PTZ_MSG_RIGHT_OFFSET:
			{
				break;
			}

		case SDK_PTZ_MSG_DOWN_OFFSET:
			{
				break;	
			}

		case SDK_PTZ_MSG_UP_OFFSET:
			{
				break;
			}

		case SDK_PTZ_MSG_ZOOM_MULTIPLE:
			{
				break;
			}

		case SDK_PTZ_MSG_POINT_CENTER:
			{
				break;
			}

		case SDK_PTZ_MSG_VIEW_CENTER:
			{
				break;
			}
	}

	return 0;

}


int msg_ptz_dispatch( sdk_msg_t *pmsg )
{

	int opera = pmsg->args;
	int chno =  pmsg->chann;

	if ( CHANN_TYPE_ALL == chno ) 
	{
		pmsg->size = 0;
		return 0;
	}

	sdk_ptz_ctl_t *pPtz = (sdk_ptz_ctl_t*)pmsg->data;

	pmsg->size = Remote_RunPtzCmd(chno, opera, pPtz, pmsg->extend);

	return 0;

}
int Remote_GetPtzpresets(int channel, sdk_preset_all_t *pPresets )
{
	if ( channel < 0 || channel >= MAX_DISPLAY_NUM || !pPresets ) 
	{
		return 0;
	}

	int ret = 0;

	Remote_Device_Info remoteIpc;
	ret = DbMainMsgChannelInfo(channel,remoteIpc.ip,&remoteIpc.port,remoteIpc.userName,remoteIpc.password);
	if(-1==ret)
	{
		Printf(" DbMainMsgChannelInfo get channel failed\n");
		return 0;
	}
	ONVIF_AddRemoteDevice(channel,&remoteIpc);


	Onvif_Ptz_Config ptzConfig;
	memset(&ptzConfig, 0, sizeof(Onvif_Ptz_Config));

	ret = ONVIF_PtzGetConfiguration(&remoteIpc, &ptzConfig);

	Onvif_Ptz_Presets presets;
	memset(&presets, 0, sizeof(Onvif_Ptz_Presets));

	ret = ONVIF_PtzGetPresets(&remoteIpc, ptzConfig.token, &presets);

	if ( -1 == ret ) {	
		return 0;
	}


	pPresets->num = presets.sizeOfPresets;

	pPresets->num = pPresets->num > 0 ? pPresets->num : 0;

	int k = 0;
	for ( k = 0; k < pPresets->num; k++ )
	{
		pPresets->preset[k].num = atoi(presets.presets[k].presetToken);
		//strncpy((char*)pPresets->preset[k].name, (char*)presets.presets[k].presetName, sizeof( pPresets->preset[k].name ) - 1 );

		//Printf("================ preset no %d name %s \n", pPresets->preset_set[k].no, pPresets->preset_set[k].name);
	}

	return sizeof(sdk_preset_all_t);

}


int remote_msg_preset_get(sdk_msg_t *pmsg)
{
	int chno =  pmsg->chann;

	if ( CHANN_TYPE_ALL == chno ) 
	{
		pmsg->size = 0;
		return 0;
	}

	sdk_preset_param_t *pPresets = (sdk_preset_param_t*)pmsg->data;

	pmsg->size = Remote_GetPtzpresets(chno, pPresets );

	return 0;
}

int main_msg_record_query(sdk_msg_t *pmsg)
{
	record_search_file_t *p_cond = (record_search_file_t *)pmsg->data;

	record_search_file_t recordItem[1024];
	int itemNum = 0;

	//Printf("start time %d, end time %d, ch %d\r\n", p_cond->startTime, p_cond->endTime, pmsg->chann);
	memset(&recordItem, 0, sizeof(recordItem));
	itemNum = recordFindDevChannelRecord(pmsg->chann, 0, p_cond->fileType, p_cond->startTime, p_cond->endTime, (record_search_file_t *)&recordItem);

	//Printf("start time %d, end time %d, ch %d  itemNum %d\r\n", p_cond->startTime, p_cond->endTime, pmsg->chann,itemNum);
	if(itemNum > 0)
	{
		memcpy(&pmsg->data, &recordItem, itemNum * sizeof(record_search_file_t));
	}
	pmsg->size = itemNum * sizeof(record_search_file_t);

	return 0;
}

int main_msg_record_query_status(sdk_msg_t *pmsg)
{
	record_search_file_t *p_cond = (record_search_file_t *)pmsg->data;

	record_search_file_t recordItem[1024];
	int itemNum = 0;

	//Printf("start time %d, end time %d, ch %d\r\n", p_cond->startTime, p_cond->endTime, pmsg->chann);
	memset(&recordItem, 0, sizeof(recordItem));
	itemNum = recordFindDevChannelRecord(pmsg->chann, 0, p_cond->fileType, p_cond->startTime, p_cond->endTime, (record_search_file_t *)&recordItem);

	//Printf("start time %d, end time %d, ch %d  itemNum %d\r\n", p_cond->startTime, p_cond->endTime, pmsg->chann,itemNum);
	if(itemNum > 0)
	{
		pmsg->extend =itemNum;
	}
	else
	{
		pmsg->extend =0;
	}
	pmsg->size = 0;

	return 0;
}

int main_msg_p2p_get_status(sdk_msg_t *pmsg)
{
	pmsg->extend =GetP2PConnectStatus();
	Printf("GetP2PConnectStatus() =%d\n",GetP2PConnectStatus());
	pmsg->size = 0;
	return 0;
}
int main_msg_emil_test_status(sdk_msg_t *pmsg)
{
	pmsg->extend =testSmtp();
	Printf("testSmtp() =%d\n",testSmtp());
	pmsg->size = 0;
	return 0;
}
int main_msg_poll(sdk_msg_t *pmsg)
{
	int ret = 0;
	ret = databaseOperate(pmsg->msg_id, pmsg->args, &pmsg->chann, sizeof(pmsg->chann), pmsg->data, pmsg->size);
	if(ret == -1)
	{
		pmsg->size = 0;
		pmsg->ack = -1;
	}
	else
	{
		pmsg->size = ret;
	}

	return 0;
}

int main_msg_ftp(sdk_msg_t *pmsg)
{
	int ret = 0;
	ret = databaseOperate(pmsg->msg_id, pmsg->args, NULL, 0, pmsg->data, pmsg->size);

	if(ret == -1)
	{
		pmsg->size = 0;
		pmsg->ack = -1;
	}
	else
	{
		pmsg->size = ret;
	}

	return 0;
}

int main_msg_chann_cfg(sdk_msg_t *pmsg)
{
	int ret = 0;

	ret = databaseOperate(pmsg->msg_id, pmsg->args, &pmsg->chann, sizeof(pmsg->chann), pmsg->data, pmsg->size);
	if(ret == -1)
	{
		pmsg->size = 0;
		pmsg->ack = -1;
	}
	else
	{
		pmsg->size = ret;
	}

	return 0;
}

int main_msg_devlist(sdk_msg_t *pmsg)
{
	pthread_mutex_lock(&g_discoveryMutex);
	if(pmsg->args == MANUFACT_ONVIF)
	{
		memset(&onvifDev, 0, sizeof(onvifDev));
		devNum = 0;
		//Printf("start discovery\r\n");
		if(StartDiscovery(onvifDeviceInfo,false) != 0)
		{
			Printf("start discovery error\r\n");
		}
		usleep(100*1000);
		memcpy(pmsg->data, &onvifDev, devNum * sizeof(sdk_device_t));
		pmsg->size = devNum * sizeof(sdk_device_t);
	}
	else if (pmsg->args == MANUFACT_PRIVATE)
	{
		memset(&privateDev, 0, sizeof(privateDev));
		privatedevNum = 0;
		if(DD_SearchDev()!=0)
		{
			Printf("start private discovery error\r\n");
		}
		sleep(3);
		memcpy(pmsg->data, &privateDev, privatedevNum * sizeof(sdk_device_t));
		pmsg->size = privatedevNum * sizeof(sdk_device_t);

	}
	else if (pmsg->args == MANUFACT_ONVIF_PRIVATE)
	{
		memset(&privateDev, 0, sizeof(privateDev));
		privatedevNum = 0;
		if(DD_SearchDev()!=0)
		{
			Printf("start private discovery error\r\n");
		}
		sleep(3);
		memcpy(pmsg->data, &privateDev, privatedevNum * sizeof(sdk_device_t));
		memset(&onvifDev, 0, sizeof(onvifDev));
		devNum = 0;
		if(StartDiscovery(onvifDeviceInfo,false) != 0)
		{
			Printf("start discovery error\r\n");
		}
		usleep(100*1000);
		memcpy(pmsg->data + privatedevNum * sizeof(sdk_device_t) , &onvifDev, devNum * sizeof(sdk_device_t));
		pmsg->size = devNum * sizeof(sdk_device_t) + privatedevNum * sizeof(sdk_device_t);

    }
   else  if(pmsg->args == MANUFACT_ONVIF_MULTISUBNET)
   	{
		memset(&onvifDev, 0, sizeof(onvifDev));
		devNum = 0;
		Printf("start multi_subnet discovery\r\n");
		if(StartDiscovery(onvifDeviceInfo,true) != 0)
		{
			Printf("start discovery error\r\n");
		}
		usleep(100*1000);
		memcpy(pmsg->data, &onvifDev, devNum * sizeof(sdk_device_t));
		pmsg->size = devNum * sizeof(sdk_device_t);
   	}

	pthread_mutex_unlock(&g_discoveryMutex);

	return 0;
}

int main_msg_preview_cfg(sdk_msg_t *pmsg)
{
	int ret = 0;

	ret = databaseOperate(pmsg->msg_id, pmsg->args, &pmsg->chann, sizeof(pmsg->chann), pmsg->data, pmsg->size);
	if(ret == -1)
	{
		pmsg->size = 0;
		pmsg->ack = -1;
	}
	else
	{
		pmsg->size = ret;
	}

	return 0;
}

int main_msg_vo_binding(sdk_msg_t *pmsg)
{
	//	int is_preview = pmsg->args;
	sdk_vo_binding_t *pvobinding_cfg = (sdk_vo_binding_t*)pmsg->data;

	//Printf("ddddd mode %d, win_num %d, windows %d\r\n", pvobinding_cfg->mode, pvobinding_cfg->win_num, pvobinding_cfg->windows[0]);
	if(pvobinding_cfg->mode == 0)
	{
		//decodeLibFullScreen(pvobinding_cfg->windows[0]);
		SwitchScreenDisplay(pvobinding_cfg->windows[0], 1);
	}
	else if(pvobinding_cfg->mode == 1)
	{
		//decodeLibFourScreen(pvobinding_cfg->windows[0]);
		SwitchScreenDisplay(pvobinding_cfg->windows[0], 4);
	}
	else if(pvobinding_cfg->mode == 2)
	{
		decodeLibSixScreen(pvobinding_cfg->windows[0]);
	}
	else if(pvobinding_cfg->mode == 3)
	{
		//decodeLibEightScreen(pvobinding_cfg->windows[0]);
		SwitchScreenDisplay(pvobinding_cfg->windows[0], 8);
	}
	else if(pvobinding_cfg->mode == 4)
	{
		//decodeLibNineScreen(pvobinding_cfg->windows[0]);
		SwitchScreenDisplay(pvobinding_cfg->windows[0], 9);
	}
	else if(pvobinding_cfg->mode == 5)
	{
		//decodeLibSixteenScreen(pvobinding_cfg->windows[0]);
		SwitchScreenDisplay(pvobinding_cfg->windows[0], 16);
	}
	else if(pvobinding_cfg->mode == 6)
	{
		//decodeLib25Screen(pvobinding_cfg->windows[0]);
		//decodeLib24Screen(pvobinding_cfg->windows[0]);
		SwitchScreenDisplay(pvobinding_cfg->windows[0], 25);
	}
	else if(pvobinding_cfg->mode == 7)
	{
		decodeLib36Screen(pvobinding_cfg->windows[0]);
	}
	else if(pvobinding_cfg->mode == 0xFF)
	{
		if(configGetDisplayNum() == 1)
		{
			//decodeLibFullScreen(pvobinding_cfg->windows[0]);
			SwitchScreenDisplay(pvobinding_cfg->windows[0], 1);
		}
		else if(configGetDisplayNum() == 4)
		{
			//decodeLibFourScreen(pvobinding_cfg->windows[0]);
			SwitchScreenDisplay(pvobinding_cfg->windows[0], 4);
		}
		else if(configGetDisplayNum() == 6)
		{
			//decodeLibSixScreen(pvobinding_cfg->windows[0]);
			SwitchScreenDisplay(pvobinding_cfg->windows[0], 6);
		}
		else if(configGetDisplayNum() == 8)
		{
			//decodeLibEightScreen(pvobinding_cfg->windows[0]);
			SwitchScreenDisplay(pvobinding_cfg->windows[0], 8);
		}
		else if(configGetDisplayNum() == 9)
		{
			//decodeLibNineScreen(pvobinding_cfg->windows[0]);
			SwitchScreenDisplay(pvobinding_cfg->windows[0], 9);
		}
		else if(configGetDisplayNum() == 16)
		{
			//decodeLibSixteenScreen(pvobinding_cfg->windows[0]);
			SwitchScreenDisplay(pvobinding_cfg->windows[0], 16);
		}
		else if(configGetDisplayNum() == 24)
		{
			//decodeLib24Screen(pvobinding_cfg->windows[0]);
			SwitchScreenDisplay(pvobinding_cfg->windows[0], 24);
		}
		else if(configGetDisplayNum() == 25)
		{
			//decodeLib25Screen(pvobinding_cfg->windows[0]);
			SwitchScreenDisplay(pvobinding_cfg->windows[0], 25);
		}
		else if(configGetDisplayNum() == 32)
		{
			decodeLib36Screen(pvobinding_cfg->windows[0]);
		}
	}

	pmsg->size = 0;

	return 0;
}

int main_msg_group_new(sdk_msg_t *pmsg)
{
	int i = 0;
	int total_ch = 0;
	//int ch[4] = {0};
	int itemNum = 0;
	int main_display_ch;
	record_search_file_t file[1024];
	sdk_pb_group_t *p_cond = (sdk_pb_group_t *)pmsg->data;

	main_display_ch = p_cond->main_chann;
	//Printf("##################### playback main chn = %d\n", main_display_ch);
	for(i = 0; i < 4; i++)
	{
		{
			if(p_cond->chann_mask[i] != 0)
			{
				PlayBackChn[i] = p_cond->chann_mask[i]-1;
				total_ch++;
			}
		}
	}

	//if(playbackSetNum((total_ch > 1) ? 4 : 1) == -1)
	//if(playbackSetNum(total_ch,main_display_ch) == -1)
	//return 0;
	playbackSetNum(total_ch,main_display_ch);
	SwitchScreenPlayBackDisplay(PlayBackChn, total_ch);


	for(i = 0; i < total_ch; i++)
	{
		/*
		   if(i == 0)
		   {
		   playbackStart(&p_cond->record_file_info, 1, 0, 0);
		   }
		   else
		 */
		{
			itemNum = recordFindDevChannelRecord(PlayBackChn[i], 0, p_cond->record_file_info.fileType, p_cond->record_file_info.startTime, p_cond->record_file_info.endTime, &file[0]);
			if(itemNum > 0)
			{
				//playbackStart(&file[0], itemNum, 0, i);
				playbackStart(&file[0], itemNum, p_cond->record_file_info.startTime, i);
				usleep(10000);
			}
		}
	}

	return 0;
}

int main_msg_group_del(sdk_msg_t *pmsg)
{
	playbackStopAll();
	//decodeRestartAllDecode();
	return 0;
}

int main_msg_playback_switch(sdk_msg_t *pmsg)
{
	sdk_winswitch_t *pwinswitch = (sdk_winswitch_t*)pmsg->data;

	Printf("############ pwinswitch->fullscreen = %d\n", pwinswitch->fullscreen);
	Printf("############ pwinswitch->win_num = %d\n", pwinswitch->win_num);
	Printf("############ pwinswitch->ch[0] = %d\n", pwinswitch->ch[0]);
	Printf("############ pwinswitch->ch[1] = %d\n", pwinswitch->ch[1]);
	Printf("############ pwinswitch->ch[2] = %d\n", pwinswitch->ch[2]);
	Printf("############ pwinswitch->ch[3] = %d\n", pwinswitch->ch[3]);

	if(pwinswitch->fullscreen == 0)
	{
		AppPlayBackExitFullScreenStartVo(pwinswitch->ch, pwinswitch->win_num);
	}
	else if(pwinswitch->fullscreen == 1)
	{
		AppPlayBackFullScreenStartVo(pwinswitch->ch, pwinswitch->win_num);
	}

	return 0;
}

int main_msg_group_ctl(sdk_msg_t *pmsg)
{
	if(pmsg->args == SDK_PB_CONTROL_QUERY_TIME)
	{
		pmsg->extend = playbackGetPlayTime();
		pmsg->size = 0;
	}
	else if(pmsg->args == SDK_PB_CONTROL_PAUSE)
	{
#if 0
		playbackCmd(PLAYBACK_STATUS_WAIT, pmsg->chann);
#else
		playbackCmd(PLAYBACK_STATUS_WAIT, 0, 1);
		playbackCmd(PLAYBACK_STATUS_WAIT, 1, 1);
		playbackCmd(PLAYBACK_STATUS_WAIT, 2, 1);
		playbackCmd(PLAYBACK_STATUS_WAIT, 3, 1);
#endif
	}
	else if(pmsg->args == SDK_PB_CONTROL_NORMAL)
	{
#if 0
		playbackCmd(PLAYBACK_STATUS_DECODE, pmsg->chann);
#else
		playbackResetStc(SDK_PB_CONTROL_NORMAL,0);
		playbackCmd(PLAYBACK_STATUS_DECODE, 0, 1);

		playbackResetStc(SDK_PB_CONTROL_NORMAL,1);
		playbackCmd(PLAYBACK_STATUS_DECODE, 1, 1);

		playbackResetStc(SDK_PB_CONTROL_NORMAL,2);
		playbackCmd(PLAYBACK_STATUS_DECODE, 2, 1);

		playbackResetStc(SDK_PB_CONTROL_NORMAL,3);
		playbackCmd(PLAYBACK_STATUS_DECODE, 3, 1);
#endif
	}
	else if(pmsg->args == SDK_PB_CONTROL_DRAG)
	{
		//Printf("pmsg->args = %d, pmsg->extend = %d\r\n", pmsg->args, pmsg->extend);
#if 0
		playbackControl(1, pmsg->extend, 0);
#else
		playbackResetStc(SDK_PB_CONTROL_DRAG,0);
		playbackControl(1, pmsg->extend, 0);
		usleep(20000);

		playbackResetStc(SDK_PB_CONTROL_DRAG,1);
		playbackControl(1, pmsg->extend, 1);
		usleep(20000);

		playbackResetStc(SDK_PB_CONTROL_DRAG,2);
		playbackControl(1, pmsg->extend, 2);
		usleep(20000);

		playbackResetStc(SDK_PB_CONTROL_DRAG,3);
		playbackControl(1, pmsg->extend, 3);
		usleep(20000);
#endif
	}
	else if(pmsg->args == SDK_PB_CONTROL_FORWARD)
	{
		//Printf(" 1 1 pmsg->args = %d, pmsg->extend = %d\r\n", pmsg->args, pmsg->extend);
		//fprintf(stderr,"[%s:%s:%d]:speed=%d \n",__FILE__,__FUNCTION__,__LINE__,pmsg->extend);
		playbackResetStc(PLAYBACK_STATUS_FAST_FORWARD,0);
		playbackCmd(PLAYBACK_STATUS_FAST_FORWARD, 0, pmsg->extend);

		playbackResetStc(PLAYBACK_STATUS_FAST_FORWARD,1);
		playbackCmd(PLAYBACK_STATUS_FAST_FORWARD, 1, pmsg->extend);

		playbackResetStc(PLAYBACK_STATUS_FAST_FORWARD,2);
		playbackCmd(PLAYBACK_STATUS_FAST_FORWARD, 2, pmsg->extend);

		playbackResetStc(PLAYBACK_STATUS_FAST_FORWARD,3);
		playbackCmd(PLAYBACK_STATUS_FAST_FORWARD, 3, pmsg->extend);
	}
	else
	{
		Printf("playback args %d\r\n", pmsg->args);
	}

	return 0;
}

int main_msg_systim(sdk_msg_t *pmsg)
{
	/*  sdk_comm_cfg_t */
	int ret = 0;
	HI_Rtc_Time_t  hi_rtc_time={0};
	sdk_time_t *p_cfg = (sdk_time_t*)pmsg->data;

	if(pmsg->args == SDK_PARAM_SET)
	{
		memset(&hi_rtc_time,0,sizeof(hi_rtc_time));
		//Printf("p_cfg->year=%d\n",p_cfg->year);
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
			Printf("write_rtc  fail \n");
		}
		hi_rtc_time.year = hi_rtc_time.year+2000;
		ret=setTimeTo_system(&hi_rtc_time);
		if(ret<0)
		{
			Printf("setTimeTo_system  fail \n");
		}

	}
	else
	{
		memset(&hi_rtc_time,0,sizeof(hi_rtc_time));
		ret=read_rtc(&hi_rtc_time);
		if(ret<0)
		{
			Printf("read_rtc fail\n");
		}

		hi_rtc_time.year=hi_rtc_time.year+2000;
		//Printf("hi_rtc_time.year=%d\n",hi_rtc_time.year);
		//Printf("hi_rtc_time.month=%d\n",hi_rtc_time.month);
		//Printf("hi_rtc_time.day=%d\n",hi_rtc_time.day);
		//Printf("hi_rtc_time.hour=%d\n",hi_rtc_time.hour);
		//Printf("hi_rtc_time.minutes=%d\n",hi_rtc_time.minutes);
		//Printf("hi_rtc_time.seconds=%d\n",hi_rtc_time.seconds);
		ret=setTimeTo_system(&hi_rtc_time);
		if(ret<0)
		{
			Printf("setTimeTo_system  fail \n");
		}
		p_cfg->year=hi_rtc_time.year;
		p_cfg->mon=hi_rtc_time.month;
		p_cfg->day=hi_rtc_time.day;
		p_cfg->hour=hi_rtc_time.hour;
		p_cfg->min=hi_rtc_time.minutes;
		p_cfg->sec=hi_rtc_time.seconds;
		pmsg->size = sizeof(sdk_time_t);
	}


	return 0;
}

int main_msg_alarm_in_cfg(sdk_msg_t *pmsg)
{
	int ret = 0;

	ret = databaseOperate(pmsg->msg_id, pmsg->args, &pmsg->chann, sizeof(pmsg->chann), pmsg->data, pmsg->size);
	if(ret == -1)
	{
		pmsg->size = 0;
		pmsg->ack = -1;
	}
	else
	{
		pmsg->size = ret;
	}

	return 0;

}

int main_msg_comm_cfg(sdk_msg_t *pmsg)
{
	int ret = 0;

	ret = databaseOperate(pmsg->msg_id, pmsg->args, &pmsg->chann, sizeof(pmsg->chann), pmsg->data, pmsg->size);
	if(ret == -1)
	{
		pmsg->size = 0;
		pmsg->ack = -1;
	}
	else
	{
		pmsg->size = ret;
	}

	return 0;

}

int main_msg_audio_control(sdk_msg_t *pmsg)
{
	decodeEableAudio(pmsg->extend);
	pmsg->size = 0;

	return 0;
}

int main_msg_ddns_cfg(sdk_msg_t *pmsg)
{
	int ret = 0;

	ret = databaseOperate(pmsg->msg_id, pmsg->args, &pmsg->chann, sizeof(pmsg->chann), pmsg->data, pmsg->size);
	if(ret == -1)
	{
		pmsg->size = 0;
		pmsg->ack = -1;
	}
	else
	{
		pmsg->size = ret;
	}

	return 0;
}

int main_msg_upnp_cfg(sdk_msg_t *pmsg)
{
	int ret = 0;

	ret = databaseOperate(pmsg->msg_id, pmsg->args, &pmsg->chann, sizeof(pmsg->chann), pmsg->data, pmsg->size);
	if(ret == -1)
	{
		pmsg->size = 0;
		pmsg->ack = -1;
	}
	else
	{
		pmsg->size = ret;
	}

	return 0;
}


/* ====================  ================== */

#if 0
typedef struct msg_desc_s
{
	int  id;
	int(*local_msg_cb)(sdk_msg_t *pmsg);
	int(*remote_msg_cb)(sdk_msg_t *pmsg);
	char desc[128];
} msg_desc_t;

#endif

/*
 * main map tab;
 * IDmain.h:SDK_MAIN_MSG_ID_E;
 */


typedef struct _LOG_ITEM_
{
	char time[32]; // 2013-03-29 xx:xx:xx 
	int	 major_type;
	int	 minor_type;
	int	 args;
	char user[32];
	char ip_addr[64];
	char desc[128];	

}LOG_ITEM;

typedef struct _logInfo
{
	int curIndex;
	int total;
	LOG_ITEM logItem[MAX_LOG];
}logInfo;


static int writeLogItem( LOG_ITEM *pLogItem  )
{

	FILE *fp = fopen(LOG_FILE, "r+");

	if (!fp) {
		fp = fopen(LOG_FILE, "w+");
		if (!fp) { 
			return -1;
		}

		logInfo log;
		memset(&log, 0, sizeof(logInfo));		
		log.curIndex = 0;

		memcpy(&log.logItem[log.curIndex], pLogItem, sizeof(LOG_ITEM));

		fwrite(&log, sizeof(logInfo), 1, fp);
		fclose(fp);	

		return 1;
	}	

	int curIndex = -1;

	fread(&curIndex, sizeof(int), 1, fp);

	curIndex %= MAX_LOG;

	fseek(fp, 4+4+ curIndex * sizeof(LOG_ITEM), SEEK_SET);
	fwrite((void*)pLogItem, sizeof(LOG_ITEM), 1, fp);

	curIndex += 1;
	curIndex %= MAX_LOG;

	fseek(fp, 0, SEEK_SET);
	fwrite(&curIndex, sizeof(int), 1, fp);

	fclose(fp);

	fp = NULL;

	return 1;
}

int writeLog(int major_type, int minor_type, int args, char *user, char *ipaddr, char *desc)
{


	LOG_ITEM logItem;
	memset(&logItem, 0, sizeof(LOG_ITEM));

	logItem.major_type = major_type;
	logItem.minor_type = minor_type;

	if (user) {
		strncpy(logItem.user, user, sizeof(logItem.user) - 1);
	}

	if (ipaddr) {
		strncpy(logItem.ip_addr, ipaddr, sizeof(logItem.ip_addr) - 1);
	}

	if ( desc ) {
		strncpy(logItem.desc, desc, sizeof(logItem.desc) - 1);
	}

	struct tm *ptm;
	time_t tnow;

	tnow = time(NULL);	
	ptm = localtime(&tnow);

	sprintf(logItem.time, "%04d-%02d-%02d %02d:%02d:%02d", 
			ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
			ptm->tm_hour, ptm->tm_min, ptm->tm_sec );

	return writeLogItem(&logItem);

}

int DeleteLog()
{

	FILE *fp = fopen(LOG_FILE, "r+");

	if (!fp) {
		fp = fopen(LOG_FILE, "w+");
		if (!fp) { 
			return -1;
		}
	}

	logInfo log;
	memset(&log, 0, sizeof(logInfo));		
	fwrite(&log, sizeof(logInfo), 1, fp);
	fclose(fp);	

	return 1;	
}

int GetLog( logInfo *pLog )
{

	if ( !pLog ) {
		return -1;
	}

	FILE *fp = fopen(LOG_FILE, "rb");

	if (!fp ) {
		return -1;
	}

	fread(pLog, sizeof(logInfo), 1, fp);

	fclose(fp);

	return 0;
}

int PrintLog(  )
{

	FILE *fp = fopen(LOG_FILE, "rb");

	if (!fp ) {
		return -1;
	}

	logInfo log;
	logInfo *pLog = &log;

	fread(pLog, sizeof(logInfo), 1, fp);

	fclose(fp);

	int i = 0;
	for ( i=0; i<MAX_LOG; i++ ) {
		if ( pLog->logItem[i].time[0] ) {
			Printf(" %d time %s major_type %d minor_type %d args %d user %s ip_addr %s desc %s \n",
					i, pLog->logItem[i].time, 
					pLog->logItem[i].major_type,
					pLog->logItem[i].minor_type,
					pLog->logItem[i].args, 
					pLog->logItem[i].user,
					pLog->logItem[i].ip_addr,
					pLog->logItem[i].desc );
		}
	}

	return 0;
}

int main_msg_ntp(sdk_msg_t *pmsg)
{
	int ret = 0;

	ret = databaseOperate(pmsg->msg_id, pmsg->args, &pmsg->chann, sizeof(pmsg->chann), pmsg->data, pmsg->size);
	if(ret == -1)
	{
		pmsg->size = 0;
		pmsg->ack = -1;
	}
	else
	{
		pmsg->size = ret;
	}
	return 0;
}

int main_msg_log(sdk_msg_t *pmsg)
{
	int ret = 0;
	ret = databaseOperate(SDK_MAIN_MSG_LOG,pmsg->args,NULL,pmsg->chann,pmsg->data, pmsg->size);
	if(ret == -1)
	{
		pmsg->size = 0;
		pmsg->ack = -1;
	}
	else
	{
		pmsg->size = ret;
	}
	return 0;
}

int main_msg_smtp(sdk_msg_t *pmsg)
{
	int ret = 0;

	ret = databaseOperate(pmsg->msg_id, pmsg->args, &pmsg->chann, sizeof(pmsg->chann), pmsg->data, pmsg->size);
	if(ret == -1)
	{
		pmsg->size = 0;
		pmsg->ack = -1;
	}
	else
	{
		pmsg->size = ret;
	}
	Printf("ppppppppppppppppppppppp  pmsg->size =%d\n",pmsg->size);
	return 0;
}

int main_msg_Zoom(sdk_msg_t *pmsg)
{
	//Printf("type =%d,subtype =%d \n",pmsg->msg_id,pmsg->args);
	sdk_zoom_t *pZoom = (sdk_zoom_t *)pmsg->data;
	ZoomInThread(pmsg->args, pZoom);
	pmsg->size = 0;
	return 0;
}

int main_msg_p2p(sdk_msg_t *pmsg)
{
	int ret = 0;
	ret = databaseOperate(pmsg->msg_id, pmsg->args, &pmsg->chann, sizeof(pmsg->chann), pmsg->data, pmsg->size);
	if(ret == -1)
	{
		pmsg->size = 0;
		pmsg->ack = -1;
	}
	else
	{
		pmsg->size = ret;
	}
	return 0;
}
int main_msg_getChannelBitrate(sdk_msg_t *pmsg)
{
	unsigned int channelNo;
	channelNo= pmsg->chann;
	getNetworkChannelRecvBitrate(channelNo,0,pmsg->data,&pmsg->size,&pmsg->extend);
	//pmsg->extend=getNetworkChannelRecvBitrate(channelNo,0);
	//pmsg->size =0;
	return 0;
}


int main_msg_cruise_cfg( sdk_msg_t *pmsg )
{
	int ret = 0;

	ret = databaseOperate(SDK_MAIN_MSG_CRUISE_CFG, pmsg->args, &pmsg->chann, sizeof(pmsg->chann), pmsg->data, pmsg->size );
	pmsg->size = ret;

	if ( SDK_PARAM_SET == pmsg->args ) {

		sdk_cruise_param_t *pCruise = (sdk_cruise_param_t *)pmsg->data;

		if ( pCruise->enable ) {
			ptzStartCruise(pmsg->chann);
		} else {
			ptzStopCruise(pmsg->chann);
		}
	}

	return 0;
}


int main_msg_cruise_begin( sdk_msg_t *pmsg )
{
	int channel = pmsg->chann;
	ptzStartCruise(channel);
	pmsg->size = 0;

	return 0;
}

int main_msg_locat_image_attr_cfg(sdk_msg_t *pmsg)
{
	sdk_image_attr_t *pVideoCsc = (sdk_image_attr_t *)pmsg->data;
	int ret = 0;
	if ( SDK_PARAM_SET == pmsg->args ) 
	{
		hi_vdec_setVideoLayerCSC(pVideoCsc);
	}
	ret = databaseOperate(pmsg->msg_id, pmsg->args, &pmsg->chann, sizeof(pmsg->chann), pmsg->data, pmsg->size);
	if(ret == -1)
	{
		pmsg->size = 0;
		pmsg->ack = -1;
	}
	else
	{
		pmsg->size = ret;
	}
	return 0;
}

int main_msg_rotate_attr_cfg(sdk_msg_t *pmsg)
{
	/*只有3535有旋转的功能*/
#ifdef HI3535
	hi_dec_SetRotate(pmsg->chann,pmsg->args);
#endif
	pmsg->size = 0;
}

int main_msg_Preset(sdk_msg_t *pmsg)
{
	//Printf("[main_msg_Preset] :%d", pmsg->chann);
	int opera = pmsg->args;
	int chno =  pmsg->chann;

	sdk_preset_t *pPreset = (sdk_preset_t *)pmsg->data;

	if ( CHANN_TYPE_ALL == chno ) {
		pmsg->size = 0;
		return 0;
	}

	if ( SDK_PARAM_GET == opera )
	{	 
		pmsg->size = SetPreset( chno , pPreset);
		if(-1==pmsg->size)
		{
			pmsg->ack=-1;
		}
	} else if ( SDK_PARAM_SET == opera ) {
		pmsg->size = RecallPreset(chno, pPreset );
		if(-1==pmsg->size)
		{
			pmsg->ack=-1;
		}
	}

	return 0;
}


int main_msg_CruisePresets(sdk_msg_t *pmsg)
{
	//Printf("[main_msg_CruisePresets] :%d", pmsg->chann);
	int opera = pmsg->args;
	int chno =  pmsg->chann;

	sdk_cruise_param_t *pCruisePreset = (sdk_cruise_param_t *)pmsg->data;

	if ( CHANN_TYPE_ALL == chno ) {
		pmsg->size = 0;
		return 0;
	}

	if ( SDK_PARAM_GET == opera )
	{	 
		pmsg->size = GetPtzCruiseAllPreset( chno , pCruisePreset);
		if(-1==pmsg->size)
		{
			pmsg->ack=-1;
		}
	} else if ( SDK_PARAM_SET == opera ) {
		pmsg->size = SetPtzCruiseAllPreset(chno, pCruisePreset );
		if(-1==pmsg->size)
		{
			pmsg->ack=-1;
		}
	}

	return 0;
}


int main_msg_AllPresets(sdk_msg_t *pmsg)
{
	//Printf("[main_msg_AllPresets] :%d", pmsg->chann);
	int opera = pmsg->args;
	int chno =  pmsg->chann;

	sdk_preset_all_t *pAllPreset = (sdk_preset_all_t *)pmsg->data;

	if ( CHANN_TYPE_ALL == chno ) {
		pmsg->size = 0;
		return 0;
	}

	if ( SDK_PARAM_GET == opera )
	{	 
		pmsg->size = GetAllPreset( chno , pAllPreset);
		if(-1==pmsg->size)
		{
			pmsg->ack=-1;
		}
	} 

#if 0
	else if ( SDK_PARAM_SET == opera ) {
		pmsg->size = GetAllPreset(chno, pAllPreset );
		if(-1==pmsg->size)
		{
			pmsg->ack=-1;
		}
	}
#endif

	return 0;
}


int  Remote_GetIPCNetworkPara( int channel , sdk_modify_ip* sdk_netwok_para)
{
#if 0
	if (channel < 0 || channel >= MAX_DISPLAY_NUM || !sdk_netwok_para)
	{
		return 0;
	}
	int ret ;
	Remote_Device_Info remoteIpc;
	Remote_Device_Info remoteIpc;
	strncpy(remoteIpc.ip,sdk_netwok_para.old_ip_addr,MAX_LEN);
	strncpy(remoteIpc.userName,sdk_netwok_para.user_name,MAX_LEN);
	strncpy(remoteIpc.password,sdk_netwok_para.user_pwd,MAX_LEN);
	remoteIpc.port = sdk_netwok_para.port;


	NetworkInterfaces  pIPCNetworkPara;
	ONVIF_AddRemoteDevice(channel,&remoteIpc);
	ret=ONVIF_GetNetworkInterfaces(&remoteIpc ,&pIPCNetworkPara);
	if(ret!=1)
		return -1;
	// strncpy(sdk_netwok_para-> ,pIPCNetworkPara.SingleNetworkInterface[0].InterfaceName,MAX_LEN);
	//strncpy(sdk_netwok_para->ip_addr,pIPCNetworkPara.SingleNetworkInterface[0].IpAddress,MAX_LEN);
	//strncpy(sdk_netwok_para->mask,pIPCNetworkPara.SingleNetworkInterface[0].NetMask,MAX_LEN);
	//strncpy(sdk_netwok_para->mac,pIPCNetworkPara.SingleNetworkInterface[0].HwAddress,MAX_LEN);

	return sizeof(sdk_ip_info_t);
#endif
}


int  Remote_SetIPCNetworkPara( int channel ,int index, sdk_modify_ip* sdk_netwok_para)
{
	if (channel < 0 || channel >= MAX_DISPLAY_NUM || !sdk_netwok_para)
	{
		return 0;
	}
	int ret ;
	Remote_Device_Info remoteIpc;
	strncpy(remoteIpc.ip,sdk_netwok_para->old_ip_addr,MAX_LEN);
	strncpy(remoteIpc.userName,sdk_netwok_para->user_name,MAX_LEN);
	strncpy(remoteIpc.password,sdk_netwok_para->user_pwd,MAX_LEN);
	remoteIpc.port = sdk_netwok_para->port;

	NetworkInterfaces  pIPCNetworkPara;
	//
	strncpy(pIPCNetworkPara.SingleNetworkInterface[0].token,"eth0" ,MAX_LEN);
	strncpy(pIPCNetworkPara.SingleNetworkInterface[0].IpAddress,sdk_netwok_para->new_ip_addr,MAX_LEN);
	strncpy(pIPCNetworkPara.SingleNetworkInterface[0].NetMask,sdk_netwok_para->mask,MAX_LEN);
	pIPCNetworkPara.SingleNetworkInterface[0].DHCP=false;
	ret=ONVIF_SetNetworkInterfaces2(index,&remoteIpc ,&pIPCNetworkPara);
	if(ret!=1)
		return -1;
	return sizeof(sdk_modify_ip);

}


int Remote_SetIPCNetworkParaByPrivate(int channel , sdk_modify_ip* sdk_netwok_para)
{

	if (channel < 0 || channel >= MAX_DISPLAY_NUM || !sdk_netwok_para)
	{
		return 0;
	}
	DSC_MODIFY_INFO  private_info;
	memset(&private_info,0,sizeof(DSC_MODIFY_INFO));

	snprintf(private_info.dstInfo.ucIpAddr, sizeof(private_info.dstInfo.ucIpAddr), sdk_netwok_para->old_ip_addr);
	snprintf(private_info.dstInfo.ucMacAddr,sizeof(private_info.dstInfo.ucMacAddr), sdk_netwok_para->macaddr);  
	snprintf(private_info.networkInfo.ucIpAddr,sizeof(private_info.networkInfo.ucIpAddr),sdk_netwok_para->new_ip_addr);
	snprintf(private_info.networkInfo.ucGatewayAddr,sizeof(private_info.networkInfo.ucGatewayAddr),sdk_netwok_para->gateway);
	snprintf(private_info.networkInfo.ucNetmaskAddr,sizeof(private_info.networkInfo.ucNetmaskAddr),sdk_netwok_para->mask);
	snprintf(private_info.securityInfo.ucUsername,sizeof(private_info.securityInfo.ucUsername), sdk_netwok_para->user_name);  
	snprintf(private_info.securityInfo.ucPassword,sizeof(private_info.securityInfo.ucPassword), sdk_netwok_para->user_pwd);
	Printf("newip =%s, destip =%s, destmacaddr =%s ,username =%s,password=%s\r\n",
			private_info.networkInfo.ucIpAddr,
			private_info.dstInfo.ucIpAddr,
			private_info.dstInfo.ucMacAddr,
			private_info.securityInfo.ucUsername,
			private_info.securityInfo.ucPassword);
	private_info.networkInfo.ulMemberIsValid= DSC_NETWORK_IP_VALID ;
	DD_ModifyDevNetConfig(&private_info);  
	return 0;

}


int main_msg_IPC_NetworkPara(sdk_msg_t *pmsg)
{
	//Printf("[main_msg_IPC_NetworkPara] :%d", pmsg->chann);
	int opera = pmsg->args;
	int chno =  pmsg->chann;
	int type = pmsg->extend;

	sdk_modify_ip *sdk_netwok_para = (sdk_modify_ip*) pmsg->data;

	if ( CHANN_TYPE_ALL == chno ) {
		pmsg->size = 0;
		return 0;
	}

#if 1
	if ( SDK_PARAM_SET == opera ) 
	{

		if(MANUFACT_ONVIF==type)
			{
				pmsg->size =  Remote_SetIPCNetworkPara( chno ,sdk_netwok_para->index, sdk_netwok_para);
				if(-1==pmsg->size)
				{
					pmsg->ack=-1;
				}
			}
		else if (MANUFACT_PRIVATE==type)
		{
			pmsg->size =  Remote_SetIPCNetworkParaByPrivate( chno , sdk_netwok_para);
			if(-1==pmsg->size)
			{
				pmsg->ack=-1;
			}
		}
	}
#endif

	return 0;
}

int  Remote_GetIPCDevInfo( int channel , sdk_ipc_devinfo_t* sdk_ipc_devpara)
{
	if (channel < 0 || channel >= MAX_DISPLAY_NUM || !sdk_ipc_devpara)
	{
		return 0;
	}
	int ret ;
	Remote_Device_Info remoteIpc;
	ret = DbMainMsgChannelInfo(channel,remoteIpc.ip,&remoteIpc.port,remoteIpc.userName,remoteIpc.password);
	if(-1==ret)
	{
		Printf( "DbMainMsgChannelInfo get channel failed\n");
		return 0;
	}	  
	Onvif_DeviceInfo pGetDevinfoRespone;
	pGetDevinfoRespone.Status = false;
	ONVIF_AddRemoteDevice(channel,&remoteIpc);
	ONVIF_GetDeviceInformationFromCache(&remoteIpc, &pGetDevinfoRespone);
	if(pGetDevinfoRespone.Status)
	{
		if(isalnum(pGetDevinfoRespone.FirmwareVersion[0]))
		{
			strncpy(sdk_ipc_devpara->FirmwareVersion,pGetDevinfoRespone.FirmwareVersion,127);
		}
		if(isalnum(pGetDevinfoRespone.HardwareId[0]))
		{
			strncpy(sdk_ipc_devpara->HardwareId,pGetDevinfoRespone.HardwareId,127);
		} 
		if(isalnum(pGetDevinfoRespone.Manufacturer[0]))
		{
			strncpy(sdk_ipc_devpara->Manufacturer,pGetDevinfoRespone.Manufacturer,127);
		}
		if(isalnum(pGetDevinfoRespone.Model[0]))
		{
			strncpy(sdk_ipc_devpara->Model,pGetDevinfoRespone.Model,127);
		}
		if(isalnum(pGetDevinfoRespone.SerialNumber[0]))
		{
			strncpy(sdk_ipc_devpara->SerialNumber,pGetDevinfoRespone.SerialNumber,127);
		}
	}
	return sizeof(sdk_ipc_devinfo_t);

}



int main_msg_IPC_DeviceInfo(sdk_msg_t *pmsg)
{
	//Printf("[main_msg_IPC_IPC_DeviceInfo] :%d", pmsg->chann);
	int opera = pmsg->args;
	int chno =  pmsg->chann;
	int type = pmsg->extend ;

	sdk_ipc_devinfo_t *sdk_ipc_devInfo = (sdk_ipc_devinfo_t*)pmsg->data;

	if ( CHANN_TYPE_ALL == chno ) {
		pmsg->size = 0;
		return 0;
	}

	if ( SDK_PARAM_GET == opera )
	{	
		if(MANUFACT_ONVIF == type)
		{
			pmsg->size =Remote_GetIPCDevInfo( chno ,sdk_ipc_devInfo );
			if(-1==pmsg->size)
			{
				pmsg->ack=-1;
			}
		}
		else if(MANUFACT_RTSP ==type )
		{
			memset(sdk_ipc_devInfo,0,sizeof(sdk_ipc_devinfo_t));
			pmsg->size = sizeof(sdk_ipc_devinfo_t);
		}
		else if(MANUFACT_PRIVATE ==type )
		{
			memset(sdk_ipc_devInfo,0,sizeof(sdk_ipc_devinfo_t));
			pmsg->size = sizeof(sdk_ipc_devinfo_t);
		}
	} 

#if 0
	else if ( SDK_PARAM_SET == opera ) {
		pmsg->size =  Remote_SetIPCNetworkPara( chno , sdk_netwok_para);
		if(-1==pmsg->size)
		{
			pmsg->ack=-1;
		}
	}
#endif

	return 0;
}

int main_msg_auto_maintain(sdk_msg_t *pmsg)
{
	int ret = 0;
	//Printf("pmsg->msg_id =%d pmsg->args =%d\n",pmsg->msg_id,pmsg->args);
	ret = databaseOperate(pmsg->msg_id, pmsg->args, NULL, NULL, pmsg->data, pmsg->size);
	if(ret == -1)
	{
		pmsg->size = 0;
		pmsg->ack = -1;
	}
	else
	{
		pmsg->size = ret;
	}
	return 0;
}

#define MAX_IP 254
#define MIN_IP 2 

void  SetNetModifyResponse(NetConfigResponse * pResponse)
{
	snprintf(g_netConfigResponse.ucIpAddr,sizeof(g_netConfigResponse.ucIpAddr),pResponse->ucIpAddr);
	g_netConfigResponse.ucErrorCode = pResponse->ucErrorCode ;
	g_netConfigResponse.state = pResponse->state;

}



bool Ping( char * ipaddr,int sec)
{
	char pCmd[128];
	char tmp_buf[1024] = {0};
	FILE *fp;
	sprintf(pCmd,"ping -w %d %s" ,sec,ipaddr);
	char * q =NULL;
	fp = popen(pCmd, "r");
	while(!feof(fp))
	{
		fgets(tmp_buf, sizeof(tmp_buf), fp);
		q = strstr(tmp_buf,"seq=");
		if(q)
		{
			fprintf(stderr,"%s \r\n",tmp_buf);
			pclose(fp);
			return true;
		}
	}
	pclose(fp);
	return false;
}

int private_protocol_ad_hoc_network(int all_num,int * added_num,sdk_network_t * network_para,char * last_ip)
{
	memset(&privateDev, 0, sizeof(privateDev));	
	privatedevNum = 0;
	DD_SearchDev();
	sleep(3);
	int allprivatedev = privatedevNum;
	int trytime = 0;
	int a,b,c,ip_first;
	int i = 0 ;
	int j = 0  ;
	DSC_MODIFY_INFO  private_info;
	sdk_channel_t  channel_para;
	char ip_addr[MAX_IP_ADDR_LEN];
	if(network_para->ip_addr)
	{
		sscanf(network_para->ip_addr,"%d.%d.%d.%d",&a,&b,&c,&ip_first);
	}
	if(ip_first>MAX_IP || ip_first < MIN_IP )
	{
		Printf("ip overload !!!! \r\n");
		return  -1;
	}

	//	 fprintf(stderr,"%s:%d: devnum = %d: ipfirst = %d\r\n",__FILE__,__LINE__,allprivatedev,ip_first);
	for(i=0;i!=allprivatedev;++i)
	{       
		do
		{
			if((MAX_IP==ip_first) ||j==all_num)
			{
				fprintf(stderr,"%s:%d ,come here num =%d \r\n",__func__,__LINE__,all_num );
				return 0;	
			}
			trytime = 8;
			sprintf(ip_addr,"%d.%d.%d.%d",a,b,c,++ip_first);
			if(! Ping(ip_addr,1))
			{
				memset(private_info.securityInfo.ucPassword,0,sizeof(private_info.securityInfo.ucPassword));
				snprintf(private_info.dstInfo.ucIpAddr, sizeof(private_info.dstInfo.ucIpAddr), privateDev[i].ip_addr);
				snprintf(private_info.dstInfo.ucMacAddr,sizeof(private_info.dstInfo.ucMacAddr), privateDev[i].macaddr);  
				snprintf(private_info.networkInfo.ucIpAddr,sizeof(private_info.networkInfo.ucIpAddr),ip_addr);
				//strncpy(private_info.srcInfo.ucIpAddr,network_para->ip_addr,DSC_IPV4ADDR_LEN);
				//strncpy(private_info.srcInfo.ucMacAddr,network_para->macaddr,DSC_MACADDR_LEN);
				snprintf(private_info.networkInfo.ucGatewayAddr,sizeof(private_info.networkInfo.ucGatewayAddr),network_para->gateway);
				snprintf(private_info.networkInfo.ucNetmaskAddr,sizeof(private_info.networkInfo.ucNetmaskAddr),network_para->netmask);
				snprintf(private_info.securityInfo.ucUsername,sizeof(private_info.securityInfo.ucUsername), privateDev[i].user.user_name);	
				snprintf(private_info.securityInfo.ucPassword,sizeof(private_info.securityInfo.ucPassword), privateDev[i].user.user_pwd);
#if 0
				Printf("newip =%s, destip =%s, destmacaddr =%s ,username =%s,password=%s\r\n",
						private_info.networkInfo.ucIpAddr,
						private_info.dstInfo.ucIpAddr,
						private_info.dstInfo.ucMacAddr,
						private_info.securityInfo.ucUsername,
						private_info.securityInfo.ucPassword);
				private_info.networkInfo.ulMemberIsValid= DSC_NETWORK_IP_VALID ;
#endif
				private_info.networkInfo.ulMemberIsValid= DSC_NETWORK_IP_VALID ;
				DD_ModifyDevNetConfig(&private_info);  
				do{
					usleep(1000*500);
					--trytime;
				}while((g_netConfigResponse.state==-1)&&(trytime));
				if((0!=strncmp((char*)g_netConfigResponse.ucIpAddr,(char*)private_info.networkInfo.ucIpAddr,DSC_IPV4ADDR_LEN)) || (g_netConfigResponse.ucErrorCode !=0))
				{
					fprintf(stderr,"channel  =%d modify  ip  failed !!!!!!!!!!!! ip = %s ,code =%d  !!!!!!!!!!!!!!!!!\r\n",j ,g_netConfigResponse.ucIpAddr,g_netConfigResponse.ucErrorCode);
					break;
				}
				g_netConfigResponse.state== -1;
				channel_para.enable = 1;
				channel_para.chann_no = j ;
				channel_para.chann_win_mode =1;
				channel_para.port = privateDev[i].port;
				channel_para.manufacturer = MANUFACT_PRIVATE;
				strncpy(channel_para.ip_addr,ip_addr,MAX_IP_ADDR_LEN);
				strncpy(channel_para.user.user_name,privateDev[i].user.user_name,MAX_USER_NAME_LEN);
				strncpy(channel_para.user.user_pwd,privateDev[i].user.user_pwd,MAX_USER_NAME_LEN);		
				databaseOperate(SDK_MAIN_MSG_CHANN_CFG,SDK_PARAM_SET,&j,sizeof(int),&channel_para,sizeof(sdk_channel_t));
				strncpy(allDev[j].ip_addr,privateDev[i].ip_addr,MAX_USER_NAME_LEN);		 
				*added_num = ++j;
				if(i==allprivatedev-1)
				{
					strncpy(last_ip,ip_addr,MAX_IP_ADDR_LEN); 	
				}
				break;
			}		

		} while(1);

	}
	// fprintf(stderr,"%s:%d ,come here \r\n",__func__,__LINE__);
	if(*added_num == 0)
	{
		strncpy(last_ip,network_para->ip_addr,MAX_IP_ADDR_LEN); 
	}

}

bool  IPCompare(char * ip_addr,int num)
{
	int k ;
	for(k=0;k!=num;++k)
	{
		if(0==strncmp(ip_addr,allDev[k].ip_addr,MAX_IP_ADDR_LEN))
		{
			return true;
		} 
	}
	return false;
}


int onvif_ad_hoc_network(int display_num,int added_num,sdk_network_t * network_para,char * ipaddr)
{

	// fprintf(stderr,"%s:%d  added_num =%d \r\n",__func__,__LINE__,added_num);
	memset(&onvifDev, 0, sizeof(onvifDev));
	devNum = 0;
	NetworkInterfaces  SignleInterface;
	char newIPaddr[MAX_IP_ADDR_LEN];
	if(StartDiscovery(onvifDeviceInfo,false) != 0)
	{
		fprintf(stderr,"start discovery error\r\n");
	}
	sleep(2);
	int allonvifdev = devNum;
	int i = 0,j = added_num  ;
	int a, b,c,ip_first;
	int ret = 0;
	if(ipaddr)
	{
		sscanf(ipaddr,"%d.%d.%d.%d",&a,&b,&c,&ip_first);
	}
	if(ip_first>MAX_IP || ip_first < MIN_IP )
	{
		return  -1;
	}
	Remote_Device_Info pDeviceInfo;
	sdk_channel_t  channel_para;
	//

	for(i=0;i!=allonvifdev;++i)
	{
		do
		{          	    
			if((MAX_IP==ip_first) ||(j==display_num))
				return 0;
			if(IPCompare(onvifDev[i].ip_addr,added_num))
				break;						
			sprintf(newIPaddr,"%d.%d.%d.%d",a,b,c,++ip_first);	 
			if(! Ping(newIPaddr,1))
			{
				strncpy(pDeviceInfo.ip,onvifDev[i].ip_addr,MAX_IP_ADDR_LEN);
				pDeviceInfo.port = onvifDev[i].port;
				strncpy(pDeviceInfo.userName,"admin",MAX_USER_NAME_LEN);
				strncpy(pDeviceInfo.password,
						"admin",MAX_USER_NAME_LEN);
				strncpy(SignleInterface.SingleNetworkInterface[0].IpAddress,newIPaddr,MAX_USER_NAME_LEN);
				strcpy(SignleInterface.SingleNetworkInterface[0].InterfaceName,"eth0");
				strcpy(SignleInterface.SingleNetworkInterface[0].token,"eth0");
				strncpy(SignleInterface.SingleNetworkInterface[0].NetMask,network_para->netmask,MAX_USER_NAME_LEN);	
				ret = ONVIF_SetNetworkInterfaces(&pDeviceInfo,&SignleInterface);

#if 0
				fprintf(stderr,"oldip =%s, port =%d ,user=%s ,password=%s ,newip =%s mask =%s\r\n",
						pDeviceInfo.ip,
						pDeviceInfo.port,
						pDeviceInfo.userName,
						pDeviceInfo.password,
						SignleInterface.SingleNetworkInterface[0].IpAddress,
						SignleInterface.SingleNetworkInterface[0].NetMask

				       );
#endif
				if(-1==ret)
				{
					fprintf(stderr,"ONVIF_SetNetworkInterfaces failed \r\n");
					break;
				}
				//Printf("ONVIF_SetNetworkInterfaces\r\n");
				//strncpy(pDeviceInfo.ip,newIPaddr,MAX_IP_ADDR_LEN);
				//ONVIF_SetDefaultGateway(&pDeviceInfo,network_para->gateway);	
				channel_para.enable = 1;
				channel_para.chann_no = j;
				channel_para.chann_win_mode =1;
				channel_para.port = onvifDev[i].port;
				channel_para.manufacturer = MANUFACT_ONVIF;
				strncpy(channel_para.ip_addr,newIPaddr,MAX_IP_ADDR_LEN);
				strncpy(channel_para.user.user_name,onvifDev[i].user.user_name,MAX_USER_NAME_LEN);
				strncpy(channel_para.user.user_pwd,onvifDev[i].user.user_pwd,MAX_USER_NAME_LEN);			 
				databaseOperate(SDK_MAIN_MSG_CHANN_CFG,SDK_PARAM_SET,&j,sizeof(int),&channel_para,sizeof(sdk_channel_t));			   
				++j;
				usleep(1000);
				break;
			}
			usleep(1000);
		} while(1);
		if(j==display_num)
		{
			fprintf(stderr,"j=%d,num=%d\r\n",j , display_num);
			break;
		}

	}


	return 0;
}

static Ad_Hoc_Network_status  RunningStatus = STATUS_INIT;

static void * Ad_Hoc_Network(void *param)
{
	int ret =0;
	int display_num =  configGetDisplayNum();
	char first_ipaddr[MAX_IP_ADDR_LEN] ;
	char  last_ipaddr[MAX_IP_ADDR_LEN];
	char  gateway[MAX_IP_ADDR_LEN];
	char  netmask[MAX_IP_ADDR_LEN];
	char  macaddr[64];
	int int_ip = 0;
	int  added_num = 0 ;
	sdk_network_t local_net_para;
	struct in_addr  addr1;
	DSC_NETWORK_INFO pInfo;
	ret = DD_InitNetworkInfo(&pInfo);
	if(ret!=0)
	{
		Printf("ip get failed \r\n");
		return NULL;
	}

	strncpy(local_net_para.ip_addr,pInfo.ucIpAddr,MAX_IP_ADDR_LEN);
	strncpy(local_net_para.gateway,pInfo.ucGatewayAddr,MAX_IP_ADDR_LEN);
	strncpy(local_net_para.netmask,pInfo.ucNetmaskAddr,MAX_IP_ADDR_LEN);
	strncpy(local_net_para.macaddr,pInfo.ucMacAddr,MAX_IP_ADDR_LEN);

	RunningStatus = STATUS_RUNNING;
	g_netConfigResponse.state==-1;
	private_protocol_ad_hoc_network(display_num,&added_num,&local_net_para,last_ipaddr);
	//fprintf(stderr,"display_num =%d,added_num=%d,last_ipadd=%s\r\n",display_num,added_num,last_ipaddr);
	if(display_num>added_num)
	{
		onvif_ad_hoc_network(display_num,added_num,&local_net_para,last_ipaddr);
	}	 
	fprintf(stderr," ################Ad_Hoc_Network end!!!!!!!!!!!!!!!!!!!!!!\r\n");
	RunningStatus = STATUS_END;
	return NULL;


}

int Remote_Start_Ad_Hoc_Network()
{     
	static pthread_t threadId;

	int ret = 0;
	ret= checkNetworkStatus("eth0"); 
	if(0==ret)
	{
		Printf("network is down!!!\r\n");
		return -1;
	}
	if(STATUS_RUNNING==RunningStatus)
	{
		Printf("%s:%d STATUS_RUNNING\r\n",__FILE__,__LINE__);
		return -1;
	}

	if(pthread_create(&threadId, NULL, Ad_Hoc_Network, NULL) < 0)
	{
		Printf("Creat thread failed\r\n");
		return -1;
	}  

	return 0;
}

int main_msg_auto_bind(sdk_msg_t *pmsg)
{
	int opera = pmsg->args;
	//if ( SDK_PARAM_SET == opera )
	{	 
		pmsg->size =Remote_Start_Ad_Hoc_Network();
		if(-1==pmsg->size)
		{
			pmsg->ack=-1;
		}
	} 
#if 0
	else if ( SDK_PARAM_SET == opera ) 
	{
		pmsg->size =  Remote_Cancel_Ad_Hoc_Network();
		if(-1==pmsg->size)
		{
			pmsg->ack=-1;
		}
	}
#endif
	return 0;

}


#if 1




int main_msg_GetMotionDetectionSensitivity(int channel ,sdk_MotionDetectionAnalytics_t * pSensitivity )
{

  if (channel < 0 || channel > MAX_DISPLAY_NUM|| !pSensitivity) 
	{
		return 0;
	}
	int ret ;
	Remote_Device_Info remoteIpc;
	sdk_MotionDetectionAnalytics_t  pSensitivityTemp;
	ret = DbMainMsgChannelInfo(channel,remoteIpc.ip,&remoteIpc.port,remoteIpc.userName,remoteIpc.password);
	if(-1==ret)
	{
		Printf( "DbMainMsgChannelInfo   get channel failed\n");
		return 0;
	}
	ONVIF_AddRemoteDevice(channel,&remoteIpc);
	Onvif_Profiles getProfilesRespone = { 0 };
	ret = ONVIF_GetProfilesFromCache(&remoteIpc, &getProfilesRespone);
    ret= ONVIF_GetMotionDetectionSensitivity(&remoteIpc,getProfilesRespone.Onvif_Profiles[0].videoAnalyticsConfig.token,&pSensitivityTemp);   
	return ret;

}

int main_msg_SetMotionDetectionSensitivity(int channel ,sdk_MotionDetectionAnalytics_t * pSensitivity )
{

  if (channel < 0 || channel > MAX_DISPLAY_NUM|| !pSensitivity) 
	{
		return 0;
	}
	int ret ;
	Remote_Device_Info remoteIpc;
	sdk_MotionDetectionAnalytics_t  pSensitivityTemp;
	ret = DbMainMsgChannelInfo(channel,remoteIpc.ip,&remoteIpc.port,remoteIpc.userName,remoteIpc.password);
	if(-1==ret)
	{
		Printf( "DbMainMsgChannelInfo   get channel failed\n");
		return 0;
	}
	ONVIF_AddRemoteDevice(channel,&remoteIpc);
	Onvif_Profiles getProfilesRespone = { 0 };
	ret = ONVIF_GetProfilesFromCache(&remoteIpc, &getProfilesRespone);
    ret= ONVIF_GetMotionDetectionSensitivity(&remoteIpc,getProfilesRespone.Onvif_Profiles[0].videoAnalyticsConfig.token,&pSensitivityTemp);   

	pSensitivity->Column= 	pSensitivityTemp.Column ;
	pSensitivity->Row= 	pSensitivityTemp.Row;
	ret= ONVIF_SetMotionDetectionSensitivity(&remoteIpc,getProfilesRespone.Onvif_Profiles[0].videoAnalyticsConfig.token,pSensitivity);
	return ret;

}



int main_msg_SetMotionDetectionArea(int channel , sdk_MotionDetectionRule_t  *pruleInfo  )
{

  if (channel < 0 || channel > MAX_DISPLAY_NUM|| !pruleInfo) 
	{
		return 0;
	}
	int ret ;
	Remote_Device_Info remoteIpc;
	ret = DbMainMsgChannelInfo(channel,remoteIpc.ip,&remoteIpc.port,remoteIpc.userName,remoteIpc.password);
	if(-1==ret)
	{
		Printf( "DbMainMsgChannelInfo   get channel failed\n");
		return 0;
	}
	ONVIF_AddRemoteDevice(channel,&remoteIpc);
	Onvif_Profiles getProfilesRespone = { 0 };
	ret = ONVIF_GetProfilesFromCache(&remoteIpc, &getProfilesRespone); 
	ONVIF_SetMotionDetectionRule_S(&remoteIpc,getProfilesRespone.Onvif_Profiles[0].videoAnalyticsConfig.token,&pruleInfo);
	return ret;

}





int main_msg_GetMotionDetectionArea(int channel , sdk_MotionDetectionRule_t  *pruleInfo  )
{

  if (channel < 0 || channel > MAX_DISPLAY_NUM|| !pruleInfo) 
	{
		return 0;
	}
	int ret ;
	Remote_Device_Info remoteIpc;
	ret = DbMainMsgChannelInfo(channel,remoteIpc.ip,&remoteIpc.port,remoteIpc.userName,remoteIpc.password);
	if(-1==ret)
	{
		Printf( "DbMainMsgChannelInfo   get channel failed\n");
		return 0;
	}
	ONVIF_AddRemoteDevice(channel,&remoteIpc);
	Onvif_Profiles getProfilesRespone = { 0 };
	ret = ONVIF_GetProfilesFromCache(&remoteIpc, &getProfilesRespone); 
	ONVIF_GetMotionDetectionRule(&remoteIpc,getProfilesRespone.Onvif_Profiles[0].videoAnalyticsConfig.token,&pruleInfo);
	return ret;

}


#endif





static msg_desc_t _msg_desc[SDK_MAIN_MSG_BUTT+1] =
{
	{0                             , NULL                    , NULL    ,"NULL"}
	,{SDK_MAIN_MSG_VERSION          , main_msg_version       , NULL    ,"MSG_VERSION"}          //    sdk_version_t 
	,{SDK_MAIN_MSG_STATUS           , NULL/*main_msg_status*/        , NULL    ,"MSG_STATUS"}           //    sdk_status_t
	,{SDK_MAIN_MSG_PARAM_DEFAULT    , main_msg_param_def     , NULL    ,"MSG_PARAM_DEFAULT"}    //    sdk_default_param_t, SDK_PARAM_MASK_ID_E
	,{SDK_MAIN_MSG_UPGRAD           , main_msg_upgrad        , NULL    ,"MSG_UPGRAD"}           //        SDK_UPGRAD_OP_E
	,{SDK_MAIN_MSG_USER             , main_msg_user          , NULL    ,"MSG_USER"}             //    SDK_USER_OP_E
	,{SDK_MAIN_MSG_LOG              , main_msg_log/*main_msg_log*/           , NULL    ,"MSG_LOG"}              //        SDK_LOG_OP_E
	,{SDK_MAIN_MSG_SYS_CFG          , main_msg_sys_cfg       , NULL    ,"MSG_SYS_CFG"}          //    sdk_sys_cfg_t
	,{SDK_MAIN_MSG_NET_MNG_CFG      , main_msg_net_mng_cfg /*main_msg_net_mng_cfg*/   , NULL    ,"MSG_NET_MNG_CFG"}          //    sdk_net_mng_cfg_t
	,{SDK_MAIN_MSG_PTZ_CFG          , NULL/*main_msg_ptz_cfg*/       , NULL/*remote_msg_ptz_cfg*/ ,"MSG_PTZ_CFG"}          //    sdk_ptz_param_t
	,{SDK_MAIN_MSG_PTZ_CONTROL      , msg_ptz_dispatch/*msg_ptz_dispatch*/       , NULL/*remote_msg_ptz_dispatch*/    ,"MSG_PTZ_CONTROL"}      //
	,{SDK_MAIN_MSG_CRUISE_CFG       , main_msg_cruise_cfg/*main_msg_cruise_cfg*/    , NULL/*remote_msg_cruise_cfg*/    ,"MSG_CRUISE_CFG"}       //    sdk_cruise_param_t
	,{SDK_MAIN_MSG_PREVIEW_CFG      , main_msg_preview_cfg   , NULL    ,"MSG_PREVIEW_CFG"}      // sdk_preview_t
	,{SDK_MAIN_MSG_VO_BINDING       , main_msg_vo_binding    , NULL    ,"MSG_VO_BINDING"}       //(/) sdk_vo_binding_t
	,{SDK_MAIN_MSG_PB_GROUP_CREATE  , main_msg_group_new     , NULL    ,"MSG_PB_GROUP_CREATE"}  // sdk_pb_group_t
	,{SDK_MAIN_MSG_PB_GROUP_DESTORY , main_msg_group_del     , NULL    ,"MSG_PB_GROUP_DESTORY"} //
	,{SDK_MAIN_MSG_PB_GROUP_CONTROL , main_msg_group_ctl     , NULL    ,"MSG_PB_GROUP_CONTROL"} // SDK_PB_GROUP_CONTROL_E
	,{SDK_MAIN_MSG_CHANN_CFG        , main_msg_chann_cfg     , NULL    ,"MSG_CHANN_CFG"}        //       sdk_channel_t
	,{SDK_MAIN_MSG_ENCODE_CFG       , main_msg_encode_cfg /*main_msg_encode_cfg*/    , NULL/*remote_msg_encode_cfg*/    ,"MSG_ENCODE_CFG"}       //       sdk_encode_t
	,{SDK_MAIN_MSG_RECORD_CFG       , main_msg_record_cfg    , NULL    ,"MSG_RECORD_CFG"}       //       sdk_record_cfg_t
	,{SDK_MAIN_MSG_RECORD_QUERY     , main_msg_record_query  , NULL    ,"MSG_RECORD_QUERY"}     //       sdk_record_item_t, sdk_record_cond_t
	,{SDK_MAIN_MSG_MOTION_CFG       , main_msg_motion_cfg    , NULL/*remote_msg_motion_cfg*/    ,"MSG_MOTION_CFG"}     //   sdk_motion_cfg_t
	,{SDK_MAIN_MSG_ALARM_IN_CFG     , main_msg_alarm_in_cfg  , NULL    ,"MSG_ALARM_IN_CFG"}     //sdk_alarm_in_cfg_t  //hongmb:,pu_MsgHandle 1650 
	,{SDK_MAIN_MSG_HIDE_CFG         , NULL/*main_msg_hide_cfg*/      , NULL/*remote_msg_hide_alarm_cfg*/    ,"MSG_HIDE_CFG"}         //   sdk_hide_cfg_t
	,{SDK_MAIN_MSG_LOST_CFG         , NULL/*main_msg_lost_cfg*/      , NULL/*remote_msg_lost_cfg*/    ,"MSG_LOST_CFG"}         //   sdk_lost_cfg_t
	,{SDK_MAIN_MSG_OSD_CFG          , NULL/*main_msg_osd_cfg*/       , NULL/*remote_msg_osd_cfg*/    ,"MSG_OSD_CFG"}          //OSD                sdk_osd_cfg_t
	,{SDK_MAIN_MSG_COMM_CFG         , main_msg_comm_cfg          , NULL    ,"MSG_COMM_CFG"}         //         sdk_comm_cfg_t
	,{SDK_MAIN_MSG_OVERLAY_CFG      , NULL/*main_msg_overlay_cfg*/   , NULL/*remote_msg_overlay_cfg*/    ,"MSG_OVERLAY_CFG"} //    sdk_overlay_cfg_t
	,{SDK_MAIN_MSG_DISK                 , main_msg_disk/*main_msg_disk*/                  , NULL    ,"MSG_DISK"}                  //          SDK_DISK_OP_E
	,{SDK_MAIN_MSG_SYSTIME         ,     main_msg_systim     , NULL    ,"MSG_SYSTIME"}       //        sdk_time_t
	,{SDK_MAIN_MSG_SERIAL_CFG       , NULL/*main_msg_serial_cfg*/    , NULL    ,"MSG_SERIAL_CFG"}        //        sdk_serial_func_cfg_t
	,{SDK_MAIN_MSG_IMAGE_ATTR_CFG       , main_msg_image_attr_cfg /*main_msg_image_attr_cfg*/ , NULL/*remote_msg_image_attr_cfg*/,"MSG_IMAGE_ATTR_CFG"} //   sdk_image_attr_t; hongmb:
	,{SDK_MAIN_MSG_TV_CFG                       , NULL/*main_msg_tv_cfg*/        , NULL/*remote_msg_do_nothing*/    ,"MSG_TV_CFG"}               //TV   sdk_vo_sideSize_t
	,{SDK_MAIN_MSG_NOTIFY           , NULL/*main_msg_notify*/            , NULL    ,"MSG_NOTIFY"}                // / args:SDK_EVENT_TYPE_E, chann:, extend:/
	,{SDK_MAIN_MSG_DEVLIST          , main_msg_devlist      , NULL    ,"MSG_DEVLIST"}          // sdk_device_t
	,{SDK_MAIN_MSG_MANUAL_REC           , NULL/*main_msg_manual_rec*/    , NULL    ,"MSG_MANUAL_REC"}            //() sdk_manual_record_t
	,{SDK_MAIN_MSG_MANUAL_ALMIN         , NULL/*main_msg_manual_almin*/  , NULL/*remote_msg_do_nothing*/    ,"MSG_MANUAL_ALMIN"}         //() sdk_manual_alarmin_t
	,{SDK_MAIN_MSG_MANUAL_ALMOUT        , NULL/*main_msg_manual_almout*/ , NULL/*remote_msg_do_nothing*/    ,"MSG_MANUAL_LAMOUT"}        //() sdk_manual_alarmout_t
	,{SDK_MAIN_MSG_RECORD_BACKUP        , main_msg_record_backup , NULL    ,"MSG_RECORD_BACKUP"}         //  SDK_BACKUP_OP_E
	,{SDK_MAIN_MSG_CLOSE_SYSTEM         , main_msg_close_system  , NULL    ,"MSG_CLOSE_SYSTEM"}          //  SDK_CLOSE_SYS_OP_E
	,{SDK_MAIN_MSG_AUDIO_CONTROL        , main_msg_audio_control , NULL    ,"MSG_AUDIO_CONTROL"}         //  SDK_AUDIO_CONTROL_OP_E
	,{SDK_MAIN_MSG_PLATFORM_CFG     , NULL/*main_msg_platform_cfg*/  , NULL    ,"MSG_PLATFORM_CFG"}      //sdk_platform_t
	,{SDK_MAIN_MSG_CHANGE_REMOTE_IP , NULL/*_main_msg_ch_ip*/        , NULL             ,"MSG_CHANGE_REMOTE_IP"} //IP
	,{SDK_MAIN_MSG_DDNS_CFG         , main_msg_ddns_cfg      , NULL    ,"MSG_DDNS_CFG"}      // DDNS sdk_ddns_cfg_t
	,{SDK_MAIN_MSG_CAPABILITY           , NULL/*main_msg_capability*/    , NULL    ,"MSG_CAPABILITY"}    // sdk_device_capability_t
	,{SDK_MAIN_MSG_NET_LINK_CFG         ,main_msg_net_link_cfg/*main_msg_net_link_cfg*/          , NULL    ,"MSG_NET_LINK_CFG"}          //PPPOE SDK_NET_IDX_E
	,{SDK_MAIN_MSG_UPNP_CFG         , main_msg_upnp_cfg,       NULL,    "MSG_UPNP_CFG"}
	,{SDK_MAIN_MSG_SCREEN_CFG       , NULL                   , NULL     ,"MSG_SCREEN_CFG"}
	,{SDK_MAIN_MSG_CRUISE_CTL       , main_msg_cruise_begin                   , NULL/*remote_msg_cruise_begin*/       ,"MSG_CRUISE_CTL"}//
	,{SDK_MAIN_MSG_PRESET_SET       , remote_msg_preset_get                   , NULL/*remote_msg_preset_get*/         ,"MSG_PRESET_GET"}//sdk_preset_param_t
	,{SDK_MAIN_MSG_PREVIEW_TOUR     , NULL/*main_msg_tour_cfg*/      , NULL                          , "MSG_PREVIEW_TOUR"} // sdk_tour_t
	,{SDK_MAIN_MSG_28181            ,NULL,NULL,"MSG_28181"}
	,{SDK_MAIN_MSG_NTP              ,main_msg_ntp,NULL,"MSG_NTPD"}
	,{SDK_MAIN_MSG_SMTP             ,main_msg_smtp,NULL,"MSG_SMTP"}
	,{SDK_MAIN_MSG_ZOOM             ,main_msg_Zoom,NULL,"MSG_ZOOM"}
	,{SDK_MAIN_MSG_GET_CHANNEL_BITRATE             ,main_msg_getChannelBitrate,NULL,"MSG_CHANNEL_BITRATE"}
	,{SDK_MAIN_MSG_P2P             ,main_msg_p2p,NULL,"MSG_P2P"}
	,{SDK_MAIN_MSG_LOCAT_IMAGE_ATTR_CFG,main_msg_locat_image_attr_cfg,NULL,"MSG_LOCAT_IMAGE_ATTR"}
	,{SDK_MAIN_MSG_ROTATE,main_msg_rotate_attr_cfg,NULL,"MSG_TOTATE_ATTR"}
	,{SDK_MAIN_MSG_AUTO_BIND,main_msg_auto_bind,NULL,"MSG_AUTO_BIND"}
	,{SDK_MAIN_MSG_PRESET,main_msg_Preset,NULL,"MSG_PRESET"}
	,{SDK_MAIN_MSG_ALLPRESETS,main_msg_AllPresets,NULL,"MSG_ALLPRESETS"}
	,{SDK_MAIN_MSG_CRUISECFG,main_msg_CruisePresets,NULL,"MSG_CRUISECFG"}
	,{SDK_MAIN_MSG_PRI_SEARCH,NULL,NULL,"MSG_PRI_SEARCH"}
	,{SDK_MAIN_MSG_CHL_SWITCH,NULL,NULL,"CHL_SWITCH"}
	,{SDK_MAIN_MSG_IPC_NETWORK_CFG,main_msg_IPC_NetworkPara,NULL,"MSG_IPC_NETWORK_CFG"}
	,{SDK_MAIN_MSG_IPC_DEVICE_INFO,main_msg_IPC_DeviceInfo,NULL,"MSG_IPC_DEVICE_INFO"}
	,{SDK_MAIN_MSG_AUTO_MAINTAIN,main_msg_auto_maintain,NULL,"MSG_NVR_AUTO_MAINTAIN"}
	,{SDK_MAIN_MSG_RECORD_QUERY_STATUS, main_msg_record_query_status  , NULL    ,"MSG_RECORD_QUERY"}     //       sdk_record_item_t, sdk_record_cond_t
	,{SDK_MAIN_MSG_GET_P2P_STATUS, main_msg_p2p_get_status  , NULL    ,"MSG_RECORD_QUERY"}
	,{SDK_MAIN_MSG_SMTP_TEST_STATUS, main_msg_emil_test_status  , NULL    ,"MSG_RECORD_QUERY"}
	,{SDK_MAIN_MSG_POLL, main_msg_poll  , NULL    ,"MSG_RECORD_QUERY"}
	,{SDK_MAIN_MSG_PB_WIN_SWITCH, main_msg_playback_switch  , NULL    ,"MSG_PB_WIN_SWITCH"}
	,{SDK_MAIN_MSG_FTP, main_msg_ftp  , NULL    ,"MSG_RECORD_QUERY"}
	/*,{SDK_MAIN_MSG_BUTT             , NULL                  ,"NULL"}*/
};



static int _msg_dispatch(sdk_msg_t *pmsg)
{
	int ret = -1;
	int msg_id = pmsg->msg_id;
	int msg_src = pmsg->id.src;
	pmsg->ack = SDK_ERR_SUCCESS;//ack

	//Printf("msg_id = %d \n",msg_id);
	if(msg_id < 0 || msg_id >= SDK_MAIN_MSG_BUTT)
	{
		Printf("error: msg_id:0x%x\n", msg_id);
		goto __error;
	}
#if 0
	if((msg_id != SDK_MAIN_MSG_LOG) && (msg_id != SDK_MAIN_MSG_AUDIO_CONTROL) && (msg_id != SDK_MAIN_MSG_VO_BINDING) && (msg_id != SDK_MAIN_MSG_USER))
	{
		if(pmsg->args == SDK_PARAM_SET)
		{
			//writeLog(LOG_MAJOR_OPERATE, 0, msg_id, "Local gui", "127.0.0.1", _msg_desc[msg_id].desc);
			pmsg->size = databaseOperate(SDK_MAIN_MSG_LOG,0,NULL,0,&msg_id,0);
		}
	}
#endif

#if 0
	Printf("session_id:%d, msg_id:0x%x, pmsg->size=%d\n"
			, pmsg->session_id
			, msg_id
			, pmsg->size);

	if (pmsg->msg_id != SDK_MAIN_MSG_PREVIEW_CFG)
		Printf("session_id:%d, msg_id:0x%x, desc:%s, size=%u, sequence_id=%u, args=%u, ack=%d extend=%u, chann=%u\n"
				, pmsg->session_id
				, msg_id
				, _msg_desc[msg_id].desc
				, pmsg->size
				, pmsg->sequence_id
				, pmsg->args
				, pmsg->ack
				, pmsg->extend
				, pmsg->chann);
#endif

	if(SDK_MAIN_MSG_UPGRAD == msg_id)
	{
		//set upgrad flags;
	}

	if (0)// _msg_desc[msg_id].remote_msg_cb && adapt_param_dev_is_nvr(g_main_dev_type))
	{
		ret = _msg_desc[msg_id].remote_msg_cb(pmsg);
	}
	/*
	   else if (adapt_param_dev_is_dec(g_main_dev_type))
	   {
	   ret = -1;
	   }
	 */
	else
	{
		if(_msg_desc[msg_id].local_msg_cb)
		{
			ret = _msg_desc[msg_id].local_msg_cb(pmsg);
		}
		else
		{
			ret = 1;
		}
	}

	if(ret < 0)
	{
		goto __error;
	}

	if(SDK_MAIN_MSG_UPGRAD == msg_id)
	{
		//reboot wait other module;
	}

	pmsg->id.src = MODULE_ID_MAIN;
	pmsg->id.dst = msg_src;
	pmsg->msg_id = msg_id;
	pmsg->ack    = SDK_ERR_SUCCESS;
	pmsg->f_dir = SDK_MSG_RESP;
	return 0;

__error:
	Printf(">>>>>>>>>>>>>> error.\n");
	pmsg->id.src = MODULE_ID_MAIN;
	pmsg->id.dst = msg_src;
	pmsg->msg_id = msg_id;
	pmsg->f_dir = SDK_MSG_RESP;
	//pmsg->ack = SDK_ERR_FAILURE; // 
	return -1;
}


void startGuiSerVer()
{
	int ret = 0;

	ret = apcs_init(65537, NULL);
	apcs_reg_msg_dispatch(_msg_dispatch);
}

