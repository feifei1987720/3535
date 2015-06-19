#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "gpioLib.h"
#include "hiLib.h"


void signalFunc(int sig)
{
	printf("sig = %d\r\n",sig);
	sleep(3);
	if(sig == SIGKILL)
	{
		exit(0);
	}
	sleep(3);
}

void catchSignal()
{
	signal(SIGPIPE, signalFunc);
	signal(SIGKILL, signalFunc);
	signal(SIGINT, signalFunc);
	//      signal(SIGSEGV, signalFunc);
	//      signal(SIGABRT, signalFunc);
	//      signal(SIGBUS, signalFunc);

}


int main()
{
#if 0
	gpioLibInit();

	setGpioDir(GPIO_ADDR_12_6, 1);

	setGpioValue(GPIO_ADDR_12_6, 0);

	gpioLibDestroy();
#endif
//	hi_md_set();

	while(1)
	{
		sleep(10);
	}

	return 0;
}

