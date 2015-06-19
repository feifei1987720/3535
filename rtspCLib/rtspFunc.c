/***********************************************************************************

************************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>

#include "codeLib.h"
#include "commonLib.h"
#include "rtspFunc.h"
#include "rtspCLib.h"
#include "myMalloc.h"

#define ADD_FIELD(fmt, value) \
    ret = sprintf(buffer + *at, (fmt), (value)); \
    *at += ret;

static dev_channel_rtsp_Info_t rtspChannel[MAX_DISPLAY_NUM][RTSP_MAX_STREAM_NUM];
static pthread_mutex_t RtspMutex;

#if 0
bool save_patrol_info(int id)
{
    FILE *vidFd;
    size_t strLen;
    char patrolInfo[LEN_16_SIZE];

	/* 打开patrol.txt并写入轮询信息 */
	vidFd = fopen(PATROL_INFO_PATH, "wb");
	if (vidFd == NULL)
	{
		Printf("Failed to open file:\t/data/patrol.txt.error line: %d,error file: %s\n");
		return FALSE;
	}

	/* 保存轮巡到的设备id , 写成固定模式: id:x; */
	strLen = sprintf(patrolInfo, "id:%d;", id);

    if (fwrite(patrolInfo, strLen, 1, vidFd) <= 0)
    {
        Printf("Failed to write message to file %s\n",PATROL_INFO_PATH);
        fclose(vidFd);
        return FALSE;
    }

    fclose(vidFd);

    return TRUE;
}
#endif

/* 设置RTSP各个状态的定时时长 */
void RTSP_SetDefaultTimerLength(rtsp_info_t *pRtspInfo)
{
    int i;

    for (i=0; i<EN_RTSP_STATUS_BUTT; i++)
    {
        switch (i)
        {
        case EN_RTSP_STATUS_AVILABLE:
        case EN_RTSP_STATUS_TEARDOWN:
            pRtspInfo->statusTimeout[i] = 0;
            break;

        case EN_RTSP_STATUS_CONNET:
            pRtspInfo->statusTimeout[i] = (100 * 1000)/(RTSP_TICK_BASE * RTSP_TICK_VALUE);  //1
            break;
        case EN_RTSP_STATUS_OPTION:
        case EN_RTSP_STATUS_DESCRIBE:
        case EN_RTSP_STATUS_SETUPONE:
        case EN_RTSP_STATUS_SETUPTWO:
        case EN_RTSP_STATUS_PLAY:
            pRtspInfo->statusTimeout[i] = (10 * 100*1000)/(RTSP_TICK_BASE * RTSP_TICK_VALUE);  //5
            break;

        default:
            Printf("Timer status error!\n");
            break;
        }
    }
}


/* 建立RTSP的TCP连接 */
int RTSP_Client_SubSvr(rtsp_info_t *pRtspInfo)
{
    struct sockaddr_in ser_addr;
    ST_RTSP_COMMAND_T cmd;
    int flag = 1;
    int iret;

    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(pRtspInfo->pstRTSPDesc->usLocationPort);
    inet_pton(AF_INET, pRtspInfo->pstRTSPDesc->hostAddress, &ser_addr.sin_addr);

	/* 设置状态为CONNET */
	RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_CONNET);

	/* 设置connect的1秒超时，使阻塞连接1秒内如果连接不成功，则强制返回 */
	struct timeval tm;
	tm.tv_sec = 5;
	tm.tv_usec = 0;
	setsockopt(pRtspInfo->pstRTSPCB->iRTSPTcpSock, SOL_SOCKET, SO_SNDTIMEO, &tm, sizeof(tm));
	setsockopt(pRtspInfo->pstRTSPCB->iRTSPTcpSock, SOL_SOCKET, SO_RCVTIMEO, &tm, sizeof(tm));


    iret = connect(pRtspInfo->pstRTSPCB->iRTSPTcpSock, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
    if(iret == -1)
    {
        //		Printf( "Cannot connect to the server--------client TCP\n");
        close(pRtspInfo->pstRTSPCB->iRTSPTcpSock);
        return -1;
    }

#if 0
    struct linger stLinger;

	/*若有数据待发送则延迟关闭*/
	stLinger.l_onoff = 1;
	stLinger.l_linger = 0;
	if (-1 == setsockopt(pRtspInfo->pstRTSPCB->iRTSPTcpSock, SOL_SOCKET, SO_LINGER, &stLinger, sizeof(stLinger)))
	{
		Printf( "Set Socket SO_LINGER Failed(Errno==%d)!\n", errno);
		close(pRtspInfo->pstRTSPCB->iRTSPTcpSock);
		return -1;
	}
#endif

    int nRecvBuf = 64 * 1024; //64K //128K
    if((setsockopt( pRtspInfo->pstRTSPCB->iRTSPTcpSock, SOL_SOCKET, SO_RCVBUF, ( const char* )&nRecvBuf, sizeof( int ) )) == -1)
    {
        Printf( "Set Socket SO_RCVBUF Failed(Errno==%d)!\n", errno);
        return -1;
    }

	/* 允许地址重用 */
	int socket_opt_value = 1;
	if (setsockopt(pRtspInfo->pstRTSPCB->iRTSPTcpSock, SOL_SOCKET, SO_REUSEADDR,&socket_opt_value, sizeof(int)) == -1)
	{
		Printf( "Set Socket SO_REUSEADDR Failed(Errno==%d)!\n", errno);
		return -1;
	}

	/* 设置非阻塞socket*/
	if ( -1==ioctl(pRtspInfo->pstRTSPCB->iRTSPTcpSock, FIONBIO, &flag) )
	{
		Printf( "Set Socket FIONBIO Failed(Errno==%d)!\n", errno);
		close(pRtspInfo->pstRTSPCB->iRTSPTcpSock);
		return -1;
	}

	/* 发送OPTIONS到RTSP服务端 */
	memset(&cmd, 0, sizeof(cmd));
	
	iret = RTSP_Send_Cmd(pRtspInfo, "OPTIONS", &cmd, "");
	if (-1 == iret)
	{
		Printf( "Send Option Failed!\n");
		RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_TEARDOWN);
		return -1;
	}
	//Printf( "describe else\n");
	
	/* 设置状态为OPTION */
	RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_OPTION);

    return 0;
}


/* RTSP模块初始化 */
rtsp_info_t * RTSP_Init()
{
    rtsp_info_t *pRtspInfo = NULL;

    pRtspInfo = (rtsp_info_t *)malloc(sizeof(rtsp_info_t));
    if(NULL == pRtspInfo)
    {
        Printf("Malloc for RTSP Info failed!\n");
        fflush(stderr);
    }
    memset(pRtspInfo, 0, sizeof(rtsp_info_t));

    pRtspInfo->pstRTSPCB = (rtsp_control_block_t *)malloc(sizeof(rtsp_control_block_t));
    if (NULL == pRtspInfo->pstRTSPCB)
    {
        Printf("Malloc for RTSP Desc failed!\n");
        fflush(stderr);
        goto error;
    }
    memset(pRtspInfo->pstRTSPCB, 0, sizeof(rtsp_control_block_t));

	/* 为RTSP描述分配内存 */
	pRtspInfo->pstRTSPDesc = (rtsp_igd_desc_t *)malloc(sizeof(rtsp_igd_desc_t));
	if (NULL == pRtspInfo->pstRTSPDesc)
	{
		Printf("Malloc for RTSP Desc failed!\n");
		fflush(stderr);
		goto error;
	}
	memset(pRtspInfo->pstRTSPDesc, 0, sizeof(rtsp_igd_desc_t));

	/* 为RTP_PARSE分配内存 */
	pRtspInfo->pstRtpParse = (rtsp_rtp_parse_t *)malloc(sizeof(rtsp_rtp_parse_t));
	if (NULL == pRtspInfo->pstRtpParse)
	{
		Printf("Malloc for RTP_PARSE failed!\n");
		fflush(stderr);
		goto error;
	}
	memset(pRtspInfo->pstRtpParse, 0, sizeof(rtsp_rtp_parse_t));

    pRtspInfo->pstRTSPGlobal = (rtsp_global_t *)malloc(sizeof(rtsp_global_t));
    if (NULL == pRtspInfo->pstRTSPGlobal)
    {
        Printf("Malloc for RTP_Global failed!\n");
        fflush(stderr);
        goto error;
    }
    memset(pRtspInfo->pstRTSPGlobal, 0, sizeof(rtsp_global_t));

#if 0
    pRtspInfo->pRecvBuf = (char *)malloc(MAX_FRAME_BUFFER_LEN);
    if(NULL == pRtspInfo->pRecvBuf)
    {
        Printf("Malloc for pRecvBuf failed!\n");
        fflush(stderr);
        goto error;
    }
#endif

    return pRtspInfo;
error:
    //	RTSP_Exit(pRtspInfo);
    RTSP_Free(pRtspInfo);

    return NULL;
}

void RTSP_Set_Param(rtsp_info_t *pRtspInfo, char *pAddr, int port, char *pUserName, char *pPwd, int protocol, char *pFileName)
{
    strcpy(pRtspInfo->pstRTSPDesc->user, pUserName);
    strcpy(pRtspInfo->pstRTSPDesc->password, pPwd);
    strcpy(pRtspInfo->pstRTSPDesc->hostAddress, pAddr);
    sprintf(pRtspInfo->pstRTSPDesc->acLocationURL, "rtsp://%s", pAddr);
    pRtspInfo->pstRTSPDesc->ulLocationIP = ntohl(inet_addr("192.168.1.168"));
    pRtspInfo->pstRTSPDesc->usLocationPort = port;
    pRtspInfo->pstRTSPDesc->protocol = protocol;
    strcpy(pRtspInfo->pstRTSPDesc->fileName, pFileName);
    pRtspInfo->pstRTSPCB->iCSeq = 0;
    pRtspInfo->pstRTSPCB->ucRetryNum = 0;
}

/* RTSP模块初始化 */
int RTSP_Init_Socket(rtsp_info_t *pRtspInfo)
{
	/* 创建 RTSPTcp socket*/
	pRtspInfo->pstRTSPCB->iRTSPTcpSock = socket(AF_INET, SOCK_STREAM, 0);
	if (0 > pRtspInfo->pstRTSPCB->iRTSPTcpSock)
	{
		Printf( "Create Socket Failed(Errno==%d)!\n", errno);
		fflush(stderr);
		return -1;
	}

	if (pRtspInfo->pstRTSPDesc->protocol == TRANSPORT_UDP)
	{
		/* 创建 UdpVedio socket*/
		pRtspInfo->pstRTSPCB->iRTPUdpVedioSock = socket(AF_INET, SOCK_DGRAM, 0);
		if (0 > pRtspInfo->pstRTSPCB->iRTPUdpVedioSock)
		{
			Printf( "Create Socket Failed(Errno==%d)!\n", errno);
			fflush(stderr);
			return -1;
		}

		/* 创建 UdpAudio socket*/
		pRtspInfo->pstRTSPCB->iRTPUdpAudioSock = socket(AF_INET, SOCK_DGRAM, 0);
		if (0 > pRtspInfo->pstRTSPCB->iRTPUdpAudioSock)
		{
			Printf( "Create Socket Failed(Errno==%d)!\n", errno);
			return -1;
		}
	}

	/* 为各状态设置超时时间 */
	RTSP_SetDefaultTimerLength(pRtspInfo);

    return 0;
}

