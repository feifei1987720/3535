#include "pteClient.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "LongseDes.h"

#define PC_DEBUG_FLAG 0         /*?‰“???è?“???*/
/********************************************************************************************/
/*******************************           ???         ***************************************/
/********************************************************************************************/
#if PC_DEBUG_FLAG
#define PC_DEBUG(fmt,args...) printf("DEBUG %s-%d: "fmt"\n",__FUNCTION__,__LINE__,## args);
#else
#define PC_DEBUG(fmt,args...)
#endif

#define PC_FREE(ptr) do{if(0==(ptr))break; free(ptr); ptr=0; }while(0)
/********************************************************************************************/
/*******************************       ?…¨?±????é??        ***************************************/
/********************************************************************************************/
static list_t               *PC_G_CmdClientList = 0;      /*?”¨??????????????????*/

static int                   PC_G_Time;                   /*???é??*/
static int                   PC_G_EventLoopSwitch = 0;    /*????¨?????…?*/
static pthread_t             PC_G_EventLoopThrId = 0;     /*????¨?*/
static pthread_mutex_t       PC_G_Lock;                   /*é”?*/
/********************************************************************************************/
/*******************************       é??????????°??°???     ***************************************/
/********************************************************************************************/
/*?…???? cmd ?????????*/
static pteCmdClient_t *PC_CreateCmdClient(int ip, int port, USER_INFO *pUserInfo);
/*?…????é”????cmd?????????*/
static void PC_DeleteCmdClient(pteCmdClient_t *pCmdClient);

/*?????? cmdClient*/
static pteCmdClient_t * PC_AddCmdClient(int ip, int port, USER_INFO *pUserInfo);
/*cmdClient ??????*/
static int PC_CmdLogin(pteCmdClient_t *pCmdClient);
/*??‘é???‘???¤*/
int PC_SendCmd(int sockFd, char *pData, int *pLen);
/*????”??‘???¤*/
int PC_RecvCmd(int sockFd, char *pData, int *pLen);
/*??‘é?????è??*/
static int PC_SendHeartbeat(pteCmdClient_t *pCmdClient);
/*????”????è??*/
static int PC_ReciveHeartbeat(pteCmdClient_t *pCmdClient);

/*??????????¨?*/
void PC_EventLoop();
/*??????*/
void PC_SingleStep();

/*è????? TCP socket*/
int SK_ConnectTo(unsigned int ip,int port);
/*è????? socket é??é?????*/
int SK_SetSocketUnblock(int fd);

/********************************************************************************************/
/*******************************       ?????°?????°        ***************************************/
/********************************************************************************************/
int PC_InitCtx()
{
    list_t *pCmdClientList;
    int rsl;

    if(0!=PC_G_CmdClientList) return 0;
    do{
#if IS_PLATFORM_WINDOWS
        {
            WSADATA  Ws;
            rsl = WSAStartup(MAKEWORD(2,2), &Ws);
            if(0!=rsl)break;
        }
#endif

        pCmdClientList = List_CreateNew(0);
        if(0==pCmdClientList) break;
        /*????§????????¨?é”?*/
        rsl = pthread_mutex_init(&PC_G_Lock,NULL);
        if(0!=rsl) break;
        /*?????¨??????????¨?*/
        PC_G_EventLoopSwitch = 1;
        rsl = pthread_create(&PC_G_EventLoopThrId,0,(void*(*)(void*))PC_EventLoop,0);
        if(0!=rsl) break;

        PC_G_CmdClientList = pCmdClientList;
        return 0;
    }while(0);

    PC_UnInitCtx();
    return -1;
}

void PC_UnInitCtx()
{
    LIST_FOREACH_VARIABLE;
    pteCmdClient_t *pCmdClient;

    /*??????????¨?*/
    PC_G_EventLoopSwitch = 0;
    pthread_join(PC_G_EventLoopThrId,0);
    /*销毁链表*/
    LIST_FOREACH(PC_G_CmdClientList){
        pCmdClient = (pteCmdClient_t *)LIST_FOREACH_VALUE;
        PC_DeleteCmdClient(pCmdClient);
    }
    List_Delete(PC_G_CmdClientList);
    PC_G_CmdClientList = 0;

    /*é”????????¨?é”?*/
    pthread_mutex_destroy(&PC_G_Lock);
}


void PC_EventLoop()
{
    PC_G_Time = time(0);
    while(0!=PC_G_EventLoopSwitch){
        PC_SingleStep();
    }
}


