#include "devdiscovery.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>
#include "sdk_struct.h"
#include "LongseDes.h"
#include "global.h"
#include "commonLib.h"
#include "databaseLib.h"
#include "main.h"

#define DD_DEBUG_FLAG 1        /*打开输出*/
/********************************************************************************************/
/*******************************           宏         ***************************************/
/********************************************************************************************/
#if DD_DEBUG_FLAG
#define DD_DEBUG(fmt,args...) printf("DEBUG %s-%d: "fmt"\n",__FUNCTION__,__LINE__,## args);
#else
#define DD_DEBUG(fmt,args...)
#endif

#define DD_CLOSE_SOCKET(fd)   do{if(0>fd) break; close(fd); fd = -1; }while(0)
#define DD_FREE(ptr) do{if(0==(ptr))break; free(ptr); ptr=0; }while(0)
/********************************************************************************************/
/*******************************        全局变量       ***************************************/
/********************************************************************************************/
static pthread_t   DD_g_StartDiscoveryThrID=0;
static int         DD_g_pPipes[2] = {-1,-1};
static task_t     *DD_g_pTask[DD_TASK_MAXNUM];
int                DD_g_taskNum = 0;
extern int  privatedevNum ; 
extern sdk_device_t privateDev[256];


static DSC_NETWORK_INFO  DD_g_NetworkInfo;
/********************************************************************************************/
/*******************************       静态函数声明     ***************************************/
/********************************************************************************************/
/*新建任务*/
static task_t* DD_CreateTask(DD_TASKTYPE_TYPE type, DD_TASK_PROC procFun, void *pData, unsigned int dataLen);
/*销毁任务*/
static void    DD_DeleteTask(task_t* pTask);
/*添加任务*/
static int     DD_AddNewTask(task_t* pTask);
/*移除任务*/
static int     DD_RemoveTask(task_t* pTask);
/*执行任务*/
static int     DD_ProcTask(task_t* pTask);


/*管理设备搜索函数*/
static int DD_ProcSearchDev(struct task_t *pTask);
/*管理服务器函数*/
static int DD_ProcServer(struct task_t *pTask);
/*管理修改本地网络函数*/
static int DD_ProcModifyLocalNetConfig(struct task_t *pTask);
/*管理修改远端网络函数*/
static int DD_ProcModifyDevNetConfig(struct task_t *pTask);

/*提供服务函数*/
static int DD_HannelServerEvent(struct task_t *pTask);
/*提供修改服务*/
static int DD_HannelServerModify(DSC_MODIFY_INFO *pModify, DSC_MODIFY_RESPONSE_INFO *pModifyResponse);

/********************************************************************************************/
/*******************************       函数实现        ***************************************/
/********************************************************************************************/
/*主线程*/
int DD_StartDiscoveryThread()
{
	struct timeval timeout;
	fd_set fds;
	int maxFd;

	task_t  *pTask;
	int      curTime = 0;
	int      len;
	int      ret;
	int      i;

	DD_DEBUG("Start DevDescovery...\n");

	while(1)
	{
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		/*添加管道 fd*/
		FD_ZERO(&fds);
		maxFd = DD_g_pPipes[0];
		FD_SET(DD_g_pPipes[0],&fds);
		/*添加 task fd*/
		for(i=0;i<DD_TASK_MAXNUM;i++){
			pTask = *(DD_g_pTask + i);
			if(0==pTask) continue;
			if(DD_TASKSTATE_FAILED == pTask->state){
				DD_RemoveTask(pTask);
				DD_DeleteTask(pTask);
			}
			FD_SET(pTask->sockFd,&fds);
			maxFd = (pTask->sockFd > maxFd) ? pTask->sockFd : maxFd;
		}
		/*开始 select*/
		ret = select(maxFd+1,&fds,0,0,&timeout);
		//        DD_DEBUG("select result + %d--%d--%d",ret,maxFd,DD_g_taskNum);
		if(ret < 0)
		{
			DD_DEBUG("Socket error. rebulid...");
			sleep(1);
			continue;
		}
		else if(0==ret)
		{
			/*检查并关闭超时的搜索socket*/
			curTime = time(0);
			for(i=0;i<DD_TASK_MAXNUM;i++){
				pTask = *(DD_g_pTask + i);
				if(0==pTask) continue;
				if(curTime - pTask->time > 10){
					pTask->state = DD_TASKSTATE_TIMEOUT;
					DD_ProcTask(pTask);
					if(DD_TASKSTATE_WAIT != pTask->state){
						DD_RemoveTask(pTask);
						DD_DeleteTask(pTask);
					}
				}
			}
			continue;
		}
		/*初始化全局网络参数*/
		if(0!=DD_InitNetworkInfo(&DD_g_NetworkInfo)){
			sleep(1);
			continue;
		}
		/*添加新的搜索socket*/
		if(1 == FD_ISSET(DD_g_pPipes[0],&fds)){
			len = read(DD_g_pPipes[0],&pTask,sizeof(task_t*));
			if(sizeof(task_t*)==len){
				if(DD_TASK_MAXNUM <= DD_g_taskNum){
					pTask->state = DD_TASKSTATE_FAILED;
				}
				DD_ProcTask(pTask);
				if(DD_TASKSTATE_WAIT != pTask->state){
					DD_DeleteTask(pTask);
				}else{
					DD_AddNewTask(pTask);
				}
			}
		}
		for(i=0;i<DD_TASK_MAXNUM;i++){
			pTask = *(DD_g_pTask + i);
			if(0==pTask) continue;
			if(1==FD_ISSET(pTask->sockFd,&fds)){
				DD_ProcTask(pTask);
				if(DD_TASKSTATE_WAIT!=pTask->state){
					DD_RemoveTask(pTask);
					DD_DeleteTask(pTask);
				}
			}
		}
	}
	DD_g_StartDiscoveryThrID = 0;
	return -1;
}