/* RTSP退出处理函数 */
int RTSP_Exit(rtsp_info_t *pRtspInfo)
{
    if(pRtspInfo == NULL)
    {
        return -1;
    }

    if(pRtspInfo->pstRTSPGlobal != NULL)
    {
        pRtspInfo->pstRTSPGlobal->ulTicks = 0;
    }

    if(pRtspInfo->pstRtpParse != NULL)
    {
        memset(pRtspInfo->pstRtpParse, 0, sizeof(rtsp_rtp_parse_t));
    }

    if(pRtspInfo->pstRTSPDesc != NULL)
    {
        if(TRANSPORT_UDP == pRtspInfo->pstRTSPDesc->protocol)
        {
            if(pRtspInfo->pstRTSPCB != NULL)
            {
                if(pRtspInfo->pstRTSPCB->iRTPUdpVedioSock > 0)
                {
                    close(pRtspInfo->pstRTSPCB->iRTPUdpVedioSock);
                    pRtspInfo->pstRTSPCB->iRTPUdpVedioSock = -1;
                }
                if (pRtspInfo->pstRTSPCB->iRTPUdpAudioSock > 0)
                {
                    close(pRtspInfo->pstRTSPCB->iRTPUdpAudioSock);
                    pRtspInfo->pstRTSPCB->iRTPUdpAudioSock = -1;
                }
            }
        }

        if(pRtspInfo->pstRTSPCB != NULL)
        {
            if(pRtspInfo->pstRTSPCB->iRTSPTcpSock > 0)
            {
                close(pRtspInfo->pstRTSPCB->iRTSPTcpSock);
                pRtspInfo->pstRTSPCB->iRTSPTcpSock = -1;
            }
            memset(pRtspInfo->pstRTSPCB, 0, sizeof(rtsp_control_block_t));
        }

        memset(pRtspInfo->pstRTSPDesc, 0, sizeof(rtsp_igd_desc_t));
    }

    return 0;
}

/* RTSP全局变量初始化 */
int RTSP_ResetGlob(rtsp_info_t *pRtspInfo)
{
    if(pRtspInfo == NULL)
    {
        return -1;
    }

    if(pRtspInfo->pstRTSPGlobal != NULL)
    {
        pRtspInfo->pstRTSPGlobal->ulTicks = 0;
        memset(&pRtspInfo->pstRTSPGlobal, 0, sizeof(rtsp_global_t));
    }

    return 0;
}

/* RTSP释放处理函数 */
int RTSP_Free(rtsp_info_t *pRtspInfo)
{
    RTSP_Exit(pRtspInfo);

    if(pRtspInfo->pstRTSPCB != NULL)
    {
        free(pRtspInfo->pstRTSPCB);
        pRtspInfo->pstRTSPCB = NULL;
    }

    if(pRtspInfo->pstRTSPDesc != NULL)
    {
        free(pRtspInfo->pstRTSPDesc);
        pRtspInfo->pstRTSPDesc = NULL;
    }

    if(pRtspInfo->pstRtpParse != NULL)
    {
        free(pRtspInfo->pstRtpParse);
        pRtspInfo->pstRtpParse = NULL;
    }

    if(pRtspInfo->pstRTSPGlobal != NULL)
    {
        free(pRtspInfo->pstRTSPGlobal);
        pRtspInfo->pstRTSPGlobal = NULL;
    }

    if(pRtspInfo != NULL)
    {
        free(pRtspInfo);
        pRtspInfo = NULL;
    }

#if 0
    if(pRtspInfo->pRecvBuf != NULL)
    {
        free(pRtspInfo->pRecvBuf);
        pRtspInfo->pRecvBuf = NULL;
    }
#endif

    return 0;
}

/* RTSP任务处理函数 */
int RTSP_TaskProc(rtsp_info_t *pRtspInfo,int channlNo ,int streamNo)
{
    int iret = 0;

	/* 网络接收 */
	iret = RTSP_TcpSocketRecv(pRtspInfo,channlNo,streamNo);
	if (iret == -1)
	{
		return -1;
	}

    if (pRtspInfo->pstRTSPCB->ulCBTimer > 0)
    {
        pRtspInfo->pstRTSPCB->ulCBTimer--;

		/* 各状态超时处理 */
		if (0 == pRtspInfo->pstRTSPCB->ulCBTimer)
		{
			iret = RTSP_TimerProc(pRtspInfo);
			if (iret == -1)
			{
	 			return -1;
			}
		}
	}

    return 0;
}


/* RTSP定时器处理函数 */
int RTSP_TimerProc(rtsp_info_t *pRtspInfo)
{
	/* CB状态超时 */
	switch (pRtspInfo->pstRTSPCB->ucCBStatus)
	{
		Printf("Main CB Unmown status[%u] timeout!\n", pRtspInfo->pstRTSPCB->ucCBStatus);
		case EN_RTSP_STATUS_CONNET:
		case EN_RTSP_STATUS_OPTION:
		case EN_RTSP_STATUS_DESCRIBE:
		case EN_RTSP_STATUS_SETUPONE:
		case EN_RTSP_STATUS_SETUPTWO:
		case EN_RTSP_STATUS_PLAY:
			RTSP_SetCBState(pRtspInfo,  EN_RTSP_STATUS_TEARDOWN);
			break;

    default:
        break;
    }

    return 0;
}

/* 网络接收处理 */
int RTSP_TcpSocketRecv(rtsp_info_t *pRtspInfo,int channlNo,int streamNo)
{
	/* 在这些连接状态下检测tcp是否可读 */
	switch (pRtspInfo->pstRTSPCB->ucCBStatus)
	{
		case EN_RTSP_STATUS_OPTION:
		case EN_RTSP_STATUS_DESCRIBE:
		case EN_RTSP_STATUS_SETUPONE:
		case EN_RTSP_STATUS_SETUPTWO:
		case EN_RTSP_STATUS_PLAY:
		    	if (0 >= RTSP_SockReadable(pRtspInfo->pstRTSPCB->iRTSPTcpSock))
		    	{
				return 0;
			}
	   	 break;

    default:
        return -1;
    }

	/* 复位接收缓冲区 */
	memset(pRtspInfo->pstRTSPCB->acRtspData, 0, sizeof(pRtspInfo->pstRTSPCB->acRtspData));
	pRtspInfo->pstRTSPCB->usRtspDataLen = 0;

	/* 接收tcp数据 */
	switch(RTSP_TcpRecvRtspMsg(pRtspInfo))
	{
		case EN_RTSP_RECV_CONTINUE:
	    		return 0;

    case EN_RTSP_RECV_FINISHED:
        break;

    default:
        Printf("TCP Recv Data Error!\n");
        return -1;
    }

//Printf("pRtspInfo->pstRTSPCB->ucCBStatus ======== %d\r\n", pRtspInfo->pstRTSPCB->ucCBStatus);
	/* 接收完数据,处理rtsp响应, 根据状态调用不同的处理函数 */
	switch(pRtspInfo->pstRTSPCB->ucCBStatus)
	{
		case EN_RTSP_STATUS_OPTION:
	    		return RTSP_OptionRtspRespMsg(pRtspInfo);
		case EN_RTSP_STATUS_DESCRIBE:
	    		return RTSP_DescribeRtspRespMsg(pRtspInfo,channlNo,streamNo);
		case EN_RTSP_STATUS_SETUPONE:
	    		return RTSP_SetuponeRtspRespMsg(pRtspInfo);
		case EN_RTSP_STATUS_SETUPTWO:
	    		return RTSP_SetuptwoRtspRespMsg(pRtspInfo);
		case EN_RTSP_STATUS_PLAY:
	    		return RTSP_PlayRtspRespMsg(pRtspInfo);

    default:
        Printf("Unmown status[%u] recv packet, bypass it\n", pRtspInfo->pstRTSPCB->ucCBStatus);
        return -1;
    }

    return 0;
}

/* Option响应的处理 */
int RTSP_OptionRtspRespMsg(rtsp_info_t *pRtspInfo)
{
    ST_RTSP_COMMAND_T cmd;
    int iret;

    if (EN_RTSP_STATUS_OPTION != pRtspInfo->pstRTSPCB->ucCBStatus)
    {
        Printf( "Bad State(%u) Proc Http Response Msg!\n",
                pRtspInfo->pstRTSPCB->ucCBStatus);
        return -1;
    }

	/* 200 OK则表示响应成功 */
	if(NULL == strstr(pRtspInfo->pstRTSPCB->acRtspData, "200 OK"))
	{
		Printf("Option response not 200 OK\n");
		RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_TEARDOWN);
		return -1;
	}

	/* 如果RTP使用UDP方式，则需要创建VIDEO和AUDIO的套接字 */
	if(TRANSPORT_UDP == pRtspInfo->pstRTSPDesc->protocol)
	{
		/* 创建RTP Video套接字 */
		iret = RTSP_CreateRTPVedioUDPSoc(pRtspInfo);
		if (-1 == iret)
		{
			Printf("CreateRTPVedioUDPSoc fialed\n");
			return -1;
		}

		/* 创建RTP Audio套接字 */
		iret = RTSP_CreateRTPAudioUDPSoc(pRtspInfo);
		if (-1 == iret)
		{
			Printf("CreateRTPVedioUDPSoc fialed\n");
			return -1;
		}
	}

	/* 发送describe消息 */
	memset(&cmd, 0, sizeof(cmd));
	cmd.accept = "application/sdp";
	iret = RTSP_Send_Cmd(pRtspInfo,  "DESCRIBE", &cmd, "");
	if (-1 == iret)
	{
		Printf("Send discribe fialed!\n");
		RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_TEARDOWN);
		return -1;
	}

	/* 设置状态为DESCRIBE */
	RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_DESCRIBE);

    return 0;
}