void PC_SingleStep()
{
    LIST_FOREACH_VARIABLE;
    pteCmdClient_t    *pCmdClient;

    struct timeval     slcTime;
    fd_set             readSet;
    fd_set             writeSet;
    fd_set             errSet;
    int                maxSockFd;

    int                incTime;
    int                rsl;

    /*time*/
    incTime = time(0) - PC_G_Time;
    PC_G_Time = time(0);
    if(incTime > 5) incTime = 5;

    /*select 1 ?§’é??é?”*/
    slcTime.tv_sec = 0;
    slcTime.tv_usec = 500*1000;
    FD_ZERO(&readSet);
    FD_ZERO(&writeSet);
    FD_ZERO(&errSet);

    pthread_mutex_lock(&PC_G_Lock);

    maxSockFd = -1;
    LIST_FOREACH(PC_G_CmdClientList){



        pCmdClient = (pteCmdClient_t*)LIST_FOREACH_VALUE;
        /*???????????¨??‰?”¨???cmdclientè??????????‰*/
        if(0>=pCmdClient->refCount){
            PC_DeleteCmdClient(pCmdClient);
            PC_DEBUG("remove cmd client");
            LIST_FOREACH_RM_CURNODE;
            continue;
        }

         PC_DEBUG("%d---------------------------------\n",pCmdClient->state);


        pCmdClient->timeout -= incTime;
        if(PC_CMD_INIT==pCmdClient->state){                         /*????§???? PC_CMD_INIT*/
            NET_SOCKET_CLOSE(pCmdClient->sockFd);
            pCmdClient->sockFd = SK_ConnectTo(pCmdClient->ip,pCmdClient->port);
            if(0>pCmdClient->sockFd){
                pCmdClient->state = PC_CMD_FAILED;
                pCmdClient->timeout = 5;  /*??????è??????¤±è?? 5?§’è????????é??è??*/
            }else{
                pCmdClient->state = PC_CMD_CONNECTING;
                pCmdClient->timeout = 5;  /*5?§’è????????????±????è?…???*/
                /*????????°???è?? socket set ????”¨???????????????è????????*/
                FD_SET(pCmdClient->sockFd,&writeSet);
                if(pCmdClient->sockFd > maxSockFd) maxSockFd = pCmdClient->sockFd;
            }
        }else if(PC_CMD_CONNECTING == pCmdClient->state){           /*è???????? PC_CMD_CONNECTING*/
            if(0>=pCmdClient->timeout){
                PC_DEBUG("connect failed!");
                /*è??????¤±è??, 5?§’???é??è??*/
                NET_SOCKET_CLOSE(pCmdClient->sockFd);
                pCmdClient->state = PC_CMD_FAILED;
                pCmdClient->timeout = 5;  /*??????è??????¤±è?? 5?§’è????????é??è??*/
            }else{
                PC_DEBUG("%d-------------");
                /*????????°???è?? socket set ????”¨???????????????è????????*/
                FD_SET(pCmdClient->sockFd,&writeSet);
                if(pCmdClient->sockFd > maxSockFd) maxSockFd = pCmdClient->sockFd;
            }
        }else if(PC_CMD_CONNECTED==pCmdClient->state){             /*???è????? PC_CMD_CONNECTED*/
            if(0==pCmdClient->longinStep || 2==pCmdClient->longinStep){
                /*è??è????????*/
                rsl = PC_CmdLogin(pCmdClient);
                if(0!=rsl){
                    pCmdClient->state = PC_CMD_FAILED;
                    pCmdClient->timeout = 5;  /*??????è??????¤±è?? 5?§’è????????é??è??*/
                }else{
                    /*????????°???è?? socket set ????”¨??????????????????è??*/
                    FD_SET(pCmdClient->sockFd,&readSet);
                    if(pCmdClient->sockFd > maxSockFd) maxSockFd = pCmdClient->sockFd;
                }
            }else{
                if(0>=pCmdClient->timeout){
                    pCmdClient->state = PC_CMD_FAILED;
                    pCmdClient->timeout = 5;  /*??????è??????¤±è?? 5?§’è????????é??è??*/
                }else{
                    /*????????°???è?? socket set ????”¨??????????????????è??*/
                    FD_SET(pCmdClient->sockFd,&readSet);
                    if(pCmdClient->sockFd > maxSockFd) maxSockFd = pCmdClient->sockFd;
                }
            }
        }else if(PC_CMD_LOGINED==pCmdClient->state){
            if(0>= pCmdClient->timeout){
                pCmdClient->state = PC_CMD_SEND_HEARTBEAT;
            }else{
                /*添加到可读 socket set 中用于检测是否可读*/
                FD_SET(pCmdClient->sockFd,&readSet);
                if(pCmdClient->sockFd > maxSockFd) maxSockFd = pCmdClient->sockFd;
            }
        }else if(PC_CMD_SEND_HEARTBEAT == pCmdClient->state){
            PC_DEBUG("send heart beat %s",inet_ntoa(*(struct in_addr*)&pCmdClient->ip));
            rsl = PC_SendHeartbeat(pCmdClient);
            if(0!=rsl){
                pCmdClient->state = PC_CMD_FAILED;
                pCmdClient->timeout = 5;  /*??‘é?????è???¤±è?? é??è??*/
            }else{
                pCmdClient->state = PC_CMD_RECIVE_HEARTBEAT;
                pCmdClient->timeout = 5; /*5?§’???????????‰?????‰???è??????¤?*/
            }
        }else if(PC_CMD_RECIVE_HEARTBEAT == pCmdClient->state){
            if(0>= pCmdClient->timeout){
                pCmdClient->state = PC_CMD_FAILED;
                pCmdClient->timeout = 5;  /*???è??????¤?è?…??? é??è??*/
            }else{
                /*????????°???è?? socket set ????”¨??????????????????è??*/
                FD_SET(pCmdClient->sockFd,&readSet);
                if(pCmdClient->sockFd > maxSockFd) maxSockFd = pCmdClient->sockFd;
            }
        }else if(PC_CMD_FAILED==pCmdClient->state){               /*?¤±è?? PC_CMD_FAILED*/
            if(0>=pCmdClient->timeout){
                pCmdClient->state = PC_CMD_INIT;
            }
        }

        if((PC_CMD_FAILED != pCmdClient->state) && (0<=pCmdClient->sockFd)){
            FD_SET(pCmdClient->sockFd,&errSet);
            if(pCmdClient->sockFd > maxSockFd) maxSockFd = pCmdClient->sockFd;
        }
    }

    pthread_mutex_unlock(&PC_G_Lock);

    rsl = select(maxSockFd+1,&readSet,&writeSet,&errSet,&slcTime);
     PC_DEBUG("select result is %d, maxFd is %d, cmd num is %d,",rsl,maxSockFd,List_Count(PC_G_CmdClientList));
    if(0>rsl){
        sleep(1);
        PC_DEBUG("select result is %d, maxFd is %d, cmd num is %d,",rsl,maxSockFd,List_Count(PC_G_CmdClientList));
        return;
    }else if(0==rsl){
        return;
    }

    pthread_mutex_lock(&PC_G_Lock);


    LIST_FOREACH(PC_G_CmdClientList){
        pCmdClient = (pteCmdClient_t*)LIST_FOREACH_VALUE;
        if(PC_CMD_CONNECTING==pCmdClient->state){
            if(0<FD_ISSET(pCmdClient->sockFd,&writeSet)){
                pCmdClient->state = PC_CMD_CONNECTED;
                pCmdClient->timeout = 5;    /*è?????????????????5,?§’è????‘é?????è????…*/
                pCmdClient->longinStep = 0;
            }
        }else if(PC_CMD_CONNECTED==pCmdClient->state){
            if(0<FD_ISSET(pCmdClient->sockFd,&readSet)){
                if(1==pCmdClient->longinStep || 3==pCmdClient->longinStep){
                    rsl = PC_CmdLogin(pCmdClient);
                    if(0!=rsl){
                        pCmdClient->state = PC_CMD_FAILED;
                        pCmdClient->timeout = 5;  /*??????è??????¤±è?? 5?§’è????????é??è??*/
                    }
                }
            }
        }else if(PC_CMD_RECIVE_HEARTBEAT == pCmdClient->state){
            if(0<FD_ISSET(pCmdClient->sockFd,&readSet)){
                rsl = PC_ReciveHeartbeat(pCmdClient);
                if(0>rsl){
                    pCmdClient->state = PC_CMD_FAILED;
                    pCmdClient->timeout = 5;  /*??????è??????¤±è?? 5?§’è????????é??è??*/
                }else if(0==rsl){
                    pCmdClient->state = PC_CMD_LOGINED;
                    pCmdClient->timeout = 4;   /*????”????è????…??????, 4?§’???é????‘*/
                }
            }
        }else{/*??????é?“?????????è??????‘???¤*/
            if(0<FD_ISSET(pCmdClient->sockFd,&readSet)){
                char pBuff[PC_BUFF_MAXLEN];
                int len;
                len= PC_BUFF_MAXLEN;
                rsl = PC_RecvCmd(pCmdClient->sockFd,pBuff,&len);
                if(0!=rsl){
                    pCmdClient->state = PC_CMD_FAILED;
                    pCmdClient->timeout = 5;  /*创建连接失败 5秒连接后重试*/
                }
                PC_DEBUG("hahahah.................");
            }
        }
    }
    pthread_mutex_unlock(&PC_G_Lock);
}

