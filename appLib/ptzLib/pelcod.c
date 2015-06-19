/***********************************************************************
 * \
 * pelco-d 协议  http://blog.sina.com.cn/s/blog_6210b8a30100ozkq.html
 以地址码0x01为例的部分指令：
 　　{0xff,0x01,0x00,0x08,0x00,0xff,0x08,}//上
 　　{0xff,0x01,0x00,0x10,0x00,0xff,0x10,}//下
 　　{0xff,0x01,0x00,0x04,0xff,0x00,0x04,}//左
 　　{0xff,0x01,0x00,0x02,0xff,0x00,0x02,}//右
 　　{0xff,0x01,0x00,0x20,0x00,0x00,0x21,}//变倍短
 　　{0xff,0x01,0x00,0x40,0x00,0x00,0x41,}//变倍长
 　　{0xff,0x01,0x00,0x80,0x00,0x00,0x81,}//聚焦近
 　　{0xff,0x01,0x01,0x00,0x00,0x00,0x02,}//聚焦远
 　　{0xff,0x01,0x02,0x00,0x00,0x00,0x03,}//光圈小
 　　{0xff,0x01,0x04,0x00,0x00,0x00,0x05,}//光圈大
 　　{0xff,0x01,0x00,0x0b,0x00,0x01,0x0d,}//灯光关
 　　{0xff,0x01,0x00,0x09,0x00,0x01,0x0b,}//灯光开
 　　{0xff,0x01,0x00,0x07,0x00,0x01,0x09,}//转至预置点001
 　　{0xff,0x01,0x00,0x03,0x00,0x01,0x05,}//设置预置点001
 　　{0xff,0x01,0x00,0x05,0x00,0x01,0x07,}//删除预置点001
 　　以上对应的停命令均是:
 　　{0xff,0x01,0x00,0x00,0x00,0x00,0x01,}//停命令
***********************************************************************/
#include <stdio.h>
#include <unistd.h>
#include "pelcod.h"



extern int g_s32Ptzfd;
static int sendDataBySerial(char *pCmdBuffer, int len)
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
	fsync(g_s32Ptzfd);

	
#if 1
	int i;
	for(i = 0; i < len; i ++)
	{
		printf("%02x  ", pCmdBuffer[i]);
	}
	printf("\n");
#endif
	return 0;
}


