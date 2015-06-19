/******************************************************************************

 ******************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <pthread.h>

#include "hs_rtsp_commonLib.h"
#include "hs_rtsp_base64Lib.h"
#include "rtspSLib.h"
#include "hs_rtpLib.h"
#include "hs_typeLib.h"


#include <pthread.h>
//#include <avserver_ui.h>
#include "g711Lib.h"
#include "global.h"
//#include "tiLib.h"
#include "commonLib.h"
#include "codeLib.h"
#include "rtspFunc.h"
#include "playbackLib.h"
#include "hs_rtsp_base64Lib.h"
#include "main.h"
#include "sdk_struct.h"
#include "sdk_global.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MODLE_NAME		"RTSP "
#define ANONYMOUS_ENABLE

//#define PRINT_RTSP		1
#define MAX_CLIENTS		20	/* RTSP */

static pthread_t g_rtspsvrThreadId;
static int g_rtspServerRun = 0;
static int g_updateParam = 0;

typedef struct
{
	char	user[32];	//< username
	char	password[32];	//< password
}Acount_rtsp_t;

#define ACOUNT_NUM				16
Acount_rtsp_t	g_account[ACOUNT_NUM];

RtspSvr_t	gRtspVodSvr[VOD_SVR_TYPE_MAX];
RtspMethod_t gRtspMethod[] = {
	{RTSP_METHOD_PLAY, 			RTSP_REQ_METHOD_PLAY},
   	{RTSP_METHOD_PAUSE, 		RTSP_REQ_METHOD_PAUSE},
   	{RTSP_METHOD_DESCRIBE, 		RTSP_REQ_METHOD_DESCRIBE},
   	{RTSP_METHOD_SETUP, 		RTSP_REQ_METHOD_SETUP},
   	{RTSP_METHOD_REDIRECT, 		RTSP_REQ_METHOD_REDIRECT},
   	{RTSP_METHOD_SESSION, 		RTSP_REQ_METHOD_SESSION},
	{RTSP_METHOD_OPTION, 		RTSP_REQ_METHOD_OPTIONS},
   	{RTSP_METHOD_OPTIONS, 		RTSP_REQ_METHOD_OPTIONS},
   	{RTSP_METHOD_TEARDOWN, 		RTSP_REQ_METHOD_TEARDOWN},
   	{RTSP_METHOD_RECORD, 		RTSP_REQ_METHOD_RECORD},
   	{RTSP_METHOD_GET_PARAM, 	RTSP_REQ_METHOD_GET_PARAM},
   	{RTSP_METHOD_SET_PARAM, 	RTSP_REQ_METHOD_SET_PARAM},
   	{RTSP_METHOD_EXT_METHOD, 	RTSP_REQ_METHOD_EXTENSION},
   	{0, 						RTSP_PARSE_INVALID_OPCODE}
};

RtspMethod_t gRtspStatu[] = {
	{"Continue", 100},
	{"OK", 200},
	{"Created", 201},
	{"Accepted", 202},
	{"Non-Authoritative Information", 203},
	{"No Content", 204},
	{"Reset Content", 205},
	{"Partial Content", 206},
	{"Multiple Choices", 300},
	{"Moved Permanently", 301},
	{"Moved Temporarily", 302},
	{"Bad Request", 400},
	{"Unauthorized", 401},
	{"Payment Required", 402},
	{"Forbidden", 403},
	{"Not Found", 404},
	{"Method Not Allowed", 405},
	{"Not Acceptable", 406},
	{"Proxy Authentication Required", 407},
	{"Request Time-out", 408},
	{"Conflict", 409},
	{"Gone", 410},
	{"Length Required", 411},
	{"Precondition Failed", 412},
	{"Request Entity Too Large", 413},
	{"Request-URI Too Large", 414},
	{"Unsupported Media Type", 415},
	{"Bad Extension", 420},
	{"Invalid Parameter", 450},
	{"Parameter Not Understood", 451},
	{"Conference Not Found", 452},
	{"Not Enough Bandwidth", 453},
	{"Session Not Found", 454},
	{"Method Not Valid In This State", 455},
	{"Header Field Not Valid for Resource", 456},
	{"Invalid Range", 457},
	{"Parameter Is Read-Only", 458},
	{"Internal Server Error", 500},
	{"Not Implemented", 501},
	{"Bad Gateway", 502},
	{"Service Unavailable", 503},
	{"Gateway Time-out", 504},
	{"RTSP Version Not Supported", 505},
	{"Extended Error:", 911},
	{0, RTSP_PARSE_INVALID_OPCODE}
};

char *gRtspInvalidMethod = "Invalid Method";

#define SND_MSG_KEY2	((key_t)0x31)
#define AUD_STREAM_NEWFRAME2 0x800
#define AUD_STREAM_STOP2     0x801

typedef struct TalkHead_s{
	uLong		len:10;
	uLong		reserved:22;
	uLong		timestamp;
}TalkHead_t;

typedef struct {
         long    mtype;
         int     msg;
         int 	 sampleRate;
} message_buf2;

static pthread_t g_talkThrId;
static int g_talkInUse=0;
static int g_talkClientFd=0;

 unsigned char sharedAudioData[4096];

unsigned short g_rtspport=554;

extern int gSend_IVSdata_flag;
u32 g_IVS_pts = 0;

int RTSP_GetClientNum(int chn)
{
	int i;
	int cnt = 0;
	if(chn == 0)
	{
		cnt = gRtspVodSvr[VOD_SVR_TYPE_RTSP].clientCnt[chn];
	}
	else
	{
		for(i = 1; i < MAX_VOD_CHN; i++)
		{
			cnt += gRtspVodSvr[VOD_SVR_TYPE_RTSP].clientCnt[i];
		}
	}

	return cnt;
}

int RTSP_GetUdpSendStat(int chn)
{
	return gRtspVodSvr[VOD_SVR_TYPE_RTSP].pRtpUdpSender[chn][RTP_STREAM_VIDEO]->bActive;
}

void RTSP_SetUdpSendActive(int chn, int isActive)
{
	gRtspVodSvr[VOD_SVR_TYPE_RTSP].pRtpUdpSender[chn][RTP_STREAM_AUDIO]->bActive = isActive;
	gRtspVodSvr[VOD_SVR_TYPE_RTSP].pRtpUdpSender[chn][RTP_STREAM_VIDEO]->bActive = isActive;
}

RtspSession_t *RTSP_GetSessPtrBySessId(char *id)
{
	struct list_head *pos, *q;
	RtspSession_t *pSess;

	list_for_each_safe(pos, q, &gRtspVodSvr[VOD_SVR_TYPE_RTSP].rtspSessWorkList.rtspSessList)
	{
		pSess = list_entry(pos, RtspSession_t, rtspSessList);
		if(pSess != NULL)
		{
			if(0 == strcmp(pSess->sessId, id))
			{
				return pSess;
			}
		}
	}
	return NULL;
}

static int RTSP_GetPeerInfo(int sockFd, char *ipAddr, int *port)
{
	socklen_t addrLen;
	struct sockaddr_in cliAddr;

	addrLen = sizeof(cliAddr);
	if(-1 == getpeername(sockFd, (struct sockaddr *)&cliAddr, &addrLen))
	{
		return GS_FAIL;
	}
	strncpy(ipAddr, (const char *)inet_ntoa(cliAddr.sin_addr), 64);
	*port = (int)ntohs(cliAddr.sin_port);

	return GS_SUCCESS;
}

static int RTSP_GetHostInfo(int sockFd, char *ipAddr)
{
	socklen_t addrLen;
	struct sockaddr_in hostAddr;

	addrLen = sizeof(hostAddr);
	if( -1 == getsockname(sockFd, (struct sockaddr *)&hostAddr, &addrLen))
	{
		return GS_FAIL;
	}
	strncpy(ipAddr, (const char *)inet_ntoa(hostAddr.sin_addr), 64);
	return GS_SUCCESS;
}

void RTSP_GetSessionId(char *sessId, int len)
{
	int i;
	for(i=0; i<len; i++)
	{
		sessId[i] = (char )((random()%10) + '0');
	}
	sessId[len] = 0;
}

void RTSP_SetAllSockFd(fd_set *sockFds, int *maxFd, RtspSvr_t *pRtspSvr)
{
	RtspSession_t *pSess = NULL;
	struct list_head *pos, *n;

	list_for_each_safe(pos, n, &(pRtspSvr->rtspSessWorkList.rtspSessList))
	{
		pSess = list_entry(pos, RtspSession_t, rtspSessList);
		if(pSess != NULL)
		{
			if(pSess->rtspSockFd > 0)
			{
				FD_SET(pSess->rtspSockFd, sockFds);
				*maxFd = (*maxFd >= pSess->rtspSockFd) ? *maxFd : pSess->rtspSockFd;
			}
		}
	}
}

int RTSP_GetConnectType(const char *recvBuf)
{
	if(NULL != strstr(recvBuf, "rtsp"))
	{
		return CON_TYPE_RTSP;
	}
	else if(NULL != strstr(recvBuf, UCTRL_HEAD))
	{
		return CON_TYPE_UCTRL;
	}
	
	return CON_TYPE_UNDEFINED;
}

int RTSP_GetReq(char *buf)
{
	int seq;
	int cnt;
	char method[32];
	char object[256];
	char ver[32];
	RtspMethod_t *m;

	*method = *object = '\0';
	seq = 0;

	cnt = sscanf(buf, " %31s %255s %31s", method, object, ver);
	if(cnt != 3)
	{
		
	}

	for(m=gRtspMethod; m->opcode != -1; m++)
	{
		if(!strcmp(m->describe, method))
		{
			break;
		}
	}
	return (m->opcode);
}

int RTSP_GetCseq(char *buf)
{
	int cseq = -1;
	char  trash[255];
	char  *pTmp = NULL;

	pTmp = strstr(buf, RTSP_HDR_CSEQ);
	if(pTmp == NULL)
	{

	}
	else
	{
		if(2 != sscanf(pTmp, "%254s %d", trash, &cseq))
		{
			
		}
	}
	return cseq;
}

char *RTSP_GetMethodDescrib(u32 code)
{
	RtspMethod_t *pMethod;

	for(pMethod = gRtspStatu; pMethod->opcode != RTSP_PARSE_INVALID_OPCODE;	pMethod ++)
	{
		if(pMethod->opcode == code)
		{
			return (pMethod->describe);
		}
	}
	return gRtspInvalidMethod;
}