/* Describe响应的处理 */
int RTSP_DescribeRtspRespMsg(rtsp_info_t *pRtspInfo,int channlNo,int streamNo)
{
    ST_RTSP_COMMAND_T cmd;
    char uatempbuf[100] = {0};
    int iret;
    char chMD5Resp[128];
    if (EN_RTSP_STATUS_DESCRIBE != pRtspInfo->pstRTSPCB->ucCBStatus)
    {
        Printf( "Bad State(%u) Proc Http Response Msg!\n",
                pRtspInfo->pstRTSPCB->ucCBStatus);
        return -1;
    }

	/* 401错误码表示需要进行认证，200OK表示不需要认证 */
	if (NULL != strstr(pRtspInfo->pstRTSPCB->acRtspData, "401 Unauthorized"))
	{
		/* 解析describe 401响应消息 */
		iret = RTSP_ParseDescribe401Resp(pRtspInfo);
		if (-1 == iret)
		{
			Printf("send discribe fialed\n");
			RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_TEARDOWN);
			return 0;
		}

		/* 解析describe 401响应消息，获取认证信息，重新发送describe消息 */
		memset(&cmd, 0, sizeof(cmd));
		cmd.accept = "application/sdp";
		cmd.authorization = pRtspInfo->pstRTSPDesc->acAuthorization;

        iret = RTSP_Send_Cmd(pRtspInfo, "DESCRIBE", &cmd, "");
        if (-1 == iret)
        {
            Printf("Send discribe fialed!\n");
            RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_TEARDOWN);
            return -1;
        }

		/* 设置状态为DESCRIBE */
		RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_DESCRIBE);
	}
	else if(NULL != strstr(pRtspInfo->pstRTSPCB->acRtspData, "200 OK"))
	{
		/* 解析describe 200 OK响应消息 */
		iret = RTSP_ParseDescribe200Resp(pRtspInfo,channlNo,streamNo);
		if (-1 == iret)
		{
			Printf("send discribe fialed\n");
			RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_TEARDOWN);
			return 0;
		}

		/* 填写RTP传输方式UDP或者TCP， 组播或者单播，端口等 */
		if(TRANSPORT_UDP == pRtspInfo->pstRTSPDesc->protocol)
		{
			sprintf(uatempbuf, "RTP/AVP;unicast;client_port=%d-%d",
	                pRtspInfo->pstRTSPCB->usRTPVideoPort, pRtspInfo->pstRTSPCB->usRTPVideoPort+1);
		}
		else if(TRANSPORT_TCP == pRtspInfo->pstRTSPDesc->protocol)
		{
			sprintf(uatempbuf, "RTP/AVP/TCP;unicast;interleaved=0-1");
		}
		else
		{
			Printf("protocol type error\n");
			return -1;
		}

		/* 发送setup one消息 */
		memset(&cmd, 0, sizeof(cmd));
		cmd.transport = uatempbuf;
		snprintf(pRtspInfo->pstRTSPDesc->acAuthorization, RTSP_MAX_ATH_LEN,
             "Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", response=\"%s\"",
             pRtspInfo->pstRTSPDesc->user,
             pRtspInfo->pstRTSPDesc->realm,
             pRtspInfo->pstRTSPDesc->nonce,
             pRtspInfo->pstRTSPDesc->acLocationURL,
             (char *)MD5Auth_BuildAuthResonseWithMD5(chMD5Resp, sizeof(chMD5Resp),
                                                     0,
                                                     pRtspInfo->pstRTSPDesc->user,
                                                     pRtspInfo->pstRTSPDesc->realm,
                                                     pRtspInfo->pstRTSPDesc->password,
                                                     pRtspInfo->pstRTSPDesc->nonce,
                                                     NULL,
                                                     NULL,
                                                     NULL,
                                                     "SETUP",
                                                     pRtspInfo->pstRTSPDesc->acLocationURL
                                                     ));
		if (0 == pRtspInfo->pstRTSPDesc->ucauthflag)
		{
			cmd.authorization = pRtspInfo->pstRTSPDesc->acAuthorization;
		}

		iret = RTSP_Send_Cmd(pRtspInfo, "SETUP", &cmd, pRtspInfo->pstRTSPDesc->actrackID1);
		if (-1 == iret)
		{
			Printf("Send setup1 fialed!\n");
			RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_TEARDOWN);
			return -1;
		}

		/* 设置状态为SETUPONE */
		RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_SETUPONE);
	}
	else
	{
		Printf("discribe response not 200/401\n");
		RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_TEARDOWN);
	}

    return 0;
}

/* Setupone响应的处理 */
int RTSP_SetuponeRtspRespMsg(rtsp_info_t *pRtspInfo)
{
    ST_RTSP_COMMAND_T cmd;
    char *pStrSession = NULL;
    char *pSrc = NULL;
    char *pDst = NULL;
    char uatempbuf[100] = {0};
    char chMD5Resp[128];
    int iret;


    if (EN_RTSP_STATUS_SETUPONE != pRtspInfo->pstRTSPCB->ucCBStatus)
    {
        Printf( "Bad State(%u) Proc Http Response Msg!\n",
                pRtspInfo->pstRTSPCB->ucCBStatus);
        return -1;
    }

    /* setup one 200 OK */
    if (NULL != strstr(pRtspInfo->pstRTSPCB->acRtspData, "200 OK"))
    {
        /* session ID */
        pStrSession = strstr(pRtspInfo->pstRTSPCB->acRtspData, "Session:");
        //fprintf(stderr,"################%s;%d  :%s\n",__func__,__LINE__, pStrSession);
        if(NULL != pStrSession)
        {
            memset(pRtspInfo->pstRTSPDesc->sessionID, 0, RTSP_MAX_SESSION_LEN);
            pSrc = pStrSession + 9;
            pDst = pRtspInfo->pstRTSPDesc->sessionID;
            while(isalnum(*pSrc))
            {
                *pDst++ = *pSrc++;
            }

        }

		/* 如果trackID2有值，则有两种媒介需要再Setup第二种；否则发送Play */
		if(0 != strlen(pRtspInfo->pstRTSPDesc->actrackID2))
		{
			/* 填写RTP传输方式UDP或者TCP， 组播或者单播，端口等 */
			if(TRANSPORT_UDP == pRtspInfo->pstRTSPDesc->protocol)
			{
				sprintf(uatempbuf, "RTP/AVP;unicast;client_port=%d-%d", pRtspInfo->pstRTSPCB->usRTPAudioPort, pRtspInfo->pstRTSPCB->usRTPAudioPort+1);
			}
			else if(TRANSPORT_TCP == pRtspInfo->pstRTSPDesc->protocol)
			{
				sprintf(uatempbuf, "RTP/AVP/TCP;unicast;interleaved=2-3");
			}
			else
			{
				Printf("protocol type error\n");
				return -1;
			}

			/* 发送setup two消息 */
			memset(&cmd, 0, sizeof(cmd));
			cmd.transport = uatempbuf;
			cmd.session = pRtspInfo->pstRTSPDesc->sessionID;
			if (0 == pRtspInfo->pstRTSPDesc->ucauthflag)
			{
				cmd.authorization = pRtspInfo->pstRTSPDesc->acAuthorization;
			}

			iret = RTSP_Send_Cmd(pRtspInfo, "SETUP", &cmd, pRtspInfo->pstRTSPDesc->actrackID2);
			if (-1 == iret)
			{
				Printf("Send setup2 fialed!\n");
				RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_TEARDOWN);
				return -1;
			}

			/* 设置状态为SETUPTWO */
			RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_SETUPTWO);
		}
		else
		{
			/* 发送play消息 */
			memset(&cmd, 0, sizeof(cmd));
			cmd.session = pRtspInfo->pstRTSPDesc->sessionID;
			cmd.range = "npt=0.000-";
			if (0 == pRtspInfo->pstRTSPDesc->ucauthflag)
			{
				cmd.authorization = pRtspInfo->pstRTSPDesc->acAuthorization;
			}

            iret = RTSP_Send_Cmd(pRtspInfo, "PLAY", &cmd, "");
            if (-1 == iret)
            {
                Printf("Send play fialed!\n");
                RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_TEARDOWN);
                return -1;
            }

            /* PLAY */
            RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_PLAY);

            //Printf("rtsp status play!\n");
        }
    }
    else
    {
        Printf("setupone response not 200 OK\n");
        RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_TEARDOWN);
    }

    return 0;
}

/* Setuptwo响应的处理 */
int RTSP_SetuptwoRtspRespMsg(rtsp_info_t *pRtspInfo)
{
    ST_RTSP_COMMAND_T cmd;
    int iret;
    char chMD5Resp[128];

    if (EN_RTSP_STATUS_SETUPTWO!= pRtspInfo->pstRTSPCB->ucCBStatus)
    {
        Printf("Bad State(%u) Proc Http Response Msg!\n",
               pRtspInfo->pstRTSPCB->ucCBStatus);
        return -1;
    }

	/* 收到setup two 200 OK响应 */
	if (NULL != strstr(pRtspInfo->pstRTSPCB->acRtspData, "200 OK"))
	{
		/* 发送play消息 */
		memset(&cmd, 0, sizeof(cmd));
		cmd.session = pRtspInfo->pstRTSPDesc->sessionID;
		cmd.range = "npt=0.000-";
		          snprintf(pRtspInfo->pstRTSPDesc->acAuthorization, RTSP_MAX_ATH_LEN,
             "Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", response=\"%s\"",
             pRtspInfo->pstRTSPDesc->user,
             pRtspInfo->pstRTSPDesc->realm,
             pRtspInfo->pstRTSPDesc->nonce,
             pRtspInfo->pstRTSPDesc->acLocationURL,
             (char *)MD5Auth_BuildAuthResonseWithMD5(chMD5Resp, sizeof(chMD5Resp),
                                                     0,
                                                     pRtspInfo->pstRTSPDesc->user,
                                                     pRtspInfo->pstRTSPDesc->realm,
                                                     pRtspInfo->pstRTSPDesc->password,
                                                     pRtspInfo->pstRTSPDesc->nonce,
                                                     NULL,
                                                     NULL,
                                                     NULL,
                                                     "PLAY",
                                                     pRtspInfo->pstRTSPDesc->acLocationURL
                                                     ));
		if (0 == pRtspInfo->pstRTSPDesc->ucauthflag)
		{
			cmd.authorization = pRtspInfo->pstRTSPDesc->acAuthorization;
		}

		iret = RTSP_Send_Cmd(pRtspInfo, "PLAY", &cmd, "");
		if (-1 == iret)
		{
			Printf("Send play fialed!\n");
			RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_TEARDOWN);
			return -1;
		}

		/* 设置状态为PLAY */
		RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_PLAY);
	}
	else
	{
		Printf("setupone response not 200 OK\n");
		RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_TEARDOWN);
	}

    return 0;
}

/* Play响应的处理 */
int RTSP_PlayRtspRespMsg(rtsp_info_t *pRtspInfo)
{
    if (EN_RTSP_STATUS_PLAY!= pRtspInfo->pstRTSPCB->ucCBStatus)
    {
        Printf("Bad State(%u) Proc Http Response Msg!\r\n", pRtspInfo->pstRTSPCB->ucCBStatus);
        return -1;
    }

    if(pRtspInfo->pstRTSPCB->acRtspData[0]&0xff =='R')
    {
        /* setup two 200 OKRTSPRTP */
        if (NULL != strstr(pRtspInfo->pstRTSPCB->acRtspData, "200 OK"))
        {
            /*  */
            RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_AVILABLE);
        }
        else
        {
            Printf("setupone response not 200 OK\r\n");
            //fprintf(stderr,"pRtspInfo->pstRTSPCB->acRtspData :  %s ",pRtspInfo->pstRTSPCB->acRtspData);
            //RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_AVILABLE);
            RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_TEARDOWN);
        }
    }
    else
    {
            RTSP_SetCBState(pRtspInfo, EN_RTSP_STATUS_AVILABLE);
    }

    return 0;
}

/* 发送Teardown消息，结束RTSP连接 */
int RTSP_SendTeardownMsg(rtsp_info_t *pRtspInfo)
{
    ST_RTSP_COMMAND_T cmd;
    int iret = 0;

	/* 发送teardown消息 */
	memset(&cmd, 0, sizeof(cmd));
	cmd.session = pRtspInfo->pstRTSPDesc->sessionID;
	if (0 == pRtspInfo->pstRTSPDesc->ucauthflag)
	{
		cmd.authorization = pRtspInfo->pstRTSPDesc->acAuthorization;
	}

    iret = RTSP_Send_Cmd(pRtspInfo, "TEARDOWN", &cmd, "");
    if (0 != iret)
    {
        //Printf("Send teardown fialed!\n");
        return 0;
    }
    else
    {
        //Printf("Send teardown success!\n");
    }

    return 0;
}

