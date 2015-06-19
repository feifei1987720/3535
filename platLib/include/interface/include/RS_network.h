 /******************************************************************************
  File Name     : NetBroadcat.h
  Version       : 
  Author        : maiguanwei
  Created       :  20110108
  Description   : 
******************************************************************************/

#ifndef _RS_NETWORK_H_
#define _RS_NETWORK_H_

#include <stdlib.h>
#include <string.h>
#include "config.h"
//#include "mainthread.h"
#include "RSNetProtocolLinux.h"
//#include "RS_comnetwork.h"
#include "RS_netbroadcast.h"
//#include "RS_tcppreview.h"
//#include "RS_udppreview.h"
#include "commondebug.h"
#include "commonnetfunction.h"
#include "parameterhi3511dvr.h"
#include "RSFrame.h"
#include "dvrfs.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define RS_SUCCESS 0
#define RS_FAIL (-1)

#define MAX_REMOTEUSER_NUM 10

#define MAX_SEND_LEN 1024
#define MAX_RECV_LEN 1024
#define MAX_MSG_RECV_LEN (10 * 1024)

#define TCP_Protocol 0
#define UDP_Protocol 1
#define TCP_DualTalk 2

#define ParamToNet true
#define NetToParam false
#define ParamError (-2)

#define TOP_ELEMENT 1
#define LIST_ELEMENT 2

#define NET_MDEC_ALIGN_LEN	(320 * 1024) /**/



typedef struct NET_DEC_FRAME
{
	HI_U8 *pu8RdvrfsAlign;      /*DVRFSBUF*/
	/*BUF*/
	HI_U8 *pu8SdBuf;   			/**/
	HI_U32 u32SdBufLen; 		/**/

	HI_U8 *pu8SdBufLf; 			/**/
	HI_U32 u32SdBufLfLen;		/**/

	HI_U8 *pu8SdFrmHd;			/**/
	HI_U32 u32SdFrmLen;			/**/
	frametype_t enCurFrmType;	/**/
	/**/
	HI_S32 s32Fd;    				/**/
	rec_output_day_t stRecOutPut;	/**/
	HI_BOOL bFirstRead;				/*?*/
	/*ITIME*/
//	datetime_t stCurITime;
	record_time_t stCurITime;
}NET_DEC_FRAME_S;


// 
typedef struct {
	int           chn;
	unsigned int  type;
	record_time_t start_time;
	record_time_t end_time;
}RS_timepoint_setting;



typedef struct {
	MsgHead Head;
	char ExtBuffer[256];
}MsgHeadWrap;


typedef struct {
	PackHead packhead;
	char ExtBuffer[256];
}PackHeadWrap;



#define RecordPlayHandType  1
#define RecordDownloadHandType  2

#define RecordSendOverLoad -2 



typedef struct
{
	RecordDetailRsp *pHead;
	RecordDetailRsp *pTail;
	unsigned int count;
}DownloadList_t;

typedef struct
{
	int socketFD;
	char *buffer;
}RS_RecordPlayThreadContent_t;



typedef enum{
	MotionOn = 0,
	MotionOff,
	IOAlarmOn,
	IOAlarmOff,
	MaxAlarmType,
}RS_AlarmType;





//LoginCheckRet
//
#define USERLOGINED -1
#define SYSTEM_ERROR -2 
#define NOUSERNAME -3
#define PASSWORD_ERROR -4
#define USERKEYSAME -5

//systemparameter_t m_pNetSysPara;
//extern LoginUserInfo_t *m_pLoginUserInfo;
//extern systemparameter_t m_pNetSysPara;
//extern unsigned int m_UserKey; //


#define MAX_RECORD_THREAD 32
#define MAX_UDP_PREVIEW_NUM 32

extern pthread_mutex_t m_UserInfo_mutex;
extern pthread_mutex_t m_AlarmReport_mutex;
extern pthread_cond_t m_AlarmReport_cond;
//extern LoginUserInfo_t *m_UserInfo;


typedef struct tag_LoginUserInfo_t
{
	unsigned int UserKey;
	unsigned int LocalUserID; //ID
	int socketFD; //SOCKETID
	int TCPsocket[MAX_CHANNEL_NUM];
	unsigned int UDPsessionID[MAX_CHANNEL_NUM]; //UDPID
	int DualTalksocket;  //socket
	int MsgHeadSeq;  //, 
	int AlarmReportFlag;  //truefalse
	
	int PreviewThreadRunFlag; 	//
	unsigned int UdpPreviewID[MAX_UDP_PREVIEW_NUM];  //UDPID

	int RecordPlayThreadRunFlag; //
	pthread_t RecordThreadId[MAX_RECORD_THREAD]; //THREAD ID

	int DualTalkThreadRunFlag; 	//
	pthread_t DualTalkRecvThreadId; //THREAD ID

	unsigned char RS_MotionStateBackup[MAX_CHANNEL_NUM];
	unsigned char RS_IOStateBackup[MAX_CHANNEL_NUM];
	int AlarmReportRunFlag;
	pthread_t AlarmReportThreadId;

	pthread_mutex_t User_mutex;
	tag_LoginUserInfo_t *pNext;
}LoginUserInfo_t;