int RTSP_GetHead(int err, RtspSession_t *pSess)
{
	char *pTmp = NULL;

	RTSP_CLEAR_SENDBUF(pSess);
	pTmp = pSess->sendBuf;
	pTmp += sprintf(pTmp, "%s %d %s\r\n", RTSP_VERSION, err, RTSP_GetMethodDescrib( err ));
    	pTmp += sprintf(pTmp,"CSeq: %d\r\n", pSess->lastRecvSeq);
    	pTmp += sprintf(pTmp,"Server: Rtsp Server \r\n");

	return (strlen(pSess->sendBuf));
}

int RTSP_GetCurClientCnt(void)
{
	int n;
	int clients = 0;

	for(n=0; n<MAX_VOD_CHN; n++)
	{
		clients += gRtspVodSvr[VOD_SVR_TYPE_RTSP].clientCnt[n];
	}

	return clients;
}


int RTSP_CheckChn(int chn)
{
	int curClientCnt = 0;


	if(chn<0 || chn> MAX_VOD_CHN)
	{
		return GS_FAIL;
	}

	curClientCnt = RTSP_GetCurClientCnt();
	if(curClientCnt >= MAX_CLIENTS)
	{
		return GS_FAIL;
	}

	return GS_SUCCESS;
}


int RTSP_GetChannel(const char *szChn)
{
	int chn = -1;

	if(szChn == NULL)
	{
		return GS_FAIL;
	}
	else
	{
		chn = atoi(szChn);
		if((chn < 0) || (chn >= MAX_VOD_CHN))
		{
			return GS_FAIL;
		}
	}
	return chn;
}

int RTSP_SendReply(int err, int simple, char *addon, RtspSession_t *pSess)
{
	int n;
	char *pTmp = pSess->sendBuf;

	if(simple == 1)
	{
		pTmp += RTSP_GetHead(err, pSess);
	}

	if(addon)
	{
		pTmp += sprintf(pTmp, "%s", addon);
	}

	if(simple)
	{
		strcat(pSess->sendBuf, RTSP_LRLF);
	}

	pSess->readToSend = True;

	n = send(pSess->rtspSockFd, pSess->sendBuf, strlen(pSess->sendBuf), MSG_NOSIGNAL);
	if(n != strlen(pSess->sendBuf))
	{
		return GS_FAIL;
	}

	#if PRINT_RTSP
	printf("\n===================== S->C =====================\n");
	printf("rtsp send: %s \n", pSess->sendBuf);
	printf("\n===================== S->C =====================\n");
	#endif 

	return GS_SUCCESS;
}

int RTSP_RecvMsgParse(int *headLen, int *bodyLen, RtspSession_t *pSess)
{
	int end;
	int msgEnd;
	int msgLen;
	int msgBodyExist;
	int msgBodyLen;
	int endCount;
	int writeSpace;
	short *pLen ;
	char  c;
	char  *p = NULL;

	if(pSess->recvBuf[0] == '$')
	{
		pLen = (short *)&pSess->recvBuf[2];
		msgBodyLen = ntohs(*pLen);
		if(msgBodyLen <= pSess->inSize)
		{
			if(headLen)
			{
				*headLen = 4;
			}
			if(bodyLen)
			{
				*bodyLen = msgBodyLen;
			}
		}
		return GS_FAIL;
	}
	msgEnd = msgBodyExist = msgLen = msgBodyLen = 0;

	while(msgLen <= pSess->inSize)
	{
		end = strcspn(&pSess->recvBuf[msgLen], "\r\n");
		if(end > 0)
		{
			msgLen += end;
		}
		else
		{
			return GS_FAIL;
		}

		if(msgLen > pSess->inSize)
		{
			return GS_FAIL;
		}

		endCount = writeSpace = 0;
		while(!msgEnd && ((msgLen + endCount + writeSpace) < pSess->inSize))
		{
			c = pSess->recvBuf[msgLen + endCount + writeSpace];
			if(c == '\r' || c == '\n')
			{
				endCount ++;
			}
			else if ((endCount < 3) && (c == ' ' || c == '\t'))
			{
				writeSpace ++;
			}
			else
			{
				break;
			}
		}

		if(endCount > 2 || (endCount == 2 && (pSess->recvBuf[msgLen] == pSess->recvBuf[msgLen + 1])))
		{
			msgEnd = 1;
		}
		msgLen += endCount + writeSpace;
		if(msgEnd)
		{
			msgLen += msgBodyLen;
			if(msgLen <= pSess->inSize)
			{
				break;
			}
		}

		if(msgLen >= pSess->inSize)
		{
			return GS_FAIL;
		}

		if(!msgBodyExist)
		{
			if(!strncasecmp(&pSess->recvBuf[msgLen], RTSP_HDR_CONTENTLENGTH, strlen(RTSP_HDR_CONTENTLENGTH)))
			{
				msgBodyExist = 1;
				msgLen += strlen(RTSP_HDR_CONTENTLENGTH);
				while(msgLen < pSess->inSize)
				{
					c = pSess->recvBuf[msgLen];
					if(c == ':' || c == ' ')
					{
						msgLen ++;
					}
					else
					{
						break;
					}

					if( -1 == sscanf(&pSess->recvBuf[msgLen], "%d", &msgBodyLen))
					{
						return GS_FAIL;
					}
				}
			}
		}

	}

	if(headLen)
	{
		*headLen = msgLen - msgBodyLen;
	}

	if(bodyLen)
	{
		for(endCount = pSess->inSize - msgLen, p = &pSess->recvBuf[msgLen];
			endCount && (*p == '\0'); p++, msgBodyLen ++, endCount --);
		*bodyLen = msgBodyLen;
	}

	return GS_SUCCESS;
}

int RTSP_ResponseMsgCheck(int *stat, char *buf)
{
	u32 state;
	char  version[32];
	char  trash[256];
	u32 parameterNum;

	parameterNum = sscanf(buf, " %31s %u %255s ", version, &state, trash);
	if(strncasecmp(version, "RTSP/", 5))
	{
		return RTSP_PARSE_ISNOT_RESP;
	}

	if(parameterNum < 3 || state == 0)
	{
		return RTSP_PARSE_ISNOT_RESP;
	}

	*stat = state;
	return RTSP_PARSE_IS_RESP;
}

int RTSP_ParseUrl(int *port, char *server, char *fileName, const char *url)
{
	int ret = GS_FAIL;
	int havePort = 0;
	int len, n;
	char *pTmp = NULL;
	char *pStr = NULL;
	char *pEnd, *pPort;
	char *full = (char *)malloc(strlen(url) + 1);


	*port = DEFAULT_RTSP_PORT;
	strcpy(full, url);

	if(0 == strncmp(full, "rtsp://", 7))
	{
		pStr = (char *)malloc(strlen(url) + 1);
		if(pStr != NULL)
		{
			strcpy(pStr, &full[7]);
			if(strchr(pStr, '/'))
			{
				len = 0;
				pEnd = strchr(pStr, '/');
				len = pEnd - pStr;
				for(n=0; n<strlen(url); n++)
				{
					pStr[n] = 0;
				}
				strncpy(pStr, &full[7], len);
			}

			if(strchr(pStr, ':'))
			{
				havePort = 1;
			}
			free(pStr);
			pStr = NULL;
			pTmp = strtok(&full[7], " :/\t\n");
			if(pTmp != NULL)
			{
				strcpy(server, pTmp);
				if(havePort)
				{
					pPort = strtok(&full[strlen(server) + 7 + 1], " /\t\n");
					if(pPort != NULL)
					{
						*port = atol(pPort);
					}
				}
				pTmp = strtok(NULL, " ");
				if(pTmp )
				{
					strcpy(fileName, pTmp);
				}
				else
				{
					fileName[0] = '\0';
				}
				ret = GS_SUCCESS;
			}
		}
		else
		{
			Printf("malloc error \n");
			ret = GS_FAIL;
		}
	}
	else
	{
		pTmp = strtok(full, "\t\n");
		if(pTmp)
		{
			strncpy(fileName, pTmp, 16);
			server[0] = '\0';
			ret = GS_SUCCESS;
		}
	}
	free(full);
	full = NULL;

	return ret;
}

int RTSP_PraseUserPwd(char *buff, char *name, char *passwd, char *uri)
{
	char *p,*q,*r = NULL;
	char *outer_ptr = NULL;
	char *inner_ptr = NULL;
	p = strstr(buff, RTSP_HDR_AUTHORIZATION);
	p = p+22;//p username
	r = p;

	while ((p = strtok_r(p,",",&outer_ptr)) != NULL)
	{
			q = strtok_r(p,"=",&inner_ptr);
			if(0 == strcmp(q,"username"))
			{
				if(NULL != (q = strtok_r(NULL,"=",&inner_ptr)))
				{
					q = q+1;
					strncpy(name,q,LEN_32-1);
					name[LEN_32-1] = 0;
					q = strrchr(name,'"');
					if(q)
						*q = 0;
				}
				else
				{
					memset(name,0,LEN_32);
					return GS_FAIL;
				}
				p = NULL;
			}
			else if(0 == strcmp(q+1,"uri"))
			{
				if(NULL != (q = strtok_r(NULL,"=",&inner_ptr)))
				{
					q = q+1;
					strncpy(uri,q,127);
					uri[127-1] = 0;
					q = strrchr(uri,'"');
					if(q)
						*q = 0;

				}
				else
				{
					memset(uri,0,sizeof(uri));
					return GS_FAIL;
				}
				p = NULL;
			}
			else if(0 == strcmp(q+1,"response"))// response
			{
				if(NULL != (q = strtok_r(NULL,"=",&inner_ptr)))
				{
					q = q+1;
					strncpy(passwd,q,LEN_32-1);
					passwd[LEN_32-1]=0;
					q = strrchr(passwd,'"');
					if(q)
						*q = 0;
				}
				else
				{
					memset(passwd,0,LEN_32);
					return GS_FAIL;
				}
				p = NULL;
			}

			p = NULL;
	}


	return GS_SUCCESS;
}

void RTSP_SessListInit(RtspSvr_t *pRtspSvr)
{
	INIT_LIST_HEAD(&pRtspSvr->rtspSessWorkList.rtspSessList);
}

void RTSP_SessListLock(void )
{
	pthread_mutex_lock(&gRtspVodSvr[VOD_SVR_TYPE_RTSP].rtspListMutex);
}

void RTSP_SessListUnlock(void)
{
	pthread_mutex_unlock(&gRtspVodSvr[VOD_SVR_TYPE_RTSP].rtspListMutex);
}

