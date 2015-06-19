#include "ptz.h"
#include "pelcod.h"
#include "pelcop.h"


int g_s32Ptzfd;
pthread_mutex_t g_ptzLock;
PtzSerialConfig_t g_ptzPara[MAX_PTZ_NUM];

int sendCmdBySerial(char *pCmdBuffer, int len);
int sendCmdToPtzPelcod(PtzCmd_t cmd, char addr, int speed, void *other);
int sendCmdToPtzPelcop(PtzCmd_t cmd, char addr,	int speed, void *other);


/*********************************************************************
	Function:  
		PtzInit
	Description:   
		打开用于和云台进行通讯的串口，该函数需要指定打开的串口
	Calls:		 
  	Called By:     
	parameter:	       
  	Return:       
  	author:
 ********************************************************************/
int ptzInit(char *pSerialDevFile)
{
	pthread_mutex_init(&g_ptzLock, NULL);
	pthread_mutex_lock(&g_ptzLock);
	
	g_s32Ptzfd = open(pSerialDevFile, O_RDWR);
	if(g_s32Ptzfd < 0)
	{
		printf("OpenPtzSerial open failed(%s)\n", pSerialDevFile);		
		pthread_mutex_unlock(&g_ptzLock);		
		return -1;
	}
	else
	{
		printf("OpenPtzSerial open success(%s) !\n", pSerialDevFile);
	}
	
	pthread_mutex_unlock(&g_ptzLock);
	
	return 0;
}



/*********************************************************************
	Function: PtzDelInit  
	Description:   
	Calls:		 
  	Called By:     
	parameter:	       
  	Return:       
  	author:
 ********************************************************************/
int ptzDelInit(void *other)
{
	pthread_mutex_lock(&g_ptzLock);
	
	if(g_s32Ptzfd >= 0)
	{
		close(g_s32Ptzfd);
		g_s32Ptzfd = -1;
	}
	
	pthread_mutex_unlock(&g_ptzLock);
	
	return 0;
}

/*********************************************************************
	Function:  
		SetSerial
	Description:   
		设置串口的配置参数
	Calls:		 
  	Called By:     
	parameter:	       
  	Return:       
  	author:
 ********************************************************************/
static int setSerial(PtzSerialConfig_t *para)
{
	pthread_mutex_lock(&g_ptzLock);
	
	struct termios   opt;
	speed_t tmp;
	tcgetattr(g_s32Ptzfd, &opt); 
	/*设置波特率*/
	switch(para->nBaudRate)
	{
		case 1200:
			tmp = B1200;
			break;
		case 2400:			
			tmp = B2400;		
			break;
		case 4800:
			tmp = B4800;		
			break;
		case 9600:
			tmp = B9600;
			break;
		case 19200:
			tmp = B19200;
			break;
		case 38400:
			tmp = B38400;
			break;
		case 57600:
			tmp = B57600;
			break;
		case 115200:
			tmp = B115200;
			break;
		default:
			tmp = B9600;
			break;
	}
	
	tcflush(g_s32Ptzfd, TCIOFLUSH);     
	cfsetispeed(&opt, tmp);
	cfsetospeed(&opt, tmp);   
	tcsetattr(g_s32Ptzfd, TCSANOW, &opt); 
	/*设置数据位*/
	tcflush(g_s32Ptzfd, TCIOFLUSH);   
	tcgetattr(g_s32Ptzfd, &opt); 
	tcflush(g_s32Ptzfd, TCIOFLUSH);
	opt.c_cflag &= ~CSIZE;
	switch (para->nDataBit) 						
	{   
		case 5:
			opt.c_cflag |= CS5; 
			break;
		case 6:
			opt.c_cflag |= CS6; 
			break;
		case 7:		
			opt.c_cflag |= CS7; 
			break;
		case 8:     
			opt.c_cflag |= CS8;
			break;   
		default:    
			fprintf(stderr,"Unsupported data size\n");
			break;
	}
	/* 设置停止位*/  
	switch (para->nStopBit)
	{   
		case 1:    
			opt.c_cflag &= ~CSTOPB;  
			break;  
		case 2:    
			opt.c_cflag |= CSTOPB;  
		   	break;
		default:    
			 fprintf(stderr,"Unsupported stop bits\n"); 
			 break;
	} 
	/*设置奇偶位*/
	switch (para->nCheckType) 
	{   
		case 0:
			opt.c_cflag &= ~PARENB;  			 /* Clear parity enable */
			opt.c_iflag &= ~INPCK;     			/* Clear parity checking */ 
			break;  
		case 1:
			opt.c_cflag |= (PARODD | PARENB); 	/* 设置为奇效验*/  
			opt.c_iflag |= INPCK;            		 /* Enable parity checking */ 
			break;  
		case 2:
			opt.c_cflag |= PARENB;     			/* Enable parity */    
			opt.c_cflag &= ~PARODD;   			/* 转换为偶效验*/     
			opt.c_iflag |= INPCK;       			/* Enable parity checking */
			break;
		default:   
			fprintf(stderr,"Unsupported parity\n"); 
			break;
	}  
	
	tcflush(g_s32Ptzfd, TCIFLUSH); 
	opt.c_oflag  &= ~OPOST;
	opt.c_cc[VTIME] = 0;
	opt.c_cc[VMIN] = 4;

	tcflush(g_s32Ptzfd, TCIFLUSH);	/* Update the options and do it NOW */
	if(tcsetattr (g_s32Ptzfd, TCSANOW, &opt) != 0)
	{
		perror("SetupSerial 3");
		return -1;
	}

	pthread_mutex_unlock(&g_ptzLock);
	
	return 0;
}

