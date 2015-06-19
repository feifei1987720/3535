/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Zhong caiwang <caiwang213@gmail.com>
 * Apr 2011-04-16
 *
 * Common functions define.
 */

#ifndef __COMM_FUNCS_H__
#define __COMM_FUNCS_H__
#include "comm_def.h"
#include "exception.h"

#define DAY_SECOND 86400UL
#define BUFSIZE    8192

#ifndef MIN
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

#define TELNET_CMD_GET       1
#define TELNET_CMD_SET       2
#define TELNET_BUF_SIZE      (512)

typedef enum
{
	THREAD_TCPLISTEN,
	THREAD_UDPLISTEN,
	THREAD_MAINMSGLOOP,
	THREAD_PREVIEW,
	THREAD_DOWNLOAD,
	THREAD_DUALTALKRECV,
	THREAD_DUALTALKDEC,
	THREAD_ALARMREPORT,
	THREAD_UPGRADE,
	THREAD_EMAILTEST,
	THREAD_DDNSTEST,
	THREAD_SNAPTREPORT,
	THREAD_LT_HEART,
	THREAD_LT_ALARM,
	THREAD_HDDFORMAT,
	THREAD_TUOBAOREPORT,
	THREAD_SEARCHREMOTE,
	THREAD_SET_REMOTENET,
	THREAD_GETHDDFORMATINFO,
	THREAD_ACTIVATECLOUD,
}THREADFUNC_S;

typedef struct tag_ThreadArg
{
	THREADFUNC_S Func;
	VOID         *pClass;
	VOID         *pArg;
	VOID         *pUser;
}THREADARG_S;

typedef VOID *(*THREADENTRY_T)(VOID *);
typedef VOID  Sigfunc( S32 );


typedef enum tag_STATUS
{       
	WRITE_STATUS,
	READ_STATUS,
	EXCPT_STATUS,
}IOSTATUS_E;

typedef struct _DEV_INFO_S_
{
	const S8  *ps8DevID;
	const S8  *ps8DevType;
	const S8  *ps8DevName;
	const S8  *ps8DevMac;
	const S8  *ps8DevKey;
	const S8  *ps8RemoteIP;
	const S8  *ps8LocalIP;
	U16 u16MediaPort;
	U16 u16HttpPort;
	U32 u32ChannelNum;
	
	S8 s8UrMetDdnsFlag;
	S8 reserves[3];
} DEV_INFO_S;

typedef enum{
	SELECTFD_RETURN_ERROR = -1,
	SELECTFD_RETURN_OVERTIME= 0,
	SELECTFD_RETURN_SUCCESS = 1,
}SELECTFD_RETURN_E;

typedef enum{
	SELECTFD_STAT_READ  = -1,
	SELECTFD_STAT_WRITE = 0,
	SELECTFD_STAT_EXCPT = 1,
}SELECTFD_STAT_E;

typedef enum{
    COMMAND_TYPE,
    AUDIO_VEDIO_TYPE
}DSCP_TYPE_E;

typedef struct DscpLevelAndType
{
    S32 rawDscpLevel;
    DSCP_TYPE_E type;        // type = 0   type = 1 
}DSCP_LEVEL_TYPE_S;

/*  */
//VOID SignalHander(S32 signal);
//VOID CaptureAllSignal();
extern VOID InitRandom();
extern U32 GetRandom(S32 s32Val);
extern S32 CompareDateTime( const DateTime *pTime1, const DateTime *pTime2 );
extern S32 CompareRecordTime( const REC_TIME_S *pTime1, const REC_TIME_S *pTime2 );

extern S8 *StrToLower( S8 *ps8Str );
extern S8 *StrToUpper( S8 *ps8Str );

extern LONG InterlockedCmp(LPLONG volatile lpValue1, LPLONG volatile lpValue2);
extern LONG InterlockedAssign(LPLONG volatile lpDstValue, LPLONG volatile lpSrcValue);
extern LONG InterlockedIncrement(LPLONG volatile lpValue);

