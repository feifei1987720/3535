#ifndef DEVDISCOVERY_H
#define DEVDISCOVERY_H

#include "discovery.h"
#include "LongseDes.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define DD_TASK_MAXNUM    64
#define DD_SEARCHER_MAX   12
#define DD_DEVNUM_MAX     512
#define DD_MENDER         32

struct task_t;
typedef int (*CbFuncType)(char* inBuff, int inLen, char* outBuff, int *outLen);
typedef struct FuncItem{
    char key;
    CbFuncType fun;
}FuncItem;

typedef enum DD_TASKTYPE_TYPE{
    DD_TASKTYPE_SERVER     = 0x00,
    DD_TASKTYPE_DOMODIFY,

    DD_TASKTYPE_SEARCH     = 0x20,
    DD_TASKTYPE_MODIFY,
}DD_TASKTYPE_TYPE;

typedef enum DD_TASKSTATE_TYPE{
    DD_TASKSTATE_UNDO      = 0,
    DD_TASKSTATE_WAIT      = 1,
    DD_TASKSTATE_TIMEOUT   = 2,
    DD_TASKSTATE_COMPLETE  = 3,
    DD_TASKSTATE_FAILED    = 4,
}DD_TASKSTATE_TYPE;

typedef int (*DD_TASK_PROC)(struct task_t *pTask);
typedef struct task_t{
    DD_TASKTYPE_TYPE    type;
    DD_TASKSTATE_TYPE   state;
    int                 sockFd;
    int                 time;

    DD_TASK_PROC        procFun;
    void               *pData;
    unsigned int        dataLen;
}task_t;

typedef struct mender_t{
    int sockFd;
    int time;
    unsigned char ucIpAddr[DSC_IPV4ADDR_LEN];
}mender_t;

typedef enum DD_MODIFY_RESULT{

    DD_MODIFY_SUCCESS   = 0,
    DD_MODIFY_FAILED    = 1,
    DD_MODIFY_TIMEOUT   = 2,
}DD_MODIFY_RESULT;

int discovery(void);

int DD_StartServer();

/*搜索网络设备*/
int DD_SearchDev();
/*发现新设备*/
int DD_FoundNewDev(DSC_SEARCH_RESPONSE_INFO *pInfo);
/*改变远程设备网络参数*/
int DD_ModifyDevNetConfig(DSC_MODIFY_INFO *pInfo);
/*改变远程设备网络参数回复*/
int DD_ModifyDevNetConfigResponse(unsigned char start, DSC_MODIFY_RESPONSE_INFO *pModifyResponse);


int DD_ModifyLocalNetConfig(DSC_MODIFY_INFO *pInfo);
int DD_CheckNetworkIsValid(DSC_NETWORK_INFO *pInfo);
int DD_CheckDevInfoIsValid(DSC_DEV_INFO *pInfo);
int DD_InitUserInfo(DSC_USER_INFO *pInfo);
int DD_InitNetworkInfo(DSC_NETWORK_INFO *pInfo);
int DD_InitDevInfo(DSC_DEV_INFO *pInfo);
int DD_InitPlatformInof(DSC_PLATFORM_INFO *pInfo);

#ifdef __cplusplus
}
#endif
#endif
