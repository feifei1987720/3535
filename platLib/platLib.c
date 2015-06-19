#include <stdlib.h>
#include <unistd.h>

#include "interface.h"
#include "platform_serv.h"

DevInterface *g_pInterface = NULL;
PlatformServer *g_pPlatformServ = NULL;

int platStart()
{
	PLATFORM_PARAM_S PlatformParam;
	memset(&PlatformParam, 0, sizeof(PLATFORM_PARAM_S));

	//strcpy(PlatformParam.Param.HuaWeiParam.s8MainDevID, "76900004000000017470");
	//strcpy(PlatformParam.Param.HuaWeiParam.s8CenterIP, "202.104.169.108");
	//PlatformParam.Param.HuaWeiParam.u32CenterPort = 3350;
	//PlatformParam.Param.HuaWeiParam.u8ProtocolType = 0;
	//
	//PlatformParam.Param.HuaWeiParam.u32AlarmInNum = 0;
	//PlatformParam.Param.HuaWeiParam.u32AlarmOutNum = 1;
	//
	//PlatformParam.Param.HuaWeiParam.u32LensNum = 1;
	//strcpy(PlatformParam.Param.HuaWeiParam.LensParam[0].s8SubDevID, "76900004010100017471");
	//strcpy(PlatformParam.Param.HuaWeiParam.LensParam[0].s8SubDevName, "test");

	//strcpy(PlatformParam.Param.RsPlatformParam.s8PuID, "1002000001000000000000000003");
	//strcpy(PlatformParam.Param.RsPlatformParam.s8PlatformIP, "125.89.62.182");
	//strcpy(PlatformParam.Param.RsPlatformParam.s8PuID, "1002000000000000000000000001");
	//strcpy(PlatformParam.Param.RsPlatformParam.s8PlatformIP, "172.18.1.123");
	strcpy(PlatformParam.Param.RsPlatformParam.s8PuID, "1002000000000000000000000051");
	strcpy(PlatformParam.Param.RsPlatformParam.s8PlatformIP, "125.89.62.182");
	//strcpy(PlatformParam.Param.RsPlatformParam.s8PuID, "1002000001112000000000000002");
	//strcpy(PlatformParam.Param.RsPlatformParam.s8PlatformIP, "112.91.85.141");
	//strcpy(PlatformParam.Param.RsPlatformParam.s8PuID, "1002000001111111111111111112");
	//strcpy(PlatformParam.Param.RsPlatformParam.s8PlatformIP, "172.18.1.111");
	PlatformParam.Param.RsPlatformParam.u32PlatformPort = 9005;
	PlatformParam.Param.RsPlatformParam.TramsmitProtocol = PLATFORM_TCP_PROTOCOL;
	PlatformParam.Param.RsPlatformParam.bCompressFlag = true;

	g_pInterface = new NetInterface(NULL);

	g_pPlatformServ = new PlatformServer(g_pInterface, true);

	g_pPlatformServ->StartPlatformService(RS_PLATFORM, &PlatformParam);
}