/*socket */
static int DD_CreateUDPSokcet(int port)
{
	int sockFd;
	int ret;
	int flags;
	const int opt = 1;
	struct sockaddr_in ipAddr; 
	do{
		//兴建 socket
		sockFd = socket(AF_INET, SOCK_DGRAM, 0);
		if(-1==sockFd){
			DD_DEBUG("Create socket failed!");
			break;
		}
		//设置该套接字为广播类型，
		ret = setsockopt(sockFd, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
		if(-1 == ret){
			DD_DEBUG("Set socket SO_BROADCAST failed!");
			break;
		}
		// 绑定地址
		memset(&ipAddr,0,sizeof(struct sockaddr_in));
		ipAddr.sin_family = AF_INET;
		ipAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		ipAddr.sin_port = htons(port);
		ret = bind(sockFd,(struct sockaddr *)&(ipAddr), sizeof(struct sockaddr_in));
		if(-1 == ret)
		{
			DD_DEBUG("Bind rsockfd failed!");
			break;
		}
		//socket 非阻塞
		flags = fcntl(sockFd,F_GETFL,0);
		ret = fcntl(sockFd,F_SETFL,flags|O_NONBLOCK);
		if(-1 == ret)
		{
			DD_DEBUG("Set socket unblock failed!");
			break;
		}
		return sockFd;
	}while(0);

	DD_CLOSE_SOCKET(sockFd);
	return -1;
}

/*外部调用接口*/
int discovery(void)   
{
	if(0!=DD_g_StartDiscoveryThrID) return 0;

	int ret;

	do{
		ret = pipe(DD_g_pPipes);
		if(0!=ret) break;
		ret= pthread_create(&DD_g_StartDiscoveryThrID,0,(void*(*)(void*))DD_StartDiscoveryThread,0);
		if(0!=ret) break;

		ret = DD_StartServer();
		if(0!=ret) break;
		return 0;
	}while(0);
	DD_CLOSE_SOCKET(DD_g_pPipes[0]);
	DD_CLOSE_SOCKET(DD_g_pPipes[1]);
	DD_g_StartDiscoveryThrID = 0;
	return -1;
}


int DD_StartServer()
{
	task_t *pTask;
	int len;

	do{
		/*新建任务*/
		pTask = DD_CreateTask(DD_TASKTYPE_SERVER,DD_ProcServer,0,0);
		if(0==pTask) break;
		/*写入任务*/
		len = write(DD_g_pPipes[1],&pTask,sizeof(task_t*));
		if(sizeof(task_t*)!=len){
			break;
		}
		return 0;
	}while(0);
	DD_DeleteTask(pTask);
	return -1;
}

int DD_SearchDev()
{
	task_t *pTask;
	int     len;

	if(0==DD_g_StartDiscoveryThrID) return -1;

	DSC_SEARCH_INFO              searchInfo;

	do{
		/*新建任务*/
		pTask = DD_CreateTask(DD_TASKTYPE_SEARCH,DD_ProcSearchDev,&searchInfo,sizeof(DSC_SEARCH_INFO));
		if(0==pTask) break;
		/*写入任务*/
		len = write(DD_g_pPipes[1],&pTask,sizeof(task_t*));
		if(sizeof(task_t*)!=len){
			break;
		}
		return 0;
	}while(0);
	DD_DeleteTask(pTask);
	return -1;
}

/*改变远程设备网络参数*/
int DD_ModifyDevNetConfig(DSC_MODIFY_INFO *pModify)
{
	task_t *pTask = 0;
	int len;

	if(0==pModify) return -1;

	do{
		/*新建任务*/
		pTask = DD_CreateTask(DD_TASKTYPE_MODIFY,DD_ProcModifyDevNetConfig,pModify,sizeof(DSC_MODIFY_INFO));
		if(0==pTask) break;
		/*写入任务*/
		len = write(DD_g_pPipes[1],&pTask,sizeof(task_t*));
		if(sizeof(task_t*)!=len){
			break;
		}
		return 0;
	}while(0);
	DD_DeleteTask(pTask);
	return -1;
}

/********************************************************************************************/
/*******************************       静态函数实现     ***************************************/
/********************************************************************************************/
static task_t* DD_CreateTask(DD_TASKTYPE_TYPE type, DD_TASK_PROC procFun, void *pData, unsigned int dataLen)
{
	task_t* pTask;

	do{
		pTask = (task_t*)malloc(sizeof(task_t));
		if(0==pTask) break;
		memset(pTask,0,sizeof(task_t));

		pTask->sockFd = -1;
		pTask->type = type;
		pTask->state = DD_TASKSTATE_UNDO;
		pTask->procFun = procFun;
		pTask->dataLen = dataLen;
		pTask->time = time(0);

		if(0!=pTask->dataLen){
			pTask->pData = malloc(pTask->dataLen);
			memcpy(pTask->pData,pData,pTask->dataLen);
		}else{
			pTask->pData = pData;
		}

		return pTask;
	}while(0);
	return 0;
}

static void DD_DeleteTask(task_t* pTask)
{
	if(0==pTask) return;

	if(0!=pTask->dataLen){
		DD_FREE(pTask->pData);
	}
	DD_CLOSE_SOCKET(pTask->sockFd);
	DD_FREE(pTask);
}

static int DD_AddNewTask(task_t* pTask)
{
	int i;
	if(0==pTask) return -1;
	if(DD_TASK_MAXNUM <= DD_g_taskNum) return -1;

	for(i=0;i<DD_TASK_MAXNUM;i++){
		if(0==DD_g_pTask[i]){
			DD_g_pTask[i] = pTask;
			DD_g_taskNum ++;
			break;
		}
	}
	DD_DEBUG("add new task success!");
	return ((i>=DD_TASK_MAXNUM)?(-1):0);
}

static int DD_RemoveTask(task_t *pTask)
{
	int i;
	if(0==pTask) return -1;
	if(0>=DD_g_taskNum) return -1;

	for(i=0;i<DD_TASK_MAXNUM;i++){
		if(pTask == DD_g_pTask[i]){
			DD_g_pTask[i] = 0;
			DD_g_taskNum --;
			break;
		}
	}
	return ((i>=DD_TASK_MAXNUM)?(-1):0);
}

static int DD_ProcTask(task_t *pTask)
{
	if(0==pTask) return -1;
	if(0!=pTask->procFun){
		return (*pTask->procFun)(pTask);
	}
	return -1;
}

static int DD_ProcSearchDev(struct task_t *pTask)
{
	DSC_SEARCH_INFO             *pSearch;
	DSC_SEARCH_RESPONSE_INFO    *pSearchResponse;
	unsigned char                pPasswork[DSC_PASSWORD_MAXLEN];

	char  inBuff[DSC_MESSAGE_MAXLEN];
	int   inLen;

	struct sockaddr_in           ipAddr;
	int   sockaddrLen;
	int   len;

	if(0==pTask) return -1;

	do{
		if(DD_TASKSTATE_UNDO == pTask->state){
			/*ipAddr*/
			memset(&ipAddr,0,sizeof(struct sockaddr_in));
			ipAddr.sin_family = AF_INET;
			ipAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
			ipAddr.sin_port = htons(DSC_BROADCAST_PORT);
			/*content*/
			pSearch = (DSC_SEARCH_INFO*)pTask->pData;
			if(0==pSearch) break;
			memset(pSearch,0,sizeof(DSC_SEARCH_INFO));
			snprintf((char*)pSearch->ucCheckCode,DSC_CHECKCODE_LEN,DSC_CHECKCODE);
			pSearch->ucMesType = DSC_MT_SEARCH;
			snprintf((char*)pSearch->srcInfo.ucIpAddr,DSC_IPV4ADDR_LEN,"%s",(char*)DD_g_NetworkInfo.ucIpAddr);
			snprintf((char*)pSearch->srcInfo.ucMacAddr,DSC_MACADDR_LEN,"%s",(char*)DD_g_NetworkInfo.ucMacAddr);
			/*create udp sock*/
			pTask->sockFd = DD_CreateUDPSokcet(0);
			if(0>pTask->sockFd) break;
			/*send*/
			sockaddrLen = sizeof(struct sockaddr_in);
			len = sendto(pTask->sockFd,pSearch,sizeof(DSC_SEARCH_INFO),0,(struct sockaddr*)&ipAddr,sockaddrLen);
			if(sizeof(DSC_SEARCH_INFO)!=len){ break; }
			pTask->state = DD_TASKSTATE_WAIT;
		}else if(DD_TASKSTATE_WAIT == pTask->state){
			pSearchResponse = (DSC_SEARCH_RESPONSE_INFO*)inBuff;
			while(1){
				sockaddrLen = sizeof(struct sockaddr_in);
				inLen = recvfrom(pTask->sockFd, inBuff, DSC_MESSAGE_MAXLEN, 0, (struct sockaddr*)&ipAddr,(socklen_t*)&sockaddrLen);
				if(inLen< (int)sizeof(DSC_SEARCH_RESPONSE_INFO)){ break; }
				if(DSC_MT_SEARCH_RESP == inBuff[DSC_MESTYPE_OFFSET]){
					PP_DES_Decode((char*)pPasswork,(char*)pSearchResponse->userInfo.ucPassword,DSC_ENCODE_KEY,DSC_PASSWORD_MAXLEN);
					memcpy((char*)pSearchResponse->userInfo.ucPassword,(char*)pPasswork,DSC_PASSWORD_MAXLEN);
					DD_FoundNewDev(pSearchResponse);
				}
			}
		}
		return 0;
	}while(0);
	pTask->state = DD_TASKSTATE_FAILED;
	return -1;
}


static int DD_ProcServer(struct task_t *pTask)
{
	int   ret;
	if(0==pTask) return -1;

	do{
		if(DD_TASKSTATE_UNDO == pTask->state){
			pTask->sockFd = DD_CreateUDPSokcet(DSC_BROADCAST_PORT);
			if(0>pTask->sockFd) break;
			pTask->state = DD_TASKSTATE_WAIT;
		}else if(DD_TASKSTATE_WAIT == pTask->state){
			while(1){
				ret = DD_HannelServerEvent(pTask);
				if(0!=ret) break;
			}
		}else if(DD_TASKSTATE_TIMEOUT == pTask->state){
			pTask->state = DD_TASKSTATE_WAIT;
			pTask->time = time(0);
		}
		return 0;
	}while(0);
	pTask->state = DD_TASKSTATE_FAILED;
	return -1;
}

static int DD_ProcModifyLocalNetConfig(struct task_t *pTask)
{
	DSC_MODIFY_INFO *pModify;
	if(0==pTask) return -1;

	pModify = pTask->pData;
	do{
		if(DD_TASKSTATE_UNDO == pTask->state){
			DD_ModifyLocalNetConfig(pModify);
			pTask->state = DD_TASKSTATE_COMPLETE;
		}
		return 0;
	}while(0);
	return -1;
}

static int DD_ProcModifyDevNetConfig(struct task_t *pTask)
{
	DSC_MODIFY_INFO            *pModify;
	DSC_MODIFY_RESPONSE_INFO   *pModifyResp;
	unsigned char               pPassword[DSC_PASSWORD_MAXLEN];
	unsigned char               pInBuff[DSC_MESSAGE_MAXLEN];
	int                         inLen;

	struct sockaddr_in          ipAddr;
	int                         sockaddrLen;
	int                         len;


	if(0==pTask) return -1;

	DD_DEBUG("DD_ProcModifyDevNetConfig task state = %d",pTask->state);

	do{
		if(DD_TASKSTATE_UNDO==pTask->state){
			pModify = (DSC_MODIFY_INFO*)pTask->pData;
			/*初始化修改包*/
			snprintf((char*)pModify->ucCheckCode,DSC_CHECKCODE_LEN,DSC_CHECKCODE);
			pModify->ucMesType = DSC_MT_MODIFY;
			PP_DES_Encode((char*)pPassword,(char*)pModify->securityInfo.ucPassword,DSC_ENCODE_KEY,DSC_PASSWORD_MAXLEN);
			memcpy((char*)pModify->securityInfo.ucPassword,(char*)pPassword,DSC_PASSWORD_MAXLEN);
			snprintf((char*)pModify->srcInfo.ucIpAddr,DSC_IPV4ADDR_LEN,"%s",(char*)DD_g_NetworkInfo.ucIpAddr);
			snprintf((char*)pModify->srcInfo.ucMacAddr,DSC_MACADDR_LEN,"%s",(char*)DD_g_NetworkInfo.ucMacAddr);
			/*初始化发送地址*/
			ipAddr.sin_family = AF_INET;
			ipAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
			ipAddr.sin_port = htons(DSC_BROADCAST_PORT);
			/*兴建 socket*/
			pTask->sockFd = DD_CreateUDPSokcet(0);
			if(0>pTask->sockFd) break;
			/*发送修改包*/
			sockaddrLen = sizeof(struct sockaddr_in);
			len = sendto(pTask->sockFd,pModify,sizeof(DSC_MODIFY_INFO),0,(struct sockaddr*)&ipAddr,sockaddrLen);
			if(sizeof(DSC_MODIFY_INFO)!=len)break;
			pTask->state = DD_TASKSTATE_WAIT;
		}else if(DD_TASKSTATE_WAIT==pTask->state){
			pModify = (DSC_MODIFY_INFO*)pTask->pData;
			pModifyResp = (DSC_MODIFY_RESPONSE_INFO*)pInBuff;

			DD_DEBUG("pModifyResp...........")
				inLen = recvfrom(pTask->sockFd, pInBuff, DSC_MESSAGE_MAXLEN, 0, (struct sockaddr*)&ipAddr,(socklen_t*)&sockaddrLen);
			if((inLen>= (int)sizeof(DSC_MODIFY_RESPONSE_INFO)) && (DSC_MT_MODIFY_RESP==pInBuff[DSC_MESTYPE_OFFSET]) ) {
				DD_DEBUG("response state-%d len-%d",pInBuff[DSC_MESTYPE_OFFSET],inLen);
				if((0==strncmp((char*)pModifyResp->dstInfo.ucIpAddr,(char*)DD_g_NetworkInfo.ucIpAddr,DSC_IPV4ADDR_LEN)) &&
						(0==strncmp((char*)pModifyResp->dstInfo.ucMacAddr,(char*)DD_g_NetworkInfo.ucMacAddr,DSC_MACADDR_LEN))){
					DD_ModifyDevNetConfigResponse(DD_TASKSTATE_COMPLETE,pModifyResp);
					pTask->state = DD_TASKSTATE_COMPLETE;
				}
			}
		}else if(DD_TASKSTATE_TIMEOUT==pTask->state){
			DD_ModifyDevNetConfigResponse(DD_TASKSTATE_TIMEOUT,0);
		}
		return 0;
	}while(0);
	pTask->state = DD_TASKSTATE_FAILED;
	return -1;
}

static int DD_HannelServerEvent(struct task_t *pTask)
{
	DSC_SEARCH_INFO           *pSearch;
	DSC_SEARCH_RESPONSE_INFO  *pSearchResp;
	DSC_MODIFY_INFO           *pModify;
	DSC_MODIFY_RESPONSE_INFO  *pModifyResp;

	unsigned char              pPassword[DSC_PASSWORD_MAXLEN];
	struct sockaddr_in         ipAddr;
	int                        sockaddrLen;

	char                       pInBuff[DSC_MESSAGE_MAXLEN];
	int                        inLen;
	char                       pOutBuff[DSC_MESSAGE_MAXLEN];
	int                        outLen;
	int                        ret;

	do{
		sockaddrLen = sizeof(struct sockaddr_in);
		inLen = recvfrom(pTask->sockFd, pInBuff, DSC_MESSAGE_MAXLEN, 0, (struct sockaddr*)&ipAddr,(socklen_t*)&sockaddrLen);
		if(inLen< (int)(DSC_CHECKCODE_LEN + sizeof(unsigned char))) break;
		/*服务搜索包*/
		if(DSC_MT_SEARCH == pInBuff[DSC_MESTYPE_OFFSET]){
			pSearch = (DSC_SEARCH_INFO*)pInBuff;
			pSearchResp = (DSC_SEARCH_RESPONSE_INFO*)pOutBuff;
			/*丢弃本机发出的搜索包*/


			if((0!=strncmp((char*)pSearch->srcInfo.ucIpAddr,(char*)DD_g_NetworkInfo.ucIpAddr,DSC_IPV4ADDR_LEN)) &&
					(0!=strncmp((char*)pSearch->srcInfo.ucMacAddr,(char*)DD_g_NetworkInfo.ucMacAddr,DSC_MACADDR_LEN))){
				/*初始化返回包*/
				memset(pSearchResp,0,sizeof(DSC_SEARCH_RESPONSE_INFO));
				snprintf((char*)pSearchResp->ucCheckCode,DSC_CHECKCODE_LEN,"%s",DSC_CHECKCODE);
				pSearchResp->ucMesType = DSC_MT_SEARCH_RESP;
				/*网络*/
				memcpy((char*)&pSearchResp->networkInfo,&DD_g_NetworkInfo,sizeof(DSC_NETWORK_INFO));
				/*设备*/
				DD_InitDevInfo(&pSearchResp->devInfo);
				/*平台*/
				DD_InitPlatformInof(&pSearchResp->playformInfo);
				/*用户*/
				ret = DD_InitUserInfo(&pSearchResp->userInfo);
				if(0!=ret) break;
				PP_DES_Encode((char*)pPassword,(char*)pSearchResp->userInfo.ucPassword,DSC_ENCODE_KEY,DSC_PASSWORD_MAXLEN);
				memcpy((char*)pSearchResp->userInfo.ucPassword,(char*)pPassword,DSC_PASSWORD_MAXLEN);
				/*返回搜索包*/
				sockaddrLen = sizeof(struct sockaddr_in);
				ipAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
				outLen = sendto(pTask->sockFd,pOutBuff,sizeof(DSC_SEARCH_RESPONSE_INFO),0,(struct sockaddr*)&ipAddr,sockaddrLen);
				if(sizeof(DSC_SEARCH_RESPONSE_INFO) != outLen) break;
			}
			/*服务修改包*/
		}else if(DSC_MT_MODIFY == pInBuff[DSC_MESTYPE_OFFSET]){
			pModify = (DSC_MODIFY_INFO*)pInBuff;
			pModifyResp =  (DSC_MODIFY_RESPONSE_INFO*)pOutBuff;
			ret = DD_HannelServerModify(pModify,pModifyResp);
			if(0==ret){
				/*返回修改包*/
				sockaddrLen = sizeof(struct sockaddr_in);
				ipAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
				outLen = sendto(pTask->sockFd,pOutBuff,sizeof(DSC_MODIFY_RESPONSE_INFO),0,(struct sockaddr*)&ipAddr,sockaddrLen);
				if(sizeof(DSC_MODIFY_RESPONSE_INFO) != outLen) break;
			}
		}
		return 0;
	}while(0);
	return -1;
}


static int DD_HannelServerModify(DSC_MODIFY_INFO *pModify, DSC_MODIFY_RESPONSE_INFO *pModifyResp)
{
	DSC_USER_INFO       userInfo;
	unsigned char       pPassword[DSC_PASSWORD_MAXLEN];
	int                 ret;
	int                 len;

	if(0==pModify) return -1;
	if(0==pModifyResp) return -1;

	DD_DEBUG("ipadd is %s\n",pModify->networkInfo.ucIpAddr);
	DD_DEBUG("netmask is %s\n",pModify->networkInfo.ucNetmaskAddr);
	DD_DEBUG("gateway is %s\n",pModify->networkInfo.ucGatewayAddr);
	DD_DEBUG("dns is %s\n",pModify->networkInfo.ucDnsAddr1);
	DD_DEBUG("mac is %s\n",pModify->networkInfo.ucMacAddr);
	DD_DEBUG("ucDhcpEnable is %d\n",pModify->networkInfo.ucDhcpEnable);

	do{
		/*过滤修改包*/
		if( (0!=strncmp((char*)pModify->dstInfo.ucIpAddr,(char*)DD_g_NetworkInfo.ucIpAddr,DSC_IPV4ADDR_LEN)) ||
				(0!=strncmp((char*)pModify->dstInfo.ucMacAddr,(char*)DD_g_NetworkInfo.ucMacAddr,DSC_MACADDR_LEN))) break;




		/*初始化修改返回包*/
		memset(pModifyResp,0,sizeof(DSC_MODIFY_RESPONSE_INFO));
		snprintf((char*)pModifyResp->ucCheckCode,DSC_CHECKCODE_LEN,DSC_CHECKCODE);
		pModifyResp->ucErrorCode = DSC_ERRORCODE_SUCCESS;
		pModifyResp->ucMesType = DSC_MT_MODIFY_RESP;

		memcpy((char*)&pModifyResp->dstInfo,(char*)&pModify->srcInfo,sizeof(DSC_TERMINAL_INFO));

		/*校验*/
		DD_InitUserInfo(&userInfo);
		PP_DES_Encode(pPassword,userInfo.ucPassword,DSC_ENCODE_KEY,DSC_PASSWORD_MAXLEN);
		if((0!=strncmp(pModify->securityInfo.ucUsername,userInfo.ucUsername,DSC_USERNAME_MAXLEN)) ||
				(0!=strncmp(pModify->securityInfo.ucPassword,pPassword,DSC_PASSWORD_MAXLEN))){
			pModifyResp->ucErrorCode = DSC_ERRORCODE_USERPASSWD_ERROR;
		}
		ret = DD_CheckNetworkIsValid(&pModify->networkInfo);
		if(0!=ret){
			pModifyResp->ucErrorCode = DSC_ERRORCODE_ARGE_ERROR;
			DD_InitDevInfo(&pModifyResp->devInfo);
		}else{
			memcpy((char*)&pModifyResp->devInfo,(char*)&pModify->devInfo,sizeof(DSC_DEV_INFO));
		}
		ret = DD_CheckDevInfoIsValid(&pModify->devInfo);
		if(0!=ret){
			pModifyResp->ucErrorCode = DSC_ERRORCODE_ARGE_ERROR;
			memcpy((char*)&pModifyResp->networkInfo,(char*)&DD_g_NetworkInfo,sizeof(DSC_DEV_INFO));
		}else{
			memcpy((char*)&pModifyResp->networkInfo,(char*)&pModify->networkInfo,sizeof(DSC_DEV_INFO));
		}
		if(DSC_ERRORCODE_SUCCESS == pModifyResp->ucErrorCode){
			task_t *pTask = 0;
			/*兴建任务*/
			pTask = DD_CreateTask(DD_TASKTYPE_DOMODIFY,DD_ProcModifyLocalNetConfig,pModify,sizeof(DSC_MODIFY_INFO));
			if(0==pTask) break;
			/*写入任务*/
			len = write(DD_g_pPipes[1],&pTask,sizeof(task_t*));
			if(sizeof(task_t*)!=len){
				break;
			}
		}
		return 0;
	}while(0);
	return -1;
}

#define MAX_PRIVATE_DEV  256
/*******************************************************************************************************************
 ************************************  不同平台只需要修改此分割线以下代码. ***********************************************
 ********************************************************************************************************************/
/*发现新设备*/
int DD_FoundNewDev(DSC_SEARCH_RESPONSE_INFO *pResponse)
{  

	//fprintf(stderr,"come here\r\n");
	//DD_DEBUG(" ip=%s, name=%s, passwd=%sr\n",pResponse->networkInfo.ucIpAddr,pResponse->userInfo.ucUsername,pResponse->userInfo.ucPassword);  
	if(0==strcmp(pResponse->playformInfo.ucDevType,"IPCAMERA"))
	{
             if(privatedevNum<MAX_PRIVATE_DEV)
             	{
		snprintf(privateDev[privatedevNum].ip_addr, sizeof(privateDev[privatedevNum].ip_addr), pResponse->networkInfo.ucIpAddr);
		snprintf(privateDev[privatedevNum].macaddr,sizeof(privateDev[privatedevNum].macaddr), pResponse->networkInfo.ucMacAddr);
		snprintf(privateDev[privatedevNum].user.user_name, sizeof(privateDev[privatedevNum].user.user_name),pResponse->userInfo.ucUsername);
		snprintf(privateDev[privatedevNum].user.user_pwd, sizeof(privateDev[privatedevNum].user.user_pwd) ,pResponse->userInfo.ucPassword);
		privateDev[privatedevNum].port =  ntohl(pResponse->devInfo.ulPrivatePort);
		privateDev[privatedevNum].device_type= 0;  // ipc
		privateDev[privatedevNum].manufacturer = MANUFACT_PRIVATE;
		privatedevNum++;
		//DD_DEBUG("mac=%s,  ip=%s, name=%s, passwd=%s,num =%d .port=%d\r\n",
		//		pResponse->networkInfo.ucMacAddr,
		//		pResponse->networkInfo.ucIpAddr,
			//	pResponse->userInfo.ucUsername,
			//	pResponse->userInfo.ucPassword,
			//	privatedevNum,ntohl(pResponse->devInfo.ulPrivatePort));  
             	}
	}

	return 0;
}

/*改变远程设备网络参数回复*/
int DD_ModifyDevNetConfigResponse(unsigned char state, DSC_MODIFY_RESPONSE_INFO *pModifyResponse)
{
     NetConfigResponse pResponse;
	if(0!=pModifyResponse)
	{
		DD_DEBUG("task state is %d-%p-%d",state,pModifyResponse,pModifyResponse->ucErrorCode);
		
              snprintf(pResponse.ucIpAddr,sizeof(pResponse.ucIpAddr),pModifyResponse->networkInfo.ucIpAddr);
					pResponse.ucErrorCode =  pModifyResponse->ucErrorCode ;
					pResponse.state = state;
                    SetNetModifyResponse(&pResponse);
									
	}
	         

	


	return 0;
}


/*改变网络参数*/
int DD_ModifyLocalNetConfig(DSC_MODIFY_INFO *pInfo)
{
	sdk_eth_cfg_t p_eth_cfg;
	int ret = 0;

	ret=databaseOperate(SDK_MAIN_MSG_NET_LINK_CFG,SDK_PARAM_GET,NULL,0,&p_eth_cfg,sizeof(p_eth_cfg));
	if(ret<0)
	{
		Printf("select SDK_MAIN_MSG_NET_LINK_CFG fail \n");
	}
	
	if( pInfo->networkInfo.ulMemberIsValid &  DSC_NETWORK_DHCP_VALID)
	{
		if(1==pInfo->networkInfo.ucDhcpEnable)
		{
			return dhcpNetFun();
		}
	}
	
	if( pInfo->networkInfo.ulMemberIsValid &  DSC_NETWORK_IP_VALID)
	{
          ret = netSetIp("eth0",inet_addr(pInfo->networkInfo.ucIpAddr));
	}

	if( pInfo->networkInfo.ulMemberIsValid &  DSC_NETWORK_NETMASK_VALID)
	{
	  if(netCheckNetMask(pInfo->networkInfo.ucNetmaskAddr)==0)
	  	{
                 netSetMask("eth0",inet_addr(pInfo->networkInfo.ucNetmaskAddr));
	  	}
	}
	if( pInfo->networkInfo.ulMemberIsValid &  DSC_NETWORK_GATEWAY_VALID)
	{
          netSetGw("eth0",inet_addr(pInfo->networkInfo.ucGatewayAddr));

	}
	if( pInfo->networkInfo.ulMemberIsValid &  DSC_NETWORK_MAC_VALID)
	{

	}
	if( pInfo->networkInfo.ulMemberIsValid &  DSC_NETWORK_DNS1_VALID)
	{
           netSetDns( pInfo->networkInfo.ucDnsAddr1,0);
	}

	if( pInfo->networkInfo.ulMemberIsValid &  DSC_NETWORK_DNS2_VALID)
	{
           netSetDns( 0,pInfo->networkInfo.ucDnsAddr2);
	}
	strcpy((char *)p_eth_cfg.ip_info.ip_addr, pInfo->networkInfo.ucIpAddr);
	strcpy((char *)p_eth_cfg.ip_info.mask, pInfo->networkInfo.ucNetmaskAddr);
	strcpy((char *)p_eth_cfg.ip_info.gateway, pInfo->networkInfo.ucGatewayAddr);
	strcpy((char *)p_eth_cfg.ip_info.dns1, pInfo->networkInfo.ucDnsAddr1);
	strcpy((char *)p_eth_cfg.ip_info.dns2, pInfo->networkInfo.ucDnsAddr2);
	p_eth_cfg.ip_info.enable_dhcp=pInfo->networkInfo.ucDhcpEnable;
	ret=databaseOperate(SDK_MAIN_MSG_NET_LINK_CFG,SDK_PARAM_SET,NULL,0,&p_eth_cfg,sizeof(p_eth_cfg));
	if(ret<0)
	{
		Printf("select SDK_MAIN_MSG_NET_LINK_CFG fail \n");
	}
	
	DD_DEBUG("DD_ModifyLocalNetConfig----%p",pInfo);
	return 0;
}

int DD_CheckNetworkIsValid(DSC_NETWORK_INFO *pInfo)
{
	DD_DEBUG("DD_CheckNetworkIsValid %p",pInfo);
	return 0;
}

int DD_CheckDevInfoIsValid(DSC_DEV_INFO *pInfo)
{
	DD_DEBUG("DD_CheckDevInfoIsValid %p",pInfo);
	return 0;
}

int DD_InitUserInfo(DSC_USER_INFO *pInfo)
{
	int  ret =0;
	int i =0;
	sdk_user_right_t userInfo[18];
	memset(userInfo,0,sizeof(userInfo));
	ret=databaseOperate(SDK_MAIN_MSG_USER,SDK_USER_QUERY,NULL,0,&userInfo,sizeof(userInfo));
	if(ret<0)
	{
		Printf("select SDK_MAIN_MSG_USER fail \n");
	}
	if(0==pInfo) return -1;
	memset(pInfo,0,sizeof(DSC_USER_INFO));

	for(i=0;i<18;i++)
	{
		if(0==strcmp(userInfo[i].user.user_name,"admin"))
		{
			snprintf((char*)pInfo->ucUsername,DSC_USERNAME_MAXLEN,"admin");
			snprintf((char*)pInfo->ucPassword,DSC_USERNAME_MAXLEN,userInfo[i].user.user_pwd);
			//Printf("pInfo->ucUsername =%s   pInfo->ucPassword =%s\n",pInfo->ucUsername,pInfo->ucPassword);
			return 0;
		}
	}
	
	//snprintf((char*)pInfo->ucUsername,DSC_USERNAME_MAXLEN,"admin");
	//snprintf((char*)pInfo->ucPassword,DSC_USERNAME_MAXLEN,"12345");
	return 0;
}

int DD_InitNetworkInfo(DSC_NETWORK_INFO *pInfo)
{
	sdk_eth_cfg_t EthInfo;
	int ret;

	if(0==pInfo) return -1;
	memset(pInfo,0,sizeof(DSC_NETWORK_INFO));

	ret = databaseOperate(SDK_MAIN_MSG_NET_LINK_CFG,SDK_PARAM_GET,NULL,0,&EthInfo,sizeof(EthInfo));
	if(0<=ret)
	{
		pInfo->ucDhcpEnable = EthInfo.ip_info.enable_dhcp;
	}
	snprintf((char*)pInfo->ucIpAddr,DSC_IPV4ADDR_LEN,(char*)EthInfo.ip_info.ip_addr);
	snprintf((char*)pInfo->ucGatewayAddr,DSC_IPV4ADDR_LEN,(char*)EthInfo.ip_info.gateway);
	snprintf((char*)pInfo->ucNetmaskAddr,DSC_IPV4ADDR_LEN,(char*)EthInfo.ip_info.mask);
	snprintf((char*)pInfo->ucDnsAddr1,DSC_IPV4ADDR_LEN,(char*)EthInfo.ip_info.dns1);
	netGetMac("eth0",pInfo->ucMacAddr);

	pInfo->ulMemberIsValid = DSC_NETWORK_DHCP_VALID | DSC_NETWORK_IP_VALID | DSC_NETWORK_NETMASK_VALID |
		DSC_NETWORK_GATEWAY_VALID | DSC_NETWORK_MAC_VALID;
	return 0;


#if 0
	char  gateway[MAX_IP_ADDR_LEN];
	char  netmask[MAX_IP_ADDR_LEN];
	char  macaddr[MAX_IP_ADDR_LEN];
	int ret  =0;
	struct in_addr  addr1;

	ret=netGetIp("eth0",&addr1.s_addr);
	if(-1==ret)
	{
		fprintf(stderr,"can't get ipaddr !!!\r\n");
		return -1;
	}

	ret = netGetGw(gateway);
	if(-1==ret)
	{
		fprintf(stderr,"can't get gateway !!!\r\n");
		return -2;
	}
	GetNetMask("eth0",netmask);
	netGetMac("eth0",macaddr);

	//fprintf(stderr,"%s:%d:ip=%s,gateway=%s,mask=%s,mac=%s\r\n",__FILE__,__LINE__,inet_ntoa(addr1),gateway,netmask,macaddr);

	strncpy(pInfo->ucIpAddr,inet_ntoa(addr1),MAX_IP_ADDR_LEN);
	strncpy(pInfo->ucGatewayAddr,gateway,MAX_IP_ADDR_LEN);
	strncpy(pInfo->ucNetmaskAddr,netmask,MAX_IP_ADDR_LEN);
	strncpy(pInfo->ucMacAddr,macaddr,MAX_IP_ADDR_LEN);
	pInfo->ulMemberIsValid = DSC_NETWORK_IP_VALID |DSC_NETWORK_NETMASK_VALID|DSC_NETWORK_GATEWAY_VALID |DSC_NETWORK_MAC_VALID;
#endif
	return 0;
}

int DD_InitDevInfo(DSC_DEV_INFO *pInfo)
{
	sdk_comm_cfg_t	commInfo;
	int ret;
	int  httpPort , rtspPort,playPort;

	if(0==pInfo) return -1;
	memset(pInfo,0,sizeof(DSC_DEV_INFO));

	pInfo->ulMemberIsValid |= DSC_DEV_NAME_VALID;
	//snprintf((char*)pInfo->ucDevName,DSC_DEVNAME_MAXLEN,"%s","newdev newdev");

	//ret = GetPort(&pInfo->ulRtspPort ,&pInfo->ulHttpPort, &pInfo->ulPlaybackPort);
	ret = GetPort(&rtspPort ,&httpPort, &playPort);
	if(0==ret){
		pInfo->ulMemberIsValid |= DSC_DEV_RTSPPORT_VALID;
		pInfo->ulMemberIsValid |= DSC_DEV_HTTPPORT_VALID;
		pInfo->ulMemberIsValid |= DSC_DEV_PLAYBACKPORT_VALID;
		pInfo->ulRtspPort = htonl(rtspPort);
		pInfo->ulHttpPort= htonl(httpPort);
		pInfo->ulPlaybackPort = htonl(playPort);
	}

	ret=databaseOperate(SDK_MAIN_MSG_COMM_CFG,SDK_PARAM_GET,NULL,0,&commInfo,sizeof(commInfo));
	if(0>ret)
	{
		//fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_COMM_CFG write  fail %s:%d\n",__FILE__,__LINE__);
		snprintf((char*)pInfo->ucDevName,DSC_DEVNAME_MAXLEN,"HS_NVR");
	}
	else
	{
		snprintf((char*)pInfo->ucDevName,DSC_DEVNAME_MAXLEN,commInfo.dvr_name);
	}
	pInfo->ulMemberIsValid |= DSC_DEV_NAME_VALID;
	return 0;
}

int DD_InitPlatformInof(DSC_PLATFORM_INFO *pInfo)
{
	if(0==pInfo) return -1;
	memset(pInfo,0,sizeof(DSC_PLATFORM_INFO));

	pInfo->ucPlatformType = 700;
	pInfo->ulMemberIsValid = DSC_PLATFORM_DEV_TYPE_VALID | DSC_PLATFORM_TYPE_VALID;
	snprintf((char*)pInfo->ucDevType,DSC_DEVTYPE_MAXLEN,"%s",DSC_DEVTYPE_NVR);


	pInfo->ucChNum = configGetDisplayNum();
	pInfo->ulMemberIsValid |= DSC_PLATFORM_CHANNEL_VALID;
#ifdef HI3535
	snprintf((char*)pInfo->ucFirmwareVersion,DSC_FIREWARE_VERSION_MAXLEN,"NVR_HI3535_%d_%s",pInfo->ucChNum,NVR_VERSION);
#else
	snprintf((char*)pInfo->ucFirmwareVersion,DSC_FIREWARE_VERSION_MAXLEN,"NVR_HI3520D_%d_%s",pInfo->ucChNum,NVR_VERSION);
#endif

	pInfo->ulMemberIsValid |= DSC_PLATFORM_CHANNEL_VALID;
	pInfo->ulMemberIsValid |= DSC_PLATFORM_SOFTWARE_VERSION_VALID;

	return 0;
}