void RTSP_SessListAdd(RtspSession_t *pSess)
{
	if(pSess != NULL)
	{
		RTSP_SessListLock();
		pSess->svrType = 0;
		list_add_tail(&pSess->rtspSessList,
				&gRtspVodSvr[pSess->svrType].rtspSessWorkList.rtspSessList);
		RTSP_SessListUnlock();
	}
}

void RTSP_SessListDel(RtspSession_t *pSess)
{
	if(pSess != NULL)
	{
		RTSP_SessListLock();
		if((pSess->channel >= 0) && (pSess->sessStat >= RTSP_STATE_PLAY))
		{
			gRtspVodSvr[pSess->svrType].clientCnt[pSess->channel] --;
		}
		list_del(&pSess->rtspSessList);
		free(pSess);
		pSess = NULL;
		RTSP_SessListUnlock();
	}
}

void RTSP_SessListPrint(RtspSvr_t *pRtsp)
{
	RtspSession_t *pSess;
	struct list_head *pos, *n;
	int ch;

	for(ch = 0; ch < MAX_VOD_CHN; ch++)
	{
		if(pRtsp->clientCnt[ch] > 0)
		{
			Printf("[Rtsp: %d]: %d clients  \n", ch, pRtsp->clientCnt[ch]);
		}
	}
	list_for_each_safe(pos, n, &(pRtsp->rtspSessWorkList.rtspSessList))
	{
		pSess = list_entry(pos, RtspSession_t, rtspSessList);
		if(pSess != NULL)
		{
			//Printf("[Rtsp: %d]: [%s: %s] \n", pSess->channel,
			//	pSess->transportType == RTP_TRANSPORT_TYPE_UDP ? "Udp": "Tcp",
			//	pSess->remoteIp);
		}
	}
}

RtspSession_t *RTSP_SessionCreate(int sockFd, VodSvrType_e vodSvrType)
{
	RtspSession_t *pSess = NULL;

	if(sockFd <= 0)
	{
		return NULL;
	}

	pSess = (RtspSession_t *)malloc(sizeof(RtspSession_t));
	if(pSess == NULL)
	{
		return NULL;
	}
	memset(pSess, 0, sizeof(RtspSession_t));
	pSess->setupFlag[0] = 0;
	pSess->setupFlag[1] = 0;
	pSess->rtspSockFd 	= sockFd;
	pSess->svrType 		= vodSvrType;
	pSess->channel		= -1;
	pSess->sessStat		= RTSP_STATE_INIT;
	if( GS_SUCCESS != RTSP_GetPeerInfo(sockFd, pSess->remoteIp, &pSess->remotePort))
	{
		free(pSess);
		pSess = NULL;
	}

	if( GS_SUCCESS != RTSP_GetHostInfo(sockFd, pSess->hostIp))
	{
		free(pSess);
		pSess = NULL;
	}
	RTSP_GetSessionId(pSess->sessId, 8);
	RTSP_SessListAdd(pSess);
	return pSess;
}

int RTSP_SessionClose(RtspSession_t *pSess)
{
	RtpUdpSender_t *pSender = NULL;
	if(pSess == NULL)
	{
		return GS_FAIL;
	}

	if(pSess->sessStat < RTSP_STATE_PLAY)
	{
		if(pSess->rtspSockFd > 0)
		{
			sock_close(pSess->rtspSockFd);
			pSess->rtspSockFd = -1;
		}
		RTSP_SessListDel(pSess);
		return GS_SUCCESS;
	}

	if(RTP_TRANSPORT_TYPE_TCP == pSess->transportType)
	{
		pSess->sessStat = RTSP_STATE_STOP;
		if(pSess->tcpThrId > 0)
		{
			pthread_join(pSess->tcpThrId, NULL);
		}

		if(pSess->pRtpTcpSender != NULL)
		{
			free(pSess->pRtpTcpSender);
			pSess->pRtpTcpSender = NULL;
		}
		if(pSess->rtspSockFd > 0){
			sock_close(pSess->rtspSockFd);
			pSess->rtspSockFd = -1;
		}
		if(gRtspVodSvr[VOD_SVR_TYPE_RTSP].clientCnt[pSess->channel] <= 0)
		{
			 /* stop sub video stream encode  */
		}
	}
	else
	{

		/*  */
		pSender = VOD_GetSenderPtr(pSess->svrType, pSess->channel, RTP_STREAM_AUDIO);
		RTSP_SetUdpSendActive(pSess->channel, False);
		if(pSender != NULL)
		{
			if(GS_SUCCESS != RTP_DelUdpSender(pSess->remoteIp, pSess->remoteRtpPort[RTP_STREAM_AUDIO], pSender))
			{
				RTSP_SetUdpSendActive(pSess->channel, True);
				return GS_FAIL;
			}
		}

		/*  */
		pSender = VOD_GetSenderPtr(pSess->svrType, pSess->channel, RTP_STREAM_VIDEO);
		if(pSender != NULL)
		{
			if(GS_SUCCESS != RTP_DelUdpSender(pSess->remoteIp, pSess->remoteRtpPort[RTP_STREAM_VIDEO], pSender))
			{
				RTSP_SetUdpSendActive(pSess->channel, True);
				return GS_FAIL;
			}
		}
		RTSP_SetUdpSendActive(pSess->channel, True);
	}

	RTSP_SessListDel(pSess);

	return GS_SUCCESS;
}

void RTSP_SessionStatCheck(RtspSvr_t *pRtspSvr)
{
	struct list_head *pos, *n;
	RtspSession_t *pRtspSess = NULL;
	list_for_each_safe(pos, n, &pRtspSvr->rtspSessWorkList.rtspSessList)
	{
		pRtspSess = list_entry(pos, RtspSession_t, rtspSessList);
		if(pRtspSess != NULL)
		{
			if(pRtspSess->sessStat == RTSP_STATE_STOP)
			{
				if(GS_SUCCESS != RTSP_SessionClose(pRtspSess))
				{
					//Printf("RTSP_SessionClose error. \n");
				}
				RTSP_SessListPrint(pRtspSvr);
			}
		}
	}
}


int RTSP_EventHandleTeamdown(RtspSession_t *pSess)
{
	int ret;
	char *pTmp = pSess->sendBuf;

	pTmp += RTSP_GetHead( RTSP_STATUS_OK, pSess);
	pTmp += sprintf(pTmp,"Session: %s\r\n", pSess->sessId);
	pTmp += sprintf(pTmp,"Connection: Close\r\n\r\n");
	ret = RTSP_SendReply( RTSP_STATUS_OK, 0, NULL, pSess);
	if(ret == GS_SUCCESS)
	{
		pSess->sessStat = RTSP_STATE_STOP;
	}
	return ret;
}

int RTSP_EventHandlePause(RtspSession_t *pSess)
{
	return RTSP_SendReply(455, 1, "Accept: OPTIONS, SETUP, " "PLAY, TEARDOWN\r\n", pSess);
}

int RTSP_EventHandleUnknown(RtspSession_t *pSess)
{
	return RTSP_SendReply(501, 1, "Accept: OPTIONS, SETUP, " "PLAY, TEARDOWN\r\n", pSess);
}

int RTSP_EventHandleOptions(RtspSession_t *pSess)
{
	int station;

	station = RTSP_GetHead(RTSP_STATUS_OK, pSess);
	sprintf(pSess->sendBuf + station, "Public: OPTIONS, DESCRIBE, SETUP, PLAY, TEARDOWN, SET_PARAMETER\r\n\r\n");
	return RTSP_SendReply(RTSP_STATUS_OK, 0, NULL, pSess);
}

int RTSP_EventHandleSetParam(RtspSession_t *pSess)
{
	char *pTmp = pSess->sendBuf;

	pTmp += RTSP_GetHead( RTSP_STATUS_OK, pSess);
	/**/
	pTmp += sprintf(pTmp,"Session: %s\r\n\r\n", pSess->sessId);

	return RTSP_SendReply(RTSP_STATUS_OK, 0, NULL, pSess);
}

void ReadAccountInfoFromFile()
{
	int i, ret, j, k;
	char userpasswd[64];
	memset(g_account, 0, sizeof(g_account));
	FILE *fp = fopen("/mnt/nand/user.info", "r");
	if(fp == NULL) return;
	i = 0;
	while(1)
	{
		ret = fscanf(fp, "%s\n", userpasswd);	
		if(ret == -1)
			break;

		k = -1;
		for(j = 0; userpasswd[++k] != ':'; j++)
		{
			g_account[i].user[j] = 	userpasswd[k];
		}
		g_account[i].user[j] = '\0';

		for(j = 0; userpasswd[++k] != '\0'; j++)
		{
			g_account[i].password[j] = userpasswd[k];
		}
		g_account[i].password[j] = '\0';

		i++;
	}

	if(i == 0)
	{
		strcpy(g_account[0].user, "admin");
		strcpy(g_account[0].password, "admin");
	}
	fclose(fp);
/*
	for(i = 0; i < ACOUNT_NUM; i++){
		if(strlen(g_account[i].user)){
			printf("%s:%s ~~~~~~~~\n", g_account[i].user, g_account[i].password);
		}	
	}
*/
}

int GetPasswordByName(char *username, char *password)
{
	int i;
	password[0]='\0';
	for(i = 0; i < ACOUNT_NUM; i++)
	{
		if(strlen(g_account[i].user) > 0 && strcmp(username, g_account[i].user) == 0)
		{
			strcpy(password, g_account[i].password);
			return GS_SUCCESS;
		}
	}

	return GS_FAIL;
}