/* / */
extern ULONG32 YSFPos;
extern ULONG32 YSFGas;
INT32 YSFEncode(ULONG8 *data, ULONG32 readlen, ULONG32 pos, ULONG32 gap);
INT32 YSFDecode(ULONG8 *data, ULONG32 readlen, ULONG32 pos, ULONG32 gap);

S32 hex_encode(VOID *decode, S8 *encode, U32 encode_size);
S32 hex_decode(const S8 *encoded, VOID *decoded, S32 decoded_size);

/*  */
extern S32 CreateJoinableThread( THREADENTRY_T entry, VOID *param, pthread_t *pid);
extern S32 CreateDetachThread( THREADENTRY_T entry, VOID *param, pthread_t *pid );

/*  */
extern SELECTFD_RETURN_E SelectFd(S32 s32SocketFd, SELECTFD_STAT_E enStat, S32 s32Sec, S32 s32uSec = 0);
extern S32 TcpListen(const S8 *host, const S8 *serv, socklen_t *addrlenp);
extern S32 TcpConnect(const S8 *host, const S8 *serv, S32 nsec);
extern S32 TcpConnectForDscp(const S8 *host, const S8 *serv, S32 nsec,DSCP_LEVEL_TYPE_S rawDscpLevelAndtype);
extern S32 UdpListen(const S8 *host, const S8 *serv, socklen_t *addrlenp);
extern S32 UdpConnect(const S8 *host, const S8 *serv);
extern S32 UdpClient(const S8 *host, const S8 *serv, SA **saptr, socklen_t *lenp);
extern S32 ReadData( S32 s32SockFd, S8 *pu8Buffer, S32 s32Size );            /*  */
extern S32 ReadData( S32 s32SockFd, S8 *pu8Buffer, S32 s32Size, S32 s32Sec); /*  */
extern S32 WriteData( S32 s32SockFd, S8 *pu8Buffer, S32 s32Size );            /*  */
extern S32 WriteData( S32 s32SockFd, S8 *pu8Buffer, S32 s32Size, S32 s32Sec); /*  */

extern S32 UdpReadData( S32 s32SockFd, S8 *pu8Buffer, S32 s32Size, SA *pstSA, S32 *s32AddrLen);
extern S32 UdpWriteData( S32 s32SockFd, S8 *pu8Buffer, S32 s32Size, SA *pstSA, S32 s32AddrLen);

extern S32 GetIpAndNetMask( U8 *pu8IpAddr, U8 *pu8NetMask, const S8 *pu8IfName = "eth0" );
extern S32 GetGateWay( U8 *pu8GateWay, const S8 *pu8IfName = "eth0" );
extern BOOL GetNetLinkState( const S8 *pu8IfName = "eth0" ); /*  truefalse*/
extern S32 GetDnsAddr( U8 *pu8Dns );

/*  */
extern S32 TelnetStart( const S8 *host, const S8 *serv, const S8 *user, const S8 *password);
extern S32 TelnetCmd( S32 s32SockFd, const S8 *ps8CmdStr, S8 *ps8RspStr, S32 *s32RspSize );
extern S32 TelnetEnd( S32 s32SockFd );

/* */
S32 Ddns_Register(DEV_INFO_S *pstDevInfo, S32 *s32Result, S8 *ps8DevID);
S32 Ddns_Update(DEV_INFO_S *pstDevInfo, S32 *s32Result, S8 *ps8DevID);

/*  */
/*
 * IP  192.168.12.125]  U8 
 * bDir = true U8IPAddr  S8IPAddr
 * bDir = false S8IPAddr  U8IPAddr
 * */
S32 IPAddrAdapter(S8 * S8IPAddr, U8* U8IPAddr, BOOL bDir);
S32 base64(const void *in, unsigned int inlen, char *out);

S32 GetMacAddress(const S8 *ps8EthName, U8 *pu8Mac);

#endif
