#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <malloc.h>

#include "global.h"
#include "appLib.h"
#include "codeLib.h"
#include "commonLib.h"
#include "myThreadLib.h"
#include "networkLib.h"
#include "rtspCLib.h"
#include "recordLib.h"
#include "decodeLib.h"
#include "hiLib.h"
#include "sample_comm.h"
#include "playbackLib.h"
#include "diskLib.h"
#include "myEvLib.h"
#include "watchdogLib.h"
#include "gpioLib.h"
#include "configLib.h"

#include "hifb.h"


void initParamSetNetwork(sdk_eth_cfg_t *p_eth_cfg)
{
	printf("********************p_eth_cfg->ip_info.mac =%s\n",p_eth_cfg->ip_info.mac);
	
	netSetIp("eth0", inet_addr((char *)p_eth_cfg->ip_info.ip_addr));
	netSetMask("eth0", inet_addr((char *)p_eth_cfg->ip_info.mask));
	netSetGw("eth0", inet_addr((char *)p_eth_cfg->ip_info.gateway));
	netSetRoute("eth0", inet_addr((char *)p_eth_cfg->ip_info.gateway));
	netSetDns(inet_addr((char *)p_eth_cfg->ip_info.dns1), inet_addr((char *)p_eth_cfg->ip_info.dns2));
	netSetMac("eth0",&p_eth_cfg->ip_info.mac );
}

void initParamInitNetwork()
{
	int no = 0;
	int len = 0;
	sdk_eth_cfg_t net;

	memset(&net, 0, sizeof(sdk_eth_cfg_t));
	len = databaseOperate(SDK_MAIN_MSG_NET_LINK_CFG, SDK_PARAM_GET, &no, sizeof(no), &net, sizeof(sdk_eth_cfg_t));
	if(len == sizeof(sdk_eth_cfg_t))
	{
		initParamSetNetwork(&net);
	}
}

void initParamInit()
{
	initParamInitNetwork();
}
int main(int argc, char **argv)
{
	int bshow = 0;
	int fd = 0;
//	configLibInit();

	fd = open("/dev/fb/0", O_RDWR, 0);
	if(fd > 0)
	{
		ioctl(fd, FBIOPUT_SHOW_HIFB, &bshow);
		close(fd);
	}

	fd = open("/dev/fb/3", O_RDWR, 0);
	if(fd > 0)
	{
		ioctl(fd, FBIOPUT_SHOW_HIFB, &bshow);
		close(fd);
	}
	databaseLibInit();

	initParamInit();
#if 1
	decodeLibInit(PIC_HD1080, configGetDisplayNum());
	decodeLibdestroy(configGetDisplayNum());
#endif
	return 0;
}