pteClient_t* PC_CreateNew()
{
    pteClient_t *pClient;
    do{
        pClient = (pteClient_t*)malloc(sizeof(pteClient_t));
        if(0==pClient) break;
        memset(pClient,0,sizeof(pteClient_t));

        pClient->sockFd = NET_SOCKET_INVALID;
        return pClient;
    }while(0);

    PC_Delete(pClient);
    return 0;
}

void PC_Delete(pteClient_t* pClient)
{
    if(0==pClient) return;
    if(0!=pClient->pCmdClient){
        pClient->pCmdClient->refCount --;
    }
    PC_FREE(pClient);
}


int PC_Login(pteClient_t* pClient, int ip, int port, USER_INFO *pUserInfo)
{
    int                i;

    if(0==pClient) return -1;
    if(0==pUserInfo) return -1;

    do{
        if(0!=pClient->pCmdClient){
            if((ip!=pClient->pCmdClient->ip) || (port!=pClient->pCmdClient->port) ||
               (0!=strncmp((char*)pUserInfo->ucUsername,(char*)pClient->pCmdClient->userInfo.ucUsername,32)) ||
               (0!=strncmp((char*)pUserInfo->ucPassWord,(char*)pClient->pCmdClient->userInfo.ucPassWord,32))){
                pClient->pCmdClient->refCount --;
                pClient->pCmdClient = 0;
            }
        }
        if(0==pClient->pCmdClient){
            pClient->pCmdClient = PC_AddCmdClient(ip,port,pUserInfo);
            if(0==pClient->pCmdClient) break;
        }
        for(i=0;i<3;i++){
            if(PC_CMD_LOGINED == pClient->pCmdClient->state) break;
            sleep(1);
        }
        return ((PC_CMD_LOGINED == pClient->pCmdClient->state)?(0):(-1));
    }while(0);
    return -1;
}

