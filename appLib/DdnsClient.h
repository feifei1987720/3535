/*
*******************************************************************************
*
**  : 
*******************************************************************************
*/

#ifndef __DDNS_CLIENT_H__
#define __DDNS_CLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif
int StartDdnsThread();
int StopDdnsThread();
void  DdnsUpdate(int UpdateFlg);
#ifdef __cplusplus
}
#endif


#endif//__DDNS_CLIENT_H__