/* 接收tcp数据 */
int RTSP_TcpRecvRtspMsg(rtsp_info_t *pRtspInfo)
{
    int iDataLen;

    iDataLen = recv(pRtspInfo->pstRTSPCB->iRTSPTcpSock, pRtspInfo->pstRTSPCB->acRtspData, RTSP_DATA_BUFER_SIZE, 0);
#if 0
    if((pRtspInfo->pstRTSPCB->acRtspData[0]&0xff) == '$')
    {
        int  ret = netSocketSelectWait(pRtspInfo->pstRTSPCB->iRTSPTcpSock, 6000);
        iDataLen = recv(pRtspInfo->pstRTSPCB->iRTSPTcpSock, pRtspInfo->pstRTSPCB->acRtspData, RTSP_DATA_BUFER_SIZE, 0);
        fprintf(stderr,"@@@@@@@@@@@@@@@@@@@@@@@  %s iDataLen===%d %x   %x  %x     \n",strerror(errno),iDataLen ,pRtspInfo->pstRTSPCB->acRtspData[0],pRtspInfo->pstRTSPCB->acRtspData[1],pRtspInfo->pstRTSPCB->acRtspData[2]);
    }
#endif
    if(0 >= iDataLen)
    {
        Printf("TCP Recv Data Error!\n");
        return EN_RTSP_RECV_ERROR;
    }
    pRtspInfo->pstRTSPCB->usRtspDataLen = (unsigned short)iDataLen;
    pRtspInfo->pstRTSPCB->acRtspData[iDataLen] = '\0';

    return EN_RTSP_RECV_FINISHED;
}

/* 解析Describe的，401响应，获取相关的认证信息 */
int RTSP_ParseDescribe401Resp(rtsp_info_t *pRtspInfo)
{
    int nOffset = 0;
    char *pAuth = NULL;
    char *pDigest = NULL;
    char *pRealm = NULL;
    char *pNonce = NULL;
    int i=0;
    char chMD5Resp[128];

    pRtspInfo->pstRTSPDesc->ucauthflag = 0;

    pAuth = strstr(pRtspInfo->pstRTSPCB->acRtspData, "WWW-Authenticate:");
    if( NULL == pAuth )
    {
        return -1;
    }

      //2015-03-25 changed  for being compatible with a new ip-cam  by  cq  ,
   // char *pRTSPD = strstr(pAuth + sizeof("WWW-Authenticate:"), "RTSPD");
    char *pRTSPD = strstr(pAuth + sizeof("WWW-Authenticate:"), "Basic");
    if ( pRTSPD ) {
        char base64Out[128] = { 0 };
        char base64In[128] = { 0 };
        int count = sprintf(base64In, "%s:%s", pRtspInfo->pstRTSPDesc->user, pRtspInfo->pstRTSPDesc->password );
        codeBase64Enc(base64Out, base64In, count);
        snprintf(pRtspInfo->pstRTSPDesc->acAuthorization, RTSP_MAX_ATH_LEN,
                 "Basic %s", base64Out);

        //fprintf(stderr, "=========== base64Out %s \n", base64Out);

        strncpy(pRtspInfo->pstRTSPDesc->realm, "RTSPD", 5);
        pRtspInfo->pstRTSPDesc->realm[5] = '\0';

        return 0;
    }

    pDigest = strstr(pAuth + sizeof("WWW-Authenticate:"), "Digest");
    if( NULL == pDigest )
    {
        return -1;
    }

    pRealm = strstr(pDigest + sizeof("Digest"), "realm=\"");
    if( NULL == pRealm )
    {
        return -1;
    }

    nOffset = strlen("realm=\"")  ;

    for(i = 0; i < RTSP_MAX_REALM_LEN; i++)
    {
        if((pRtspInfo->pstRTSPDesc->realm[i] = *(pRealm+nOffset)) == '\"')
        {
            pRtspInfo->pstRTSPDesc->realm[i] = 0;
            break;
        }
        nOffset++;
    }

    pNonce = strstr(pDigest + sizeof("Digest") + 1, "nonce=\"");
    if( NULL == pNonce )
    {
        return -1;
    }

    nOffset = strlen("nonce=\"") ;
    for(i = 0; i < RTSP_MAX_REALM_LEN; i++)
    {
        if( (pRtspInfo->pstRTSPDesc->nonce[i] = *(pNonce + nOffset) ) == '\"' )
        {
            pRtspInfo->pstRTSPDesc->nonce[i] = 0;
            break;
        }
        nOffset++;
    }

    snprintf(pRtspInfo->pstRTSPDesc->acAuthorization, RTSP_MAX_ATH_LEN,
             "Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", response=\"%s\"",
             pRtspInfo->pstRTSPDesc->user,
             pRtspInfo->pstRTSPDesc->realm,
             pRtspInfo->pstRTSPDesc->nonce,
             pRtspInfo->pstRTSPDesc->acLocationURL,
             (char *)MD5Auth_BuildAuthResonseWithMD5(chMD5Resp, sizeof(chMD5Resp),
                                                     0,
                                                     pRtspInfo->pstRTSPDesc->user,
                                                     pRtspInfo->pstRTSPDesc->realm,
                                                     pRtspInfo->pstRTSPDesc->password,
                                                     pRtspInfo->pstRTSPDesc->nonce,
                                                     NULL,
                                                     NULL,
                                                     NULL,
                                                     "DESCRIBE",
                                                     pRtspInfo->pstRTSPDesc->acLocationURL
                                                     ));

   // fprintf(stderr, "===========RTSP_ParseDescribe401Resp acAuthorization %s \n",
  //          pRtspInfo->pstRTSPDesc->acAuthorization);

    return 0;
}

//sprop-parameter-sets=Z2QAKK2EBUViuKxUdCAqKxXFYqOhAVFYrisVHQgKisVxWKjoQFRWK4rFR0ICorFcVio6ECSFITk8nyfk/k/J8nm5s00IEkKQnJ5Pk/J/J+T5PNzZprQCgC3SpAAAAwHgAABwgYEAALcbAADN/m974XhEI1A=,aO48sA==;

int RTSP_GetResolutionFromSPS(const char *pAcRtspData, int *width, int *height )
{
    if ( !width || !height ) {
        return -1;
    }

    char *pBegin = strstr(pAcRtspData, "sprop-parameter-sets=");
    if ( !pBegin ) {
        return -1;
    }

    pBegin += strlen("sprop-parameter-sets=");

    char *pEnd = strstr(pBegin, "==");
    if ( !pEnd ) {
        return -1;
    }

    char base64Sps[1024] = { 0 };
    int base64Len = 0;
    while ( ',' != *pBegin ) {

        base64Sps[base64Len] = *pBegin;

        base64Len++;
        pBegin++;
    }

    //decode base64
    return procSPS(base64Sps, base64Len, width, height);

}

