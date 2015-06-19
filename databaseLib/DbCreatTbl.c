#include <stdlib.h>
#include <stdio.h>

#include "commonLib.h"
#include "sqlite3.h"
#include "sdk_struct.h"
#include "global.h"

/******************************************
函数功能:数据库读写测试-- 创建新表单

******************************************/
//
void DbCreate_DbBkTestTbl(sqlite3 *db,char *TblName)
{
	char sql_cmd[256] = {0};
	char *err_msg = NULL;
	Printf("====Database Create New Table :%s ====\n",TblName);
	sprintf(sql_cmd,"CREATE TABLE [%s](\
		[idx] INT(1) PRIMARY KEY NOT NULL,\
		[BackupTest] CHAR(32) NOT NULL);",TblName
		);
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		system("cp /config/system.db   /data/system.db -fv");
		if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
		{
			system("cp /mnt/system.db   /data/system.db -fv");
			system("reboot");
		}
		
	}
}
/******************************************
函数功能:数据库读写测试-- 创建新表单

******************************************/
void DbInit_DbBkTestTbl(sqlite3 *db,char *TblName)
{
	char sql_cmd[64] = {0};
	char *err_msg = NULL;
	Printf("#####Database Init New Table :%s #####\n",TblName);
	sprintf(sql_cmd,"INSERT INTO %s VALUES (0,\" Backup Test!\");",TblName);
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);	
	}
}

 // 移动侦测配置
void DbCreate_MotionTbl(sqlite3 *db,char *TblName)
{
	char sql_cmd[380] = {0};
	char *err_msg = NULL;
	Printf("====Database Create New Table :%s ====\n",TblName);
	sprintf(sql_cmd,
			"CREATE TABLE [%s](\
			[channel] INT(1) PRIMARY KEY NOT NULL,\
			[motionEnable] INT(1) NOT NULL,\
			[beepEnable] INT(1) NOT NULL,\
			[beepTime]  INT(1) NOT NULL,\
			[recordEnable] INT(1) NOT NULL,\
			[recordTime] INT(1) NOT NULL,\
			[smtpEnable] INT(1) NOT NULL,\
			[ftpEnable] INT(1) NOT NULL,\
			[sensitivity]  INT(1) NOT  NULL,\
			[blockstatus]  CHAR(750) NOT  NULL\
			);",TblName
		);
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);	
	}

}
 void DbInit_MotionTbl(sqlite3 *db,char *TblName)
 {
 	int j = 0;
	char sql_cmd[1024] = {0};
	char str[32] = {0};
	char *err_msg = NULL;
	Printf("#####Database Init New Table :%s #####\n",TblName);
	sprintf(sql_cmd,"INSERT INTO %s VALUES ",TblName);
	for(j = 0; j < MAX_DISPLAY_NUM; j++)
	{
		memset(str,0,sizeof(str));
		sprintf(str,"(%d,0,0,0,0,60,0,0,50,\"\"",j);
		strcat(sql_cmd,str);
		if(j == MAX_DISPLAY_NUM -1)
		{
			strcat(sql_cmd,");");
		}
		else
		{
			strcat(sql_cmd,"),");
		}
	}
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);	
	}
 }

//NTP配置
void DbCreate_NtpTbl(sqlite3 *db,char *TblName)
{
	char sql_cmd[512] = {0};
	char *err_msg = NULL;
	printf("====Database Create New Table :%s ====\n",TblName);
	sprintf(sql_cmd,"CREATE TABLE [%s](\
			[idx2] INT(1) PRIMARY KEY NOT NULL,\
			[timezone] INT(1) NOT NULL,\
			[ntpserver] CHAR(128) NOT NULL,\
			[enable] INT(1) NOT NULL,\
			[interval] INT(1) NOT NULL,\
			[dateFormat] INT(1) NOT NULL,\
			[dateSeparator] INT(1) NOT NULL,\
			[timeFormat] INT(1) NOT NULL,\
			[port] INT(4) NOT NULL\
		);",TblName\
	);
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);	
	}
}


void DbInit_NtpTbl(sqlite3 *db,char *TblName)
{
	char sql_cmd[512] = {0};
	char *err_msg = NULL;
	printf("#####Database Init New Table :%s #####\n",TblName);
	sprintf(sql_cmd,"INSERT INTO %s VALUES (0,24,\"clock.via.net\",0,60,0,0,0,123);",TblName);
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);	
	}
}

