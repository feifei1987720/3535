/*
*******************************************************************************
**  Copyright (c) 2012, All rights reserved.
**    
**  : 
*******************************************************************************
*/

#ifndef __APP_LIB_H__
#define __APP_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "param.h"
#include "sdk_struct.h"

#include "appFunTable.h"
#include "midWare.h"

void appLibStart();
void appLibInit();
void appLibDestroy();


void appStartDecodeChannel(sdk_channel_t * pDevChannelInfo);
void appParamSetNetwork(sdk_eth_cfg_t *p_eth_cfg);
int appStartDecode();
int appParamGetResolution(int *resolution, int *encodeMode);
void appParamInit();
int P2pCtrl(int P2pType,int P2pEn);
int P2pInitStart(void);
int DanaleP2pSnUpdate(void);
int hi_vdec_getVideoLayerCSC(sdk_image_attr_t *pVideoCsc);
int hi_vdec_setVideoLayerCSC(sdk_image_attr_t *pVideoCsc);

void syncSystemTime();

int cmdOperate(int type, int subType, void *data, int len, void *pRecvData, int recvDataLen);

void SmtpSetUpdateParamFlag(unsigned char flag);
int GetNetworkFlow(char * interface ,double * recv_rate, double * send_rate);
int  DiskQuery(void *pRecvData);
int DbBackup(void);
void appNtpSettime(void);
#ifdef __cplusplus
}
#endif


#endif//__APP_LIB_H__