/* Describe200okDescribe */
int RTSP_ParseDescribe200Resp(rtsp_info_t *pRtspInfo,int channlNo,int streamNo)
{
    char *pPointTrack1 = NULL;
    char *pPointTrack2 = NULL;
    char *pContenBase = NULL;
    int iloop = 0;
    char chMD5Resp[128];
    char Temptrack[256];
    rtsp_describe_Info_t   rtspDesInfo;
#if 0
    Video_Param video_params = {0};
    Audio_Param audio_params ={0,G711_A};
#endif
    int iWidth, iHeight, iFrameRate, iBitRate;

    pPointTrack1 = strstr(pRtspInfo->pstRTSPCB->acRtspData, "trackID=");
    if(NULL != pPointTrack1)
    {
        if(isdigit(*(pPointTrack1+8)))
        {


            pRtspInfo->pstRTSPDesc->trackID1 = *(pPointTrack1+8)-'0';
            snprintf(pRtspInfo->pstRTSPDesc->actrackID1, RTSP_MAX_TRACKID_LEN, "/trackID=%d",pRtspInfo->pstRTSPDesc->trackID1);


        }
        else if(isalpha(*(pPointTrack1+8)))
        {
            sscanf(pPointTrack1+8,"%s",Temptrack);
            snprintf(pRtspInfo->pstRTSPDesc->actrackID1, RTSP_MAX_TRACKID_LEN, "/trackID=%d",Temptrack);

        }

    }
    else
    {
        pPointTrack1 = strstr(pRtspInfo->pstRTSPCB->acRtspData, "track=");
        if(NULL != pPointTrack1)
        {
            if(isdigit(*(pPointTrack1+6)))
            {

                pRtspInfo->pstRTSPDesc->trackID1 = *(pPointTrack1+6)-'0';
                snprintf(pRtspInfo->pstRTSPDesc->actrackID1, RTSP_MAX_TRACKID_LEN, "/track=%d", pRtspInfo->pstRTSPDesc->trackID1);
                Printf("get trackId digit %s \r\n",pRtspInfo->pstRTSPDesc->actrackID1);


            }
            else if(isalpha(*(pPointTrack1+6)))
            {
                sscanf(pPointTrack1+6,"%s",Temptrack);
                snprintf(pRtspInfo->pstRTSPDesc->actrackID1, RTSP_MAX_TRACKID_LEN, "/track=%s", Temptrack);

                Printf("get trackId1 alpha  %s   \r\n",pRtspInfo->pstRTSPDesc->actrackID1);

            }
        }

    }


    if ( !pPointTrack1 )
    {
        pPointTrack1 = strstr(pRtspInfo->pstRTSPCB->acRtspData, "a=control:track");
        if (NULL != pPointTrack1)
        {
            pRtspInfo->pstRTSPDesc->trackID1 = *(pPointTrack1+15)-'0';

            //fprintf(stderr, "get trackId id = %d \r\n", pRtspInfo->pstRTSPDesc->trackID1);
            snprintf(pRtspInfo->pstRTSPDesc->actrackID1, RTSP_MAX_TRACKID_LEN, "/track%d",pRtspInfo->pstRTSPDesc->trackID1);
        }

        if ( ! pPointTrack1 )
        {
            pPointTrack1 = strstr(pRtspInfo->pstRTSPCB->acRtspData, "a=control:rtsp");

            if ( pPointTrack1 ) {
                snprintf(pRtspInfo->pstRTSPDesc->actrackID1, RTSP_MAX_TRACKID_LEN, "%s", pPointTrack1+10);
            }
        }
    }

    if ( !pPointTrack1 )
    {
        Printf("get trackId failed \r\n");
        return -1;
    }

    memset(pRtspInfo->pstRTSPDesc->actrackID2, 0, RTSP_MAX_TRACKID_LEN);
    pPointTrack2 = strstr(pPointTrack1+1, "trackID=");
    if(NULL != pPointTrack2)
    {

        if(isdigit(*(pPointTrack2+8)))
        {
            pRtspInfo->pstRTSPDesc->trackID2 = *(pPointTrack2+8)-'0';
            snprintf(pRtspInfo->pstRTSPDesc->actrackID2, RTSP_MAX_TRACKID_LEN, "/trackID=%d",pRtspInfo->pstRTSPDesc->trackID2);
        }
        else if(isalpha(*(pPointTrack2+8)))
        {
            sscanf(pPointTrack2+8,"%s",Temptrack);
            snprintf(pRtspInfo->pstRTSPDesc->actrackID2, RTSP_MAX_TRACKID_LEN, "/track=%s", Temptrack);


        }

        //  pRtspInfo->pstRTSPDesc->trackID2 = *(pPointTrack2+8) - '0';
        //  snprintf(pRtspInfo->pstRTSPDesc->actrackID2, RTSP_MAX_TRACKID_LEN, "/trackID=%d",pRtspInfo->pstRTSPDesc->trackID2);
    }
    else
    {

        pPointTrack2 = strstr(pPointTrack1+1, "track=");
        if(NULL != pPointTrack2)
        {

            if(isdigit(*(pPointTrack2+6)))
            {
                pRtspInfo->pstRTSPDesc->trackID2 = *(pPointTrack2+6)-'0';
                snprintf(pRtspInfo->pstRTSPDesc->actrackID2, RTSP_MAX_TRACKID_LEN, "/track=%d",pRtspInfo->pstRTSPDesc->trackID2);
            }
            else if(isalpha(*(pPointTrack2+6)))
            {
                sscanf(pPointTrack2+6,"%s",Temptrack);
                snprintf(pRtspInfo->pstRTSPDesc->actrackID2, RTSP_MAX_TRACKID_LEN, "/track=%s", Temptrack);
                Printf("get trackId2 alpha  %s   \r\n",pRtspInfo->pstRTSPDesc->actrackID2);


            }

        }
    }

    if ( NULL == pPointTrack2 ) {
        pPointTrack2 = strstr(pPointTrack1+15, "a=control:track");
        if (pPointTrack2 )
        {
            pRtspInfo->pstRTSPDesc->trackID2 = *(pPointTrack2+15)-'0';
            snprintf(pRtspInfo->pstRTSPDesc->actrackID2, RTSP_MAX_TRACKID_LEN, "/track%d",pRtspInfo->pstRTSPDesc->trackID2);
        }
    }

    pContenBase = strstr(pRtspInfo->pstRTSPCB->acRtspData, "Content-Base:");
    if( NULL != pContenBase )
    {
        pContenBase = pContenBase + 13;
        iloop = 0;
        while(((RTSP_MAX_ATH_LEN - 1) != iloop) && (*pContenBase != '\r'))
        {
            pRtspInfo->pstRTSPDesc->acContenBase[iloop] = *pContenBase;
            iloop++;
            pContenBase++;
        }
        pRtspInfo->pstRTSPDesc->acContenBase[iloop] = '\0';
    }

    if (strncmp(pRtspInfo->pstRTSPDesc->realm , "RTSPD", 5) != 0 ) {

        snprintf(pRtspInfo->pstRTSPDesc->acAuthorization, RTSP_MAX_ATH_LEN,
                 "Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", response=\"%s\"",
                 pRtspInfo->pstRTSPDesc->user,
                 pRtspInfo->pstRTSPDesc->realm,
                 pRtspInfo->pstRTSPDesc->nonce,
                 pRtspInfo->pstRTSPDesc->acLocationURL,
                 (char *)MD5Auth_BuildAuthResonseWithMD5(chMD5Resp, sizeof(chMD5Resp),
                                                         0,
                                                         pRtspInfo->pstRTSPDesc->user,
                                                         pRtspInfo->pstRTSPDesc->realm,
                                                         pRtspInfo->pstRTSPDesc->password,
                                                         pRtspInfo->pstRTSPDesc->nonce,
                                                         NULL,
                                                         NULL,
                                                         NULL,
                                                         "DESCRIBE",
                                                         pRtspInfo->pstRTSPDesc->acLocationURL
                                                         ));
    }

	/* 获取vedio的长、宽、帧率、比特率参数 */
	int isscanfret = 0;
	char * pserver2=NULL;
	char *pServer = NULL;
	/*
	unsigned char basejiema[512]="gd(??Eb??Tt *+?b??QX?+??qX??@TV+??GB??\V*:$?!9<?'??O??y??MB???O??'??<?????,?x `@?]K????@????";
	memcpy(SPS_stream,basejiema,512);
	SPS_stream_stat==1;
	*/
	//SEI_stream_stat=1;
	if((pserver2= strstr(pRtspInfo->pstRTSPCB->acRtspData, "\r\nServer: GrandStream Rtsp Server V200R001 ")))
	{
		pServer = strstr(pRtspInfo->pstRTSPCB->acRtspData, "\r\nServer: GrandStream Rtsp Server V200R001 ");
		if (NULL == pServer)
		{
			pServer = strstr(pRtspInfo->pstRTSPCB->acRtspData, "\r\nServer: GrandStream Rtsp Server ");
			if ( pServer == NULL ) {
				Printf("pServer == NULL error \n");
				fflush(stderr);
			}

            isscanfret = sscanf(pServer+ strlen("\r\nServer: GrandStream Rtsp Server "),
                                "%d*%d*%d*%d\r\n", &iWidth, &iHeight, &iFrameRate, &iBitRate);
        }
        else
        {
            isscanfret = sscanf(pServer+ strlen("\r\nServer: GrandStream Rtsp Server V200R001 "),
                                "%d*%d*%d*%d\r\n", &iWidth, &iHeight, &iFrameRate, &iBitRate);
        }
    }else{
        pServer = strstr(pRtspInfo->pstRTSPCB->acRtspData, "\r\nServer: Rtsp Server  ");
        if (NULL == pServer)
        {
            pServer = strstr(pRtspInfo->pstRTSPCB->acRtspData, "\r\nServer: Rtsp Server ");
            if ( pServer == NULL ) {
                //				Printf("pServer == NULL error \n");
                fflush(stderr);
            } else {

                isscanfret = sscanf(pServer+ strlen("\r\nServer: Rtsp Server "),
                                    "%d*%d*%d*%d\r\n", &iWidth, &iHeight, &iFrameRate, &iBitRate);
            }
        }
        else
        {
            isscanfret = sscanf(pServer+ strlen("\r\nServer:  Rtsp Server  "),
                                "%d*%d*%d*%d\r\n", &iWidth, &iHeight, &iFrameRate, &iBitRate);
        }
    }

    pServer = strstr(pRtspInfo->pstRTSPCB->acRtspData, "\r\na=videoinfo:");
    if (NULL == pServer)
    {
        pServer = strstr(pRtspInfo->pstRTSPCB->acRtspData, "\r\na=videoinfo:");
        if ( pServer == NULL ) {
            fflush(stderr);
        } else {

            isscanfret = sscanf(pServer+ strlen("\r\na=videoinfo:"),
                                "%d*%d*%d*%d\r\n", &iWidth, &iHeight, &iFrameRate, &iBitRate);
        }
    }
    else
    {
        isscanfret = sscanf(pServer+ strlen("\r\na=videoinfo:"),
                            "%d*%d*%d*%d\r\n", &iWidth, &iHeight, &iFrameRate, &iBitRate);
    }

    int otherCam = 0;


    RTSP_GetResolutionFromSPS(pRtspInfo->pstRTSPCB->acRtspData, &iWidth, &iHeight);
    if ( 0 != iWidth && 0 != iHeight ) {
        otherCam = 1;
    }
    //Printf("iWidth ==== %d. %d\r\n", iWidth, iHeight);

    char *pDimensions = strstr(pRtspInfo->pstRTSPCB->acRtspData, "x-dimensions");
    if(pDimensions != NULL)
    {
        sscanf( pDimensions + strlen("x-dimensions:"), "%d, %d\r\n", &iWidth, &iHeight);
        if((0 != iWidth) && (0 != iHeight))
        {
            otherCam = 1;
        }

        //Printf("=============== iWidth %d iHeight %d \r\n",  iWidth, iHeight);
    }

    char *pFrame = strstr(pRtspInfo->pstRTSPCB->acRtspData, "x-framerate");
    if(pFrame != NULL)
    {
        sscanf(pFrame + strlen("x-framerate:"), "%d\r\n", &iFrameRate);

        //		Printf("=============== framerate %d \r\n",  iFrameRate);
    }


    memset(&rtspDesInfo, 0, sizeof(rtspDesInfo));
    pServer = strstr(pRtspInfo->pstRTSPCB->acRtspData, "\r\n\r\n");
    if (NULL == pServer)
    {
        Printf("rtsp describe fail \n");
    }
    else
    {

        //		memcpy(rtspDesInfo.Describebuf, pServer, strlen(pServer));

        strncpy(rtspDesInfo.Describebuf, pServer, sizeof(rtspDesInfo.Describebuf) - 1);

#if 1

        char *trackid1 = NULL;
        char *trackid2 = NULL;
        trackid1 = strstr(rtspDesInfo.Describebuf, "trackID=");
        if((trackid1 != NULL) && (atoi(trackid1+8) >=1))
        {
            //Printf("%d \r\n",atoi(trackid1+8));
            *(trackid1+8) = '0';
        }

        if (!trackid1 )
        {
            trackid1 = strstr(rtspDesInfo.Describebuf, "a=control:track");
            if((trackid1 != NULL) && (atoi(trackid1+15) >=1))
            {
                //Printf("%d \r\n",atoi(trackid1+15));
                *(trackid1+15)='0';
            }
            if ( ! trackid1 )
            {
                trackid1 = strstr(rtspDesInfo.Describebuf, "a=control:rtsp");
                if ( trackid1 )
                {
                   *(trackid1+15)='0';
                }
            }
        }

        if (!trackid1 )
        {
            Printf("get trackId failed \r\n");
            return -1;
        }

        trackid2 = strstr(trackid1 +1, "trackID=");
        if((trackid2 != NULL) && (atoi(trackid2+8) >=2))
        {
            *(trackid2+8) = '1';
        }
        if ( !trackid2 )
        {
            trackid2 = strstr(trackid1 +1, "a=control:track");
            if((trackid2 != NULL) && (atoi(trackid2+15) >=2))
            {
                //Printf("%d \r\n",atoi(trackid2+15));
                *(trackid2+15)='1';
            }

        }


        #endif
    }
    rtspDesInfo.channelNo=channlNo;
    rtspDesInfo.streamNo=streamNo;


    if ( ((NULL != pServer) && (4 == isscanfret)) || otherCam )
    {
        if (!(pRtspInfo->pstRTSPGlobal->nVideoWidth && pRtspInfo->pstRTSPGlobal->nVideoHeight ))
        {
            //Printf("gs_rtsp.c 1178 pRtspInfo->pstRTSPGlobal->nVideoWidth=%d\n",pRtspInfo->pstRTSPGlobal->nVideoWidth);
            pRtspInfo->pstRTSPGlobal->nVideoWidth  	= iWidth;
            pRtspInfo->pstRTSPGlobal->nVideoHeight 	= iHeight;
            pRtspInfo->pstRTSPGlobal->nFrameRate  	= iFrameRate;
            pRtspInfo->pstRTSPGlobal->nBitRate	 	= iBitRate*1024;
            rtspDesInfo.videoHeigth =iHeight;
            rtspDesInfo.videoWeith =iWidth;
            rtspDesInfo.videoFreamRate=iFrameRate;
            rtspDesInfo.videoBitRate=iBitRate*1024;
            rtspSetDescribeInfo(&rtspDesInfo);

#if 0
            video_params.width= iWidth;
            video_params.height = iHeight;
            video_params.frameRate = iFrameRate;
            video_params.bitRate = iBitRate * 1024;
#endif
            //  Printf("Video parameter:width %d,heigth %d,framerate %d,bitrate %d\n", iWidth,iHeight,iFrameRate,iBitRate*1024);
        }
        //else if(pRtspInfo->pstRTSPGlobal->nVideoWidth != iWidth || pRtspInfo->pstRTSPGlobal->nVideoHeight != iHeight)
        else if((pRtspInfo->pstRTSPGlobal->nVideoWidth != iWidth || pRtspInfo->pstRTSPGlobal->nVideoHeight != iHeight)&(iHeight>1920))
        {
            /* patrol.txt*/
            //			save_patrol_info(pRtspInfo->pstRTSPDesc->id);

            pRtspInfo->pstRTSPGlobal->nVideoWidth 	= iWidth;
            pRtspInfo->pstRTSPGlobal->nVideoHeight	= iHeight;
            pRtspInfo->pstRTSPGlobal->nFrameRate	= iFrameRate;
            pRtspInfo->pstRTSPGlobal->nBitRate	 	= iBitRate*1024;
            rtspDesInfo.videoHeigth =iHeight;
            rtspDesInfo.videoWeith =iWidth;
            rtspDesInfo.videoFreamRate=iFrameRate;
            rtspDesInfo.videoBitRate=iBitRate*1024;
            rtspSetDescribeInfo(&rtspDesInfo);
#if 0
            video_params.width= iWidth;
            video_params.height = iHeight;
            video_params.frameRate = iFrameRate;
            video_params.bitRate = iBitRate * 1024;

            patrol_resolution_status = 1;

            Printf("Video parameter:video_params.width %d,video_params.height %d,video_params.frameRate %d,video_params.bitRate %d.\n",
                   video_params.width,video_params.height,video_params.frameRate, video_params.bitRate);
#endif
        }
        else
        {
            //			Printf("not change 200\r\n");
        }
    }

	/* 获取音频模式 */
	unsigned char ucAudioMode = 0;
	char *pPointAudio = strstr(pRtspInfo->pstRTSPCB->acRtspData, "a=rtpmap:0 PCMU/8000");
	if(pPointAudio != NULL)
	{
		ucAudioMode = G711_U;
	}
	else
	{
		pPointAudio = strstr(pRtspInfo->pstRTSPCB->acRtspData,"a=rtpmap:8 PCMA/8000");
		if( pPointAudio != NULL)
		{
			ucAudioMode = G711_A;
		}
		else
		{
			pPointAudio = strstr(pRtspInfo->pstRTSPCB->acRtspData, "a=rtpmap:97 G726-32/8000");
			if(pPointAudio != NULL)
			{
				ucAudioMode = G726_32KBPS;
			}
			else
			{
				pPointAudio = strstr(pRtspInfo->pstRTSPCB->acRtspData, "a=rtpmap:97 G726-40/8000");
				if(pPointAudio != NULL)
				{
					ucAudioMode = G726_40KBPS;
				}
				else
				{
					pPointAudio = strstr(pRtspInfo->pstRTSPCB->acRtspData, "a=rtpmap:97 G726-16/8000");
					if(pPointAudio != NULL)
					{
						ucAudioMode = G726_16KBPS;
					}
					else
					{
						pPointAudio = strstr(pRtspInfo->pstRTSPCB->acRtspData, "a=rtpmap:97 G726-24/8000");
						if(pPointAudio != NULL)
						{
							ucAudioMode = G726_24KBPS;
						}
					}
				}
			}
		}
	}

	if (ucAudioMode != 0)
	{
		/* 通过管道发送audio参数到speech线程 */
	//	audio_params.audioCoder=ucAudioMode;

        if (pRtspInfo->pstRTSPGlobal->ucAudioMode == 0)
        {
            pRtspInfo->pstRTSPGlobal->ucAudioMode = ucAudioMode;
        }
        else if (pRtspInfo->pstRTSPGlobal->ucAudioMode != ucAudioMode)
        {
            pRtspInfo->pstRTSPGlobal->ucAudioMode = ucAudioMode;
        }

    }

    return 0;
}

