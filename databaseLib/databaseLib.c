/**********************************************************
 * databaseLib.c
//  说明:
1. 以下进程对system.db数据库进行了读写操作:
1)/app/bin/DbBackup 用于数据库备份
2)P2P_FreeIp和P2P_Danale的P2P客户端获取admin用户名和P2P序列号

 **********************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/reboot.h>

#include "param.h"
#include "commonLib.h"
#include "appLib.h"
#include "recordLib.h"
#include "diskLib.h"
#include "onvif_client.h"
#include "databaseLib.h"
#include "sqlite3.h"
#include "alarmLib.h"
#include "configLib.h"
#include "playbackLib.h"
#include "DbSystem.h"
#include "DbCreatTbl.h"

pthread_mutex_t databaseMutex = PTHREAD_MUTEX_INITIALIZER;

//
int UerEnumExch(int *subType)
{
	int j = 0;
	char UserSubTypeEnum[4][2] = {
		{SDK_USER_MODI,SDK_PARAM_SET},
		{SDK_USER_QUERY,SDK_PARAM_GET},
		{SDK_USER_ADD,SDK_PARAM_INSERT},
		{SDK_USER_DEL,SDK_PARAM_DELETE}
	};
	for(j = 0; j < 4; j++)
	{
		if(*subType == UserSubTypeEnum[j][0])
		{
			*subType =  UserSubTypeEnum[j][1];
			break;
		}
	}		
	return 0;
}

static const DbTblInit TblInitArr[] =
{   
	// |TblName               |non-existent word      |Create New  Table                    |Init New Table
	{"DbBkTestTbl",		"BackupTest",			DbCreate_DbBkTestTbl,		DbInit_DbBkTestTbl},
	{"motionDetect",		"blockstatus",			DbCreate_MotionTbl,			DbInit_MotionTbl},
	{"smtpCfg",			"idx2",				DbCreate_SmtpTbl,			DbInit_SmtpTbl},
	{"logInfoTbl",			"user",				DbCreate_LogInfoTbl,			NULL},
	{"CommCfgInfo",		"transparent",			DbCreate_CommTbl,			DbInit_CommTbl},
	{"devChannelInfo",		"rtspsuburl",			DbCreate_ChannelTbl,			DbInit_ChannelTbl},
	{"ntpCfg",			"idx2",				DbCreate_NtpTbl,				DbInit_NtpTbl},
	{"ddns",				"idx1",				DbCreate_DdnsTbl,			DbInit_DdnsTbl},
	{"p2pCfgTbl",			"IosAppUrl",			DbCreate_P2pTbl,				DbInit_P2pTbl},
	{"RecordChannelTime",	"recCurSegNum",		DbCreate_RecTimeTbl,			DbInit_RecTimeTbl},
	{"ImgAttrTbl",			"idx",				DbCreate_ImgAttrTbl,			DbInit_ImgAttrTbl},
	{"ptzCruiseInfo",		"idx1",				DbCreate_CruiseTbl,			DbInit_CruiseTbl},
	{"ptzAllPreset",		"idx",				DbCreate_AllPresetTbl,		DbInit_AllPresetTbl},
	{"autoMaintain",		"idx1",				DbCreate_AutoMaintainTbl,		DbInit_AutoMaintainTbl},
	{"autoPoll",			"idx2",				DbCreate_pollTbl,				DbInit_pollTbl},
	{"ftpConf", 			"idx",				DbCreate_FtpTbl,				DbInit_FtpTbl}
};  
/* 
   数据库初始化
 */