int PC_GetStream(pteClient_t* pClient, int channel, int streamType)
{
    char              pBuff[PC_BUFF_MAXLEN] = {0};
    ARG_CMD          *pCmd;
    STREAM_INFO      *pStreamInfo;
    int               rsl;
    int               len;
    pteCmdClient_t   *pCmdClient;


    if(0==pClient) return -1;
    pCmdClient = pClient->pCmdClient;
    if(0==pCmdClient) return -1;
    if((PC_CMD_LOGINED != pCmdClient->state) && (PC_CMD_SEND_HEARTBEAT != pCmdClient->state)
            && (PC_CMD_RECIVE_HEARTBEAT != pCmdClient->state)) return -1;


    pCmd                = (ARG_CMD *)pBuff;
    pCmd->ulFlag        = ARG_HEAD_CMD;
    pCmd->ulVersion     = ARG_SDK_VERSION_1_1;
    pCmd->usCmd         = CMD_GET_STREAM;
    pCmd->ulID          = pCmdClient->id;
    pCmd->ulBufferNum   = 1;
    pCmd->ulBufferSize  = sizeof(STREAM_INFO);

    pStreamInfo         = (STREAM_INFO*)(pBuff + ARG_HEAD_LEN);
    pStreamInfo->ucCH   = channel;
    pStreamInfo->ucVideoStreamType = streamType;

    len = ARG_HEAD_LEN + pCmd->ulBufferNum * pCmd->ulBufferSize;

    do{
        NET_SOCKET_CLOSE(pClient->sockFd);
        pClient->sockFd = SK_ConnectTo(pCmdClient->ip,pCmdClient->port);
        if(0>pClient->sockFd) break;
        /*5?§’??‰??…è?????*/
        rsl = SK_SelectWaitWriteable(pClient->sockFd,5000);
        if(0!=rsl) break;
        /*??‘é???‘???¤*/
        rsl = PC_SendCmd(pClient->sockFd,pBuff,&len);
        if(0!=rsl) break;
        /*5?§’??‰??…?‘???¤????¤?*/
        rsl = SK_SelectWaitReadable(pClient->sockFd,5000);
        if(0!=rsl) break;
        len = PC_BUFF_MAXLEN;
        rsl = PC_RecvCmd(pClient->sockFd,pBuff,&len);
        if(0!=rsl) break;

        return pCmd->ucState;
    }while(0);
    NET_SOCKET_CLOSE(pClient->sockFd);
    return -1;
}