// 日志配置
void DbCreate_LogInfoTbl(sqlite3 *db,char *TblName)
{
	char sql_cmd[256] = {0,};
	char *err_msg = NULL;
	Printf("====Database Create New Table :%s ====\n",TblName);
	sprintf(sql_cmd,
			"CREATE TABLE [%s](\
			[idx] INT(1) PRIMARY KEY NOT NULL, \
			[logid] INT(1) NOT NULL,\
			[time] CHAR(64) NOT NULL,\
			[user] CHAR(32) NOT NULL\
			);",TblName
		);
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);	
	}
}
// SMTP配置
void DbCreate_SmtpTbl(sqlite3 *db,char *TblName)
{
	char sql_cmd[550] = {0};
	char *err_msg = NULL;
	Printf("====Database Create New Table :%s ====\n",TblName);
	sprintf(sql_cmd,
			"CREATE TABLE [%s](\
			[idx2] INT(1) NOT NULL,\
			[anonymity] INT(1) NOT NULL,\
			[encryption_type] INT(1) NOT NULL,\
			[file_enable] INT(1) NOT NULL,\
			[health_enable] INT(1) NOT NULL,\
			[health_interval] INT(1) NOT NULL,\
			[interval] INT(1) NOT NULL,\
			[port] INT(1) NOT NULL,\
			[email_enable] INT(1) NOT NULL,\
			[passwd] CHAR(32) NOT NULL,\
			[receiver] CHAR(32) NOT NULL,\
			[sender] CHAR(32) NOT NULL,\
			[smtp_server] CHAR(128) NOT NULL,\
			[theme] CHAR(32) NOT NULL,\
			[user] CHAR(32) NOT NULL\
			);",TblName
		);
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);	
	}

	
}
 void DbInit_SmtpTbl(sqlite3 *db,char *TblName)
 {
 	char sql_cmd[256] = {0};
	char *err_msg = NULL;
	Printf("#####Database Init New Table :%s #####\n",TblName);
	sprintf(sql_cmd,"INSERT INTO %s VALUES (0,0,1,0,0,0,1,465,0,\"\",\"\",\"\",\"smtp.naver.com\",\"Notice_Netcury_NVR\",\"\");",TblName);
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);	
	}
 }

// 普通配置
void DbCreate_CommTbl(sqlite3 *db,char *TblName)
{
	char sql_cmd[512] = {0};
	char *err_msg = NULL;
	Printf("====Database Create New Table :%s ====\n",TblName);
	sprintf(sql_cmd,
			"CREATE TABLE [%s](\
			[idx] INT(1) PRIMARY KEY NOT NULL,\
			[language] INT(1) NOT NULL,\
			[recycleRecord] INT(1) NOT NULL,\
			[dvrId] INT(1) NOT NULL,\
			[outDevice] INT(1) NOT NULL,\
			[videoMode] INT(1) NOT NULL,\
			[resolution] INT(1) NOT NULL,\
			[standbyTime] INT(1) NOT NULL,\
			[devName] CHAR(32) NOT NULL,\
			[wizardEnable] INT(1) NOT NULL,\
			[transparent] INT(1) NOT NULL\
			);",TblName
		);
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);	
	}

}
 void DbInit_CommTbl(sqlite3 *db,char *TblName)
 {
 	char sql_cmd[256] = {0};
	char *err_msg = NULL;
	Printf("#####Database Init New Table :%s #####\n",TblName);
	sprintf(sql_cmd,"INSERT INTO CommCfgInfo VALUES ( 0, 1, 0, 0, 2, 1, 1, 30,\"NVR\",1,18);");
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);	
	}
 }
