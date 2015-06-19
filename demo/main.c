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
#include "databaseLib.h"
#ifndef X86
	#include "decodeLib.h"
	#include "hiLib.h"
	#include "sample_comm.h"
#endif
#include "playbackLib.h"
#include "rtspSLib.h"
#include "diskLib.h"
#include "myEvLib.h"
#include "watchdogLib.h"
#include "gpioLib.h"
#include "alarmLib.h"
#include "configLib.h"
#include "gpioLib.h"
#include "boaLib/include/boa.h"
#include "p2pLib.h"
#include "motionLib.h"
#include "devdiscovery.h"
#include "snapLib.h"
#include "processmanage.h"

#if 0
void signal_process_info(struct sigaction *act)
{
	switch(act->sa_flags)
	{
		case (int)SIG_DFL:
			{
				printf("using default handler.\n");
				break;
			}
		case (int)SIG_IGN:
			{
				printf("ignore the signal.\n");
				break;
			}
		default:
			{
				printf("use handler: %0x.\n",act->sa_flags);
				break;
			}
	}
	return;
}
void usr1_handler(void)
{

}
int catchSignal(void)
{
	int i;
	struct sigaction act,oldact;
	act.sa_handler =  usr1_handler;
	act.sa_flags = SA_NODEFER|SA_RESETHAND;  //SA_RESETHANDSA_NODEFER:
	sigaction(SIGUSR1,&act,&oldact);  //SIGUSR1usr1_handler
	for(i=1;i<12;i++)
	{
		printf("signal %d handler is:",i);
		sigaction(i,NULL,&oldact); //act=NULL,
		signal_process_info(&oldact); //
	}
	return 0;
}
#else
void signalFunc(int sig)
{
	Printf("sig = %d\r\n",sig);                
	if((sig == SIGKILL) || (sig == SIGINT))
	{
		sleep(1);
		stopWtdthread();
		exit(0);
	}
}

void catchSignal()
{
	signal(SIGPIPE, signalFunc);
	signal(SIGKILL, signalFunc);
	signal(SIGINT, signalFunc);
//	signal(SIGSEGV, signalFunc);
//	signal(SIGABRT, signalFunc);
//	signal(SIGBUS, signalFunc);
}
#endif

void memroySet()
{
	mallopt(M_MMAP_MAX, 0);// mallocmmap
	mallopt(M_TRIM_THRESHOLD, -1);// sbrk
}

int main(int argc, char **argv)
{
	int i = 0;
	char ip[8][64] = {"192.168.16.213", "192.168.16.213", "192.168.16.213", "192.168.16.213", "192.168.16.213", "192.168.16.213"};
	int ipNum = 2;
	#if 1
#ifndef X86
	int resolution = PIC_HD1080;
#endif

	catchSignal();

//	memroySet();

	if(argc > 1)
	{
		for(i = 1; i < argc; i++)
		{
			memset(ip[i - 1], 0, sizeof(ip[i - 1]));
			strcpy(ip[i - 1], argv[i]);
		}
		ipNum = argc - 1;
	}
	if(ipNum < 2)
	{
		ipNum = 2;
	}
	databaseLibInit();
	//writeLog(LOG_MAJOR_SYSTEM, 0, 0, "system", "127.0.0.1", "system start");
	system("rm -rf  /config/*.log");

	P2pSnUpdate();
	//P2P¿ØÖÆ
	P2pInitStart();
	
	startWtdThread();
	configLibInit();
	gpioLibInit();
	streamLibInit();
	//logLibInit();
	startGuiSerVer();
	//gpioSetIcPower(1);
	//PHONE_ThrStart();
	//databaseLibInit();
	

//	gpioSetBeep(0);//
//	gpioSetDiskPower(0);

	setpriority(PRIO_PROCESS, getpid(), -20);

	/*  */
//	myEvLibInit();
	myThreadLibInit();
	diskLibInit();
	threadPoolInit(6 * configGetDisplayNum(), 6 * configGetDisplayNum());/**/
	decodeLibInit(0, configGetDisplayNum());
	networkLibInit();
	appLibInit();

	startBoaThread();
	//diskLibInit();
	playbackLibInit();
	recordLibInit();
	p2pTHRStart(0);

	///StartDdnsThread();
	///TE_platformCreate();

#ifndef X86
	hi_audio_init(PT_G711U);
#endif
	alarmLibInit();

	sleep(1);
	rtspServerStart();
	usleep(100*1000);
	discovery();
	appLibStart();
	sleep(2);
	startMotion();
	startProcessManage();
	////startSmtpThread();


	#ifndef HI3531
	SnapThreadStart();
	#endif


	setPthreadPriority(PTHREAD_SCHEDULED_PRIORITY_LOW_EST);
	#endif
	//appNtpSettime();
	while(1)
	{
		sleep(60);
	}

	stopWtdthread();
	decodeLibdestroy(configGetDisplayNum());
	networkLibDestroy();
	appLibDestroy();
	myEvLibDestroy();
	diskLibDestroy();

	hi_audio_destroy();
	alarmLibDestroy();
	gpioLibDestroy();
	streamLibUninit();

	return 0;
}