int RTSP_EventHandleDescribe(RtspSession_t *pSess)
{
	int  port = 0, chn = 0, ch = 0, vencType = 0;
	//int  isAnonymousDisable = 0;
	int	frameRate = 0;
	char surveServer[64];
	char server[128];
	char szChn[256];
	char url[256];
	char uri[256];
	char name[LEN_32];
	char pwd[LEN_32];
	char md5Pwd[LEN_64];
	MD5_CTX 	md5p;
	//UserPwd_t   userInfo;
	char sdp[4096] = {0};// = NULL;
	char *pTmp = NULL;
	char *p = NULL;
	int width, height;
	int urlLen;
//	int device =0;
//	char tmpCh[32] = {0};
	int i = 0;
	char *q = NULL;
	rtsp_describe_Info_t  rtsp_des_server_Info;
	int isPlayBack = 0;//
	playback_rtsp_t playbackRtsp;

	if(pSess == NULL)
	{
		return GS_FAIL;
	}

	ReadAccountInfoFromFile();

	memset(&md5p, 0, sizeof(MD5_CTX));

	if(!sscanf(pSess->recvBuf, " %*s %254s ", url))
	{
		RTSP_SendReply(RTSP_STATUS_BAD_REQUEST, 1, NULL, pSess);
		return GS_FAIL;
	}

	urlLen = strlen(url);
	if((strstr(url,"H264") != NULL) || (strstr(url,"h264") != NULL))
	{//
		isPlayBack = 1;
		if( GS_SUCCESS != RTSP_ParseUrl(&port, server, szChn, url))
		{
			RTSP_SendReply(RTSP_STATUS_BAD_REQUEST, 1, NULL, pSess);
			return GS_FAIL;
		}

		szChn[strlen(szChn) - strlen(".H264")] = 0;
		q = szChn;

		while(1)
		{
			p = strsep(&q, "_");
			if(p != NULL)
			{
				if(i == 0)
				{
					pSess->diskNo = atoi(p);
				}
				else if(i == 1)
				{
					pSess->recordNo = atoi(p);
				}
				else if(i == 2)
				{
					pSess->startAddr = atoi(p);
				}
				else if(i == 3)
				{
					pSess->endAddr = atoi(p);
				}
				i++;
			}
			else
			{
				break;
			}
		}
		pSess->isPlayBack = 1;

		Printf("channelNo =%d, diskNo = %d, recordNo = %d, startAddr = %d, endAddr = %d\r\n",
				pSess->channelNo, pSess->diskNo, pSess->recordNo, pSess->startAddr, pSess->endAddr);
		/*  */
	}
	else
	{
		if( GS_SUCCESS != RTSP_ParseUrl(&port, server, szChn, url))
		{
			//Printf("url is not found !\n");
			RTSP_SendReply(RTSP_STATUS_BAD_REQUEST, 1, NULL, pSess);
			return GS_FAIL;
		}
		chn = atoi(szChn);
		pSess->channelNo = chn/10;
		pSess->streamNo = chn%10;
	}


	#if 1
	if(chn == 0)
	{
		ch = 0;
	}
	else if(chn == 1)
	{
		ch = 1;
	}
	else if(chn == 2)
	{
		ch = 2;
	}
	else
	{
		ch = 0;
	}
	#endif


	if(isPlayBack == 1)
	{
		if(playbackRtspGetFileTimeRange(pSess->diskNo, pSess->recordNo, pSess->startAddr, pSess->endAddr, &playbackRtsp) != 0)
		{
			RTSP_SendReply(551, 1, NULL, pSess);
			return GS_FAIL;
		}
		width = playbackRtsp.width; //gAVSERVER_UI_ctrl.avserverConfig.encodeConfig[ch].cropWidth;
		height = playbackRtsp.height;//gAVSERVER_UI_ctrl.avserverConfig.encodeConfig[ch].cropHeight;
		frameRate = playbackRtsp.frameRate;
	}
	else
	{
		memset(&rtsp_des_server_Info,0,sizeof(rtsp_des_server_Info));
		rtspGetDescribeInfo( pSess->channelNo, pSess->streamNo, &rtsp_des_server_Info);
	
		width=rtsp_des_server_Info.videoWeith;
		height=rtsp_des_server_Info.videoHeigth;
		
		if(networkCheckStreamExist(pSess->channelNo, pSess->streamNo) == 0)
		{
			RTSP_SendReply(551, 1, NULL, pSess);
			//sock_close(pSess->rtspSockFd);
			return GS_FAIL;
		}
	}

	Printf(" 1111111describe  width =%d   height=%d fream=%d pSess->channelNo =%d rtsp_des_server_Info.Describebuf =%s\n",width,height,rtsp_des_server_Info.videoFreamRate,pSess->channelNo,rtsp_des_server_Info.Describebuf);

	if(NULL != strstr(pSess->recvBuf, RTSP_HDR_ACCEPT))
	{
		if(NULL == strstr(pSess->recvBuf, RTSP_SDP))
		{
			RTSP_SendReply(551, 1, NULL, pSess);
			return GS_FAIL;
		}
	}

	pTmp = strstr(pSess->recvBuf, RTSP_HDR_USER_AGENT);
	if(pTmp != NULL)
	{
		p = pTmp;
		while(0 != (char)(*p))
		{
			if((char)(*p) == 10 || (char)(*p) == 13)
			{
				break;
			}

			if((char)(*p) == ':')
			{
				pTmp = p + 2;
			}
			p ++;
		}
		if(p != pTmp)
		{
			strncpy(pSess->userAgent, pTmp, p - pTmp);
		}
	}

        
	//if( isAnonymousDisable == IS_UNABLE )
#ifdef ANONYMOUS_ENABLE
if(0)
#else
if(1)
#endif
	{
		memset(surveServer, 0, 64);
		strncpy(surveServer, "Surveillance Server", 20);
		pTmp = strstr(pSess->recvBuf, RTSP_HDR_AUTHORIZATION);	
		if(pTmp == NULL)
		{
			RTSP_GetSessionId(pSess->nonce, 8);
			Printf("send 401 \n");
			p = pSess->sendBuf;
			p += sprintf(p, "%s %d %s\r\n", RTSP_VERSION, 401,RTSP_GetMethodDescrib(401));
			p += sprintf(p, "Cseq: %d \r\n", pSess->lastRecvSeq);
			/*  */
			//frameRate = CFG_GetvideoFrameRate(ch);
			if(frameRate == 0){
				frameRate = 30000;
			}
			p += sprintf(p, "Server: Rtsp Server" " %d*%d*%d*%d\r\n", width, height, frameRate/1000, 4096);
			p += sprintf(p, "WWW-Authenticate: Digest " "realm=\"Surveillance Server\", nonce=\"%s\"\r\n\r\n",pSess->nonce);
			strcpy(pSess->uri, url);
			RTSP_SendReply(401, 0, NULL, pSess);
			return GS_SUCCESS;
		}
		else
		{
			if( GS_SUCCESS == RTSP_PraseUserPwd(pSess->recvBuf, name, pwd, uri))
			{
				if(0 != strcmp(name, "guest"))
				{
					char password[32];
					if(GS_SUCCESS == GetPasswordByName(name, password))
					{
						memset(md5Pwd, 0, LEN_64);
						MD5Auth_BuildAuthResonseWithMD5((char *)md5Pwd, LEN_64, 0, name, surveServer, password, pSess->nonce, NULL, NULL, NULL, "DESCRIBE", uri);
						if(0 != strcmp(pwd, md5Pwd))
						{
							#if 0
							p = pSess->sendBuf + RTSP_GetHead(401, pSess);
							#else
							p = pSess->sendBuf;
							p += sprintf(p, "%s %d %s\r\n", RTSP_VERSION, 401,RTSP_GetMethodDescrib(401));
							p += sprintf(p, "Cseq: %d \r\n", pSess->lastRecvSeq);
							/*  */
							//frameRate = CFG_GetvideoFrameRate(ch);
							if(frameRate == 0){
								frameRate = 30000;
							}
							p += sprintf(p, "Server: Rtsp Server" " %d*%d*%d*%d\r\n", width, height, frameRate/1000, 4096);
							#endif
							RTSP_GetSessionId(pSess->nonce, 8);
							p += sprintf(p, "WWW-Authenticate: Digest " "realm=\"Surveillance Server\", " "nonce=\"%s\"\r\n\r\n", pSess->nonce);
							strcpy(pSess->uri, url);
							RTSP_SendReply(401, 0, NULL, pSess);
							return GS_SUCCESS;
						}
					}
					else
					{
						#if 0
						p = pSess->sendBuf + RTSP_GetHead(401, pSess);
						#else
						p = pSess->sendBuf;
						p += sprintf(p, "%s %d %s\r\n", RTSP_VERSION, 401,RTSP_GetMethodDescrib(401));
						p += sprintf(p, "Cseq: %d \r\n", pSess->lastRecvSeq);
						/*  */
						//frameRate = CFG_GetvideoFrameRate(ch);
						if(frameRate == 0){
							frameRate = 30000;
						}
						p += sprintf(p, "Server: Rtsp Server" " %d*%d*%d*%d\r\n", width, height, frameRate/1000, 4096);
						#endif

						RTSP_GetSessionId(pSess->nonce, 8);
						p += sprintf(p, "WWW-Authenticate: Digest " "realm=\"Surveillance Server\", " "nonce=\"%s\"\r\n\r\n", pSess->nonce);
						strcpy(pSess->uri, url);
						RTSP_SendReply(401, 0, NULL, pSess);
						return GS_SUCCESS;
					}
				}
			}
			else
			{
				#if 0
				p = pSess->sendBuf + RTSP_GetHead(401, pSess);
				#else
				p = pSess->sendBuf;
				p += sprintf(p, "%s %d %s\r\n", RTSP_VERSION, 401,RTSP_GetMethodDescrib(401));
				p += sprintf(p, "Cseq: %d \r\n", pSess->lastRecvSeq);
				/*  */
				//frameRate = CFG_GetvideoFrameRate(ch);
				if(frameRate == 0){
					frameRate = 30000;
				}
				p += sprintf(p, "Server: Rtsp Server" " %d*%d*%d*%d\r\n", width, height, frameRate/1000, 4096);
				#endif
				RTSP_GetSessionId(pSess->nonce, 8);
				p += sprintf(p, "WWW-Authenticate: Digest " "realm=\"Surveillance Server\", nonce=\"%s\"\r\n\r\n", pSess->nonce);
				strcpy(pSess->uri, url);
				RTSP_SendReply(401, 0, NULL, pSess);
				//Printf("your username or passworld is NULL\n");
				return GS_SUCCESS;
			}	
		}
	}

	memset(pSess->sendBuf, 0, sizeof(pSess->sendBuf));

	#if 0
	pTmp = pSess->sendBuf + RTSP_GetHead(200, pSess);
	#else
	pTmp = pSess->sendBuf;
	pTmp += sprintf(pTmp, "%s %d %s\r\n", RTSP_VERSION, 200, RTSP_GetMethodDescrib(200));
	pTmp += sprintf(pTmp, "Cseq: %d \r\n", pSess->lastRecvSeq);
	/*  */
	//frameRate = CFG_GetvideoFrameRate(ch);
	if(frameRate == 0){
		frameRate = 30000;
	}
	pTmp += sprintf(pTmp, "Server: Rtsp Server" " %d*%d*%d*%d\r\n", width, height, frameRate/1000, 4096);
	#endif
	pTmp += sprintf(pTmp, "Content-Type: application/sdp\r\n");

	//if(isPlayBack == 1)
	if(1)
	{

#if 1
		p = sdp;
		p += sprintf(p,"v=0\r\n");
		p += sprintf(p,"o=StreamingServer 3331435948 1116907222000 IN IP4 %s\r\n",pSess->hostIp);
		p += sprintf(p,"s=h264.mp4\r\n");
		p += sprintf(p,"c=IN IP4 0.0.0.0\r\n");
		p += sprintf(p,"t=0 0\r\n");
		p += sprintf(p,"a=control:*\r\n");

		/**/
		/*H264 TrackID=0 RTP_PT 96*/
		//vencType = VENC_TYPE_JPEG;//VENC_TYPE_H264;//CFG_GetVideoEncType(ch);

		//if(gAVSERVER_config.encodeConfig[ch].codecType == ALG_VID_CODEC_H264)
		vencType = VENC_TYPE_H264;
		//else	
		//vencType = VENC_TYPE_JPEG;

//		struct tm tmpTm1 = {0}, tmpTm2 = {0};
//		localtime_r(&playbackRtsp.startTime, &tmpTm1);
//		localtime_r(&playbackRtsp.endTime, &tmpTm2);
		switch(vencType)
		{
			case VENC_TYPE_H264:
				p += sprintf(p,"m=video 0 RTP/AVP 96\r\n");
				p += sprintf(p,"a=control:trackID=0\r\n");
				p += sprintf(p,"a=rtpmap:96 H264/90000\r\n");
				p += sprintf(p,"a=ptime:40\r\n");
				p += sprintf(p,"a=range:npt=0-%d\r\n", playbackRtsp.endTime - playbackRtsp.startTime);//
//				p += sprintf(p,"t=%lu-%lu\r\n", playbackRtsp.startTime, playbackRtsp.endTime);//
//				p += sprintf(p,"a=range:clock=%04d%02d%02dT%02d%02d%02d-%04d%02d%02dT%02d%02d%02d\r\n",
//						tmpTm1.tm_year + 1900, tmpTm1.tm_mon + 1, tmpTm1.tm_mday, tmpTm1.tm_hour, tmpTm1.tm_min, tmpTm1.tm_sec,
//						tmpTm2.tm_year + 1900, tmpTm2.tm_mon + 1, tmpTm2.tm_mday, tmpTm2.tm_hour, tmpTm2.tm_min, tmpTm2.tm_sec);
//				p += sprintf(p,"rtsp-caching=100\r\n");//100ms
				p += sprintf(p,"a=fmtp:96 packetization-mode=1; " "sprop-parameter-sets=%s\r\n", (char *)RTSP_Media_Para_GetBase64(ch));//
//				p += sprintf(p,"a=fmtp:96 packetization-mode=1; " "sprop-parameter-sets=%s,%s\r\n", (char *)MP4ToBase64(playbackRtsp.sps, playbackRtsp.spsLen), (char *)MP4ToBase64(playbackRtsp.pps, playbackRtsp.ppsLen));//
				break;

			case VENC_TYPE_JPEG:
				p += sprintf(p,"m=video 0 RTP/AVP 26\r\n");
				p += sprintf(p,"a=control:trackID=0\r\n");
				p += sprintf(p,"a=rtpmap:26 JPEG/90000\r\n");
				break;

			case VENC_TYPE_MPEG4:	//added by xsf for MPEG4
				p += sprintf(p, "m=video 0 RTP/AVP 99\r\n");
				//p += sprintf(p, "m=video 49170/2 RTP/AVP 99\r\n");
				p += sprintf(p, "b=AS:1024\r\n");
				p += sprintf(p, "a=control:trackID=0\r\n");
				p += sprintf(p, "a=rtpmap:99 MP4V-ES/90000\r\n");
				p += sprintf(p, "a=fmtp:99 profile-level-id=1\r\n");
				break;

			default:
				p += sprintf(p,"m=video 0 RTP/AVP 96\r\n");
				p += sprintf(p,"a=control:trackID=0\r\n");
				p += sprintf(p,"a=rtpmap:96 H264/90000\r\n");
				p += sprintf(p,"a=fmtp:96 packetization-mode=1; " "sprop-parameter-sets=%s\r\n",( char *)RTSP_Media_Para_GetBase64(ch));
				break;
		}
		p += sprintf(p, "a=videoinfo:%d*%d*%d*%d\r\n", width, height, frameRate/1000, 4096);

#if 0
		p += sprintf(p,"m=video 0 RTP/AVP 96\r\n");
		p += sprintf(p,"a=control:trackID=0\r\n");
		p += sprintf(p,"a=rtpmap:96 H264/90000\r\n");
#endif

		/**/
		//p += sprintf(p,"m=audio 0 RTP/AVP 8\r\n");
		/* RTP/AVP 97 --> G726  */
		/* RTP/AVP 8  --> G711-A */
		/* RTP/AVP 0  --> G711-U */
		/*, G711-A */

		//if(AUDIO_ENC_FMT_ALAW == CFG_GetAudioEncType()){
		//if(0){
		/* G711-A */
		p += sprintf(p,"m=audio 0 RTP/AVP 0\r\n");
		p += sprintf(p,"a=control:trackID=1\r\n");
		p += sprintf(p,"a=rtpmap:0 PCMU/8000\r\n");
		//}
		/* G726-16 */
		//p += sprintf(p,"a=rtpmap:97 G726-%d/8000\r\n",16);
		p += sprintf(p,"a=ptime:20\r\n\r\n");

		pTmp += sprintf(pTmp,"Content-length: %d\r\n", strlen(sdp));
		pTmp += sprintf(pTmp,"Content-Base: rtsp://%s/%d\r\n\r\n", pSess->hostIp,pSess->streamNo);

		strcat(pTmp, sdp);
		strcpy(pSess->uri, (const char *)url);
#endif
	}
	else
	{
		pTmp += sprintf(pTmp,"Content-length: %d\r\n", strlen(rtsp_des_server_Info.Describebuf));
    		pTmp += sprintf(pTmp,"Content-Base: rtsp://%s/%d\r\n\r\n", pSess->hostIp,pSess->streamNo);
	
		strcat(pTmp, rtsp_des_server_Info.Describebuf);
		strcpy(pSess->uri, (const char *)url);
	}



	return RTSP_SendReply(200, 0, NULL, pSess);
}