// 通道配置
void DbCreate_ChannelTbl(sqlite3 *db,char *TblName)
{
	char sql_cmd[512] = {0};
	char *err_msg = NULL;
	Printf("====Database Create New Table :%s ====\n",TblName);
//如果要添加字段，请插入到最后一列，不要插到中间。
	sprintf(sql_cmd,
			"CREATE TABLE [%s](\
			[enable]INT(1) NOT NULL,\
			[channelName] CHAR(32)  NOT NULL,\
			[channelNo] INT(1) NOT NULL,\
			[streamNo] INT(1) NOT NULL,\
			[addr] CHAR(32)  NOT NULL,\
			[port] INT(1) NOT NULL,\
			[userName] CHAR(32)  NOT NULL,\
			[userPwd] CHAR(32)  NOT NULL,\
			[protocol] INT(1) NOT NULL,\
			[manProtocolType] INT(1) NOT NULL,\
			[rtspmainurl] CHAR(128) NOT NULL,\
			[rtspsuburl] CHAR(128) NOT NULL\
			);",TblName
		);
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);	
	}

}
 void DbInit_ChannelTbl(sqlite3 *db,char *TblName)
 {
 	char sql_cmd[4096] = {0};
	char str[1024] = {0};
	char *err_msg = NULL;
	int i = 0;
	Printf("#####Database Init New Table :%s #####\n",TblName);
	sprintf(sql_cmd,"INSERT INTO %s VALUES ",TblName);
	for(i = 0; i < 32; i++)
	{
		memset(str, 0, sizeof(str));
		sprintf(str,"(0,\"\",%d,1,\"\",0,\"admin\",\"\",0,0,\"rtsp://192.168.1.168:554/0\",\"rtsp://192.168.1.168:554/1\"",i);
		strcat(sql_cmd,str);
		if(i == 32 -1)
		{
			strcat(sql_cmd,");");
		}
		else
		{
			strcat(sql_cmd,"),");
		}
	}
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);	
	}
 }
// DDNS配置
void DbCreate_DdnsTbl(sqlite3 *db,char *TblName)
{
	char sql_cmd[260] = {0};
	char *err_msg = NULL;
	Printf("====Database Create New Table :%s ====\n",TblName);
	sprintf(sql_cmd,
			"CREATE TABLE [%s](\
			[idx1]INT(1) NOT NULL,\
			[enable]INT(1) NOT NULL,\
			[type] INT(1) NOT NULL,\
			[interTime] INT(1) NOT NULL,\
			[domain] CHAR(128)  NOT NULL,\
			[userName] CHAR(64) NOT NULL,\
			[userPwd] CHAR(32) NOT NULL,\
			[userType] INT(1) NOT NULL\
			);",TblName
		  );
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);	
	}

}
 void DbInit_DdnsTbl(sqlite3 *db,char *TblName)
 {
	char sql_cmd[64] = {0};
	char *err_msg = NULL;
	int i = 0;
	Printf("#####Database Init New Table :%s #####\n",TblName);
	for(i = 0; i < MAX_DDNS_TYPE; i++)
	{
		memset(sql_cmd,0,sizeof(sql_cmd));
		sprintf(sql_cmd,"INSERT INTO %s VALUES (%d,0,%d,60,\"\",\"\",\"\",0);",TblName,i,i);
		if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);	
		}
	}
 }
// P2P配置
void DbCreate_P2pTbl(sqlite3 *db,char *TblName)
{
	char sql_cmd[256] = {0};
	char *err_msg = NULL;
	Printf("====Database Create New Table :%s ====\n",TblName);
	sprintf(sql_cmd,
			"CREATE TABLE [%s](\
			[idx] INT(1) PRIMARY KEY NOT NULL,\
			[Manufacture]CHAR(32) NOT NULL,\
			[P2pEn]INT(1)  NOT NULL,\
			[SerialNum]CHAR(64) NOT NULL,\
			[AndroidAppUrl]CHAR(128) NOT NULL,\
			[IosAppUrl]CHAR(128) NOT NULL\
			);",TblName
		);
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);	
	}
}
 void DbInit_P2pTbl(sqlite3 *db,char *TblName)
 {
 	char sql_cmd[256] = {0};
	char *err_msg = NULL;
	Printf("#####Database Init New Table :%s #####\n",TblName);
	sprintf(sql_cmd,"INSERT INTO %s VALUES (\
		0 \
		,\"FreeIp\"\
		,1\
		,\"\"\
		,\"http://www.freeip.com/some_file/FreeIp.apk\"\
		,\"https://itunes.apple.com/cn/app/free-ip/id898690336?mt=8\"\
		);",TblName
		);
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("insert failed!!\n\n");
	}
	memset(sql_cmd,0,sizeof(sql_cmd));
	sprintf(sql_cmd,"INSERT INTO %s VALUES (\
	1 \
	,\"Danale\"\
	,0\
	,\"\"\
	,\"http://map.m.360.cn/t/S_Rd6Ds\"\
	,\"https://itunes.apple.com/cn/app/dana/id802942016?mt=8\"\
	);",TblName
	);
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);	
	}
 }

