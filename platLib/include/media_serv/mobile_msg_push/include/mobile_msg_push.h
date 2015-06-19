#ifndef _MOBILE_MSG_PUSH_H_
#define _MOBILE_MSG_PUSH_H_

#include "intf_base.h"

/*
 *
 *pSendLink: 
 *                http://push.umeye.cn/rest/message/box/send.json
 *pSetupLink: 	boxidsid
 *                 http://push.umeye.cn/rest/message/box/setup.json
*/
void StartMsgPushServer_HSD(DevInterface* pIntf, char* pSendLink=NULL, char* pSetupLink=NULL);


/*
 *
 *
*/
void StopMsgPushServer_HSD();



/*
 *
 *0-1
*/
int MobilePushMsg(S32 s32SockFd);



#endif //_MOBILE_MSG_PUSH_H_