/********************************************************************************************/
/*******************************       é??????????°?????°     ***************************************/
/********************************************************************************************/
static pteCmdClient_t * PC_AddCmdClient(int ip, int port, USER_INFO *pUserInfo)
{
    LIST_FOREACH_VARIABLE;
    pteCmdClient_t *pCmdClient;
    pteCmdClient_t *pCmdClientIndex;

    do{
        pthread_mutex_lock(&PC_G_Lock);
        pCmdClient = 0;
        LIST_FOREACH(PC_G_CmdClientList){
            pCmdClientIndex = (pteCmdClient_t *)LIST_FOREACH_VALUE;
            if((ip==pCmdClientIndex->ip) && (port==pCmdClientIndex->port) &&
               (0==strncmp((char*)pUserInfo->ucUsername,(char*)pCmdClientIndex->userInfo.ucUsername,32)) &&
               (0==strncmp((char*)pUserInfo->ucPassWord,(char*)pCmdClientIndex->userInfo.ucPassWord,32)))
            {
                pCmdClient = pCmdClientIndex;
                break;
            }
        }
        if(0==pCmdClient){
            pCmdClient = PC_CreateCmdClient(ip,port,pUserInfo);
            if(0==pCmdClient) break;
            List_PushBack(PC_G_CmdClientList,pCmdClient);
        }
        pCmdClient->refCount ++;
        pthread_mutex_unlock(&PC_G_Lock);
        return pCmdClient;
    }while(0);
    pthread_mutex_unlock(&PC_G_Lock);
    return 0;
}


static pteCmdClient_t *PC_CreateCmdClient(int ip, int port, USER_INFO *pUserInfo)
{
    pteCmdClient_t *pCmdClient;

    if(0==pUserInfo) return 0;

    do{
        pCmdClient = (pteCmdClient_t*)malloc(sizeof(pteCmdClient_t));
        if(0==pCmdClient) break;
        memset(pCmdClient,0,sizeof(pteCmdClient_t));

        pCmdClient->ip = ip;
        pCmdClient->port = port;
        memmove(&pCmdClient->userInfo,pUserInfo,sizeof(USER_INFO));

        pCmdClient->sockFd = -1;
        pCmdClient->state = PC_CMD_INIT;
        pCmdClient->refCount = 0;
        return pCmdClient;
    }while(0);
    return 0;
}

static void PC_DeleteCmdClient(pteCmdClient_t *pCmdClient)
{
    if(0==pCmdClient) return;
    NET_SOCKET_CLOSE(pCmdClient->sockFd);
    PC_FREE(pCmdClient);
}