void databaseLibInit()
{
	sqlite3 *db;
	char **dbResult; 
	int nRow = 0, nColumn = 0;
	char sql_cmd[128] = {0};
	int i = 0;
	sdk_log_item_t LogItem;
	sdk_ntp_conf_t p_ntp_conf;
	pthread_mutex_init(&databaseMutex, NULL);
	pthread_mutex_lock(&databaseMutex);
	/**/
	if(SQLITE_OK != sqlite3_open(DATABASE_FILE, &db))
	{
		sqlite3_close(db);
		pthread_mutex_unlock(&databaseMutex);
		return;
	}
	for(i = 0;i < TBL_INIT_SIZE; i++)
	{
		memset(sql_cmd,0,sizeof(sql_cmd));
		sprintf(sql_cmd,"select * from %s",TblInitArr[i].TblName);
		if(sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, NULL) != SQLITE_OK)// 表不存在
		{
			TblInitArr[i].TblCreate(db,TblInitArr[i].TblName);// 创建新的数据表
			if(TblInitArr[i].TblValueInit != NULL)
			{
				TblInitArr[i].TblValueInit(db,TblInitArr[i].TblName);  //初始化新的数据表
			}
		}
		else if(strlen(TblInitArr[i].AddField) != ADD_FIELD_EMPTY)// 表已存在，且新增的字段名不为空
		{
			memset(sql_cmd,0,sizeof(sql_cmd));
			sprintf(sql_cmd,"select %s from %s",TblInitArr[i].AddField,TblInitArr[i].TblName);// 获取某个字段
			if(sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, NULL) != SQLITE_OK)
			{
				// 字段不存在，则重新建表，将该字段添加到新表中。
				memset(sql_cmd,0,sizeof(sql_cmd));
				sprintf(sql_cmd,"drop table %s",TblInitArr[i].TblName);// 删除已存在的表
				if(sqlite3_exec(db, sql_cmd, NULL, NULL, NULL) == SQLITE_OK)
				{
					TblInitArr[i].TblCreate(db,TblInitArr[i].TblName);  // 创建新的数据表
					if(TblInitArr[i].TblValueInit != NULL)
					{
						TblInitArr[i].TblValueInit(db,TblInitArr[i].TblName);  //初始化新的数据表
					}
				}
			}
		}
		sqlite3_free_table(dbResult);
	}
	sqlite3_close(db);
	pthread_mutex_unlock(&databaseMutex);
	// 数据库备份
	Printf("Database init Backup!\n");
	DbBackup();
	
	// 插入日志---开机
	memset(&LogItem,0,sizeof(sdk_log_item_t));
	LogItem.logId = ((LOG_SYS_MANAGE << 8) | LOG_SYS_START);
	databaseOperate(SDK_MAIN_MSG_LOG,SDK_PARAM_INSERT,NULL,0,&LogItem,0);
	databaseOperate(SDK_MAIN_MSG_NTP, SDK_PARAM_GET, NULL, 0, &p_ntp_conf, sizeof(sdk_ntp_conf_t));
	// 将sn.txt中保存的大拿P2P序列号保存到数据库中。
	//P2pSnUpdate();
	//P2P控制
	//P2pInitStart();
	
}


