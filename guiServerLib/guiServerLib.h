/*
 * =====================================================================================
 *
 *       Filename:  guiServerLib.h
 *
 *    Description:  guiServerLib
 *
 *        Version:  1.0
 *        Created:  20110523 015214
 *       Revision:  none
 *       Compiler:  gcc
 *
 *
 * =====================================================================================
 */
#ifndef __GUI_SERVER_H__
#define __GUI_SERVER_H__


#include "main.h"

#if defined (__cplusplus)
extern "C" {
#endif


int apcs_init(unsigned short port, void *p);
int apcs_reg_msg_dispatch(sdk_msg_dispatch_cb fun);
int apcs_reg_msg_dispatch_ptz(sdk_msg_dispatch_cb fun);
int apcs_deinit(void);

int apcs_msg_send(int module_id, sdk_msg_t *pmsg);


#if defined (__cplusplus)
}
#endif

#endif //__GUI_SERVER_H__