static int PC_CmdLogin(pteCmdClient_t *pCmdClient)
{
    char        pBuff[PC_BUFF_MAXLEN] = {0};
    ARG_CMD    *pCmd;
    int         rsl;
    int         len;

    if(0> pCmdClient->sockFd) return -1;

    pCmd                = (ARG_CMD *)pBuff;
    pCmd->ulFlag        = ARG_HEAD_CMD;
    pCmd->ulVersion     = ARG_SDK_VERSION_1_1;
    pCmd->ulID          = 0;
    pCmd->ulBufferNum   = 0;
    pCmd->ulBufferSize  = 0;

    do{
        if(0==pCmdClient->longinStep){
            len = (ARG_HEAD_LEN+pCmd->ulBufferNum*pCmd->ulBufferSize);
            pCmd->usCmd = CMD_GET_SERIALNUM;
            rsl = PC_SendCmd(pCmdClient->sockFd,pBuff,&len);
            if(0!=rsl) break;
            pCmdClient->longinStep = 1;
        }else if(1==pCmdClient->longinStep){
            len = PC_BUFF_MAXLEN;
            rsl = PC_RecvCmd(pCmdClient->sockFd,pBuff,&len);
            if(0!=rsl) break;
            if(0!=len){
                memmove((char*)pCmdClient->serialNum,(pBuff+ARG_HEAD_LEN),ARG_SERIALNUM_LEN);
                pCmdClient->longinStep = 2;
            }
        }else if(2==pCmdClient->longinStep){
            pCmd->usCmd = CMD_ACT_LOGIN;
            pCmd->ulBufferNum = 1;
            pCmd->ulBufferSize = sizeof(USER_INFO);

            len = (ARG_HEAD_LEN+pCmd->ulBufferNum*pCmd->ulBufferSize);
            USER_INFO *pUserInfo = (USER_INFO *)(pBuff+ARG_HEAD_LEN);
            snprintf((char*)pUserInfo->ucUsername,ARG_USERNAME_LEN,"%s",(char*)pCmdClient->userInfo.ucUsername);
            PP_DES_Encode((char*)pUserInfo->ucPassWord,(char*)pCmdClient->userInfo.ucPassWord,
                          (char*)pCmdClient->serialNum,ARG_PASSWD_LEN);
            PP_DES_Encode((char*)pUserInfo->ucSerialNum,(char*)pCmdClient->serialNum,
                          (char*)pCmdClient->serialNum,ARG_SERIALNUM_LEN);

            rsl = PC_SendCmd(pCmdClient->sockFd,pBuff,&len);
            if(0!=rsl) break;
            pCmdClient->longinStep = 3;
        }else if(3==pCmdClient->longinStep){
            len = PC_BUFF_MAXLEN;
            rsl = PC_RecvCmd(pCmdClient->sockFd,pBuff,&len);
            if(0!=rsl) break;
            if(0!=pCmd->ucState) break;
            if(0!=len){
                pCmdClient->state = PC_CMD_LOGINED;
                pCmdClient->timeout = 4;
                pCmdClient->id = pCmd->ulID;
                PC_DEBUG("connect success! id is %d",(int)pCmdClient->id);
            }
        }
        return 0;
    }while(0);
    return -1;
}

static int PC_SendHeartbeat(pteCmdClient_t *pCmdClient)
{
    char        pBuff[PC_BUFF_MAXLEN] = {0};
    ARG_CMD    *pCmd;
    int         rsl;
    int         len;

    if((0==pCmdClient) || 0> pCmdClient->sockFd) return -1;

    pCmd                = (ARG_CMD *)pBuff;
    pCmd->ulFlag        = ARG_HEAD_CMD;
    pCmd->ulVersion     = ARG_SDK_VERSION_1_1;
    pCmd->usCmd         = CMD_ACT_HEADRTBEAT;
    pCmd->ulID          = pCmdClient->id;
    pCmd->ulBufferNum   = 0;
    pCmd->ulBufferSize  = 0;

    do{
        len = ARG_HEAD_LEN;
        rsl = PC_SendCmd(pCmdClient->sockFd,pBuff,&len);
        if(0!=rsl) break;
        return 0;
    }while(0);
    return -1;
}

static int PC_ReciveHeartbeat(pteCmdClient_t *pCmdClient)
{
    char        pBuff[PC_BUFF_MAXLEN] = {0};
    int         rsl;
    int         len;

    if(0==pCmdClient) return -1;

    do{
        len = PC_BUFF_MAXLEN;
        rsl = PC_RecvCmd(pCmdClient->sockFd,pBuff,&len);
        if(0!=rsl) break;
        if(0==len){
            return 1;
        }
        return 0;
    }while(0);
    return -1;
}


int PC_SendCmd(int sockFd, char *pData, int *pLen)
{
    ARG_CMD    *pCmd;
    int         sendLen;
    int         len;

    if(0>sockFd) return -1;
    if(0==pData) return -1;
    if(0==pLen)  return -1;

    pCmd = (ARG_CMD*)pData;
    sendLen = (ARG_HEAD_LEN+pCmd->ulBufferNum*pCmd->ulBufferSize);
    if(sendLen > *pLen) return -1;

    do{
        len = send(sockFd,(void*)pData,sendLen,0);
        if(sendLen != len){
            break;
        }
        *pLen = len;
        return 0;
    }while(0);
    *pLen = 0;
    return -1;
}