/*********************************************************************
	Function:   
		PtzStop
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcodPtzStop(char u8Addr)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xff;
	CmdBuffer[1] = u8Addr;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x00;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = 0x00;

	CmdBuffer[6] = CmdBuffer[1] + CmdBuffer[2] + CmdBuffer[3] + CmdBuffer[4] + CmdBuffer[5];

	return sendDataBySerial(CmdBuffer, 7);
}

/*********************************************************************
	Function:   
		PtzLeft
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcodPtzLeft(char u8Addr,  int s32Speed)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xff;
	CmdBuffer[1] = u8Addr;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x04;
	CmdBuffer[4] = s32Speed;
	CmdBuffer[5] = s32Speed;

	CmdBuffer[6] = CmdBuffer[1] + CmdBuffer[2] + CmdBuffer[3] + CmdBuffer[4] + CmdBuffer[5];

	return sendDataBySerial(CmdBuffer, 7);
}

/*********************************************************************
	Function:   
		PtzRight
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcodPtzRight(char u8Addr,  int s32Speed)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xff;
	CmdBuffer[1] = u8Addr;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x02;
	CmdBuffer[4] = s32Speed;
	CmdBuffer[5] = s32Speed;

	CmdBuffer[6] = CmdBuffer[1] + CmdBuffer[2] + CmdBuffer[3] + CmdBuffer[4] + CmdBuffer[5];

	return sendDataBySerial(CmdBuffer, 7);
}

/*********************************************************************
	Function:   
		PtzUp
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcodPtzUp(char u8Addr,  int s32Speed)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8]; 

		CmdBuffer[0] = 0xff;
		CmdBuffer[1] = u8Addr;

		CmdBuffer[2] = 0x00;
		CmdBuffer[3] = 0x08;
		CmdBuffer[4] = s32Speed;
		CmdBuffer[5] = s32Speed;

		CmdBuffer[6] = CmdBuffer[1] + CmdBuffer[2] + CmdBuffer[3] + CmdBuffer[4] + CmdBuffer[5];

		return sendDataBySerial(CmdBuffer, 7);
}

/*********************************************************************
	Function:   
		PtzDown
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcodPtzDown(char u8Addr,  int s32Speed)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xff;
	CmdBuffer[1] = u8Addr;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x10;
	CmdBuffer[4] = s32Speed;
	CmdBuffer[5] = s32Speed;

	CmdBuffer[6] = CmdBuffer[1] + CmdBuffer[2] + CmdBuffer[3] + CmdBuffer[4] + CmdBuffer[5];

	return sendDataBySerial(CmdBuffer, 7);
}

/*********************************************************************
	Function:   
		PtzUpRight
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcodPtzUpRight(char u8Addr,  int s32Speed)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8]; 

	CmdBuffer[0] = 0xff;
	CmdBuffer[1] = u8Addr;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x0a;
	CmdBuffer[4] = s32Speed;
	CmdBuffer[5] = s32Speed;

	CmdBuffer[6] = CmdBuffer[1] + CmdBuffer[2] + CmdBuffer[3] + CmdBuffer[4] + CmdBuffer[5];

	return sendDataBySerial(CmdBuffer, 7);
}

/*********************************************************************
	Function:   
		PtzUpLeft
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcodPtzUpLeft(char u8Addr,  int s32Speed)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8]; 

	CmdBuffer[0] = 0xff;
	CmdBuffer[1] = u8Addr;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x0c;
	CmdBuffer[4] = s32Speed;
	CmdBuffer[5] = s32Speed;

	CmdBuffer[6] = CmdBuffer[1] + CmdBuffer[2] + CmdBuffer[3] + CmdBuffer[4] + CmdBuffer[5];

	return sendDataBySerial(CmdBuffer, 7);
}

/*********************************************************************
	Function:   
		PtzDownLeft
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcodPtzDownLeft(char u8Addr,  int s32Speed)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xff;
	CmdBuffer[1] = u8Addr;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x14;
	CmdBuffer[4] = s32Speed;
	CmdBuffer[5] = s32Speed;

	CmdBuffer[6] = CmdBuffer[1] + CmdBuffer[2] + CmdBuffer[3] + CmdBuffer[4] + CmdBuffer[5];
	return sendDataBySerial(CmdBuffer, 7);
}

/*********************************************************************
	Function:   
		PtzDownRight
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcodPtzDownRight(char u8Addr,  int s32Speed)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xff;
	CmdBuffer[1] = u8Addr;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x12;
	CmdBuffer[4] = s32Speed;
	CmdBuffer[5] = s32Speed;

	CmdBuffer[6] = CmdBuffer[1] + CmdBuffer[2] + CmdBuffer[3] + CmdBuffer[4] + CmdBuffer[5];

	return sendDataBySerial(CmdBuffer, 7);
}

/*********************************************************************
	Function:   
		PtzIris_Inc
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcodPtzIrisInc(char u8Addr)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xff;
	CmdBuffer[1] = u8Addr;

	CmdBuffer[2] = 0x02;
	CmdBuffer[3] = 0x00;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = 0x00;

	CmdBuffer[6] = CmdBuffer[1] + CmdBuffer[2] + CmdBuffer[3] + CmdBuffer[4] + CmdBuffer[5];

	return sendDataBySerial(CmdBuffer, 7);
}

/*********************************************************************
	Function:   
		PtzAutoStop
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcodPtzAutoStop(char u8Addr,  int s32Speed)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xff;
	CmdBuffer[1] = u8Addr;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x07;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = 0x60;

	CmdBuffer[6] = CmdBuffer[1] + CmdBuffer[2] + CmdBuffer[3] + CmdBuffer[4] + CmdBuffer[5];

	return sendDataBySerial(CmdBuffer, 7);
}

/*********************************************************************
	Function:   
		PtzAuto
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcodPtzAuto(char u8Addr,  int s32Speed)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xff;
	CmdBuffer[1] = u8Addr;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x07;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = 0x63;

	CmdBuffer[6] = CmdBuffer[1] + CmdBuffer[2] + CmdBuffer[3] + CmdBuffer[4] + CmdBuffer[5];

	return sendDataBySerial(CmdBuffer, 7);
}

/*********************************************************************
	Function:   
		PtzIris_Dec
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcodPtzIrisDec(char u8Addr)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xff;
	CmdBuffer[1] = u8Addr;

	CmdBuffer[2] = 0x04;
	CmdBuffer[3] = 0x00;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = 0x00;

	CmdBuffer[6] = CmdBuffer[1] + CmdBuffer[2] + CmdBuffer[3] + CmdBuffer[4] + CmdBuffer[5];

	return sendDataBySerial(CmdBuffer, 7);
}

/*********************************************************************
	Function:   
		PtzZoom_Inc
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcodPtzZoomInc(char u8Addr)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xff;
	CmdBuffer[1] = u8Addr;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x20;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = 0x00;

	CmdBuffer[6] = CmdBuffer[1] + CmdBuffer[2] + CmdBuffer[3] + CmdBuffer[4] + CmdBuffer[5];

	return sendDataBySerial(CmdBuffer, 7);
}

/*********************************************************************
	Function:   
		pelcodPtzZoomDec
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcodPtzZoomDec(char u8Addr)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xff;
	CmdBuffer[1] = u8Addr;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x40;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = 0x00;

	CmdBuffer[6] = CmdBuffer[1] + CmdBuffer[2] + CmdBuffer[3] + CmdBuffer[4] + CmdBuffer[5];

	return sendDataBySerial(CmdBuffer, 7);
}

/*********************************************************************
	Function:   
		pelcodPtzFocusInc
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcodPtzFocusInc(char u8Addr)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xff;
	CmdBuffer[1] = u8Addr;

	CmdBuffer[2] = 0x01;
	CmdBuffer[3] = 0x00;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = 0x00;

	CmdBuffer[6] = CmdBuffer[1] + CmdBuffer[2] + CmdBuffer[3] + CmdBuffer[4] + CmdBuffer[5];

	return sendDataBySerial(CmdBuffer, 7);
}

/*********************************************************************
	Function:   
		pelcodPtzFocusDec
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcodPtzFocusDec(char u8Addr)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xff;
	CmdBuffer[1] = u8Addr;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x80;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = 0x00;

	CmdBuffer[6] = CmdBuffer[1] + CmdBuffer[2] + CmdBuffer[3] + CmdBuffer[4] + CmdBuffer[5];

	return sendDataBySerial(CmdBuffer, 7);
}

/*********************************************************************
	Function:   
		pelcodPtzGotoPoint
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcodPtzGotoPoint(char u8Addr, int s32Point)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xff;
	CmdBuffer[1] = u8Addr;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x07;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = s32Point;

	CmdBuffer[6] = CmdBuffer[1] + CmdBuffer[2] + CmdBuffer[3] + CmdBuffer[4] + CmdBuffer[5];

	return sendDataBySerial(CmdBuffer, 7);
}

/*********************************************************************
	Function:   
		pelcodPtzSetPoint
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcodPtzSetPoint(char u8Addr, int s32Point)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xff;
	CmdBuffer[1] = u8Addr;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x03;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = s32Point;

	CmdBuffer[6] = CmdBuffer[1] + CmdBuffer[2] + CmdBuffer[3] + CmdBuffer[4] + CmdBuffer[5];
	return sendDataBySerial(CmdBuffer, 7);
}

/*********************************************************************
	Function:   
		pelcodPtzClearPoint
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcodPtzClearPoint(char u8Addr, int s32Point)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}
	char CmdBuffer[8];
	CmdBuffer[0] = 0xff;
	CmdBuffer[1] = u8Addr;
	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x05;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = s32Point;
	CmdBuffer[6] = CmdBuffer[1] + CmdBuffer[2] + CmdBuffer[3] + CmdBuffer[4] + CmdBuffer[5];
   
   return sendDataBySerial(CmdBuffer, 7);   
}