int RTSP_EventHandleSetup(RtspSession_t *pSess)
{
	int ret;
	int svrPort = 0;
	int trackId, chn;
	int rtpPort, rtcpPort;
	char url[255], trash[255];
	char server[255], obj[255], line[255];
	char *p = NULL, *pTmp = NULL;

	if(!sscanf(pSess->recvBuf, " %*s %254s ", url))
	{
		RTSP_SendReply(400, 1, NULL, pSess);
		return GS_FAIL;
	}

	if(GS_SUCCESS != RTSP_ParseUrl(&svrPort, (char *)server, (char *)obj, (const char *)url))
	{
		RTSP_SendReply(400, 1, NULL, pSess);
		return GS_FAIL;
	}

	p = strstr(obj, "trackID=");
	if(p == NULL)
	{   
	if((p = strstr(obj, "trackID")) ==NULL)

	  {
		if((p = strstr(obj, "track")) ==NULL)
		  {
		  	 RTSP_SendReply(406, 1, "Require: Transport settings " "of rtp/udp;port=nnnn. ", pSess);
			 return GS_FAIL;
		  }
		else
		   {
                      sscanf(p, "%5s%d", trash, &trackId);
	               sscanf(obj, "%d/%s", &chn, trash);
 		    }
	  }
	  else
	   {
                  sscanf(p, "%7s%d", trash, &trackId);
	           sscanf(obj, "%d/%s", &chn, trash);	  
	   }
	}
	else
	{
               sscanf(p, "%8s%d", trash, &trackId);
	        sscanf(obj, "%d/%s", &chn, trash);
	}

	pSess->channel = chn;

	p = strstr(pSess->recvBuf, RTSP_HDR_TRANSPORT);
	if(p == NULL)
	{
		RTSP_SendReply(406, 1, "Require: Transport settings"
										" of rtp/udp;port=nnnn. ", pSess);
		return GS_FAIL;
	}

#if 0
	if(trackId == TRACK_ID_VIDEO)
	{
		pSess->reqStreamFlag[RTP_STREAM_VIDEO] = True;
	}
	else if(trackId == TRACK_ID_AUDIO)
	{
		pSess->reqStreamFlag[RTP_STREAM_AUDIO] = True;
	}
	else
	{
		//dbg(Err, DbgNoPerror, "track id = %d error \n", trackId);
		RTSP_SendReply(400, 1, NULL, pSess);
		return GS_FAIL;
	}
#else
	pSess->reqStreamFlag[RTP_STREAM_VIDEO] = True;
	pSess->reqStreamFlag[RTP_STREAM_AUDIO] = True;
#endif
	/*
	 * Transport: RTP/AVP;unicast;client_port=6972-6973;source=10.71.147.222;
	 * 		   server_port=6970-6971;ssrc=00003654
	 * trash = "Transport:"
	 * line = "RTP/AVP;unicast;client_port=6972-6973;source=10.71.147.222;
	 *         server_port=6970-6971;ssrc=00003654"
	 */

	if(2 != sscanf(p, "%10s%255s", trash, line))
	{
		RTSP_SendReply(400, 1, 0, pSess);
		return GS_FAIL;
	}

	p = strstr(line, "client_port");
	if(p != NULL)
	{
		p = strstr(p, "=");
		sscanf(p+1, "%d", &rtpPort);

		p = strstr(p, "-");
		sscanf(p+1, "%d", &rtcpPort);

		pSess->remoteRtpPort[trackId]  = rtpPort;
		pSess->remoteRtcpPort[trackId] = rtcpPort;
		pSess->transportType = RTP_TRANSPORT_TYPE_UDP;

		pTmp = pSess->sendBuf;
		pTmp += RTSP_GetHead(200, pSess);
		pTmp += sprintf(pTmp,"Session: %s\r\n", pSess->sessId);
		pTmp += sprintf(pTmp,"Transport: RTP/AVP;unicast;client_port=%d-%d;" "source=%s;server_port=%d-%d;ssrc=00004E87\r\n\r\n", rtpPort, rtcpPort,
				pSess->hostIp, VOD_GetSenderPtr(VOD_SVR_TYPE_RTSP, chn, RTP_STREAM_VIDEO)->rtpPort, VOD_GetSenderPtr(VOD_SVR_TYPE_RTSP, chn, RTP_STREAM_VIDEO)->rtcpPort);
	}
	else
	{

		p = strstr(line, "/TCP");
		if(p != NULL)
		{
			pSess->transportType = RTP_TRANSPORT_TYPE_TCP;
			pTmp = pSess->sendBuf;
			pTmp += RTSP_GetHead(200, pSess);
			pTmp += sprintf(pTmp,"Session: %s;timeout=120\r\n", pSess->sessId);

			p = strstr(line, "interleaved");
			if(p != NULL)
			{
				if(trackId == TRACK_ID_VIDEO)
				{
					if( pSess->setupFlag[0] == 0 )
					{
						p = strstr(p, "=");
						if(p != NULL)
						{
							sscanf(p+1, "%d", &pSess->interleaved[RTP_STREAM_VIDEO].rtp);
						}

						p = strstr(p, "-");
						if(p != NULL)
						{
							sscanf(p+1, "%d", &pSess->interleaved[RTP_STREAM_VIDEO].rtcp);
						}
						else
						{
							pSess->interleaved[RTP_STREAM_VIDEO].rtcp =
										pSess->interleaved[RTP_STREAM_VIDEO].rtp + 1;
						}

						pTmp += sprintf(pTmp, "Transport: RTP/AVP/TCP;unicast;" "interleaved=%d-%d\r\n\r\n", pSess->interleaved[RTP_STREAM_VIDEO].rtp, pSess->interleaved[RTP_STREAM_VIDEO].rtcp);
						pSess->setupFlag[0] = 1;
					}
				}
				else if(trackId == TRACK_ID_AUDIO)
				{
					if( pSess->setupFlag[1] == 0 )
					{
						p = strstr(p, "=");
						if(p != NULL){
							sscanf(p+1, "%d", &pSess->interleaved[RTP_STREAM_AUDIO].rtp);
						}

						p = strstr(p, "-");
						if(p != NULL){
							sscanf(p+1, "%d", &pSess->interleaved[RTP_STREAM_AUDIO].rtcp);
						}else{
							pSess->interleaved[RTP_STREAM_AUDIO].rtcp =
										pSess->interleaved[RTP_STREAM_AUDIO].rtp + 1;
						}

						pTmp += sprintf(pTmp, "Transport: RTP/AVP/TCP;unicast;" "interleaved=%d-%d\r\n\r\n", pSess->interleaved[RTP_STREAM_AUDIO].rtp, pSess->interleaved[RTP_STREAM_AUDIO].rtcp);
						pSess->setupFlag[1] = 1;
					}
				}
				else
				{
					RTSP_SendReply(461, 1, "Unsupported Transport", pSess);
					return GS_FAIL;
				}
			}
		 }
	}
	ret = RTSP_SendReply(200, 0, NULL, pSess);
	if(ret == GS_SUCCESS)
	{
		pSess->sessStat = RTSP_STATE_READY;
	}
	return ret;
}