/*********************************************************************
	Function:  
		SetSerial
	Description:   
		设置串口的配置参数
	Calls:		 
  	Called By:     
	parameter:	       
  	Return:       
  	author:
 ********************************************************************/

int setPtzSerial(int chn, PtzSerialConfig_t *para)
{
	memcpy(&g_ptzPara[chn], para, sizeof(PtzSerialConfig_t));
	
	return 0;
}

/*********************************************************************
	Function:  
		ptzCmdDeal
	Description:   
		处理上层发送的云台控制命令 ,设置预支点的时候需要将相应的预置点号传递给void *other
	Calls:		 
  	Called By:     
	parameter:	       
  	Return:       
  	author:
 ********************************************************************/
int ptzCmdDeal(int s32Chn, PtzCmd_t cmd, int s32Speed, void *other)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	setSerial(&g_ptzPara[s32Chn]);
	
	switch(g_ptzPara[s32Chn].nProtocol)
	{
		case PP_PELCO_D:
			return sendCmdToPtzPelcod(cmd, g_ptzPara[s32Chn].nAddr, s32Speed, other);
			break;
		case PP_PELCO_P:
			return sendCmdToPtzPelcop(cmd, g_ptzPara[s32Chn].nAddr, s32Speed, other);
			break;
		default:
			printf("SendCmdToPtz not support ptz protocol\n");
			break;
	}

	return -1;
}

/*********************************************************************
	Function:   
	Description:  
		  Byte1 	  Byte2     Byte3     Byte4    Byte5  Byte6   Byte7 
		SynchByte  Address  Command1  Command 2  Data1  Data2  CheckSum
	Calls:		 
  	Called By:     
	parameter:	       
  	Return:       
  	author:
 ********************************************************************/
int sendCmdToPtzPelcod(PtzCmd_t cmd, char addr,  int speed,void *other)
{
	
	int s32Point = 0;
	if(other)
	{
		s32Point = *(int *)other;
		if(s32Point < 0)
		{
			printf("the preset point num error\n");
			return -1;
		}
	}

	switch(cmd)
	{
		///start
		case PTZ_MSG_UP_START:
			pelcodPtzUp(addr,speed);
			break;
		case PTZ_MSG_DOWN_START:
			pelcodPtzDown(addr,speed);
			break;
		case PTZ_MSG_LEFT_START:
			pelcodPtzLeft(addr,speed);
			break;
		case PTZ_MSG_RIGHT_START:
			pelcodPtzRight(addr,speed);
			break;
		case PTZ_MSG_LEFT_UP_START:
			pelcodPtzUpLeft(addr,speed);
			break;
		case PTZ_MSG_LEFT_DOWN_START:		
			pelcodPtzDownLeft(addr,speed);
			break;
		case PTZ_MSG_RIGHT_UP_START:	
			pelcodPtzUpRight(addr,speed);
			break;
		case PTZ_MSG_RIGHT_DOWN_START:
			pelcodPtzDownRight(addr,speed);
			break;
		case PTZ_MSG_PRESET_SET:
			pelcodPtzSetPoint(addr, s32Point);
			break;
		case PTZ_MSG_PRESET_CALL:			
			pelcodPtzGotoPoint(addr, s32Point);
			break;
		case PTZ_MSG_PRESET_DEL:			
			pelcodPtzClearPoint(addr, s32Point);
			break;

		case PTZ_MSG_ZOOM_ADD_START:
			pelcodPtzZoomInc(addr);
			break;
		case PTZ_MSG_ZOOM_SUB_START:
			pelcodPtzZoomInc(addr);
			break;
		case PTZ_MSG_FOCUS_ADD_START:
			pelcodPtzFocusInc(addr);
			break;
		case PTZ_MSG_FOCUS_SUB_START:
			pelcodPtzFocusDec(addr);
			break;
		case PTZ_MSG_IRIS_ADD_START:
			pelcodPtzIrisInc(addr);
			break;
		case PTZ_MSG_IRIS_SUB_START:
			pelcodPtzIrisDec(addr);
			break;

		///stop
		case PTZ_MSG_UP_STOP:
		case PTZ_MSG_DOWN_STOP:
		case PTZ_MSG_LEFT_STOP:
		case PTZ_MSG_RIGHT_STOP:
		case PTZ_MSG_LEFT_UP_STOP:
		case PTZ_MSG_LEFT_DOWN_STOP:		
		case PTZ_MSG_RIGHT_UP_STOP:	
		case PTZ_MSG_RIGHT_DOWN_STOP:
		case PTZ_MSG_ZOOM_ADD_STOP:
		case PTZ_MSG_ZOOM_SUB_STOP:
		case PTZ_MSG_FOCUS_ADD_STOP:
		case PTZ_MSG_FOCUS_SUB_STOP:
		case PTZ_MSG_IRIS_ADD_STOP:
		case PTZ_MSG_IRIS_SUB_STOP:
			pelcodPtzStop(addr);
			break;
		default:
			printf("SendCmdToPtzPelcod not support cmd\n");
			return -1;
			break;
	}

	
	
	return 0;
}


