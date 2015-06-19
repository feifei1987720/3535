/************************************************************************
 * pelco-p 协议  http://blog.sina.com.cn/s/blog_6210b8a30100ozkq.html
 以地址码0x01为例：
 {0xa0,0x01,0x00,0x08,0x00,0x30,0xaf,0x39,}//上
 {0xa0,0x01,0x00,0x10,0x00,0x30,0xaf,0x21,}//下
 {0xa0,0x01,0x00,0x04,0x10,0x00,0xaf,0x15,}//左
 {0xa0,0x01,0x00,0x02,0x10,0x00,0xaf,0x13,}//右
 {0xa0,0x01,0x00,0x40,0x00,0x00,0xaf,0x41,}//变倍短
 {0xa0,0x01,0x00,0x20,0x00,0x00,0xaf,0x21,}//变倍长
 {0xa0,0x01,0x02,0x00,0x00,0x00,0xaf,0x03,}//聚焦近
 {0xa0,0x01,0x01,0x00,0x00,0x00,0xaf,0x00,}//聚焦远
 {0xa0,0x01,0x08,0x00,0x00,0x00,0xaf,0x09,}//光圈小
 {0xa0,0x01,0x04,0x00,0x00,0x00,0xaf,0x05,}//光圈大
 　　无命令字发送//灯光关
 　　无命令字发送//灯光开
 {0xa0,0x01,0x00,0x96,0x00,0x20,0xaf,0xb7,}//自动巡航
 {0xa0,0x01,0x00,0x99,0x00,0x20,0xaf,0xb8,}//关闭自动巡航
 {0xa0,0x01,0x00,0x07,0x00,0x01,0xaf,0x07,}//转至预置点001
 {0xa0,0x01,0x00,0x03,0x00,0x01,0xaf,0x03,}//设置预置点001
 {0xa0,0x01,0x00,0x05,0x00,0x01,0xaf,0x01,}//删除预置点001
 以上对应的停命令均是（上下左右、变倍、聚焦、光圈均有停命令）:
 {0xa0,0x01,0x00,0x00,0x00,0x00,0xaf,0x01,}
 ************************************************************************/
#include <stdio.h>
#include <unistd.h>

