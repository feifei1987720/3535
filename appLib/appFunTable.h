#ifndef __APPFUNCTABLE_H__
#define __APPFUNCTABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "guiServerLib.h"
#include "sdk_struct.h"
#include "main.h"

void *procMsgThread( void *arg );

#define FILE_NUM 6
typedef struct _MYUpdate_FileInfo
{
	char filename[32];
	unsigned int offset;
	unsigned int size;
}MYUpdate_FileInfo;

typedef struct _MYUpdate_Info
{
	char platform_name[32];
	MYUpdate_FileInfo files[FILE_NUM];
}MYUpdate_Info;

typedef struct msg_desc_s
{
	int  id;
	int(*local_msg_cb)(sdk_msg_t *pmsg);
	int(*remote_msg_cb)(sdk_msg_t *pmsg);
	char desc[128];
} msg_desc_t;

//int main_msg_net_mng_cfg(  msg_desc_t *pMesg );


void startGuiSerVer();
int SearchPrivateDevInfo(int *num, sdk_device_t **ptr);
int Remote_GetEncodeCfg(int channel, sdk_encode_t *pEncode );
int Remote_SetEncodeCfg(int channel, sdk_encode_t *pEncode );
int Remote_GetImageParam(int channel, sdk_image_attr_t * pImg);
int Remote_SetImageParam(int channel, sdk_image_attr_t * pImg);
int Remote_RunPtzCmd(int channel, int opera, sdk_ptz_ctl_t *pPtz, unsigned int extend );
int Remote_GetPtzpresets(int channel, sdk_preset_all_t *pPresets );

#ifdef __cplusplus
}
#endif

#endif  /*__APPFUNCTABLE_H__*/