int RTSP_EventHandlePlay(RtspSession_t *pSess)
{
	int ret;
	int avType;
	char *pTmp = NULL;
	RtpUdpSender_t *pUdpSender = NULL;
	RtpTargetHost_t *pTarget = NULL;

	if(RTSP_STATE_READY != pSess->sessStat)
	{
		return GS_FAIL;
	}

	/* TCP */
	if(RTP_TRANSPORT_TYPE_TCP == pSess->transportType)
	{
		pSess->pRtpTcpSender = (RtpTcpSender_t *)malloc(sizeof(RtpTcpSender_t));
		if(pSess->pRtpTcpSender == NULL)
		{
			RTSP_SendReply(403, 1, NULL, pSess);
			return GS_FAIL;
		}
		pSess->pRtpTcpSender->interleaved[RTP_STREAM_VIDEO].rtp = pSess->interleaved[RTP_STREAM_VIDEO].rtp;
		pSess->pRtpTcpSender->interleaved[RTP_STREAM_VIDEO].rtcp = pSess->interleaved[RTP_STREAM_VIDEO].rtcp;
		pSess->pRtpTcpSender->interleaved[RTP_STREAM_AUDIO].rtp = pSess->interleaved[RTP_STREAM_AUDIO].rtp;
		pSess->pRtpTcpSender->interleaved[RTP_STREAM_AUDIO].rtcp = pSess->interleaved[RTP_STREAM_AUDIO].rtcp;
		pSess->pRtpTcpSender->tcpSockFd = pSess->rtspSockFd;
		pSess->pRtpTcpSender->lastTs = 3600;
		pSess->pRtpTcpSender->AudioSeq = 1200;
		pSess->pRtpTcpSender->audioG711Ssrc = RTP_DEFAULT_SSRC + pSess->channel;
		pSess->pRtpTcpSender->videoH264Ssrc = RTP_DEFAULT_SSRC + pSess->channel + 128;
		pSess->sessStat 	= RTSP_STATE_PLAY;
		/* TCP */
		if( GS_SUCCESS != RTP_TcpStartSendMediaData(pSess))
		{
			pSess->sessStat = RTSP_STATE_STOP;
			RTSP_SendReply(911, 1, "create tcp thread error \n", pSess);
		}
	}
	else
	{
	/* UDP  */
		Printf("Add new Udp client %d, chn = %d\n",pSess->svrType, pSess->channel);
		for(avType=0; avType<RTP_STREAM_MAX; avType++)
		{
			pUdpSender = VOD_GetSenderPtr(pSess->svrType, pSess->channel, avType);
 			if(pUdpSender != NULL)
			{
 				if(pSess->reqStreamFlag[avType] == True && pSess->remoteRtpPort[avType] != 0 )
				{
					pTarget = (RtpTargetHost_t *)RTP_AddUdpSender(pSess->remoteIp, pSess->remoteRtpPort[avType], pUdpSender);
					if(pTarget == NULL)
					{
						RTSP_SendReply(403, 1, NULL, pSess);
						return GS_FAIL;
					}
					pSess->rtpTargets[avType] = pTarget;
					pTarget->pRtspSess		  = pSess;
					//pTarget->hostState 		  = RTP_TARGETHOST_STATE_REQ_IFRAME;
					pUdpSender->bActive 	  = True;
				}
			}
			else
			{
				Printf("pUdpSender is NULL, avType = %d error \n", avType);
			}
		}
	}
	memset(pSess->sendBuf, 0, MAX_RTSP_BUF_LEN);
	pTmp = pSess->sendBuf;
	pTmp += RTSP_GetHead(200, pSess);
	pTmp += sprintf(pTmp, "Session: %s;timeout=120\r\n\r\n", pSess->sessId);
	ret = RTSP_SendReply(200, 0, NULL, pSess);
	if(ret == GS_SUCCESS)
	{
		pSess->sessStat = RTSP_STATE_PLAY;
		gRtspVodSvr[pSess->svrType].clientCnt[pSess->channel] ++;
		RTSP_SessListPrint(&gRtspVodSvr[pSess->svrType]);
	}
	return ret;
}

int RTSP_EventHandle(int event, int stat, RtspSession_t *pSess)
{
	int ret;

#if PRINT_RTSP
	printf("\n================== C->S ================\n");
	printf("rtsp recv:  %s \n", pSess->recvBuf);
	printf("\n================ C->S ==================\n");
#endif

	switch(event)
	{
		case RTSP_REQ_METHOD_OPTIONS:
			ret = RTSP_EventHandleOptions(pSess);
			break;

		case RTSP_REQ_METHOD_TEARDOWN:
			ret = RTSP_EventHandleTeamdown(pSess);
			break;

		case RTSP_REQ_METHOD_SETUP:
			ret = RTSP_EventHandleSetup(pSess);
			break;

		case RTSP_REQ_METHOD_DESCRIBE:
			ret = RTSP_EventHandleDescribe(pSess);
			break;

		case RTSP_REQ_METHOD_PLAY:
			ret = RTSP_EventHandlePlay(pSess);
			break;

		case RTSP_REQ_METHOD_PAUSE:
			ret = RTSP_EventHandlePause(pSess);
			break;

		case RTSP_REQ_METHOD_SET_PARAM:
			ret = RTSP_EventHandleSetParam(pSess);
			break;

		default:
			ret = RTSP_EventHandleUnknown(pSess);
			break;
	}
	return ret;
}

int RTSP_SessionProcess(RtspSession_t *pSess)
{
	int stat, seqNum = 0;
	int headLen, bodyLen;
	int opcode;
	int cseq = -1;
	if(pSess == NULL){
		return GS_FAIL;
	}
	if(GS_SUCCESS != RTSP_RecvMsgParse(&headLen, &bodyLen, pSess)){
		return GS_SUCCESS;
	}

	if(RTSP_PARSE_IS_RESP == RTSP_ResponseMsgCheck(&stat, pSess->recvBuf))
	{
		if(seqNum != pSess->lastSendSeq + 1)
		{
			//Printf("last send sn is %d != resp seq = %d\n",pSess->lastSendSeq, seqNum);
		}
		opcode = RTSP_MAKE_RESP_CMD(pSess->lastSendReq);
		if(stat > RTSP_BAD_STATUS_BEGIN)
		{
			//Printf("response had status = %d. \n", stat);
		}
	}
	else
	{
		opcode = RTSP_GetReq(pSess->recvBuf);
		if(opcode == RTSP_PARSE_INVALID_OPCODE)
		{
			return GS_FAIL;
		}
		else if(opcode == RTSP_PARSE_INVALID)
		{
			return GS_FAIL;
		}
		cseq = RTSP_GetCseq(pSess->recvBuf);
		if(cseq > 0)
		{
			pSess->lastRecvSeq = cseq;
		}
		else
		{
			//Printf("invalid cseq = %d \n", cseq);
		}
		stat = 0;
	}

	return RTSP_EventHandle(opcode, stat, pSess);
}