#include "pelcop.h"

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
int pelcopPtzStop(char u8Addr)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xa0;
	CmdBuffer[1] = u8Addr - 1;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x00;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = 0x00;

	CmdBuffer[6] = 0xaf;
	CmdBuffer[7] = CmdBuffer[0]^CmdBuffer[1]^CmdBuffer[2]^CmdBuffer[3]^CmdBuffer[4]^CmdBuffer[5]^CmdBuffer[6];

	return sendDataBySerial(CmdBuffer, 8);
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
int pelcopPtzLeft(char u8Addr,  int s32Speed)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xa0;
	CmdBuffer[1] = u8Addr - 1;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x04;
	CmdBuffer[4] = s32Speed;
	CmdBuffer[5] = 0x00;

	CmdBuffer[6] = 0xaf;
	CmdBuffer[7] = CmdBuffer[0]^CmdBuffer[1]^CmdBuffer[2]^CmdBuffer[3]^CmdBuffer[4]^CmdBuffer[5]^CmdBuffer[6];
	return sendDataBySerial(CmdBuffer, 8);

	return 0;
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
int pelcopPtzRight(char u8Addr,  int s32Speed)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xa0;
	CmdBuffer[1] = u8Addr - 1;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x02;
	CmdBuffer[4] = s32Speed;
	CmdBuffer[5] = 0x00;

	CmdBuffer[6] = 0xaf;
	CmdBuffer[7] = CmdBuffer[0]^CmdBuffer[1]^CmdBuffer[2]^CmdBuffer[3]^CmdBuffer[4]^CmdBuffer[5]^CmdBuffer[6];

	return sendDataBySerial(CmdBuffer, 8);
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
int pelcopPtzUp(char u8Addr,  int s32Speed)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8]; 

	CmdBuffer[0] = 0xa0;
	CmdBuffer[1] = u8Addr - 1;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x08;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = s32Speed;

	CmdBuffer[6] = 0xaf;
	CmdBuffer[7] = CmdBuffer[0]^CmdBuffer[1]^CmdBuffer[2]^CmdBuffer[3]^CmdBuffer[4]^CmdBuffer[5]^CmdBuffer[6];

	return sendDataBySerial(CmdBuffer, 8);
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
int pelcopPtzDown(char u8Addr,  int s32Speed)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xa0;
	CmdBuffer[1] = u8Addr - 1;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x10;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = s32Speed;

	CmdBuffer[6] = 0xaf;
	CmdBuffer[7] = CmdBuffer[0]^CmdBuffer[1]^CmdBuffer[2]^CmdBuffer[3]^CmdBuffer[4]^CmdBuffer[5]^CmdBuffer[6];

	return sendDataBySerial(CmdBuffer, 8);
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
int pelcopPtzUpRight(char u8Addr,  int s32Speed)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8]; 

	CmdBuffer[0] = 0xa0;
	CmdBuffer[1] = u8Addr - 1;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x0a;
	CmdBuffer[4] = s32Speed;
	CmdBuffer[5] = s32Speed;

	CmdBuffer[6] = 0xaf;
	CmdBuffer[7] = CmdBuffer[0]^CmdBuffer[1]^CmdBuffer[2]^CmdBuffer[3]^CmdBuffer[4]^CmdBuffer[5]^CmdBuffer[6];

	return sendDataBySerial(CmdBuffer, 8);
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
int pelcopPtzUpLeft(char u8Addr,  int s32Speed)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8]; 

	CmdBuffer[0] = 0xa0;
	CmdBuffer[1] = u8Addr - 1;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x0c;
	CmdBuffer[4] = s32Speed;
	CmdBuffer[5] = s32Speed;

	CmdBuffer[6] = 0xaf;
	CmdBuffer[7] = CmdBuffer[0]^CmdBuffer[1]^CmdBuffer[2]^CmdBuffer[3]^CmdBuffer[4]^CmdBuffer[5]^CmdBuffer[6];

	return sendDataBySerial(CmdBuffer, 8);
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
int pelcopPtzDownLeft(char u8Addr,  int s32Speed)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xa0;
	CmdBuffer[1] = u8Addr - 1;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x14;
	CmdBuffer[4] = s32Speed;
	CmdBuffer[5] = s32Speed;

	CmdBuffer[6] = 0xaf;
	CmdBuffer[7] = CmdBuffer[0]^CmdBuffer[1]^CmdBuffer[2]^CmdBuffer[3]^CmdBuffer[4]^CmdBuffer[5]^CmdBuffer[6];

	return sendDataBySerial(CmdBuffer, 8);
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
int pelcopPtzDownRight(char u8Addr,  int s32Speed)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xa0;
	CmdBuffer[1] = u8Addr - 1;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x12;
	CmdBuffer[4] = s32Speed;
	CmdBuffer[5] = s32Speed;

	CmdBuffer[6] = 0xaf;
	CmdBuffer[7] = CmdBuffer[0]^CmdBuffer[1]^CmdBuffer[2]^CmdBuffer[3]^CmdBuffer[4]^CmdBuffer[5]^CmdBuffer[6];

	return sendDataBySerial(CmdBuffer, 8);
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
int pelcopPtzIrisInc(char u8Addr)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xa0;
	CmdBuffer[1] = u8Addr - 1;

	CmdBuffer[2] = 0x04;
	CmdBuffer[3] = 0x00;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = 0x00;

	CmdBuffer[6] = 0xaf;
	CmdBuffer[7] = CmdBuffer[0]^CmdBuffer[1]^CmdBuffer[2]^CmdBuffer[3]^CmdBuffer[4]^CmdBuffer[5]^CmdBuffer[6];

	return sendDataBySerial(CmdBuffer, 8);
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
int pelcopPtzAutoStop(char u8Addr,  int s32Speed)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xa0;
	CmdBuffer[1] = u8Addr - 1;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x07;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = 0x60;

	CmdBuffer[6] = 0xaf;
	CmdBuffer[7] = CmdBuffer[0]^CmdBuffer[1]^CmdBuffer[2]^CmdBuffer[3]^CmdBuffer[4]^CmdBuffer[5]^CmdBuffer[6];

	return sendDataBySerial(CmdBuffer, 8);
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
int pelcopPtzAuto(char u8Addr,  int s32Speed)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xa0;
	CmdBuffer[1] = u8Addr - 1;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x07;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = 0x63;

	CmdBuffer[6] = 0xaf;
	CmdBuffer[7] = CmdBuffer[0]^CmdBuffer[1]^CmdBuffer[2]^CmdBuffer[3]^CmdBuffer[4]^CmdBuffer[5]^CmdBuffer[6];

	return sendDataBySerial(CmdBuffer, 8);
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
int pelcopPtzIrisDec(char u8Addr)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xa0;
	CmdBuffer[1] = u8Addr - 1;

	CmdBuffer[2] = 0x08;
	CmdBuffer[3] = 0x00;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = 0x00;

	CmdBuffer[6] = 0xaf;
	CmdBuffer[7] = CmdBuffer[0]^CmdBuffer[1]^CmdBuffer[2]^CmdBuffer[3]^CmdBuffer[4]^CmdBuffer[5]^CmdBuffer[6];

	return sendDataBySerial(CmdBuffer, 8);
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
int pelcopPtzZoomInc(char u8Addr)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xa0;
	CmdBuffer[1] = u8Addr - 1;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x20;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = 0x00;

	CmdBuffer[6] = 0xaf;
	CmdBuffer[7] = CmdBuffer[0]^CmdBuffer[1]^CmdBuffer[2]^CmdBuffer[3]^CmdBuffer[4]^CmdBuffer[5]^CmdBuffer[6];

	return sendDataBySerial(CmdBuffer, 8);
}

