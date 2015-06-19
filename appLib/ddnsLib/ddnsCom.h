/********************************************************************

	ddnsNoti.h

********************************************************************/


#ifndef __DDNS_COM_H__
#define __DDNS_COM_H__


#define uint8_t char

typedef enum _DDNS_TYPE
{
	_DDNS_ORAY = 0
	,_DDNS_NOIP
	,_DDNS_DYNDNS
	,_DDNS_CHANGEIP
	,_DDNS_APRESS
	,_DDNS_MYQSEE
	,_DDNS_NOTI
	,_DDNS_SMARTEYES
}_DDNS_TYPE;

typedef struct ddns_cfg_s {
	uint8_t type;						// DDNSSDK_DDNS_TYPE_E
	uint8_t server[128];	// DDNS(IP)
	uint8_t user[64];
	uint8_t pwd[32];
	uint8_t domain[128];	// (dyndns,domain',')
}ddns_cfg_t;


int StartDdns(ddns_cfg_t *ddns);

#endif