/* 创建RTSP相关信息 */
void RTSP_Build_Common (char *buffer, int *at, const int iCSeq, ST_RTSP_COMMAND_T *cmd)
{
    int ret;

    ADD_FIELD("CSeq: %d\r\n", iCSeq);
    if (cmd && cmd->accept)
    {
        ADD_FIELD("Accept: %s\r\n", cmd->accept);
    }
    if (cmd && cmd->authorization)
    {
        ADD_FIELD("Authorization: %s\r\n", cmd->authorization);
    }
    if (cmd && cmd->session)
    {
        ADD_FIELD("Session: %s\r\n", cmd->session);
    }
    if (cmd && cmd->range)
    {
        ADD_FIELD("Range: %s\r\n", cmd->range);
    }
    if (cmd && cmd->scale != 0)
    {
        ADD_FIELD("Scale: %d\r\n", cmd->scale);
    }
    if (cmd && cmd->speed != 0)
    {
        ADD_FIELD("Speed: %d\r\n", cmd->speed);
    }
    if (cmd && cmd->transport)
    {
        ADD_FIELD("Transport: %s\r\n", cmd->transport);
    }
    ADD_FIELD("User-Agent: %s\r\n", USER_AGENT);
}

/* 发送RTSP消息函数 */
int RTSP_Send_Cmd (rtsp_info_t *pRtspInfo, const char *cmd_name, ST_RTSP_COMMAND_T *cmd, const char* track_name)
{
    int iret = 0;
    char buffer[RTSP_DATA_BUFER_SIZE] = {0};
    int len = 0;
    int iCSeq = 0;

    iCSeq = pRtspInfo->pstRTSPCB->iCSeq;
    iCSeq++;
    //Printf("gs_rtsp.c 1427 cmd_name=%s\n",cmd_name);
    /*if(!strncmp(cmd_name,"DESCRIBE",8))
    {
        memset(pRtspInfo->pstRTSPDesc->acLocationURL,0,sizeof(pRtspInfo->pstRTSPDesc->acLocationURL));
        memcpy(pRtspInfo->pstRTSPDesc->acLocationURL,"rtsp://192.168.1.118:8557/PSIA/Streaming/channels/2?videoCodecType=H.264",100);
    }
    else if(!strncmp(cmd_name,"SETUP",5))
    {
        memset(pRtspInfo->pstRTSPDesc->acLocationURL,0,sizeof(pRtspInfo->pstRTSPDesc->acLocationURL));
        memcpy(pRtspInfo->pstRTSPDesc->acLocationURL,"rtsp://192.168.1.118:8557/PSIA/Streaming/channels/2?videoCodecType=H.264/track1",100);
    }
    else
    {
        memset(pRtspInfo->pstRTSPDesc->acLocationURL,0,sizeof(pRtspInfo->pstRTSPDesc->acLocationURL));
        memcpy(pRtspInfo->pstRTSPDesc->acLocationURL,"rtsp://192.168.1.118:8557/PSIA/Streaming/channels/2?videoCodecType=H.264",100);
    }*/

    if ( strncmp(track_name, "rtsp", 4) == 0 ) {
        len = sprintf(buffer, "%s %s RTSP/1.0\r\n", cmd_name, track_name);
    } else {
        len = sprintf(buffer, "%s %s%s RTSP/1.0\r\n", cmd_name, pRtspInfo->pstRTSPDesc->acLocationURL, track_name);
    }

    RTSP_Build_Common(buffer, &len, iCSeq, cmd);

	iret = sprintf(buffer + len, "\r\n"); /*消息末尾有两个回车换行*/
	len += iret;

    if(pRtspInfo->pstRTSPCB->iRTSPTcpSock <= 0)
    {
        return -1;
    }
    else
    {
        if(0 >= RTSP_SockWriteable(pRtspInfo->pstRTSPCB->iRTSPTcpSock))
        {
            return -1;
        }

		/* 发送消息 */
		iret = send(pRtspInfo->pstRTSPCB->iRTSPTcpSock, buffer, len, 0);
		if (len != iret)
		{
			Printf("Send packet failed, iret[%d], errno[%d]!\n", iret, errno);
			return -1;
		}
	}

    pRtspInfo->pstRTSPCB->iCSeq = iCSeq;

    return 0;
}

/* 创建RTP的Vedio UDP 套接字 */
int RTSP_CreateRTPVedioUDPSoc(rtsp_info_t *pRtspInfo)
{
    struct sockaddr_in stAddr;
    int iSocket = 1;
    int iret;
    unsigned short usTempPort = RTP_VEDIO_PORT;
    int nRtpRecvBufLen = 2*1024;

    iSocket = pRtspInfo->pstRTSPCB->iRTPUdpVedioSock;

    while(1)
    {
        stAddr.sin_family = AF_INET;
        stAddr.sin_addr.s_addr = INADDR_ANY;
        stAddr.sin_port = htons(usTempPort);
        iret = bind(iSocket, (const struct sockaddr *)&stAddr, sizeof(stAddr));
        if(0 == iret)
        {
            pRtspInfo->pstRTSPCB->usRTPVideoPort= usTempPort;
            break;
        }

        usTempPort += 2;
    }

    pRtspInfo->pstRTSPCB->iRTPUdpVedioSock = iSocket;

    iret = setsockopt(pRtspInfo->pstRTSPCB->iRTPUdpVedioSock, SOL_SOCKET, SO_RCVBUF, &nRtpRecvBufLen, sizeof(nRtpRecvBufLen));
    if(-1 == iret)
    {
        Printf("rtp vedio setsockopt Socket Failed(Errno==%d)!\n", errno);
        return -1;
    }

    return 0;
}

/* 创建RTP的Audio UDP 套接字 */
int RTSP_CreateRTPAudioUDPSoc(rtsp_info_t *pRtspInfo)
{
    struct sockaddr_in stAddr;
    int iSocket = 1;
    int iret;
    unsigned short usTempPort = RTP_AUDIO_PORT;
    int nRtpRecvBufLen = 1400;

    iSocket = pRtspInfo->pstRTSPCB->iRTPUdpAudioSock;

    while(1)
    {
        stAddr.sin_family = AF_INET;
        stAddr.sin_addr.s_addr = INADDR_ANY;
        stAddr.sin_port = htons(usTempPort);
        iret = bind(iSocket, (const struct sockaddr *)&stAddr, sizeof(stAddr));
        if(0 == iret)
        {
            pRtspInfo->pstRTSPCB->usRTPAudioPort = usTempPort;
            break;
        }

        usTempPort += 2;
    }

    pRtspInfo->pstRTSPCB->iRTPUdpAudioSock = iSocket;

    iret = setsockopt(pRtspInfo->pstRTSPCB->iRTPUdpAudioSock, SOL_SOCKET, SO_RCVBUF, &nRtpRecvBufLen, sizeof(nRtpRecvBufLen));
    if(-1 == iret)
    {
        Printf("rtp audio setsockopt Socket Failed(Errno==%d)!\n", errno);
        return -1;
    }

    return 0;
}