typedef struct 
{
	LoginUserInfo_t *head;
	LoginUserInfo_t *tail;
	int Count;
}LoginUserList;

extern LoginUserInfo_t *m_UserList;

typedef struct //DATA
{
	MsgHead head;
	LoginReq Reqbody;
}NetLoginReq_t;

typedef struct  //DATA
{
	MsgHead head;
	LoginRsp Rspbody;
}NetLoginRsp_t;

typedef struct 
{
	PackHead Head;
	char Data[MaxPackSize];
}Pack;


typedef struct 
{
	unsigned char Status;
	char Reserved[7];
	DateTime AlarmTime;
}RS_AlarmReport;


//Remote Upgrade

typedef struct 
{
	int socketfd;
	unsigned int HeadSeq;
	unsigned long UpgradeFileSize;
	unsigned long UpgradeBitmap[MaxUpgradeModules];
	RS_UpgradeFileHeader_t RS_UpgradeHead;
}RS_UpgradeThreadContent_t;


//communication
int RS_TcpSend(int socketfd, char *buffer, unsigned int size);
int RS_TcpRecv(int socketfd, char *buffer, unsigned int maxsize);
int RS_UdpSend(int socketfd, char *buffer, struct sockaddr_in cliaddr, unsigned int size);
int RS_UdpRecv(int socketfd, char *buffer, struct sockaddr_in *cliaddr);
int RS_TCPBlockSendData(RSFrameHead SendFrameHead, int  SendSocket, char *databuffer, int size);
int RS_TCPBlockRecvData(int  socketfd, char *databuffer);
int RS_UDPBlockSendData(RSFrameHead SendFrameHead, int  SenSocket, char *databuffer, int size, struct sockaddr_in ClientAddr);
int RS_RepectAckFlag(int socketfd, MsgHead head, int ackflag, char *ExtStructbuffer = NULL, char ExtStructSize = 0);
int RS_RecordPlaySendFrame(int clientfd, NET_DEC_FRAME_S *pdec, unsigned long long *CurFrameTime, char RecordHandleType);
int RS_PackTypeSend(int clientfd, int packtype);
int RS_TcpRecvHead(int socketfd, MsgHeadWrap *pHeadWrap);
int RS_TcpRecvData(int socketfd, char *Databuffer, unsigned int Size);
//communication


int RS_Network_Init();

int RS_GetLocalTime(DateTime *time);

int UpdataFromSystemPara(systemparameter_t *pNetSysPara);
int NetHeartbeatProcess(unsigned int clientfd, char *buffer); //
int NetTcpPtzProcess(int clientfd, char *buffer); //
int PtzFun(MsgHead RecvHead, PTZControlReq PTZControlbody);
int NetQueryRecordProcess(int clientfd, char *buffer);
int QueryRecordDetailFun(int clientfd, MsgHead RecvHead, QueryRecordReq QueryRecordReqbody);
int	QueryRecordDayOutlineFun(int clientfd, MsgHead RecvHead, QueryRecordReq QueryRecordReqbody);

//preview
int RS_NetPreviewProcess(int clientfd, MsgHead head, char *buffer, LoginUserInfo_t *UserInfo);
int RS_TCPPreviewProcess(int clientfd, MsgHead RecvHead, char *buffer);

//net replay  xiaona
int RecordPlayProcess(int clientfd, MsgHead RecvHead, char *buffer);
int InitSendFrameBufAndDvrfsBuf(NET_DEC_FRAME_S *pdec);
int FreeSendFrameBufAndDvrfsBuf(NET_DEC_FRAME_S *pdec);
int UdpServer_Init();



//Login
int UserListDelete(LoginUserInfo_t *ReqListNode);
int UserListAdd(LoginUserInfo_t *ReqListNode);
int SearchUserListForUserKey(unsigned int CurUser, unsigned int UserKey, LoginUserInfo_t **UserInfo);

//AlarmReport
int RS_AlarmStatusUpToReport(int channel, RS_AlarmType type, int subtype, DateTime time);

//Param Querry & Set
int RS_QueryParamProcess(int clientfd, MsgHead RecvHead, char *buffer, LoginUserInfo_t *UserInfo);
int CheckUserAuthority(int ReqUser, int SubType);
int RS_SetParamProcess(int clientfd, MsgHead RecvHead, char *buffer, LoginUserInfo_t *UserInfo);
void RS_ParseRecvMsg(int msgSubType, int totalsize, char *databuffer);
int RS_GetElementData(void* Targetbuffer, UINT TargetSize, char* Sourcebuffer, unsigned int *parsesize);

typedef int (*FormationDataCallback)(systemparameter_t *pNetSysPara, void *tmpOsdset, void *tmpSoleOsdset, unsigned char Chn, bool Direction, unsigned int parsesize);
typedef int (*ElementDataCallback)(char *databuffer, void *reqbuffer);
int RS_Formation(FormationDataCallback fun, char *databuffer, unsigned int ChannelMask, unsigned int ElementSize, unsigned int ListSize, unsigned int level = TOP_ELEMENT);
int RS_Parse(ElementDataCallback elementFun, ElementDataCallback itemFun, ElementDataCallback itemlistFun, char *databuffer, void *reqbuffer, unsigned int TotalSize);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
