/*
*******************************************************************************
*
**  : 
*******************************************************************************
*/

#ifndef __DBSYSTEM_H__
#define __DBSYSTEM_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "sqlite3.h"
#define TBL_DEFAULT_CFG_SIZE	(sizeof(DefaultCfgArr)/sizeof(DefaultCfg))
typedef int (*DefaultFunc)(void *data, int len, void *pRecvData, int recvDataLen,sqlite3 *db);

typedef struct DefaultCfg{
	int DefaultEnum;
	DefaultFunc DbUpdate_DefaultCfg;
}DefaultCfg;

typedef struct _PortChanInfo 
{
	int prev_tcp_port;
	int cur_tcp_port;
	int prev_rtsp_port;
	int cur_rtsp_port;
	int prev_http_port;
	int cur_http_port;
}PortChanInfo;

int DbInsert_LogCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbSelect_LogCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbDelete_LogCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbUpdate_MotionCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbSelect_MotionCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int GetGlobal_MotionCfg(void *data, int len, void *pRecvData, int recvDataLen);
int DbUpdate_DefaultCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbSelect_SysCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int GetGlobal_SysCfg(void *data, int len, void *pRecvData, int recvDataLen);
int DbUpdate_ChannCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbSelect_ChannCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int GetGlobal_ChannCfg(void *data, int len, void *pRecvData, int recvDataLen);
int DbUpdate_RecordCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbSelect_RecordCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int GetGlobal_RecordCfg(void *data, int len, void *pRecvData, int recvDataLen);
int DbUpdate_PreviewCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbSelect_PreviewCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int GetGlobal_PreviewCfg(void *data, int len, void *pRecvData, int recvDataLen);
int DbUpdate_AlarmInCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbSelect_AlarmInCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbUpdate_CommCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbSelect_CommCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int GetGlobal_CommCfg(void *data, int len, void *pRecvData, int recvDataLen);
int DbUpdate_NetLinkCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbSelect_NetLinkCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int GetGlobal_NetLinkCfg(void *data, int len, void *pRecvData, int recvDataLen);
int DbUpdate_NetMngCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbSelect_NetMngCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int GetGlobal_NetMngCfg(void *data, int len, void *pRecvData, int recvDataLen);
int DbUpdate_CruiseCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbSelect_CruiseCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int GetGlobal_CruiseCfg(void *data, int len, void *pRecvData, int recvDataLen);
int DbUpdate_UserCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbSelect_UserCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int GetGlobal_UserCfg(void *data, int len, void *pRecvData, int recvDataLen);
int DbInsert_UserCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbDelete_UserCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbUpdate_DdnsCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbSelect_DdnsCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int GetGlobal_DdnsCfg(void *data, int len, void *pRecvData, int recvDataLen);
int DbUpdate_UpnpCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbSelect_UpnpCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbUpdate_28181Cfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbSelect_28181Cfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbUpdate_NtpCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbSelect_NtpCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int GetGlobal_NtpCfg(void *data, int len, void *pRecvData, int recvDataLen);
int DbUpdate_SmtpCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbSelect_SmtpCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int GetGlobal_SmtpCfg(void *data, int len, void *pRecvData, int recvDataLen);
int DbUpdate_P2PCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbSelect_P2PCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int GetGlobal_P2PCfg(void *data, int len, void *pRecvData, int recvDataLen);
int DbUpdate_ImgAttrCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbSelect_ImgAttrCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int GetGlobal_ImgAttrCfg(void *data, int len, void *pRecvData, int recvDataLen);
int DbUpdate_AllPresetCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbSelect_AllPresetCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int GetGlobal_AllPresetCfg(void *data, int len, void *pRecvData, int recvDataLen);
int DbUpdate_AutoMaintainCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbSelect_AutoMaintainCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int GetGlobal_AutoMaintainCfg(void *data, int len, void *pRecvData, int recvDataLen);
int GetPort(unsigned int *RtspPort, unsigned int *HttpPort,unsigned int *remotePort);
int DbMainMsgChannelInfo(int channel,char * ipaddr ,int *port ,char * username,char *password);
int DbSelect_pollCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int GetGlobal_pollCfg(void *data, int len, void *pRecvData, int recvDataLen);
int DbUpdate_pollCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int GetGlobal_FtpCfg(void *data, int len, void *pRecvData, int recvDataLen);
int DbUpdate_FtpCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbSelect_FtpCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
int DbUpdate_DefaultFtp(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db);
#ifdef __cplusplus
}
#endif


#endif//__DBSYSTEM_H__

