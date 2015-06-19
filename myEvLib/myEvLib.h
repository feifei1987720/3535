/*
*******************************************************************************
**  Copyright (c) 2012, All rights reserved.
**    QQ125174730
**  : 
*******************************************************************************
*/

#ifndef __MY_EV_LIB_H__
#define __MY_EV_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "networkLib.h"


/*  */
void myEvAddSocketEvent(int sock, network_dev_channel_info_t *pDevChannelInfo);

/*  */
void myEvDelSocketEvent(int sock);

/*  */
int myEvLibInit();

/*  */
void myEvLibDestroy();


#ifdef __cplusplus
}
#endif


#endif//__MY_EV_LIB_H__