int RTSP_SvrCreate(int port, int chnNum, VodSvrType_e vodSvrType, RtspSvr_t *pRtspSvr)
{
	int	chn;
	int audioType;
	int videoType;
	int ptType;

	RtpUdpSender_t	*pRtpUdpVideoSender = NULL;
	RtpUdpSender_t  *pRtpUdpAudioSender = NULL;
#if 0
	if(AUDIO_ENC_FMT_ALAW == CFG_GetAudioEncType()){
		audioType = RTP_PT_ALAW;
	}else{
		audioType = RTP_PT_ULAW;
	}
#endif
        audioType = RTP_PT_ULAW;

	pRtspSvr->svrType 		= vodSvrType;
	pRtspSvr->vodSvrState 	= VOD_SVR_STATE_INIT;
	//pRtspSvr->rtspSvrPort   = port;
	//pRtspSvr->rtspSvrSockFd = 0;
//	memset(pRtspSvr->pRtpUdpSender, 0, sizeof(RtpUdpSender_t));

	/*added by xsf for mpeg4 jpeg*/
	for(chn = 0; chn < chnNum; chn++)
	{
		//videoType = VENC_TYPE_H264;//CFG_GetVideoEncType(chn);
  		//if(gAVSERVER_config.encodeConfig[chn].codecType == ALG_VID_CODEC_H264)
			videoType = VENC_TYPE_H264;
		//else	
			//videoType = VENC_TYPE_JPEG;	
		if(videoType == VENC_TYPE_H264)
		{
			ptType = RTP_PT_H264;
		}
		else if(videoType == VENC_TYPE_MPEG4)
		{
			ptType = RTP_PT_MPEG4;
		}
		else
		{
			ptType = RTP_PT_JPEG;
		}

		if(GS_SUCCESS != RTP_CreateUdpSender(&pRtspSvr->pRtpUdpSender[chn][RTP_STREAM_VIDEO], chn, 0, ptType))
		{
			Printf("RTP_CreateUdpSender video error\n");
			return GS_FAIL;
		}
		pRtpUdpVideoSender = pRtspSvr->pRtpUdpSender[chn][RTP_STREAM_VIDEO];
		pRtpUdpVideoSender->avType 	= AV_TYPE_VIDEO;
		pRtpUdpVideoSender->pktType = PACKET_TYPE_RTP;
		pRtpUdpVideoSender->pt		= ptType;

 		if(GS_SUCCESS != RTP_CreateUdpSender(&pRtspSvr->pRtpUdpSender[chn][RTP_STREAM_AUDIO], chn, 0, audioType))
		{
			Printf("RTP_CreateUdpSender audio error\n");
			return GS_FAIL;
		}
		pRtpUdpAudioSender = pRtspSvr->pRtpUdpSender[chn][RTP_STREAM_AUDIO];
		pRtpUdpAudioSender->avType	= AV_TYPE_AUDIO;
		pRtpUdpAudioSender->pktType = PACKET_TYPE_RTP;
		pRtpUdpAudioSender->pt 		= audioType;
	}
	pthread_mutex_init(&pRtspSvr->rtspListMutex, NULL);
//	RTSP_SessListInit(pRtspSvr);

#ifdef TS_STREAM_UDP
	ES_TS_Initialize();
#endif
	return GS_SUCCESS;
}

void RTSP_SvrClose(int chnNum,RtspSvr_t *pRtspSvr)
{
	int chn = 0;
	RtpUdpSender_t *rtp_udp_sender = NULL;
	for(chn = 0; chn < chnNum; chn++)
	{
		rtp_udp_sender = pRtspSvr->pRtpUdpSender[chn][RTP_STREAM_VIDEO];
		if(rtp_udp_sender)
			fprintf(stderr,"[%s:%s:%d]:fd=%d \n",__FILE__,__FUNCTION__,__LINE__,rtp_udp_sender->rtpSockFd);
		if(rtp_udp_sender && rtp_udp_sender->rtpSockFd > 0)
			close(rtp_udp_sender->rtpSockFd);
		rtp_udp_sender = pRtspSvr->pRtpUdpSender[chn][RTP_STREAM_AUDIO];
		if(rtp_udp_sender && rtp_udp_sender->rtpSockFd > 0)
			close(rtp_udp_sender->rtpSockFd);
	}
}



int RTSP_GetServerFd()
{
	return gRtspVodSvr[VOD_SVR_TYPE_RTSP].rtspSvrSockFd;
}

int RTSP_SvrListen(RtspSvr_t *pRtspSvr)
{
	int sockOptVal = 1;
	
	struct sockaddr_in svrAddr;

	memset(&svrAddr, 0, sizeof(svrAddr));
	
	pRtspSvr->rtspSvrSockFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(-1 == pRtspSvr->rtspSvrSockFd)
	{
		Printf("socket rtspSvrSockFd error \n");
		perror("socket");
		return GS_FAIL;
	}

	if(fcntl(pRtspSvr->rtspSvrSockFd, F_SETFL, O_NONBLOCK) == -1)
	{
		perror("set nonblock");
	}
	if (fcntl(pRtspSvr->rtspSvrSockFd, F_SETFD, 1) == -1)
	{
		perror("setfd");
	}

	if(-1 == setsockopt(pRtspSvr->rtspSvrSockFd , SOL_SOCKET, SO_REUSEADDR,&sockOptVal, sizeof(int)))
	{
		Printf("setsockopt error \n");
		sock_close(pRtspSvr->rtspSvrSockFd);
		pRtspSvr->rtspSvrSockFd = -1;
		return GS_FAIL;
	}

	memset(&svrAddr, 0, sizeof(svrAddr));
	svrAddr.sin_addr.s_addr 	= htonl(INADDR_ANY);
	svrAddr.sin_family 		= AF_INET;
	svrAddr.sin_port  		= htons(pRtspSvr->rtspSvrPort);

	if(-1 == bind(pRtspSvr->rtspSvrSockFd, (struct sockaddr *)&svrAddr,sizeof(struct sockaddr)))
	{
		Printf("bind error , sockfd = %d \n",pRtspSvr->rtspSvrSockFd);
		sock_close(pRtspSvr->rtspSvrSockFd);
		pRtspSvr->rtspSvrSockFd = -1;
		return GS_FAIL;
	}

	if(-1 == listen(pRtspSvr->rtspSvrSockFd,SOMAXCONN))
	{
		Printf("listen error \n");
		sock_close(pRtspSvr->rtspSvrSockFd);
		pRtspSvr->rtspSvrSockFd = -1;
		return GS_FAIL;
	}
	return GS_SUCCESS;
}

void RTSP_ParamsInit()
{
	RtspSvr_t *pRtsp = &gRtspVodSvr[VOD_SVR_TYPE_RTSP];
	memset(pRtsp, 0, sizeof(RtspSvr_t));
	RTP_Base64Init(0);
	RTP_Base64Init(1);
	RTP_Base64Init(2);
}

int RTSP_FreeUdpSender(RtspSvr_t *pRtspSvr, int vodChnNum, int streamNum)
{
	int vodChn, stream;
	for(vodChn=0; vodChn<vodChnNum; vodChn++)
	{
		for(stream=0; stream<streamNum; stream++)
		{
			free(pRtspSvr->pRtpUdpSender[vodChn][stream]);
			pRtspSvr->pRtpUdpSender[vodChn][stream] = NULL;
		}
	}
	return GS_SUCCESS;
}

void RTSP_CloseAllClient(RtspSvr_t *pRtspSvr)
{
	struct list_head *pos, *n;
	RtspSession_t *pSess = NULL;
	if(pRtspSvr == NULL)
	{
		return;
	}

	list_for_each_safe(pos, n, &pRtspSvr->rtspSessWorkList.rtspSessList)
	{
		pSess = list_entry(pos, RtspSession_t, rtspSessList);
		if(pSess != NULL)
		{
			if(GS_SUCCESS != RTSP_SessionClose(pSess))
			{
				//dbg(Err, DbgNoPerror, "RTSP_SessionClose error. \n");
			}
		}
	}
}

void *AUDIO_TalkThrFxn(void *pArgs)
{
#if 0
	//int maxFd = 0;
	int  rLen;
	//fd_set 	fds;
	//fd_set	wFds;	
	//struct timeval  timeout;
	TalkHead_t  head;
	unsigned char audioCompressed[2000];

	printf("Enter talk thread\n");

	//initTalk();

	do{
				memset(&head, 0, sizeof(TalkHead_t));
				rLen = Net_TcpRecvN(g_talkClientFd, (unsigned char *)&head, sizeof(TalkHead_t));
				//printf("recv head %d\n", head.len);
				if(rLen != sizeof(head)){
					dbg(Err, DbgPerror, "talk recv head error \n");
					goto __TalkThrFxnExit;
				}
				
				if(head.len < 0 || head.len > 2000){
					dbg(Err, DbgNoPerror, "talk head len = %d errorno = %d \n", 
						head.len, errno);
					goto __TalkThrFxnExit;
				}
				rLen = Net_TcpRecvN(g_talkClientFd, (unsigned char *)&audioCompressed, head.len);
				//printf("recv data: %d\n", rLen);
				if(rLen != head.len){
					dbg(Err, DbgPerror, "talk recv audio data error %d, "
						"errno = %d\n", rLen, errno);
					goto __TalkThrFxnExit;
				}

				ulaw2linear_g((unsigned char *)(audioCompressed+4),(short *)&sharedAudioData, head.len * 2);
				//writeAudioData((const unsigned char *)sharedAudioData, 320);
	}while(1);

__TalkThrFxnExit:
	close(g_talkClientFd);
	g_talkInUse = 0;
#endif

	return NULL;
}


