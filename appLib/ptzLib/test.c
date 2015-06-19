#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "ptz.h"

int main(int argc, char** argv)
{
	int chn = 0;
	char * pSerialDevFile = "/dev/ttyAMA2";
	ptzInit(pSerialDevFile);
	PtzSerialConfig_t initpara;
	initpara.nAddr = 0;
	initpara.nBaudRate = 2400;
	initpara.nCheckType = 0;
	initpara.nDataBit = 8;
	initpara.nStopBit = 1;
	initpara.nProtocol = 0;
	
	setPtzSerial(chn, &initpara);
	
	int cmd;
	while(1)
	{
		printf("--->");
		scanf("%d", &cmd);
		if(cmd > PTZ_MSG_IRIS_SUB_STOP || cmd == PTZ_MSG_PRESET_SET ||\
			cmd == PTZ_MSG_PRESET_CALL || cmd == PTZ_MSG_PRESET_DEL)
		{
			continue;
		}
		else
		{
			ptzCmdDeal(chn, cmd, 15, NULL);
		}
		
		
		sleep (1);
	}
	return 0;
}