static  DbMainCfg DbMainMsgCfgArr[] =
{      //|MainType                                            |update                                    |select                              | insert                             |delete                      |SelectFlg   
	{SDK_MAIN_MSG_MOTION_CFG,		DbUpdate_MotionCfg,		DbSelect_MotionCfg,	NULL,				NULL,			SELECT_PARA_FALSE,   	GetGlobal_MotionCfg	},
	{SDK_MAIN_MSG_PARAM_DEFAULT,	DbUpdate_DefaultCfg,		NULL,				NULL,				NULL,			SELECT_PARA_FALSE,   	NULL				},
	{SDK_MAIN_MSG_SYS_CFG,			NULL,					DbSelect_SysCfg,		NULL,				NULL,			SELECT_PARA_FALSE,   	GetGlobal_SysCfg		},
	{SDK_MAIN_MSG_CHANN_CFG,		DbUpdate_ChannCfg,		DbSelect_ChannCfg,	NULL,				NULL,			SELECT_PARA_FALSE,   	GetGlobal_ChannCfg	},
	{SDK_MAIN_MSG_RECORD_CFG,		DbUpdate_RecordCfg,		DbSelect_RecordCfg,	NULL,				NULL,			SELECT_PARA_FALSE,   	GetGlobal_RecordCfg	},
	{SDK_MAIN_MSG_PREVIEW_CFG,		DbUpdate_PreviewCfg,	DbSelect_PreviewCfg,	NULL,				NULL,			SELECT_PARA_FALSE,   	GetGlobal_PreviewCfg	},
	{SDK_MAIN_MSG_COMM_CFG,		DbUpdate_CommCfg,		DbSelect_CommCfg,	NULL,				NULL,			SELECT_PARA_FALSE,    GetGlobal_CommCfg	},
	{SDK_MAIN_MSG_NET_LINK_CFG,	DbUpdate_NetLinkCfg,	DbSelect_NetLinkCfg,	NULL,				NULL,			SELECT_PARA_FALSE,   	GetGlobal_NetLinkCfg	},
	{SDK_MAIN_MSG_NET_MNG_CFG,		DbUpdate_NetMngCfg,	DbSelect_NetMngCfg,	NULL,				NULL,			SELECT_PARA_FALSE,   	GetGlobal_NetMngCfg	},
	{SDK_MAIN_MSG_CRUISECFG,		DbUpdate_CruiseCfg,		DbSelect_CruiseCfg,	NULL,				NULL,			SELECT_PARA_FALSE,   	GetGlobal_CruiseCfg	},
	{SDK_MAIN_MSG_USER,			DbUpdate_UserCfg,		DbSelect_UserCfg,	DbInsert_UserCfg,	DbDelete_UserCfg,SELECT_PARA_FALSE, 	GetGlobal_UserCfg	},
	{SDK_MAIN_MSG_DDNS_CFG,		DbUpdate_DdnsCfg,		DbSelect_DdnsCfg,	NULL,				NULL,			SELECT_PARA_FALSE,   	GetGlobal_DdnsCfg	},
	{SDK_MAIN_MSG_NTP,				DbUpdate_NtpCfg,		DbSelect_NtpCfg,		NULL,				NULL,			SELECT_PARA_FALSE,	GetGlobal_NtpCfg		},
	{SDK_MAIN_MSG_LOG,				NULL,					DbSelect_LogCfg,		DbInsert_LogCfg,		DbDelete_LogCfg,SELECT_PARA_FALSE,	NULL				},
	{SDK_MAIN_MSG_SMTP,			DbUpdate_SmtpCfg,		DbSelect_SmtpCfg,	NULL,				NULL,			SELECT_PARA_FALSE,	GetGlobal_SmtpCfg	},
	{SDK_MAIN_MSG_P2P,				DbUpdate_P2PCfg,		DbSelect_P2PCfg,		NULL,				NULL,			SELECT_PARA_FALSE,	GetGlobal_P2PCfg	},
	{SDK_MAIN_MSG_LOCAT_IMAGE_ATTR_CFG,DbUpdate_ImgAttrCfg,DbSelect_ImgAttrCfg,NULL,				NULL,			SELECT_PARA_FALSE,	GetGlobal_ImgAttrCfg	},
	{SDK_MAIN_MSG_ALLPRESETS, 		DbUpdate_AllPresetCfg,	DbSelect_AllPresetCfg,NULL,				NULL,			SELECT_PARA_FALSE,	GetGlobal_AllPresetCfg},
	{SDK_MAIN_MSG_AUTO_MAINTAIN, 		DbUpdate_AutoMaintainCfg,	DbSelect_AutoMaintainCfg,NULL,				NULL,			SELECT_PARA_FALSE,	GetGlobal_AutoMaintainCfg},
	{SDK_MAIN_MSG_POLL, 		DbUpdate_pollCfg,	DbSelect_pollCfg,NULL,				NULL,			SELECT_PARA_FALSE,	GetGlobal_pollCfg},
	{SDK_MAIN_MSG_FTP, 				DbUpdate_FtpCfg,			DbSelect_FtpCfg,			NULL,				NULL,	SELECT_PARA_FALSE,	GetGlobal_FtpCfg}
};
/*
   函数功能:数据库操作
   说明:调用此函数时，必须要传入subType的枚举值。

 */