void DbCreate_RecTimeTbl(sqlite3 *db,char *TblName)
{
	char sql_cmd[1250] = {0};
	char str[256] = {0};
	int i;
	char *err_msg = NULL;
	Printf("====Database Create New Table :%s ====\n",TblName);
	sprintf(sql_cmd,
			"CREATE TABLE [%s](\
			[idx]INT(1) NOT NULL\
			,[channelNo]INT(1) NOT NULL\
			,[streamNo] INT(1) NOT NULL\
			,[weekday] INT(1) NOT NULL\
			,[recCurSegNum] INT(1) NOT NULL\
		  ",TblName);
	for(i = 0 ; i < MAX_REC_SEG; i++)
	{
		memset(str,0,sizeof(str));
		sprintf(str,",[enable%d] INT(1) NOT NULL\
		,[recType%d] INT(1) NOT NULL\
		,[startHour%d] INT(1) NOT NULL\
		,[startMin%d] INT(1) NOT NULL\
		,[stopHour%d] INT(1) NOT NULL\
		,[stopMin%d] INT(1) NOT NULL\
		",i,i,i,i,i,i);
		strcat(sql_cmd,str);
	}
	strcat(sql_cmd,");");
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
	}

}

 void DbInit_RecTimeTbl(sqlite3 *db,char *TblName)
 {
	char * sql_cmd = (char *)malloc(22 * 1024 * sizeof(char));
	char *err_msg = NULL;
	char str[64] = {0};
	int i, j,k;
	Printf("#####Database Init New Table :%s #####\n",TblName);
	sprintf(sql_cmd,"INSERT INTO %s VALUES ",TblName);
	for(i = 0; i < MAX_DISPLAY_NUM; i++)// 通道数
	{
		for(j = 0; j < MAX_WEEK_NUM -1; j++)// 星期
		{
			sprintf(str,"(%d,%d,0,%d,1",(MAX_WEEK_NUM -1)*i+j,i,j);
			strcat(sql_cmd,str);
			memset(str,0,sizeof(str));
			sprintf(str,",0,1,0,0,24,0");
			strcat(sql_cmd,str);
			for(k = 0; k < MAX_REC_SEG -1; k++)// 时间段
			{
				memset(str,0,sizeof(str));
				sprintf(str,",0,0,0,0,0,0");
				strcat(sql_cmd,str);
			}
			if((i == MAX_DISPLAY_NUM -1) && (j == MAX_WEEK_NUM -2))
			{
				strcat(sql_cmd,");");
			}
			else
			{
				strcat(sql_cmd,"),");
			}
		}
		
	}
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
	}
	free(sql_cmd);
 }

// 图像设置
void DbCreate_ImgAttrTbl(sqlite3 *db,char *TblName)
{
	char sql_cmd[256] = {0};
	char *err_msg = NULL;
	Printf("====Database Create New Table :%s ====\n",TblName);
	sprintf(sql_cmd,
			"CREATE TABLE [%s](\
			[idx]INT(1) NOT NULL,\
			[Bright]INT(1) NOT NULL,\
			[Contrast] INT(1) NOT NULL,\
			[Saturation] INT(1) NOT NULL,\
			[Color] INT(1) NOT NULL\
			);",TblName
		  );
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);	
	}
}
 void DbInit_ImgAttrTbl(sqlite3 *db,char *TblName)
 {
 	char sql_cmd[256] = {0};
	char *err_msg = NULL;
	Printf("#####Database Init New Table :%s #####\n",TblName);
	sprintf(sql_cmd,"INSERT INTO %s VALUES (\
		0\
		,128 \
		,128 \
		,128 \
		,128 \
		);",TblName
		);
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);	
	}
 }