/*********************************************************************
	Function:   
		PtzZoom_Dec
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcopPtzZoomDec(char u8Addr)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xa0;
	CmdBuffer[1] = u8Addr - 1;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x40;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = 0x00;

	CmdBuffer[6] = 0xaf;
	CmdBuffer[7] = CmdBuffer[0]^CmdBuffer[1]^CmdBuffer[2]^CmdBuffer[3]^CmdBuffer[4]^CmdBuffer[5]^CmdBuffer[6];

	return sendDataBySerial(CmdBuffer, 8);
}

/*********************************************************************
	Function:   
		PtzFocus_Inc
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcopPtzFocusInc(char u8Addr)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xa0;
	CmdBuffer[1] = u8Addr - 1;

	CmdBuffer[2] = 0x01;
	CmdBuffer[3] = 0x00;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = 0x00;

	CmdBuffer[6] = 0xaf;
	CmdBuffer[7] = CmdBuffer[0]^CmdBuffer[1]^CmdBuffer[2]^CmdBuffer[3]^CmdBuffer[4]^CmdBuffer[5]^CmdBuffer[6];

	return sendDataBySerial(CmdBuffer, 8);
}

/*********************************************************************
	Function:   
		PtzFocus_Dec
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcopPtzFocusDec(char u8Addr)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xa0;
	CmdBuffer[1] = u8Addr - 1;

	CmdBuffer[2] = 0x02;
	CmdBuffer[3] = 0x00;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = 0x00;

	CmdBuffer[6] = 0xaf;
	CmdBuffer[7] = CmdBuffer[0]^CmdBuffer[1]^CmdBuffer[2]^CmdBuffer[3]^CmdBuffer[4]^CmdBuffer[5]^CmdBuffer[6];

	return sendDataBySerial(CmdBuffer, 8);
}

/*********************************************************************
	Function:   
		pelcopPtzGotoPoint
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcopPtzGotoPoint(char u8Addr, int s32Point)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xa0;
	CmdBuffer[1] = u8Addr - 1;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x07;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = s32Point;

	CmdBuffer[6] = 0xaf;
	CmdBuffer[7] = CmdBuffer[0]^CmdBuffer[1]^CmdBuffer[2]^CmdBuffer[3]^CmdBuffer[4]^CmdBuffer[5]^CmdBuffer[6];

	return sendDataBySerial(CmdBuffer, 8);
}

/*********************************************************************
	Function:   
		pelcopPtzSetPoint
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/
int pelcopPtzSetPoint(char u8Addr, int s32Point)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}

	char CmdBuffer[8];

	CmdBuffer[0] = 0xa0;
	CmdBuffer[1] = u8Addr - 1;

	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x03;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = s32Point;
	CmdBuffer[6] = 0xaf;
	CmdBuffer[7] = CmdBuffer[0]^CmdBuffer[1]^CmdBuffer[2]^CmdBuffer[3]^CmdBuffer[4]^CmdBuffer[5]^CmdBuffer[6];

	return sendDataBySerial(CmdBuffer, 8);
}

/*********************************************************************
	Function:   
		pelcopPtzClearPoint
	Description:   
	Calls:		 
  	Called By:     
	parameter:
  	Return:
  		success 0
  		fail -1       
 ********************************************************************/

int pelcopPtzClearPoint(char u8Addr, int s32Point)
{
	if(g_s32Ptzfd < 0)
	{
		printf("SendCmdToPtz you must open the ptz serial first!\n");
		return -1;
	}
	char CmdBuffer[8];
	CmdBuffer[0] = 0xa0;
	CmdBuffer[1] = u8Addr - 1;
	CmdBuffer[2] = 0x00;
	CmdBuffer[3] = 0x05;
	CmdBuffer[4] = 0x00;
	CmdBuffer[5] = s32Point;
	CmdBuffer[6] = 0xaf;
	CmdBuffer[7] = CmdBuffer[0]^CmdBuffer[1]^CmdBuffer[2]^CmdBuffer[3]^CmdBuffer[4]^CmdBuffer[5]^CmdBuffer[6];

	return sendDataBySerial(CmdBuffer, 8);
}

