


#ifdef __cplusplus
extern "C" {
#endif


#ifndef _HEROSPEED_P2P_APP_MSG_DEF_H_
#define _HEROSPEED_P2P_APP_MSG_DEF_H_

#include "P2PSDK.h"



#define  MAX_SERVERIP_LENGTH   32
#define  MAX_ID_LENGTH   13    //id?????13
#define  MAX_USERNAME_LENGTH  64
#define  MAX_PASSWORD_LENGTH  64

typedef enum
{
    P2P_PLAY_REAL_STREAM = 0,
    P2P_STOP_REAL_STREAM = 1,
    P2P_PLAY_RELAY_STREAM = 2 ,
    P2P_STOP_RELAY_STREAM = 3,
    P2P_CLIENT_DISCONNECT = 4,
    P2P_RELAYCLIENT_DISCONNECT = 5,
    P2P_START_PTZ_CONTROL = 6,
    P2P_GET_DEVICE_STREAMINFO = 7,
    P2P_GET_DEVICE_STREAMINFO_RES = 8,
   // any else .... 
}P2pCmdType;
typedef enum
{
   Msg_Data,  // 消息数据 
   RealStream_Data, // 实时流数据
   RecordStream_Data, // 录像流数据
   // any else .... 
}StreamType;

typedef struct  m_mediainfo
{
    int    run;
    int    msessionid;	
    short streamType;  // 1:主码流 2:副码流
    short channelNo;  // 通道号(从1开始)	
}P2pMediainfo,*MP2pMediainfo;


typedef struct
{
    unsigned char ucUsername[16];
    unsigned char ucPassWord[16];
    unsigned char ucSerialNum[16];
}USER_INFO;




int p2pTHRStart();



/*************************/
typedef int (*HandleCommandCallback)(P2pCmdType cmd,char* indata,int inlen,char* outdata,int outlen);  


int   P2p_RegisterCommandHandle(HandleCommandCallback handlecommand);

/*向P2P服务器注册         serverIp:P2P服务器地址 myId:设备ID      username:设备用户名  password:设备密码*/
int   P2P_InitService(char* serverIp, char* myId,const char*username,const char* password); 
/*向P2P服务器注销服务*/
int P2P_StopService();

/************************
原型:typedef void (*HandleStatusCallback)(int status)
说明:上报P2P注册状态
输入:status:1/成功   0/失败 
************************/
typedef void (*HandleStatusCallback)(int status);

/************************
原型:int   P2p_RegisterStatusHandle(HandleStatusCallback   Pstatushandle)
说明:注册P2P状态回调函数
输入:Pstatushandle:回调函数地址
输出: 0/成功   -1/失败
************************/
int   P2p_RegisterStatusHandle(HandleStatusCallback   Pstatushandle);



/*获取p2p链接状态*/
int GetP2PConnectStatus();

/*开始发送媒体流数据*/
int P2pStartSendData(int sessionid,char* data, int len,StreamType mstreamtype);
/*停止媒体流服务*/
int P2pStopService(int sessionid);

/*开始发送媒体流数据*/
int P2pStartSendRelayData(int sessionid,char* data, int len,StreamType mstreamtype);
/*停止媒体流服务*/
int P2pStopRelayService(int sessionid);


typedef void *(*sthread)(void*);

#ifdef __cplusplus
}
#endif

#endif