// 云台PTZ巡航
void DbCreate_CruiseTbl(sqlite3 *db,char *TblName)
{
	char *sql_cmd = (char *)malloc(6 * 1024 * sizeof(char));
	char str[256] = {0};
	char *err_msg = NULL;
	int i = 0;
	Printf("====Database Create New Table :%s ====\n",TblName);
	sprintf(sql_cmd,
		"CREATE TABLE [%s](\
		[idx1]INT(1) NOT NULL\
		,[ch]INT(1) NOT NULL\
		,[line_num] INT(1) NOT NULL\
		,[line_en] INT(1) NOT NULL\
		",TblName
	  );
	for(i = 0; i < MAX_CRUISE_PRESET_NUM; i++)
	{
		memset(str,0,sizeof(str));

		sprintf(str,",[cru_num%d] INT(1) NOT NULL\
		,[cru_en%d] INT(1) NOT NULL\
		,[cru_sp%d] INT(1) NOT NULL\
		,[cru_dt%d] INT(1) NOT NULL\
		,[cru_pre%d_num] INT(1) NOT NULL\
		,[cru_pre%d_en] INT(1) NOT NULL\
		",i,i,i,i,i,i);
		strcat(sql_cmd,str);
	}
	strcat(sql_cmd,");");
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
	}
	// 初始化表单
	free(sql_cmd);
}
 void DbInit_CruiseTbl(sqlite3 *db,char *TblName)
 {
 	char *sql_cmd = (char *)malloc(45 * 1024 * sizeof(char));
	char str[64] = {0};
	char *err_msg = NULL;
	int i = 0, j = 0, k = 0;
	Printf("#####Database Init New Table :%s #####\n",TblName);
	sprintf(sql_cmd,"INSERT INTO %s VALUES ",TblName);
	for(i = 0; i < MAX_DISPLAY_NUM; i++)// 通道
	{
		for(j = 0; j < MAX_CRUISE_LINE_NUM; j++)// 三条巡航线路
		{
			sprintf(str,"(%d,%d,%d,0 ", j + 3 * i, i, j);
			strcat(sql_cmd,str);
			for(k = 0; k < MAX_CRUISE_PRESET_NUM; k++)// 每条巡航线路上有32个预置点
			{
				memset(str,0,sizeof(str));
				sprintf(str,",%d,0,0,0,0,0",k + 1);
				strcat(sql_cmd,str);
			}
			if((i == MAX_DISPLAY_NUM -1) && (j == MAX_CRUISE_LINE_NUM -1))
			{
				strcat(sql_cmd,");");
			}
			else
			{
				strcat(sql_cmd,"),");
			}
		}
	}
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
	}
	free(sql_cmd);
 }

// 云台PTZ巡航
void DbCreate_AllPresetTbl(sqlite3 *db,char *TblName)
{
	char *sql_cmd = (char *)malloc(4 * 1024 * sizeof(char));
	char str[248] = {0};
	char *err_msg = NULL;
	int i = 0;
	Printf("====Database Create New Table :%s ====\n",TblName);
	sprintf(sql_cmd,
		"CREATE TABLE [%s](\
		[idx]INT(1) NOT NULL\
		,[ch]INT(1) NOT NULL\
		",TblName);	

	for(i = 0; i < MAX_PRESET_NUM; i++)
	{
		memset(str,0,sizeof(str));
		sprintf(str,",[preset%d_en] INT(1) NOT NULL",i);
		strcat(sql_cmd,str);
	}
	strcat(sql_cmd,");");
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
	}
	free(sql_cmd);
}

 void DbInit_AllPresetTbl(sqlite3 *db,char *TblName)
 {
 	char *sql_cmd = (char *)malloc(9 * 1000 * sizeof(char));
	char *err_msg = NULL;
	char str[64] = {0};
	int i, j;
	Printf("#####Database Init New Table :%s #####\n",TblName);
	sprintf(sql_cmd,"INSERT INTO %s VALUES ",TblName);
	for(i = 0; i < MAX_DISPLAY_NUM; i++)//通道
	{
		sprintf(str,"(%d,%d", i,i);
		strcat(sql_cmd,str);
		for(j = 0; j < MAX_PRESET_NUM; j++)
		{
			memset(str,0,sizeof(str));
			sprintf(str,",0");
			strcat(sql_cmd,str);
		}
		if(i == MAX_DISPLAY_NUM - 1)
		{
			strcat(sql_cmd,");");
		}
		else
		{
			strcat(sql_cmd,"),");
		}
		
	}
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
	}	
	free(sql_cmd);
 }

//自动维护
void DbCreate_AutoMaintainTbl(sqlite3 *db,char *TblName)
{
	char sql_cmd[512] = {0};
	char *err_msg = NULL;
	Printf("====Database Create New Table :%s ====\n",TblName);
	sprintf(sql_cmd,
			"CREATE TABLE [%s](\
			[idx1]INT(1) NOT NULL,\
			[interval]INT(1) NOT NULL,\
			[hour] INT(1) NOT NULL\
			);",TblName
		  );
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);	
	}
}

