#ifndef __MIDWARE_H__
#define __MIDWARE_H__
/******************************************************
目前/appLib下面建了ddnsLib、 ptzLib、 smtpLib
三个目录要求这个三个目录能独立编译
如何对这三个库的调用，我们弄成一个
中间件，中间件，由些头文件和各个.c
文件组成。

******************************************************/

#include "sdk_struct.h"

/////DDNS接口层
int StartDdnsThread();
int StopDdnsThread();
void  DdnsUpdate(int UpdateFlg);


/////SMTP接口层
int  startSmtpThread(void);
int  stopSmtpThread(void);
int updateSmtpSetting(unsigned char flag);
int testSmtp(void);


/////NTP接口层
int startNtpClientThread(void);
int stopNtpClientThread(void);
int updateNtpSetting(int flag);

/////FTP接口层
int SentToFTP(char *strFileName);
int UpdateFtpDate(int value);


/////DHCP功能
int dhcpNetFun();


/////onvif层云台控制接口
int GetAllPreset(int channel,sdk_preset_all_t * preset_set);

int SetPtzCruiseAllPreset(int channel,sdk_cruise_param_t  * cruise_param );

int SetPreset(int channel , sdk_preset_t * preset);

int RecallPreset(int channel ,sdk_preset_t *preset);

int GetPtzCruiseAllPreset(int channel, sdk_cruise_param_t * cruise_param);


int ptzStartCruise(int channel);
int ptzStopCruise(int channel);

int ptzCruiseInit();
int ptzCruiseDestroy();

void ptzSetStatus(int channel ,int flag);
int testSmtp(void);

#endif