/*********************************************************************

	Function:   
	Description:   
	Calls:		 
  	Called By:     
	parameter:	       
  	Return:       
  	author:
 ********************************************************************/
int sendCmdToPtzPelcop(PtzCmd_t cmd, char addr,	int speed,void *other)
{
	int s32Point = 0;
	if(other)
	{
		s32Point = *(int *)other;
		if(s32Point < 0)
		{
			printf("the preset point num error\n");
			return -1;
		}
	}
	switch(cmd)
	{
		///start
		case PTZ_MSG_UP_START:
			pelcopPtzUp(addr,speed);
			break;
		case PTZ_MSG_DOWN_START:
			pelcopPtzDown(addr,speed);
			break;
		case PTZ_MSG_LEFT_START:
			pelcopPtzLeft(addr,speed);
			break;
		case PTZ_MSG_RIGHT_START:
			pelcopPtzRight(addr,speed);
			break;
		case PTZ_MSG_LEFT_UP_START:
			pelcopPtzUpLeft(addr,speed);
			break;
		case PTZ_MSG_LEFT_DOWN_START:		
			pelcopPtzDownLeft(addr,speed);
			break;
		case PTZ_MSG_RIGHT_UP_START:	
			pelcopPtzUpRight(addr,speed);
			break;
		case PTZ_MSG_RIGHT_DOWN_START:
			pelcopPtzDownRight(addr,speed);
			break;
		case PTZ_MSG_PRESET_SET:
			pelcodPtzSetPoint(addr, s32Point);
			break;
		case PTZ_MSG_PRESET_CALL:
			pelcodPtzGotoPoint(addr, s32Point);
			break;
		case PTZ_MSG_PRESET_DEL:
			pelcodPtzClearPoint(addr, s32Point);
			break;

		case PTZ_MSG_ZOOM_ADD_START:
			pelcopPtzZoomInc(addr);
			break;
		case PTZ_MSG_ZOOM_SUB_START:
			pelcopPtzZoomInc(addr);
			break;
		case PTZ_MSG_FOCUS_ADD_START:
			pelcopPtzFocusInc(addr);
			break;
		case PTZ_MSG_FOCUS_SUB_START:
			pelcopPtzFocusDec(addr);
			break;
		case PTZ_MSG_IRIS_ADD_START:
			pelcopPtzIrisInc(addr);
			break;
		case PTZ_MSG_IRIS_SUB_START:
			pelcopPtzIrisDec(addr);
			break;


		///stop
		case PTZ_MSG_UP_STOP:
		case PTZ_MSG_DOWN_STOP:
		case PTZ_MSG_LEFT_STOP:
		case PTZ_MSG_RIGHT_STOP:
		case PTZ_MSG_LEFT_UP_STOP:
		case PTZ_MSG_LEFT_DOWN_STOP:		
		case PTZ_MSG_RIGHT_UP_STOP: 
		case PTZ_MSG_RIGHT_DOWN_STOP:
		case PTZ_MSG_ZOOM_ADD_STOP:
		case PTZ_MSG_ZOOM_SUB_STOP:
		case PTZ_MSG_FOCUS_ADD_STOP:
		case PTZ_MSG_FOCUS_SUB_STOP:
		case PTZ_MSG_IRIS_ADD_STOP:
		case PTZ_MSG_IRIS_SUB_STOP:
			pelcopPtzStop(addr);
			break;
		default:
			printf("SendCmdToPtzPelco_d not support cmd\n");
			return -1;
			break;
	}

	
	
	return 0;
}

/*********************************************************************
	Function:   
	Description:   
	Calls:		 
  	Called By:     
	parameter:	       
  	Return:       
  	author:
 ********************************************************************/
 int sendCmdBySerial(char *pCmdBuffer, int len)
{
	if(g_s32Ptzfd < 0)
	{
		return -1;
	}
	
	if(write(g_s32Ptzfd, pCmdBuffer, len) != len)
	{
		printf("SendCmdBySerial write failed\n");
		return -1;
	}
	int i = 0;
	for(i = 0; i < len; i ++)
	{
		printf("%02x  ", pCmdBuffer[i]);
	}
	
	fsync(g_s32Ptzfd);

	return 0;
}