void DbInit_AutoMaintainTbl(sqlite3 *db,char *TblName)
 {
 	char sql_cmd[512] = {0};
	char *err_msg = NULL;
	Printf("#####Database Init New Table :%s #####\n",TblName);
	sprintf(sql_cmd,"INSERT INTO %s VALUES (\
		0\
		,0 \
		,3 \
		);",TblName
		);
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);	
	}
 }

// encode param
void DbCreate_EncodeInfoTbl(sqlite3 *db,char *TblName)
{
	char sql_cmd[512] = {0};
	char *err_msg = NULL;
	Printf("====Database Create New Table :%s ====\n",TblName);
	sprintf(sql_cmd,
			"CREATE TABLE [%s](\
			[idx1]INT(1) NOT NULL,\
			[channelNo]INT(1) NOT NULL,\
			[encodeType]INT(1) NOT NULL,\
			[mainResolution]INT(1) NOT NULL,\
			[mainEncodeMode]INT(1) NOT NULL,\
			[mainFrameRate]INT(1) NOT NULL,\
			[mainBiteRate]INT(1) NOT NULL,\
			[subResolution]INT(1) NOT NULL,\
			[subEncodeMode]INT(1) NOT NULL,\
			[subBitRate]INT(1) NOT NULL,\
			[subFrameRate] INT(1) NOT NULL\
			);",TblName
		  );
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);	
	}
}

void DbInit_EncodeInfoTTbl(sqlite3 *db,char *TblName)
 {
 	char sql_cmd[512] = {0};
	char *err_msg = NULL;
	Printf("#####Database Init New Table :%s #####\n",TblName);
	sprintf(sql_cmd,"INSERT INTO %s VALUES (\
		0\
		,0 \
		,0 \
		,0 \
		,0 \
		,0 \
		,0 \
		,0 \
		,0 \
		,0 \
		,0 \
		);",TblName
		);
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);	
	}
 }

// encode param
void DbCreate_pollTbl(sqlite3 *db,char *TblName)
{
	char sql_cmd[512] = {0};
	char *err_msg = NULL;
	Printf("====Database Create New Table :%s ====\n",TblName);
	sprintf(sql_cmd,
			"CREATE TABLE [%s](\
			[idx2]INT(1) NOT NULL,\
			[enable]INT(1) NOT NULL,\
			[time]INT(1) NOT NULL,\
			[mode]INT(1) NOT NULL\
			);",TblName
		  );
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);	
	}
}

void DbInit_pollTbl(sqlite3 *db,char *TblName)
 {
 	char sql_cmd[512] = {0};
	char *err_msg = NULL;
	Printf("#####Database Init New Table :%s #####\n",TblName);
	sprintf(sql_cmd,"INSERT INTO %s VALUES (0,0,5,0);",TblName
		);
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);	
	}
 }




 //FTP
  void DbCreate_FtpTbl(sqlite3 *db,char *TblName)
  {
	  char sql_cmd[512] = {0};
	  char *err_msg = NULL;
	  Printf("====Database Create New Table :%s ====\n",TblName);
	  sprintf(sql_cmd,
			  "CREATE TABLE [%s](\
			  [idx]INT(1) NOT NULL,\
			  [bFtpEnable]INT(1) NOT NULL,\
			  [servier_ip]CHAR(41) NOT NULL,\
			  [username]CHAR(32) NOT NULL,\
			  [password] CHAR(16) NOT NULL,\
			  [foldername]CHAR(128) NOT NULL,\
			  [port] INT(4) NOT NULL\
			  );",TblName
			);
	  if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	  {
		  Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);   
	  }
  
  }



  void DbInit_FtpTbl(sqlite3 *db,char *TblName)
  {
	  char sql_cmd[512] = {0};
	  char *err_msg = NULL;
	  Printf("#####Database Init New Table :%s #####\n",TblName);
	  sprintf(sql_cmd,"INSERT INTO %s VALUES (\
		  0\
		  ,0 \
		  ,\"\" \
		  ,\"\" \
		  ,\"\" \
		  ,\"\" \
		  ,21 \
		  );",TblName
		  );
	  if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	  {
		  Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);   
	  }
  }