/* 判断套接字是否连接成功 */
int RTSP_SockConnectable(int sock_fd)
{
    struct timeval tv;
    fd_set rfds;
    fd_set wfds;
    int retval = 0;

    if (sock_fd < 0)
    {
        Printf("illegal socket fd[%d]!\n", sock_fd);
        return -1;
    }

    FD_ZERO(&rfds);
    FD_SET(sock_fd, &rfds);
    FD_ZERO(&wfds);
    FD_SET(sock_fd, &wfds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    retval = select(sock_fd+1, &rfds, &wfds, NULL, &tv);
    if (retval <= 0)
    {
        return retval;
    }

    if (FD_ISSET(sock_fd, &rfds) || FD_ISSET(sock_fd, &wfds))
    {
        return 1;
    }
    else
    {
        return 0;   /*timeout*/
    }
}

/* 判断套接字是否可读 */
int RTSP_SockReadable(int sock_fd)
{
    struct timeval tv;
    fd_set rfds;
    int retval = 0;
    int iErrCode, iErrCodeLen;

    if (sock_fd < 0)
    {
        Printf("illegal socket fd[%d]\n",sock_fd);
        return -1;
    }

    FD_ZERO(&rfds);
    FD_SET(sock_fd, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 1000000;
    retval = select(sock_fd+1, &rfds, NULL, NULL, &tv);
    if (retval <= 0)
    {
        return retval;
    }

    if (FD_ISSET(sock_fd, &rfds))
    {
        iErrCode = 1;
        iErrCodeLen=sizeof(iErrCode);
        getsockopt(sock_fd,SOL_SOCKET,SO_ERROR,&iErrCode,(socklen_t *)(&iErrCodeLen));
        if (iErrCode != 0)
        {
            return -1;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 0;   /*timeout*/
    }
}

/* 判断套接字是否可写 */
int RTSP_SockWriteable(int sock_fd)
{
    struct timeval tv;
    fd_set wfds;
    int retval = 0;
    int iErrCode, iErrCodeLen;

    if (sock_fd <= 0)
    {
        //Printf( "illegal socket fd[%d]!\n",sock_fd);
        return -1;
    }

	FD_ZERO(&wfds);
	FD_SET(sock_fd, &wfds);
	tv.tv_sec = 0;
	tv.tv_usec = 0; /*根本不等待，检查描述字后立即返回，即轮询*/
	retval = select(sock_fd+1, NULL, &wfds, NULL, &tv);
	if (retval <= 0)
	{
		return retval;
	}

	if (FD_ISSET(sock_fd, &wfds))
	{
		/*获取待处理错误并清除，有待处理错误表示可读写*/
		iErrCode = 1;
		iErrCodeLen=sizeof(iErrCode);
		getsockopt(sock_fd,SOL_SOCKET,SO_ERROR,&iErrCode,(socklen_t *)(&iErrCodeLen));
		if (iErrCode != 0)
		{
			return -1;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		return 0;   /*timeout*/
	}
}

/* 设置RTSP状态和设定定时器时长 */
int RTSP_SetCBState(rtsp_info_t *pRtspInfo, int ucState)
{
    if (EN_RTSP_STATUS_BUTT <= ucState)
    {
        Printf("State(%u) Error!\n", ucState);
        return -1;
    }

    pRtspInfo->pstRTSPCB->ucCBStatus = ucState;
    pRtspInfo->pstRTSPCB->ulCBTimer = pRtspInfo->statusTimeout[ucState];

    return 0;
}

/* 获取当前系统时间 */
int RTSP_GetTick(rtsp_info_t *pRtspInfo)
{
    struct timeval tv;
    struct timezone tz;
    unsigned long ulCurrent,ulTimeLen;

	/* 获取当前系统tick */
	if ((gettimeofday(&tv, &tz)!=0) || (tv.tv_usec>1000000))
	{
		Printf("fetch clock ticks failed!\n");
		return -1;
	}

    ulCurrent = (unsigned long)tv.tv_usec/(RTSP_TICK_VALUE*1000);
    if (ulCurrent < pRtspInfo->pstRTSPGlobal->ulTicks)
    {
        ulTimeLen = 100 - pRtspInfo->pstRTSPGlobal->ulTicks + ulCurrent;
    }
    else
    {
        ulTimeLen = ulCurrent - pRtspInfo->pstRTSPGlobal->ulTicks;
    }

    if (ulTimeLen >= RTSP_TICK_BASE)
    {
        pRtspInfo->pstRTSPGlobal->ulTicks = ulCurrent;
        return 0;
    }

    return -1;
}

/* RTSP URL */
int RTSP_URL(rtsp_info_t *pRtspInfo)
{
    char port[6];
    char uribuff[RTSP_MAX_URL_LEN];

    sprintf(port,"%d", pRtspInfo->pstRTSPDesc->usLocationPort);
    sprintf(uribuff, "%s:%s/%s", pRtspInfo->pstRTSPDesc->acLocationURL,port,pRtspInfo->pstRTSPDesc->fileName);
    strcpy(pRtspInfo->pstRTSPDesc->acLocationURL, uribuff);

    return 0;
}



static struct timeval g_DefTimeout = {1,0};	//5s

ssize_t Recv(int s, void *buf, size_t len, int flags,struct timeval timeout)
{
    fd_set	 file_set;
    int ret;
    FD_ZERO(&file_set);
    FD_SET(s,&file_set);

    ret=select(s + 1, &file_set, NULL, NULL, &timeout);
    if(ret < 0)
    {
        Printf("Recv seletc error \n");
        fflush(stderr);
        return -1;
    }
    else if(ret == 0)
    {
        /*time out*/
        Printf("recv Time Out\n");
        fflush(stderr);
        return -1;
    }

    if(FD_ISSET(s,&file_set))
    {
        ret = recv(s, buf, len, flags);
        if(ret == -1)
        {
            Printf("recv len %d error \n", len);
            fflush(stderr);
            return -1;
        }
    }
    else
    {
        Printf("Not in FD_ISSET \n");
        fflush(stderr);
        ret = -1;
    }

    return ret;
}

int  RecvFull(int sock, char* buffer, int len)
{
    int cur=0, ret;
    do
    {
        ret = Recv(sock, buffer+cur, len-cur,0 ,g_DefTimeout);
        if(ret <= 0)
            return -1;
        cur += ret;
    }
    while(cur < len);

    return len;
}


#if 0
int getRtspBufInfo(char *pBuf, int *pChannel, int *pDataLen)
{
    if((pBuf[0]&0xff) != '$')
    {
        return -1;
    }

    *pChannel = (pBuf[1]&0xff);
    if(*pChannel < 0x00 || *pChannel > 0x03)
    {
        return -1;
    }

    *pDataLen = ((pBuf[2] & 0xff) << 8) + (pBuf[3] & 0xff);
    if(*pDataLen <= 0)
    {
        return -1;
    }

    return 0;
}

#else

int getRtspBufInfo(char *pBuf, int *pChannel, int *pDataLen)
{

    if((pBuf[0]&0xff) != '$')
    {
        if((pBuf[0]&0xff) == 'R')
        {
            return  0;
        }
        else
        {
            return -1;
        }
    }
    *pChannel = (pBuf[1]&0xff);
    if(*pChannel < 0x00 || *pChannel > 0x03)
    {
        return -1;
    }

    *pDataLen = ((pBuf[2] & 0xff) << 8) + (pBuf[3] & 0xff);
    if(*pDataLen <= 0)
    {
        return -1;
    }
    return 1;
}

#endif

#if 1
int Recv_TCP(int sock, stream_packet_queue_t *pPacketQueue, stream_packet_t *pPacket, int packetLen, int *recvLen)
{
    int ret = 0;
    char  Magic = 0;
    signed char  Channel = 0;
    short Length = 0;
    char tmpBuf[4] = {0};
    char buffer[1024 * 10] = {0};

    ret = RecvFull(sock, tmpBuf, 4);//, 0 ,g_DefTimeout);
    if(ret <= 0)
    {
        return -1;
    }
    Magic = tmpBuf[0] & 0xff;
    if(Magic != '$')
    {
        memcpy(buffer, &Magic, 1);
        Length = Recv(sock, &buffer[1], sizeof(buffer) - 1 , 0 ,g_DefTimeout);
        if(Length <= 0)
        {
            return -1;
        }

        *recvLen = 1 + Length;
        return 0xFF;
    }

    Channel = tmpBuf[1] & 0xff;
    if(Channel < 0x00 || Channel > 0x03)
    {
        return -1;
    }

    Length = ((tmpBuf[2] & 0xff) << 8) + (tmpBuf[3] & 0xff);
    if(Length <= 0)
    {
        return -1;
    }
    if(Length <= MAX_FRAME_BUFFER_LEN)//packetLen >= Length)
    {
#if 0
        pPacket->pPacketBuf = malloc(((Length/4) + 1) * 4);
        //pPacket->pPacketBuf = ncx_slab_alloc(pPacketQueue->sp, (((Length/4) + 1) * 4));
        if(pPacket->pPacketBuf == NULL)
        {
            Printf("malloc packet error, len = %d\r\n", Length);
            exit(0);
        }
#endif
        ret = RecvFull(sock, pPacket->pPacketBuf, Length);
    }
    else
    {
        Printf("this packet is too big, len = %d, packetLen = %d\r\n", Length, MAX_FRAME_BUFFER_LEN);//packetLen);
        ret = -1;
    }

    if(ret <= 0)
    {
        return -1;
    }

    *recvLen = Length;

    return Channel;
}
#endif

#if 0
/**********************************************************************************
*              视频数据处理函数
**********************************************************************************/
int videoDataProc(unsigned char *recv_buf, unsigned char *recv_buf2, int len)
{
    unsigned char *buf = (unsigned char*)recv_buf+12;	// remove RTP head
    unsigned char nal = buf[0];
    unsigned char type = (nal & 0x1F);
    unsigned char nal_type = type;   			//real type
    int pts = 0;
    unsigned char start_sequence[] = {0, 0, 0, 1};
    rtp_head_t * hRtp_head = NULL;
    static int offset = 0;

    //#define RTSP_TIME_DELTA
#ifdef RTSP_TIME_DELTA
    struct timeval uTimeStart, uTimeStop;
    int timeDelta;
#endif

    if(type >= 1 && type <= 23)
        type = 1;
    len -= 12;

    unsigned char reconstructed_nal;

	switch (type)
	{
		case 0:	//undefined
			Printf("Receive rtsp.c 2056data type 0: Undefined, MJPEG Data???\n");
			break;
		case 1:
			//获取时间戳
			hRtp_head = (rtp_head_t *) recv_buf;
			pts = hRtp_head->ts;
			offset = 0;
			memcpy(recv_buf2 + offset, start_sequence, 4);
			offset += 4;
			memcpy(recv_buf2 + offset, buf, len);
			offset += len;
			
			if (SUCCESS != LIVEBUF_OutputSendBuf(VIDEO_DECODE_CHANNEL, recv_buf2, offset,
			                                                            STREAM_TYPE_VIDEO, pts,0))
			{
				Printf(" rtsp.c 2071output_send_buf error");
				fflush(stderr);
				offset = 0;
				return -1;
			}

        offset = 0;
        break;
    case 24:
    case 25:
    case 27:
    case 29://unhandled type
        Printf("rtsp.c 2082Receive Unhandled data type\n");
        fflush(stderr);
        break;
    case 28:
        buf++;
        len--;

        unsigned char fu_indicator = nal;
        unsigned char fu_header = *buf;
        unsigned char start_bit = fu_header >> 7;
        unsigned char end_bit   = fu_header &0x40;
        nal_type = (fu_header & 0x1F);

        reconstructed_nal = fu_indicator & 0xE0;
        reconstructed_nal |= nal_type;
        buf++;
        len--;

			if (start_bit)
			{
				#ifdef RTSP_TIME_DELTA
				gettimeofday(&uTimeStart, NULL);
				#endif
				//获取时间戳
				hRtp_head = (rtp_head_t *) recv_buf;
				pts = hRtp_head->ts;
				//获取第一个包
				memcpy(recv_buf2 + offset,start_sequence,sizeof(start_sequence));
				offset += sizeof(start_sequence);
				memcpy(recv_buf2 + offset, &reconstructed_nal, 1);
				offset ++;
				memcpy(recv_buf2 + offset, buf, len);
				offset += len;
			}
			else if (end_bit)           //获取最后一个包
			{
				memcpy(recv_buf2 + offset, buf, len);
				offset += len;
				
		                if (SUCCESS != LIVEBUF_OutputSendBuf(VIDEO_DECODE_CHANNEL, recv_buf2, offset,
		                                                                            STREAM_TYPE_VIDEO, pts,0))
		                {
		                    Printf("output_send_buf error");
				    fflush(stderr);
		                    offset = 0;
		                    return -1;
		                }

            offset = 0;

				#ifdef RTSP_TIME_DELTA
				gettimeofday(&uTimeStop, NULL);
				timeDelta = (int)((uTimeStop.tv_sec-uTimeStart.tv_sec)*1000000+(uTimeStop.tv_usec-uTimeStart.tv_usec));
				Printf("rtspReceiveThread,include write buffer time : %d ms,%d us,\n",timeDelta/1000, timeDelta);
				#endif
			}
			else //获取中间的包
			{
				memcpy(recv_buf2 + offset, buf, len);
				offset += len;
			}

        break;
    default:
        Printf("error type??\n");
        break;
    }

    return 0;
}
#endif

int RTSP_RTP_TcpRecv(rtsp_info_t *pRtspInfo, stream_packet_queue_t *pPacketQueue, stream_packet_t *pPacket, int packetBufLen, int *pChannel, int *pRecvLen)
{
    /* TCPnChannelVideoAudio */
    *pChannel = Recv_TCP(pRtspInfo->pstRTSPCB->iRTSPTcpSock, pPacketQueue, pPacket, packetBufLen, pRecvLen);

    return (*pChannel);
}

/* 解析数据包中的RTP头，获取时间戳、荷载类型和marker */
void RTSP_RTP_Parse_RTPHead(char * RawData, ST_RTP_HEAD *pstRtpHead)
{
    rtp_head_t * hRtp_head = NULL;

    hRtp_head = (rtp_head_t *) RawData;
    pstRtpHead->uiTimestamp = ntohl(hRtp_head->ts);
    pstRtpHead->uiNetTimestamp = hRtp_head->ts;
    pstRtpHead->ucPayloadType = hRtp_head->pt;
    pstRtpHead->usmarker = hRtp_head->marker;
    pstRtpHead->seqNo = ntohs(hRtp_head->seqno);
}


#if 1



/* 获取帧信息 */
/* 返回值   -1：错误  0：一帧的开始  1：一帧中  2：一帧的结束 */

int RTSP_GetStreamFrameInfo(rtsp_info_t *pRtspInfo, char *pPacketBuf, int dataLen, int *pOffset, int *pFrameLen, unsigned int *pPts, int *pChannel,int *isKeyFrame)
{
    unsigned char ucnaltype = 0;
    unsigned char ucstartflag = 0;
    unsigned char ucendtflag = 0;
    int ret = -1;// -1：错误  0：一帧的开始  1：一帧中  2：一帧的结束
    int playLoadLen = 0;
    int frameLen = *pFrameLen;
    rtp_head_t *pRtpHead = NULL;
    //char startFlag[4] = {0x00, 0x00, 0x00, 0x01};
    static unsigned short seqNo = 0;
    static int iFrame = 0;
    //	static unsigned int old_pts;

#define SAVE 0

#if SAVE
    static int fd = 0;
    if(fd == 0)
    {
        fd = open("h2641.h264", O_CREAT | O_WRONLY);
        if(fd <= 0)
        {
            Exit();
        }
    }
#endif

#if 0
    int i;
    for(i=0;i!=30;++i)
    {
        fprintf(stderr,"%x " ,*(pPacketBuf + i));
    }
    fprintf(stderr,"\r\n" );
#endif

    pRtpHead = (rtp_head_t *)pPacketBuf;

    if((pRtpHead->pt == EN_RTP_PT_H264))
    {

        *pChannel = 0;
        ucnaltype = (*(pPacketBuf + RTP_HEAD_LEN)) & 0xff;
        ucstartflag = (*(pPacketBuf + RTP_HEAD_LEN + 1)) & 0xff;
        ucendtflag = ((ucstartflag & 0x40) >> 6) & 0xff;

        if((iFrame == 0) && ((seqNo + 1) != htons(pRtpHead->seqno)))
        {
            //			Printf("lost packet , seqNo = %d ~ %d\r\n", seqNo, htons(pRtpHead->seqno));
        }
        seqNo = htons(pRtpHead->seqno);
        *pPts = htonl(pRtpHead->ts);

//	Printf("lllllllllllllllllllllllllllll = %d, seq = %d, ucstartflag = %d, ucendtflag = %d, pts = %lu\r\n", dataLen, htons(pRtpHead->seqno), ucstartflag, ucendtflag, pRtpHead->ts);
		/* RTP_NAL_FU_A表示该数据包分包传输，所以需要组包 */
		if(RTP_NAL_FU_A == (ucnaltype & 0x1F))
		{
			/* 帧头添加NAL头 NAL单元类型的基本属性 */
			if(1 == (ucstartflag >> 7))
			{//一帧的开始
				ret = 1;
				frameLen += 4;//加上H264关键字 0x00000001 的长度
				int nalType =  ((ucstartflag & 0x1F) | (ucnaltype & 0xE0));


                *(pPacketBuf + RTP_HEAD_LEN + 1) = nalType;
                //				Printf("xxxxxxxxxxxxxxxxxxx   type = %x\r\n", nalType);

#if SAVE
                if(iFrame == 1)
                {
                    write(fd, startFlag, 4);
                }
#endif
                *pOffset = RTP_HEAD_LEN + 1;
                playLoadLen = dataLen - RTP_HEAD_LEN - 1;
                //pRtspInfo->pstRtpParse->iRecvDataLen++;
            }
            else
            {
                *pOffset = RTP_HEAD_LEN + 2;
                playLoadLen = dataLen - RTP_HEAD_LEN - 2;
                ret = 2;
            }



			frameLen += playLoadLen;
			/* 结束标识为1，则设置发送标识 */
			if(1 == ucendtflag)
			{//一帧的结束
				//pRtspInfo->pstRtpParse->ucsendflag = GS_SEND;
				ret = 3;
			}

#if SAVE
            if(iFrame == 1)
                write(fd, pPacketBuf + (*pOffset), playLoadLen);
#endif
        }
        else if((1 <= (ucnaltype & 0x1F)) && (23 >= (ucnaltype & 0x1F)))
        {//该数据包不分包传输
            //			Printf("xxxxxxxxxxxxxxxxxlllllllllllllllljjjjjjjjjjjjjjj  ====  %x.%x.%x\r\n", *(pPacketBuf + RTP_HEAD_LEN), *(pPacketBuf + RTP_HEAD_LEN + 1), *(pPacketBuf + RTP_HEAD_LEN + 2));
            if((*(pPacketBuf + RTP_HEAD_LEN) & 0x1F) == NAL_TYPE_SPS)
            {
                iFrame = 1;
		*isKeyFrame=1;
            }

            ret = 3;
            *pOffset = RTP_HEAD_LEN;
            playLoadLen = dataLen - RTP_HEAD_LEN;
            frameLen = 4 + playLoadLen;

#if SAVE
            if(iFrame == 1)
            {
                write(fd, startFlag, 4);
                write(fd, pPacketBuf + (*pOffset), playLoadLen);
            }
#endif
        }
        else
        {
            Printf("invalid NAL type, ucnaltype=[%d]\n", ucnaltype);
            fflush(stderr);
            return -1;
        }

        *pFrameLen = frameLen;
    }
    else if(pRtpHead->pt == EN_RTP_PT_H264 +1)
    {


        *pChannel = 2;
        ret = 0;
    }
    else
    {

        *pChannel = 3;
        ret = 0 ;
    }

#if 0
    if(ret == 3)
    {
        //		Printf("ttttttttttttttttttt  cur %u, last %u,  used %d\r\n", htonl(pRtpHead->ts) / 90, old_pts, (htonl(pRtpHead->ts) / 90) - old_pts);
        old_pts = htonl(pRtpHead->ts) / 90;
    }
#endif
    return ret;
}

#endif

int rtspGetDescribeInfo(int channelNo, int streamNo, rtsp_describe_Info_t *pRtspDesInfo)
{
    if((pRtspDesInfo != NULL) && (rtspChannel[channelNo][streamNo].rtspDescribeInfo != NULL))
    {
        pthread_mutex_lock(&RtspMutex);
        memcpy(pRtspDesInfo, rtspChannel[channelNo][streamNo].rtspDescribeInfo, sizeof(rtsp_describe_Info_t));
	//pRtspDesInfo->videoHeigth =576;
	//pRtspDesInfo->videoWeith = 720;
	//pRtspDesInfo->videoFreamRate =30;
	//pRtspDesInfo->videoBitRate =4096;
        //char buff[1024] = "v=0 o=- 38990265062388 38990265062388 IN IP4 172.18.192.172 s=RTSP Session c=IN IP4 172.18.192.172 t=0 0a=control:* a=range:npt=0- m=video 0 RTP/AVP 96 a=rtpmap:96 H264/90000  a=range:npt=0- a=framerate:0S a=fmtp:96 profile-level-id=4d001e; packetization-mode=1; sprop-parameter-sets=Z00AHpWoLASaEAAAAwAQAAADAyhA,aO48gA== a=framerate:25 a=control:trackID=1 ";
        //char buff[1024] = "v=0 o=- 1408034724628258 1408034724628258 IN IP4 192.168.0.204 s=Media Presentatione=NONEb=AS:5100t=0 0a=control:rtsp://192.168.0.204:554/Streaming/Channels/2/?transportmode=unicast&profile=Profile_2m=video 0 RTP/AVP 96b=AS:5000a=control:rtsp://192.168.0.204:554/Streaming/Channels/2/trackID=0?transportmode=unicast&profile=Profile_2a=rtpmap:96 H264/90000a=fmtp:96 profile-level-id=420029; packetization-mode=1; sprop-parameter-sets=Z01AHppmBYCT/NQUFBUAAAMD6AAAw1AE,aO48gA==m=audio 0 RTP/AVP 0b=AS:50a=control:rtsp://192.168.0.204:554/Streaming/Channels/2/trackID=2?transportmode=unicast&profile=Profile_2a=rtpmap:0 PCMU/8000a=Media_header:MEDIAINFO=494D4B48010100000400010010710110401F000000FA000000000000000000000000000000000000;a=appversion:1.0";
        //Printf("pRtspDesInfo->videoHeigth=%d,pRtspDesInfo->videoWeith=%d,pRtspDesInfo->Describebuf=%s\n",pRtspDesInfo->videoHeigth,pRtspDesInfo->videoWeith,pRtspDesInfo->Describebuf);
         //strcpy(pRtspDesInfo->Describebuf ,buff );
        pthread_mutex_unlock(&RtspMutex);
    }
    return 0;
}

int rtspSetDescribeInfo(rtsp_describe_Info_t *pRtspDesInfo)
{
    int channelNo = pRtspDesInfo->channelNo;
    int streamNo = pRtspDesInfo->streamNo;

    if(rtspChannel[channelNo][streamNo].rtspDescribeInfo == NULL)
    {
        rtspChannel[channelNo][streamNo].rtspDescribeInfo = (rtsp_describe_Info_t *)malloc(sizeof(rtsp_describe_Info_t));
        assert(rtspChannel[channelNo][streamNo].rtspDescribeInfo);
    }
    pthread_mutex_lock(&RtspMutex);
    memcpy(rtspChannel[channelNo][streamNo].rtspDescribeInfo, pRtspDesInfo, sizeof(rtsp_describe_Info_t));
    pthread_mutex_unlock(&RtspMutex);
    return 0;
}



int RTSP_KeepAlive(rtsp_info_t *pRtspInfo, const char *cmd_name)
{
    ST_RTSP_COMMAND_T cmd;
    memset(&cmd, 0, sizeof(cmd));
    int iret;
    cmd.session = pRtspInfo->pstRTSPDesc->sessionID;
    iret = RTSP_Send_Cmd(pRtspInfo, cmd_name, &cmd, "");
    if (-1 == iret)
    {
        Printf("Send get_parameter failed!\n");
        return -1;
    }
    return 0;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