int databaseOperate(int type, int subType, void *data, int len, void *pRecvData, int recvDataLen)
{
	int ret = 0,i =0;
	sqlite3 *db;
	/* *************************************************   */
	//此处是 为了兼容_SDK_USER_OP的枚举值
	if(type == SDK_MAIN_MSG_USER)
	{
		UerEnumExch(&subType);
	}
	/* *************************************************   */

	if(pthread_mutex_lock(&databaseMutex) != 0)
	{
		return -1;
	}
	if(subType != SDK_PARAM_GET)
	{
		//Printf("**********SDK_PARAM_GET:%d**************\n\n",subType);
		if(SQLITE_OK != sqlite3_open(DATABASE_FILE, &db))
		{
			sqlite3_close(db);
			pthread_mutex_unlock(&databaseMutex);
			return -1;
		}
		sqlite3_exec(db, "begin transaction", NULL, NULL, NULL);
	}

	// 查表处理
	for(i = 0;i < TBL_CFG_SIZE; i++)
	{
		if(type == DbMainMsgCfgArr[i].MainType)
		{
			switch(subType)
			{
				case SDK_PARAM_SET:
				{
					if(DbMainMsgCfgArr[i].DbMainCfgUpdate != NULL)
					{
						ret = DbMainMsgCfgArr[i].DbMainCfgUpdate(data, len, pRecvData, recvDataLen,db);
					}
				}
				break;
				case SDK_PARAM_GET:
				{
					if((DbMainMsgCfgArr[i].SelectFlg == SELECT_PARA_FALSE) \
						||(DbMainMsgCfgArr[i].MainType == SDK_MAIN_MSG_LOG)\
						||(DbMainMsgCfgArr[i].MainType == SDK_MAIN_MSG_DDNS_CFG))
					{
						//Printf("11111111111111   DbMainCfgSelect:    %d111111111111111\n\n",type);
						DbMainMsgCfgArr[i].SelectFlg = SELECT_PARA_TURE;
						if(SQLITE_OK != sqlite3_open(DATABASE_FILE, &db))
						{
							sqlite3_close(db);
							pthread_mutex_unlock(&databaseMutex);
							return -1;
						}
						
						sqlite3_exec(db, "begin transaction", NULL, NULL, NULL);
						if(DbMainMsgCfgArr[i].DbMainCfgSelect != NULL)
						{
							ret = DbMainMsgCfgArr[i].DbMainCfgSelect(data, len, pRecvData, recvDataLen, db);	
						}	
						
						sqlite3_exec(db, "commit transaction", NULL, NULL, NULL);
						if(ret == -1)
						{
							sqlite3_exec(db, "rollback transaction", NULL, NULL, NULL);
						}
						sqlite3_close(db);
					}
					else
					{
						//Printf("2222222222   DbGetGlobalPara:      %d     %d22222222222222\n\n",type,DbMainMsgCfgArr[i].SelectFlg);
						if(DbMainMsgCfgArr[i].DbMainCfgSelect != NULL)
						{
							ret = DbMainMsgCfgArr[i].DbGetGlobalPara(data, len, pRecvData, recvDataLen);	
						}
					}
				}
				break;
				case SDK_PARAM_INSERT:
				{
					if(DbMainMsgCfgArr[i].DbMainCfgInsert != NULL)
					{
						ret = DbMainMsgCfgArr[i].DbMainCfgInsert(data, len, pRecvData, recvDataLen, db);
					}
				}
				break;
				case SDK_PARAM_DELETE:
				{
					if(DbMainMsgCfgArr[i].DbMainCfgDelete != NULL)
					{
						ret = DbMainMsgCfgArr[i].DbMainCfgDelete(data, len, pRecvData, recvDataLen, db);
					}
				}
				break;
				default:
				break;
			}
			break;// 退出for循环
		}
	}
	if(subType != SDK_PARAM_GET)
	{
		//Printf("=========SDK_PARAM_GET============:%d\n\n",subType);
		sqlite3_exec(db, "commit transaction", NULL, NULL, NULL);
		if(ret == -1)
		{
			sqlite3_exec(db, "rollback transaction", NULL, NULL, NULL);
		}
		sqlite3_close(db);
	}
	pthread_mutex_unlock(&databaseMutex);
	return ret;
}
