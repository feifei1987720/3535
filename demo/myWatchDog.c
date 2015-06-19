#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "gpioLib.h"
#include "watchdogLib.h"


int main()
{
	int  t=0;
	int ret=0;
	int fd =0;

	ret=openWtdog(&fd);
	if(ret<0)
	{
		printf("open fial\n");
	}
	else
	{
		printf("fd=%d\n",fd);
	}

	while(1)
	{
		t=0;
		ret=feedDog( fd, &t);
		if(ret<0)
		{
			printf("feedDog  fail\n");
			closeWtdog( fd);
			break;
		}
		else
		{
			//printf("feedDog success  \n");
		}

		usleep(500 * 1000);
	}

	closeWtdog( fd);

	return 0;
}