void *Rtsp_svrThread(void *pArgs)
{
	int ret;
	int recvLen;
	int svrSockFd, cliSockFd;
	int maxFd = 0;
	int sockOptVal = 1;
	int handleStat = GS_SUCCESS;
	int count = 0;//
	fd_set sockFds;
	socklen_t addrLen;
	ConType_e conType;
	char 	recvBuf[MAX_RTSP_BUF_LEN];
	char    respBuf[MAX_RTSP_BUF_LEN];
	struct sockaddr_in cliAddr;
	struct linger  socklinger;
	struct timeval timeout;
	int recvSize = 0;//socket
	struct list_head *pos, *n;
	RtspSession_t *pSess = NULL;
	RtspSvr_t *pRtspSvr = &gRtspVodSvr[VOD_SVR_TYPE_RTSP];
	
	while(1)
	{
		svrSockFd = pRtspSvr->rtspSvrSockFd;
		/* rtsp accept */
		while(g_rtspServerRun)
		{
			if(g_updateParam == 1)
			{
				break;
			}
			RTSP_SessionStatCheck(pRtspSvr);
			FD_ZERO(&sockFds);
			FD_SET(svrSockFd, &sockFds);
			maxFd = svrSockFd;
			RTSP_SetAllSockFd(&sockFds, &maxFd, pRtspSvr);
			timeout.tv_sec  = 1;
			timeout.tv_usec = 0;	/* 2 s timeout */
			ret = select(maxFd + 1, &sockFds, NULL, NULL, &timeout);
			if(ret < 0)
			{
				if(errno == EINTR)
				{
					continue;
				}
				dbg(Err, DbgPerror, "rtsp select erro \n");
				break;
			}
			else if(ret == 0)
			{
				continue;
			}
			else
			{
				if(FD_ISSET(svrSockFd, &sockFds))
				{
					addrLen = sizeof(cliAddr);
					cliSockFd = accept(svrSockFd, (struct sockaddr *)&cliAddr,&addrLen);
					if(cliSockFd <= 0)
					{
						Printf("rtsp server accept error,sock = %d\n",cliSockFd);
						continue;
					}
					timeout.tv_sec  = 3;
					timeout.tv_usec = 0;
					if( -1 == setsockopt(cliSockFd, SOL_SOCKET, SO_RCVTIMEO,(char*)&timeout, sizeof(timeout)))
					{
						Printf("setsockopt cliSockFd = %d error\n",cliSockFd);
					}
					if( -1 == setsockopt(cliSockFd, SOL_SOCKET, SO_SNDTIMEO,(char*)&timeout, sizeof(timeout)))
					{
						Printf("setsockopt cliSockFd = %d error\n",cliSockFd);
					}
					recvSize = 128 * 1024;
					ret = setsockopt(cliSockFd, SOL_SOCKET, SO_SNDBUF, (char *)&recvSize, sizeof(recvSize));

					socklinger.l_onoff = 1;
					socklinger.l_linger = 0;
					if( -1 == setsockopt(cliSockFd, SOL_SOCKET, SO_LINGER, &socklinger, sizeof(socklinger)))
					{
						Printf("setsockopt cliSockFd = %d error\n",cliSockFd);
					}
					if( -1 == setsockopt(cliSockFd ,IPPROTO_TCP, TCP_NODELAY,&sockOptVal, sizeof(int)))
					{
						Printf("setsockopt cliSockFd = %d error\n",cliSockFd);
					}

					memset(recvBuf, 0, MAX_RTSP_BUF_LEN);
					recvLen = recv(cliSockFd, recvBuf, MAX_RTSP_BUF_LEN, 0);
					if(recvLen <= 0)
					{
						Printf("rtsp recv error\n");
						sock_close(cliSockFd);
						continue;
					}
#if 0
					if(WORK_STAT_RUNNING != CFG_GetVencWorkStat() ||
							ID_STOP == CFG_GetRtspConnectStat()){
						dbg(Warn, DbgNoPerror, "Please reconnect after 3 seconds ... \n");
						sock_close(cliSockFd);
						cliSockFd = 0;
						continue;
					}
#endif

					conType = RTSP_GetConnectType(recvBuf);
					if(conType == CON_TYPE_RTSP)
					{
						pSess = RTSP_SessionCreate(cliSockFd, pRtspSvr->svrType);
						if(pSess == NULL)
						{
							if(cliSockFd > 2)
							{
								sock_close(cliSockFd);
								cliSockFd = -1;
							}
							continue;
						}
						memcpy(pSess->recvBuf, recvBuf, MAX_RTSP_BUF_LEN);
						pSess->inSize = recvLen;
						ret = RTSP_SessionProcess(pSess);
						if(ret != GS_SUCCESS)
						{
							//close(pSess->rtspSockFd);
							//pSess->rtspSockFd = -1;
							RTSP_SessionClose(pSess);
						}
					}
					else if(conType == CON_TYPE_UCTRL)
					{
						if(strstr(recvBuf, "TALK") != NULL)
						{
							if(g_talkInUse==1)
							{
								sprintf((char *)respBuf, "CMD:TALK MCTP/1.0 SC\n@%d@%d@%d@\n", 0, 0, 0);	
								send(cliSockFd, (char const*) respBuf, strlen(respBuf), 0);	
								if(cliSockFd > 0)
								{
									sock_close(cliSockFd);
									cliSockFd = -1;
								}
							}
							else
							{
								sprintf(respBuf, "CMD:TALK MCTP/1.0 SC\n@%d@%d@1@\n", 0, 1);   // first para @0 single, @1 dual
								send(cliSockFd, (char const*)respBuf, strlen(respBuf), 0);
								g_talkClientFd = cliSockFd;
								if(0 != pthread_create(&g_talkThrId, NULL, AUDIO_TalkThrFxn, NULL))
								{
									if(cliSockFd > 0)
									{
										sock_close(cliSockFd);
										cliSockFd = -1;
									}
								}
								else
								{
									g_talkInUse = 1;
									pthread_detach(g_talkThrId);
								}
							}
						}
						
					}
					else
					{
						if(cliSockFd > 0)
						{
							sock_close(cliSockFd);
							cliSockFd = -1;
						}
					}
				}

				/*  */
				list_for_each_safe(pos, n, &(pRtspSvr->rtspSessWorkList.rtspSessList))
				{
					pSess = list_entry(pos, RtspSession_t, rtspSessList);
					if(pSess != NULL)
					{
						cliSockFd = pSess->rtspSockFd;
						if(pSess->rtspSockFd <= 0)
						{
							Printf("pSess->rtspSockFd = %d \n",pSess->rtspSockFd);
							continue;
						}
						if(svrSockFd != cliSockFd && FD_ISSET(cliSockFd, &sockFds))
						{
							handleStat = GS_FAIL;
							memset(pSess->recvBuf, 0, MAX_RTSP_BUF_LEN);
							recvLen = recv(pSess->rtspSockFd, pSess->recvBuf,MAX_RTSP_BUF_LEN, 0);
							if(recvLen <= 0)
							{
								Printf("recv from [%s] error\n",pSess->remoteIp);
							}
							else
							{
								pSess->inSize = recvLen;
								handleStat = RTSP_SessionProcess(pSess);
							}

							if(handleStat != GS_SUCCESS)
							{
								Printf("close ch %d client ip : [%s] \n",pSess->channel, pSess->remoteIp);
								if(pSess->rtspSockFd > 0)
								{
									FD_CLR(pSess->rtspSockFd, &sockFds);
									sock_close(pSess->rtspSockFd);
								}
								RTSP_SessionClose(pSess);
								RTSP_SessListPrint(pRtspSvr);
							}
						}
					}
				}
			}
		}
		g_rtspServerRun = 0;
		usleep(200*1000);
	}

	Printf("[rtsp] server exit ... \n");

	g_rtspServerRun = 0;
	RTSP_CloseAllClient(pRtspSvr);
	RTSP_FreeUdpSender(pRtspSvr, MAX_VOD_CHN, RTP_STREAM_MAX);
	RTSP_Media_Paras_Exit(0);
	RTSP_Media_Paras_Exit(1);
	RTSP_Media_Paras_Exit(2);
	Printf("[rtsp] server exit ok \n");

	return NULL;
}

void rtspServerUpdateParam()
{
	g_updateParam = 1;
}

int rtspGetServerUpdateParam()
{
	return g_updateParam;
}


int rtspServerClose()
{
	RtspSvr_t *pRtspSvr = &gRtspVodSvr[VOD_SVR_TYPE_RTSP];
	while(g_rtspServerRun == 1 )
		usleep(10000);
	if(pRtspSvr->rtspSvrSockFd >0)
		close(pRtspSvr->rtspSvrSockFd);
	pRtspSvr->rtspSvrSockFd = 0;
	RTSP_CloseAllClient(pRtspSvr);
	RTSP_SvrClose(MAX_VOD_CHN,pRtspSvr);
	RTSP_FreeUdpSender(pRtspSvr, MAX_VOD_CHN, RTP_STREAM_MAX);
	RTSP_Media_Paras_Exit(0);
	RTSP_Media_Paras_Exit(1);
	RTSP_Media_Paras_Exit(2);
	fprintf(stderr,"[%s:%s:%d]:debug \n",__FILE__,__FUNCTION__,__LINE__);
	return 0;
}

int rtspServerReset(int port)
{
	RtspSvr_t *pRtspSvr = &gRtspVodSvr[VOD_SVR_TYPE_RTSP];

	pRtspSvr->rtspSvrPort = port;
	if(GS_SUCCESS != RTSP_SvrCreate(pRtspSvr->rtspSvrPort, MAX_VOD_CHN,VOD_SVR_TYPE_RTSP, pRtspSvr))
	{
		fprintf(stderr,"[%s:%s:%d]:debug \n",__FILE__,__FUNCTION__,__LINE__);
		dbg(Err, DbgNoPerror, "RTSP_SvrCreate error \n");
		return -1;
	}
	
	/* rtsp */
	if(GS_SUCCESS != RTSP_SvrListen(pRtspSvr))
	{
		fprintf(stderr,"[%s:%s:%d]:debug \n",__FILE__,__FUNCTION__,__LINE__);
		dbg(Err, DbgNoPerror, "RTSP_SvrListen error \n");
		return -1;
	}
	g_updateParam = 0;
	g_rtspServerRun = 1;
	fprintf(stderr,"[%s:%s:%d]:end \n",__FILE__,__FUNCTION__,__LINE__);
	return 0;
}

int rtspServerInit()
{
	sdk_net_mng_cfg_t netcfg;
	RtspSvr_t *pRtspSvr = &gRtspVodSvr[VOD_SVR_TYPE_RTSP];

	memset(&netcfg,0,sizeof(netcfg));
	RTSP_SessListInit(pRtspSvr);
	if(databaseOperate(SDK_MAIN_MSG_NET_MNG_CFG, SDK_PARAM_GET, NULL, 0, &netcfg, sizeof(sdk_net_mng_cfg_t)) != sizeof(sdk_net_mng_cfg_t))
	{
		pRtspSvr->rtspSvrPort= 554;
	}
	else
	{
		pRtspSvr->rtspSvrPort = netcfg.dvr_data_port;
	}

	if(GS_SUCCESS != RTSP_SvrCreate(pRtspSvr->rtspSvrPort, MAX_VOD_CHN,VOD_SVR_TYPE_RTSP, pRtspSvr))
	{
		fprintf(stderr,"[%s:%s:%d]:debug \n",__FILE__,__FUNCTION__,__LINE__);
		dbg(Err, DbgNoPerror, "RTSP_SvrCreate error \n");
		return -1;
	}
	
	/* rtsp */
	if(GS_SUCCESS != RTSP_SvrListen(pRtspSvr))
	{
		fprintf(stderr,"[%s:%s:%d]:debug \n",__FILE__,__FUNCTION__,__LINE__);
		dbg(Err, DbgNoPerror, "RTSP_SvrListen error \n");
		return -1;
	}
	return 0;
}

/*RTSPSVR    */
int rtspServerStart()
{
	rtspServerInit();
	g_rtspServerRun = 1;
	if(pthread_create(&g_rtspsvrThreadId, NULL, Rtsp_svrThread, (void *)&gRtspVodSvr[VOD_SVR_TYPE_RTSP]) < 0)
	{
		g_rtspServerRun = 0;
		Printf("Rtsp_svrThread  thread error\r\n");
		return -1;
	}
	return 0;
}

int rtspServerStop()
{
	g_rtspServerRun = 0;
	pthread_join(g_rtspsvrThreadId, NULL);
	return GS_SUCCESS;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