int PC_RecvCmd(int sockFd, char *pData, int *pLen)
{
    ARG_CMD    *pCmd;
    char       *pBuff;
    int         len;

    if(0>sockFd) return -1;
    if(0==pData) return -1;
    if(0==pLen)  return -1;

    pCmd = (ARG_CMD*)pData;
    pBuff = pData+ARG_HEAD_LEN;

    do{
        len = recv(sockFd,(void*)pData,ARG_HEAD_LEN,0);
        if(0 == len){
            /*socket ??‘?”?é”?è??*/
            break;
        }if(-1 == len){
            /*socket è????????è??,?????????????????????*/
            *pLen = 0;
        }else if(ARG_HEAD_LEN != len){
            /*?????‘?”?é”?è??*/
            break;
        }else{
            if(0 != pCmd->ulFlag){
                PC_DEBUG("head type error!");
                break;
            }
            if(*pLen < (int)(ARG_HEAD_LEN+pCmd->ulBufferNum*pCmd->ulBufferSize) ){
                break;
            }
            if(0!=pCmd->ulBufferNum*pCmd->ulBufferSize){
                len = recv(sockFd,(void*)pBuff,(pCmd->ulBufferNum*pCmd->ulBufferSize),0);
                if(len != (int)(pCmd->ulBufferNum*pCmd->ulBufferSize)){
                    /*?????‘?”?é”?è??*/
                    break;
                }
            }
            *pLen = ARG_HEAD_LEN + (pCmd->ulBufferNum*pCmd->ulBufferSize);
        }

        return 0;
    }while(0);
    *pLen = 0;
    return -1;
}



/********************************************************************************************/
/*******************************        socket        ***************************************/
/********************************************************************************************/

//int SK_ConnectTo(unsigned int ip,int port)
//{
//    struct sockaddr_in address;
//    int sockfd;
//    int rsl;

//    address.sin_family = AF_INET;
//    address.sin_addr.s_addr = ip;
//    address.sin_port = htons(port);

//    do{
//        sockfd = socket(AF_INET,SOCK_STREAM,0);
//        if(0>sockfd) break;
////        rsl = SK_SetSocketUnblock(sockfd);
////        if(0!=rsl) break;
//        /*é??é?????connect ?????????è?”????¤±è??*/
//        rsl = connect(sockfd,(const struct sockaddr *)&address,sizeof(struct sockaddr_in));


//        return sockfd;
//    }while(0);
//    return -1;
//}


int SK_ConnectTo(unsigned int ip,int port)
{
    struct sockaddr_in address;

    int sockfd;
    int flags;
    int rsl;

    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(-1==sockfd) return -1;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = ip;
    address.sin_port = htons(port);

    flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    connect(sockfd,(const struct sockaddr *)&address,sizeof(struct sockaddr_in));

    return sockfd;
}


int SK_SetSocketUnblock(int fd)
{
    int flags;
    int rsl;

    do{
#if IS_PLATFORM_WINDOWS
        flags = 1;
        rsl = ioctlsocket(fd,FIONBIO,(unsigned long *)&flags);
        if(0!=rsl)break;
#elif  IS_PLATFORM_LINUX
        flags = fcntl(fd,F_GETFL,0);
        rsl   = fcntl(fd,F_SETFL,flags|O_NONBLOCK);
        if(0!=rsl) break;
#endif
        return 0;
    }while(0);
    return -1;
}



int SK_SelectWaitReadable(unsigned int fd, int msec)
{
    struct timeval timeout;
    fd_set fds;
    int rsl;

    timeout.tv_sec = msec/1000;
    timeout.tv_usec = (msec%1000)*1000;
    FD_ZERO(&fds);
    FD_SET(fd,&fds);

    rsl = select(fd+1,&fds,0,0,&timeout);
    if(0<rsl && 0!=FD_ISSET(fd,&fds)){
        return 0;
    }
    return -1;
}

int SK_SelectWaitWriteable(unsigned int fd, int msec)
{
    struct timeval timeout;
    fd_set fds;
    int rsl;

    timeout.tv_sec = msec/1000;
    timeout.tv_usec = (msec%1000)*1000;
    FD_ZERO(&fds);
    FD_SET(fd,&fds);

    rsl = select(fd+1,0,&fds,0,&timeout);
    if(0<rsl && 0!=FD_ISSET(fd,&fds)){
        return 0;
    }
    return -1;
}
