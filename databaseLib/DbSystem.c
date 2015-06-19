/**********************************************************
 * databaseLib.c

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

#include "DbSystem.h"
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
#include "ntp.h"
#include "midWare.h"
#define DEFAULT_IP		       "192.168.1.88"
#define DEFAULT_MASK		"255.255.255.0"
#define DEFAULT_GW		       "192.168.1.1"
#define DEFAULT_MAC		"44:33:3A:33:44:3A"
#define DEFAULT_DNS1		"202.96.134.133"
#define DEFAULT_DNS2		"202.96.128.86"

#define DEFAULT_ETH0_NO		       0
#define DEFAULT_MULTICAST_ADDR	"0.0.0.0"
#define DEFAULT_SERVER_ADDR	       "0.0.0.0"
#define DEFAULT_PLAYBACK_PORT	       5000
#define DEFAULT_RTSP_PORT	              554
#define DEFAULT_HTTP_PORT	              80

#define TIME_ZONE_EAST8    40     //TIME_ZONE_EAST8是东8时区，即北京时间
#define  TIME_ZONE_ZERO     24     //0时区  及英国格林威治时间
#define NTP_TIME_PORT	  123	//ntp较时默认端口号
#define MAX_BUZZER_TIME   255   // 蜂鸣器鸣响最长时间
#define MAX_RECORD_TIME   255  // 移动侦测录像最长时间




static sdk_motion_cfg_v2_t g_DbMotionCfg[MAX_DISPLAY_NUM];
static sdk_channel_t g_DbChanCfg[MAX_DISPLAY_NUM];
static sdk_record_cfg_t g_DbRecCfg[MAX_DISPLAY_NUM];
static sdk_preview_t	g_DbPreviewCfg;
static sdk_comm_cfg_t g_DbCommCfg;
static sdk_ntp_conf_t g_DbNtpCfg;
static sdk_eth_cfg_t g_DbEthLinkCfg;
static sdk_net_mng_cfg_t g_DbNetMngCfg;
static sdk_email_conf_t   g_DbEmailCfg;
static sdk_p2p_conf_t g_DbP2pCfg[MAX_P2P_TYPE];
static sdk_image_attr_t g_DbImgAttrCfg;
static sdk_autoMaintain_attr_t g_DbAutoMaintainAttrCfg;
static sdk_ddns_cfg_t g_DbDdnsCfg[MAX_DDNS_TYPE];
static sdk_user_right_t	g_DbUsrCfg[MAX_USER_NUM];
static int g_DbUsrTatolNum = 0;
static sdk_sys_cfg_t g_DbSysCfg;
static sdk_preset_all_t g_DbAllPreset[MAX_DISPLAY_NUM];
static sdk_cruise_param_t g_DbCruiseCfg[MAX_DISPLAY_NUM][MAX_CRUISE_LINE_NUM];
static sdk_venc_t   g_DbVencCfg[MAX_DISPLAY_NUM];
static int GetIpResult = 0;
static sdk_poll_t g_pollCfg;
sdk_ftp_config g_DbFtpCfg;

/******************************************
函数功能:日志配置--插入日志


******************************************/
int DbInsert_LogCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[1024] = {0,};
	char st[32] = {0,};
	int result = 0;
	char **dbResult; 
	int nRow = 0, nColumn = 0;
	char *err_msg = NULL;
	unsigned int first_id = 0,last_id = 0;
	struct tm *ptm;
	time_t tnow;
	sdk_log_item_t *pItem = NULL;	
	pItem = (sdk_log_item_t *)pRecvData;
	sprintf(sql_cmd, "select * from logInfoTbl");
	result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
	if(SQLITE_OK != result)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		sqlite3_free_table(dbResult);
		return DB_OPR_ERROR;
	}
	if(nRow >= 1)
	{
		last_id = atoi(dbResult[(nRow)*nColumn]);
	}
	else
	{
		last_id = -1;
	}
	if(nRow >= MAX_LOG_ITEM)
	{
		first_id = atoi(dbResult[nColumn]);
		sprintf(sql_cmd,"delete from logInfoTbl where idx=%d",first_id);
		if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			sqlite3_free_table(dbResult);
			ret = DB_OPR_ERROR;
			return ret;
		}
	}
	
	tnow = time(NULL);	
	ptm = localtime(&tnow);
	sprintf(st,"%04d-%02d-%02d-%02d-%02d-%02d",ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	sprintf(sql_cmd,"INSERT INTO logInfoTbl VALUES (%d,%d,\'%s\',\'%s\');",last_id+1,pItem->logId,st,pItem->user);
	if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		sqlite3_free_table(dbResult);
		ret = DB_OPR_ERROR;
		return ret;
	}
	sqlite3_free_table(dbResult);
	return ret;
}
/******************************************
函数功能:日志配置--查询日志


******************************************/
int DbSelect_LogCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0, i = 0;
	int num = 0;
	int pReqType = len;
	static int LogReqidx = 0; 
	static int LogType= 0;
	char sql_cmd[1024] = {0,};
	int result = 0;
	char **dbResult; 
	int nRow = 0, nColumn = 0;
	char *err_msg = NULL;
	uint16_t LogID = 0;
	sdk_log_cond_t *pCond = NULL;
	static char timebuf_low[32] = {0}, timebuf_high[32] = {0};
	sdk_log_item_t *pItem = NULL;
	sprintf(sql_cmd, "select * from logInfoTbl order by idx desc"); // 倒序排列
	result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
	if(SQLITE_OK != result)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		sqlite3_free_table(dbResult);
		return DB_OPR_ERROR;
	}
	Printf("pReqType =%d\n",pReqType);
	if(pReqType == SDK_LOG_QUERY_NUM)//先获取日志的总数
	{
		
		pCond = (sdk_log_cond_t*)pRecvData;
		LogType  = pCond->type;
		memset(timebuf_low,0,sizeof(timebuf_low));
		memset(timebuf_high,0,sizeof(timebuf_high));
		// 起始时间
		sprintf(timebuf_low, "%04d-%02d-%02d-%02d-%02d-%02d", 
				pCond->begin_time.year, pCond->begin_time.mon, 	pCond->begin_time.day, pCond->begin_time.hour, pCond->begin_time.min, pCond->begin_time.sec);		
              // 结束时间
		sprintf(timebuf_high, "%04d-%02d-%02d-%02d-%02d-%02d", 
				pCond->end_time.year, pCond->end_time.mon, pCond->end_time.day, pCond->end_time.hour, pCond->end_time.min, pCond->end_time.sec);
		for(i=0;i<nRow;i++)
		{
		       // 查找数据库日志列表
		       // 先判断时间段，再判断日志类型
			if (( strncmp(  dbResult[(i+1)*nColumn+2],timebuf_low, 19 ) > 0  ) && ( strncmp( dbResult[(i+1)*nColumn+2],timebuf_high , 19 ) < 0  ))//logInfo2数据库表单的第三列:time
			{
				LogID = atoi(dbResult[(i+1)*nColumn+1]);//logInfo2数据库表单的第二列:type
				if(pCond->type == 0)// 全部日志
				{
					num++;// 总的日志信息条数	
				}
				else if(pCond->type == ((LogID & 0xFF00) >> 8)) 
				{
					num++;// 总的日志信息条数	
				}
				
			}	
			
		}
		ret = num;
	}
	else if (pReqType == SDK_LOG_QUERY_CONTENT)// 获取日志的内容
	{
		pItem = (sdk_log_item_t *)pRecvData;
		for(i=LogReqidx;i<nRow;i++)
		{
		       // 查找数据库日志列表
		       // 先判断时间段，再判断日志类型
		       LogReqidx ++;
			if (( strncmp( dbResult[(i+1)*nColumn+2],timebuf_low, 19 ) > 0  ) && ( strncmp( dbResult[(i+1)*nColumn+2],timebuf_high , 19 ) < 0  ))//logInfo2数据库表单的第三列:time
			{
				LogID = atoi(dbResult[(i+1)*nColumn+1]);// type
				if(LogType == 0)// 全部日志
				{
					
					pItem->logId = LogID;// 日志ID
					sscanf(dbResult[(i+1)*nColumn+2], "%04d-%02d-%02d-%02d-%02d-%02d", &pItem->time.year,&pItem->time.mon, &pItem->time.day, 
						&pItem->time.hour, &pItem->time.min, &pItem->time.sec);// 时间
						
					memset(pItem->user,0,MAX_USER_NAME_LEN);
					strncpy((char *)pItem->user, dbResult[ (i+1)*nColumn + 3], strlen(dbResult[ (i+1)*nColumn + 3]));// 用户名
					pItem++;// 下一条日志信息
					num++;
				}
				else if(LogType == ((LogID & 0xFF00) >> 8)) 
				{							
					pItem->logId = LogID;// 日志ID
					sscanf(dbResult[(i+1)*nColumn+2], "%04d-%02d-%02d-%02d-%02d-%02d", &pItem->time.year,&pItem->time.mon, &pItem->time.day, 
						&pItem->time.hour, &pItem->time.min, &pItem->time.sec);// 时间

					memset(pItem->user,0,MAX_USER_NAME_LEN);
					strncpy((char *)pItem->user, dbResult[ (i+1)*nColumn + 3], strlen(dbResult[ (i+1)*nColumn + 3]));// 用户名
					pItem++;// 下一条日志信息
					num++;
				}
				if(num >= 60)// 如果总的日志超过60条，就分多次查询，且每次只能查60条。
				{
					sqlite3_free_table(dbResult);
					return num * sizeof(sdk_log_item_t);
				}
			}
			
		}
		LogReqidx = 0;
		sqlite3_free_table(dbResult);
		return num * sizeof(sdk_log_item_t);
	}		
	sqlite3_free_table(dbResult);
	return ret;
}
/******************************************
函数功能:日志配置--删除日志


******************************************/
int DbDelete_LogCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0, i = 0, idx = 0;
	char sql_cmd[512] = {0,};
	int result = 0;
	char **dbResult; 
	int nRow = 0, nColumn = 0;
	char *err_msg = NULL;
	
	sprintf(sql_cmd, "select * from logInfoTbl");
	result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
	if(SQLITE_OK != result)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		sqlite3_free_table(dbResult);
		return DB_OPR_ERROR;
	}
	for(i=0;i<nRow;i++)
	{
		idx = atoi(dbResult[(i+1)*nColumn]);//logInfo2数据库表单的第一列:idx
		sprintf(sql_cmd,"delete from logInfoTbl where idx=%d",idx);
		if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			sqlite3_free_table(dbResult);
			return DB_OPR_ERROR;
		}
	}
	sqlite3_free_table(dbResult);
	return ret;
}

/******************************************
函数功能:移动侦测配置--更新


******************************************/
int DbUpdate_MotionCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{ 
	int i = 0, j = 0;
	int ret = 0;
	char sql_cmd[512] = {0};
	char BlockArr[750] = {0};// 64个IntString再加上分隔符后字符串的长度64 * (8 + 1)，外加一些冗余
	char IntString[8] = {0};  // 22个1二进制的数据(1111111111111111111111)转换成整型(4194303),将4194303转换成字符串后字符串的长度
	char *err_msg = NULL;
	sdk_motion_cfg_v2_t *p_md_cfg = (sdk_motion_cfg_v2_t *)pRecvData;
	int channel = *(int *)data;
	stream_alarm_t alarmInfo;
	
	if(channel == CHANN_TYPE_ALL)
	{	
		for(j = 0; (j < MAX_DISPLAY_NUM) && (j < configGetDisplayNum()); j++)
		{
			memset(sql_cmd,0,sizeof(sql_cmd));
			memset(BlockArr,0,sizeof(BlockArr));
			for(i = 0;i < MAX_BLOCK_ROW;i++)
			{
				// 将blockstatus[MAX_BLOCK_ROW]数组内的数据保存为字符串
				// 字符串中每个数据之间使用分号;隔开
				memset(IntString,0,sizeof(IntString));
				sprintf(IntString,"%d",p_md_cfg->blockstatus[i]);
				strcat(BlockArr,IntString);    
				strcat(BlockArr,";");
			}
			if((p_md_cfg->beep_time > MAX_BUZZER_TIME)||(p_md_cfg->record_time > MAX_RECORD_TIME))
			{
				p_md_cfg->beep_time = MAX_BUZZER_TIME;
				p_md_cfg->record_time = MAX_RECORD_TIME;
			}
			sprintf(sql_cmd,"update motionDetect set motionEnable=%d, beepEnable=%d, beepTime=%d, recordEnable=%d, recordTime=%d, smtpEnable=%d, ftpEnable=%d, sensitivity=%d,blockstatus='%s' where channel=%d",
					p_md_cfg->enable, p_md_cfg->beep_enable, p_md_cfg->beep_time, p_md_cfg->record_enable, p_md_cfg->record_time,
					p_md_cfg->smtp_enable, p_md_cfg->ftp_enable, p_md_cfg->sensitivity, BlockArr,j);
			if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
			{
				Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
				ret = DB_OPR_ERROR;
			}
			else
			{
				memset(&g_DbMotionCfg[j], 0 ,sizeof(sdk_motion_cfg_v2_t));
				memcpy(&g_DbMotionCfg[j], p_md_cfg,sizeof(sdk_motion_cfg_v2_t));
				if(g_DbMotionCfg[j].enable ==0)
				{
					memset(&alarmInfo, 0, sizeof(stream_alarm_t));
					alarmInfo.type = SDK_EVENT_MOTION;
					alarmInfo.channelNo = j;
					alarmInfo.streamNo = 0;
					alarmInfo.status = 0;
					streamInsertAlarmToAlarmQueue(NULL, &alarmInfo);
				}
				motionSetUpdateFlag(j,1);
				recordSetUpdateFlag(MOTION_UPDATE_FLAG_PARAM, 0, 0);
			}
		}
	}
	else
	{
		if(!memcmp(p_md_cfg, &g_DbMotionCfg[channel],sizeof(sdk_motion_cfg_v2_t)))
		{
			//Printf("motion Para no change!\n");
			return sizeof(sdk_motion_cfg_v2_t);
		}
		//Printf("motion Para change!\n");
		
		memset(sql_cmd,0,sizeof(sql_cmd));
		for(i = 0;i < MAX_BLOCK_ROW;i++)
		{
			memset(IntString,0,sizeof(IntString));
			sprintf(IntString,"%d",p_md_cfg->blockstatus[i]);
			strcat(BlockArr,IntString);    
			strcat(BlockArr,";");
		}
		sprintf(sql_cmd,"update motionDetect set motionEnable=%d, beepEnable=%d, beepTime=%d, recordEnable=%d, recordTime=%d, smtpEnable=%d, ftpEnable=%d, sensitivity=%d,blockstatus='%s' where channel=%d",
					p_md_cfg->enable, p_md_cfg->beep_enable, p_md_cfg->beep_time, p_md_cfg->record_enable, p_md_cfg->record_time,
					p_md_cfg->smtp_enable, p_md_cfg->ftp_enable, p_md_cfg->sensitivity, BlockArr,channel);
		if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);				
			ret = DB_OPR_ERROR;
		}
		else
		{
			memset(&g_DbMotionCfg[channel], 0 ,sizeof(sdk_motion_cfg_v2_t));
			memcpy(&g_DbMotionCfg[channel], p_md_cfg,sizeof(sdk_motion_cfg_v2_t));
			if(g_DbMotionCfg[channel].enable ==0)
			{
				memset(&alarmInfo, 0, sizeof(stream_alarm_t));
				alarmInfo.type = SDK_EVENT_MOTION;
				alarmInfo.channelNo = channel;
				alarmInfo.streamNo = 0;
				alarmInfo.status = 0;
				streamInsertAlarmToAlarmQueue(NULL, &alarmInfo);
			}
			motionSetUpdateFlag(channel,1);
			recordSetUpdateFlag(MOTION_UPDATE_FLAG_PARAM, 0, 0);
		}
	}
	return ret;
}
/******************************************

函数功能: 编码参数配置--查询全局变量
*******************************************/
int GetGlobal_VencCfg(void *data, int len, void *pRecvData, int recvDataLen)
{
	int ret = 0;
	sdk_venc_t*p_venc_cfg = (sdk_venc_t *)pRecvData;
	int channel = *(int *)data;
	//Printf("Get   md from global para!:%d\n",channel);
	memcpy(p_venc_cfg, &g_DbVencCfg[channel], sizeof(sdk_venc_t));
	ret = sizeof(sdk_venc_t);
	return ret;
}

int DbSelect_VencCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int i = 0,k = 0;
	int ret = 0;
	char sql_cmd[512] = {0};
	char BlockArr[750] = {0};// 64个IntString再加上分隔符后字符串的长度64 * (8 + 1)，外加一些冗余
	int result = 0;
	char *err_msg = NULL;
	char *p;
	char **dbResult; 
	int nRow = 0, nColumn = 0;
	sdk_venc_t *p_md_cfg = (sdk_venc_t *)pRecvData;
	int channel = *(int *)data;
	//Printf("Get  md from database! \n");
	memset(sql_cmd,0,sizeof(sql_cmd));
	sprintf(sql_cmd, "select * from motionDetect");
	result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
	if(SQLITE_OK != result)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		sqlite3_free_table(dbResult);
		return DB_OPR_ERROR;
	}
	for(i = 0; (i<MAX_DISPLAY_NUM)&&(i < configGetDisplayNum()); i++)
	{
		
		if((nRow==0) ||(nColumn ==0))
		{
			memset(sql_cmd,0,sizeof(sql_cmd));
			sprintf(sql_cmd, "drop table vencInfo");
			result = sqlite3_get_table(db, sql_cmd, &dbResult, NULL, NULL, &err_msg);
			if(SQLITE_OK != result)
			{
				Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
				sqlite3_free_table(dbResult);
				return DB_OPR_ERROR;
			}
			sqlite3_free_table(dbResult);
			return DB_OPR_ERROR;
		}
		memset(&g_DbVencCfg[i], 0, sizeof(sdk_venc_t));
		g_DbVencCfg[i].channelNo= atoi(dbResult[(i + 1) * nColumn + 1]);
		g_DbVencCfg[i].encodeType= atoi(dbResult[(i + 1) *  nColumn + 2]);
		g_DbVencCfg[i].mainResolution= atoi(dbResult[(i + 1) *  nColumn + 3]);
		g_DbVencCfg[i].mainEncodeMode= atoi(dbResult[(i + 1) *  nColumn + 4]);
		g_DbVencCfg[i].mainFrameRate= atoi(dbResult[(i + 1) *  nColumn + 5]);
		g_DbVencCfg[i].mainBiteRate= atoi(dbResult[(i + 1) *  nColumn + 6]);
		g_DbVencCfg[i].subResolution= atoi(dbResult[(i + 1) *  nColumn + 7]);
		g_DbVencCfg[i].subEncodeMode= atoi(dbResult[(i + 1) *  nColumn+8]);
		g_DbVencCfg[i].subBitRate= atoi(dbResult[(i + 1) *  nColumn+9]);
		g_DbVencCfg[i].subFrameRate= atoi(dbResult[(i + 1) *  nColumn+10]);
	}
	sqlite3_free_table(dbResult);
	memcpy(p_md_cfg, &g_DbVencCfg[channel], sizeof(sdk_venc_t));
	ret = sizeof(sdk_venc_t);
	return ret;
}

/******************************************
函数功能:移动侦测配置--查询全局变量


******************************************/
int GetGlobal_MotionCfg(void *data, int len, void *pRecvData, int recvDataLen)
{
	int ret = 0;
	sdk_motion_cfg_v2_t *p_md_cfg = (sdk_motion_cfg_v2_t *)pRecvData;
	int channel = *(int *)data;
	//Printf("Get   md from global para!:%d\n",channel);
	memcpy(p_md_cfg, &g_DbMotionCfg[channel], sizeof(sdk_motion_cfg_v2_t));
	ret = sizeof(sdk_motion_cfg_v2_t);
	return ret;
}
/******************************************
函数功能:移动侦测配置--查询数据库


******************************************/
int DbSelect_MotionCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int i = 0,k = 0;
	int ret = 0;
	char sql_cmd[512] = {0};
	char BlockArr[750] = {0};// 64个IntString再加上分隔符后字符串的长度64 * (8 + 1)，外加一些冗余
	int result = 0;
	char *err_msg = NULL;
	char *p;
	char **dbResult; 
	int nRow = 0, nColumn = 0;
	sdk_motion_cfg_v2_t *p_md_cfg = (sdk_motion_cfg_v2_t *)pRecvData;
	int channel = *(int *)data;
	//Printf("Get  md from database! \n");
	memset(sql_cmd,0,sizeof(sql_cmd));
	sprintf(sql_cmd, "select * from motionDetect");
	result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
	if(SQLITE_OK != result)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		sqlite3_free_table(dbResult);
		return DB_OPR_ERROR;
	}
	for(i = 0; (i<MAX_DISPLAY_NUM)&&(i < configGetDisplayNum()); i++)
	{
		
		if((nRow==0) ||(nColumn ==0))
		{
			memset(sql_cmd,0,sizeof(sql_cmd));
			sprintf(sql_cmd, "drop table motionDetect");
			result = sqlite3_get_table(db, sql_cmd, &dbResult, NULL, NULL, &err_msg);
			if(SQLITE_OK != result)
			{
				Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
				sqlite3_free_table(dbResult);
				return DB_OPR_ERROR;
			}
			sqlite3_free_table(dbResult);
			return DB_OPR_ERROR;
		}
		memset(&g_DbMotionCfg[i], 0, sizeof(sdk_motion_cfg_v2_t));
		g_DbMotionCfg[i].enable = atoi(dbResult[(i + 1) * nColumn + 1]);
		g_DbMotionCfg[i].beep_enable = atoi(dbResult[(i + 1) *  nColumn + 2]);
		g_DbMotionCfg[i].beep_time = atoi(dbResult[(i + 1) *  nColumn + 3]);
		g_DbMotionCfg[i].record_enable = atoi(dbResult[(i + 1) *  nColumn + 4]);
		g_DbMotionCfg[i].record_time = atoi(dbResult[(i + 1) *  nColumn + 5]);
		g_DbMotionCfg[i].smtp_enable = atoi(dbResult[(i + 1) *  nColumn + 6]);
		g_DbMotionCfg[i].ftp_enable = atoi(dbResult[(i + 1) *  nColumn + 7]);
		g_DbMotionCfg[i].sensitivity = atoi(dbResult[(i + 1) *  nColumn+8]);	
		k = 0;
		if(strlen(dbResult[(i + 1) *  nColumn+9]) != 0)
		{
			//将字符串提取转换为整型数据，并保存在blockstatus[MAX_BLOCK_ROW]中
			strcpy(BlockArr,dbResult[(i + 1) *  nColumn+9]);
			p = strtok(BlockArr,";");
			while(p != NULL)
			{
				g_DbMotionCfg[i].blockstatus[k++] = atoi(p);
				p = strtok(NULL,";");
			}
		}
		
	}
	sqlite3_free_table(dbResult);
	memcpy(p_md_cfg, &g_DbMotionCfg[channel], sizeof(sdk_motion_cfg_v2_t));
	ret = sizeof(sdk_motion_cfg_v2_t);
	return ret;
}
/******************************************
函数功能:移动侦测配置恢复默认


******************************************/

int DbUpdate_DefaultMotion(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int j = 0;
	int ret = 0;
	char sql_cmd[512] = {0,};
	char *err_msg = NULL;
	int channel = *(int *)data;
	stream_alarm_t alarmInfo;
	if(channel == CHANN_TYPE_ALL)
	{
		for(j = 0; (j < MAX_DISPLAY_NUM)&&(j<configGetDisplayNum()); j++)
		{
			memset(sql_cmd,0,sizeof(sql_cmd));
			sprintf(sql_cmd," update motionDetect set motionEnable=0,beepEnable=0,beepTime=1,recordEnable=0,\
				recordTime=60,smtpEnable=0,ftpEnable=0,sensitivity=50,blockstatus=' ' where channel=%d",j);
			if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
			{
				Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
				ret = DB_OPR_ERROR;
			}
			g_DbMotionCfg[j].enable = 0;
			g_DbMotionCfg[j].beep_enable = 0;
			g_DbMotionCfg[j].beep_time = 1;
			g_DbMotionCfg[j].record_enable= 0;
			g_DbMotionCfg[j].record_time= 60;
			g_DbMotionCfg[j].smtp_enable= 0;
			g_DbMotionCfg[j].ftp_enable= 0;
			g_DbMotionCfg[j].sensitivity= 50;
			memset(&g_DbMotionCfg[j].blockstatus,0,sizeof(MAX_BLOCK_ROW));
			memset(&alarmInfo, 0, sizeof(stream_alarm_t));
			alarmInfo.type = SDK_EVENT_MOTION;
			alarmInfo.channelNo = j;
			alarmInfo.streamNo = 0;
			alarmInfo.status = 0;
			streamInsertAlarmToAlarmQueue(NULL, &alarmInfo);
		}
	}
	else
	{
		if(channel <0 || ((channel >= MAX_DISPLAY_NUM)&&(channel >=configGetDisplayNum())))
		{
			return -1;
		}
		memset(sql_cmd,0,sizeof(sql_cmd));
		sprintf(sql_cmd," update motionDetect set motionEnable=0,beepEnable=0,beepTime=1,recordEnable=0,recordTime=15,smtpEnable=0,ftpEnable=0,sensitivity=80,blockstatus=' ' where channel=%d",channel);
		if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			ret = DB_OPR_ERROR;
		}
		g_DbMotionCfg[channel].enable = 0;
		g_DbMotionCfg[channel].beep_enable = 0;
		g_DbMotionCfg[channel].beep_time = 1;
		g_DbMotionCfg[channel].record_enable= 0;
		g_DbMotionCfg[channel].record_time= 60;
		g_DbMotionCfg[channel].smtp_enable= 0;
		g_DbMotionCfg[channel].ftp_enable= 0;
		g_DbMotionCfg[channel].sensitivity= 50;
		memset(&g_DbMotionCfg[channel].blockstatus,0,sizeof(MAX_BLOCK_ROW));
		memset(&alarmInfo, 0, sizeof(stream_alarm_t));
		alarmInfo.type = SDK_EVENT_MOTION;
		alarmInfo.channelNo = channel;
		alarmInfo.streamNo = 0;
		alarmInfo.status = 0;
		streamInsertAlarmToAlarmQueue(NULL, &alarmInfo);
	}
	recordSetUpdateFlag(RECORD_UPDATE_FLAG_PARAM, 0, RECORD_UPDATE_FLAG_PARAM);
	recordSetUpdateFlag(MOTION_UPDATE_FLAG_PARAM, 0, 0);
	return ret;
}
/******************************************
函数功能:系统配置--查询全局变量


******************************************/
int GetGlobal_SysCfg(void *data, int len, void *pRecvData, int recvDataLen)
{
	int ret = 0;
	sdk_sys_cfg_t *p_sys_cfg = (sdk_sys_cfg_t *)pRecvData;
	//Printf("Get sys  from global para!\n");
	memcpy(p_sys_cfg, &g_DbSysCfg,sizeof(sdk_sys_cfg_t));
	ret = sizeof(sdk_sys_cfg_t);
	return ret;
}
/******************************************
函数功能:系统配置--查询


******************************************/
int DbSelect_SysCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0};
	int result = 0;
	char **dbResult;
	char *err_msg = NULL;
	int nRow = 0, nColumn = 0;//, nRow2;
	sdk_sys_cfg_t *p_sys_cfg = (sdk_sys_cfg_t *)pRecvData;	
	//Printf("Get sys value from  database !\n");
	sprintf(sql_cmd, "select * from SystemInfo");
	result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
	if(SQLITE_OK != result)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		sqlite3_free_table(dbResult);
		return DB_OPR_ERROR;
	}
	p_sys_cfg->dev_type = atoi(dbResult[nColumn + 0]);
	p_sys_cfg->ip_chan_num = configGetDisplayNum() > MAX_DISPLAY_NUM ? MAX_DISPLAY_NUM : configGetDisplayNum();
	p_sys_cfg->alarm_in_num = configGetAlarmInNum();
	p_sys_cfg->alarm_out_num = configGetAlarmOutNum();
	sqlite3_free_table(dbResult);
	ret = sizeof(sdk_sys_cfg_t);
	memcpy(&g_DbSysCfg, p_sys_cfg, sizeof(sdk_sys_cfg_t));
	return ret;
}

/******************************************
函数功能:通道配置---更新


******************************************/
int DbUpdate_ChannCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{ 
	int  j = 0;
	int ret = 0;
	char sql_cmd[512] = {0};
	char *err_msg = NULL;
	sdk_channel_t *p_ch_cfg = (sdk_channel_t *)pRecvData;
	int channel = *(int *)data;
	stream_alarm_t alarmInfo;
	
	if(channel == CHANN_TYPE_ALL)
	{
		for(j = 0; (j < MAX_DISPLAY_NUM) && (j < configGetDisplayNum()); j++)
		{
			sprintf(sql_cmd,"update devChannelInfo  set enable=%d, channelName='%s', streamNo=%d, addr='%s', port=%d, \
						   userName='%s', userPwd='%s', protocol=%d,manProtocolType=%d,rtspmainurl='%s',rtspsuburl='%s' where channelNo=%d",
					p_ch_cfg->enable, p_ch_cfg->name, p_ch_cfg->chann_win_mode, p_ch_cfg->ip_addr, p_ch_cfg->port,
					p_ch_cfg->user.user_name, p_ch_cfg->user.user_pwd, p_ch_cfg->chann_protocol,p_ch_cfg->manufacturer
					,p_ch_cfg->rtsp_main_url,p_ch_cfg->rtsp_sub_url, j);
			if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
			{
				Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
				ret = DB_OPR_ERROR;
			}
			else
			{
				p_ch_cfg->chann_no = j;
				p_ch_cfg->chann_win_mode = p_ch_cfg->chann_win_mode;
				appStartDecodeChannel(p_ch_cfg);
				memcpy(&g_DbChanCfg[j],p_ch_cfg,sizeof(sdk_channel_t));
				if(g_DbChanCfg[j].enable ==0)
				{
					memset(&alarmInfo, 0, sizeof(stream_alarm_t));
					alarmInfo.type = SDK_EVENT_MOTION;
					alarmInfo.channelNo = j;
					alarmInfo.streamNo = 0;
					alarmInfo.status = 0;
					streamInsertAlarmToAlarmQueue(NULL, &alarmInfo);
				}
			}
		}
		recordSetUpdateFlag(RECORD_UPDATE_FLAG_PARAM, 0, RECORD_UPDATE_FLAG_PARAM);
	}
	else
	{
		if(!memcmp(p_ch_cfg, &g_DbChanCfg[channel],sizeof(sdk_channel_t)))
		{
			//Printf("channel Para no change!\n");
			//Printf("g_DbChanCfg[%d].enable =%d\n",channel,g_DbChanCfg[channel].enable);
			return sizeof(sdk_channel_t);
		}
		//Printf("channel Para change!\n");
		
		sprintf(sql_cmd,"update devChannelInfo set enable=%d, channelName='%s', streamNo=%d, addr='%s', port=%d, userName='%s', userPwd='%s', protocol=%d,\
					   manProtocolType=%d,rtspmainurl='%s',rtspsuburl='%s' where channelNo=%d",
					p_ch_cfg->enable, p_ch_cfg->name, p_ch_cfg->chann_win_mode, p_ch_cfg->ip_addr, p_ch_cfg->port,
					p_ch_cfg->user.user_name, p_ch_cfg->user.user_pwd, p_ch_cfg->chann_protocol, p_ch_cfg->manufacturer
					,p_ch_cfg->rtsp_main_url,p_ch_cfg->rtsp_sub_url,channel);
		if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			ret = DB_OPR_ERROR;
		}
		else
		{
			p_ch_cfg->chann_no = channel;
			p_ch_cfg->chann_win_mode = p_ch_cfg->chann_win_mode;
			appStartDecodeChannel(p_ch_cfg);
			recordSetUpdateFlag(RECORD_UPDATE_FLAG_PARAM, 0, RECORD_UPDATE_FLAG_PARAM);
			memcpy(&g_DbChanCfg[channel],p_ch_cfg,sizeof(sdk_channel_t));
			if(g_DbChanCfg[channel].enable ==0)
			{
				memset(&alarmInfo, 0, sizeof(stream_alarm_t));
				alarmInfo.type = SDK_EVENT_MOTION;
				alarmInfo.channelNo = channel;
				alarmInfo.streamNo = 0;
				alarmInfo.status = 0;
				streamInsertAlarmToAlarmQueue(NULL, &alarmInfo);
			}
		}

		//Printf("channel Para change=%d,enable=%d  \n",channel,p_ch_cfg->enable);
		
		if(1==p_ch_cfg->enable)
		{
		   ptzSetStatus(channel,0);
		}
		if(0==p_ch_cfg->enable)
		{
		   ptzSetStatus(channel,1);
		}
		
		recordSetUpdateFlag(RECORD_UPDATE_FLAG_PARAM, 0, RECORD_UPDATE_FLAG_PARAM);
		recordSetUpdateFlag(MOTION_UPDATE_FLAG_PARAM, 0, 0);
	}
	return ret;
}
/******************************************
函数功能:通道配置---查询


******************************************/
int GetGlobal_ChannCfg(void *data, int len, void *pRecvData, int recvDataLen)
{
	int ret = 0;
	sdk_channel_t *p_ch_cfg = (sdk_channel_t *)pRecvData;
	int channel = *(int *)data;
	if(channel == CHANN_TYPE_ALL)
	{
		//Printf("Get  all chann from global para!:%d\n\n\n\n",channel);
		memcpy(p_ch_cfg, g_DbChanCfg, configGetDisplayNum() * sizeof(sdk_channel_t));
		ret = configGetDisplayNum() * sizeof(sdk_channel_t);
	}
	else
	{
		//Printf("Get  single chann from global para!:%d\n",channel);
		memcpy(p_ch_cfg, &g_DbChanCfg[channel], sizeof(sdk_channel_t));
		ret = sizeof(sdk_channel_t);
	}
	return ret;
}
/******************************************
函数功能:通道配置---查询


******************************************/
int DbSelect_ChannCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int i = 0;
	int ret = 0;
	char sql_cmd[512] = {0};
	int result = 0;
	char **dbResult;
	char *err_msg = NULL;
	int nRow = 0, nColumn = 0;
	sdk_channel_t *p_ch_cfg = (sdk_channel_t *)pRecvData;
	int channel = *(int *)data;

	//Printf("Get  chann from database! \n");
	sprintf(sql_cmd, "select * from devChannelInfo");
	result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
	if(SQLITE_OK != result)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		sqlite3_free_table(dbResult);
		return DB_OPR_ERROR;
	}
	for(i = 0; (i<MAX_DISPLAY_NUM)&&(i < configGetDisplayNum()); i++)
	{
		memset(&g_DbChanCfg[i], 0, sizeof(sdk_channel_t));
		g_DbChanCfg[i].enable = atoi(dbResult[(i + 1) * nColumn + 0]);
		strcpy((char *)g_DbChanCfg[i].name, dbResult[(i + 1) * nColumn + 1]);
		g_DbChanCfg[i].chann_no = atoi(dbResult[(i + 1) * nColumn + 2]);
		g_DbChanCfg[i].chann_win_mode = atoi(dbResult[(i + 1) * nColumn + 3]);
		strcpy((char *)g_DbChanCfg[i].ip_addr, dbResult[(i + 1) * nColumn + 4]);
		g_DbChanCfg[i].port = atoi(dbResult[(i + 1) * nColumn + 5]);
		strcpy((char *)g_DbChanCfg[i].user.user_name, dbResult[(i + 1) * nColumn + 6]);
		strcpy((char *)g_DbChanCfg[i].user.user_pwd, dbResult[(i + 1) * nColumn + 7]);
		g_DbChanCfg[i].chann_protocol = atoi(dbResult[(i + 1) * nColumn + 8]);
		g_DbChanCfg[i].manufacturer= atoi(dbResult[(i + 1) * nColumn + 9]);
		strcpy((char *)g_DbChanCfg[i].rtsp_main_url, dbResult[(i + 1) * nColumn + 10]);
		strcpy((char *)g_DbChanCfg[i].rtsp_sub_url, dbResult[(i + 1) * nColumn + 11]);
		ret = sizeof(sdk_channel_t);
	}
	if(channel == 0xFA)
	{
      		memcpy(p_ch_cfg,g_DbChanCfg,sizeof(sdk_channel_t)*configGetDisplayNum());
	}
	else if(channel>0 && channel <=configGetDisplayNum() && channel <MAX_DISPLAY_NUM)
	{
		memcpy(p_ch_cfg,&g_DbChanCfg[channel],sizeof(sdk_channel_t));
	}
	recordSetUpdateFlag(RECORD_UPDATE_FLAG_PARAM, 0, RECORD_UPDATE_FLAG_PARAM);
	sqlite3_free_table(dbResult);
	
	return ret;
}
/******************************************
函数功能:通道配置---默认


******************************************/
int DbUpdate_DefaultChan(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int i = 0;
	int ret = 0;
	char sql_cmd[2048] = {0,};
	char *err_msg = NULL;
	int channel = *(int *)data;
	sdk_channel_t ch_cfg;
	if(channel == CHANN_TYPE_ALL)
	{
		for(i = 0; (i < MAX_DISPLAY_NUM)&&(i < configGetDisplayNum()); i++)
		{
			//Printf("1111:channel:%d  %d\n\n",channel, i);
			sprintf(sql_cmd,"update devChannelInfo set enable=0, channelName='', streamNo=1, \
				addr='', port=0, userName='admin', userPwd='', protocol=0, manProtocolType=0, \
				manProtocolType=0, rtspmainurl='rtsp://192.168.1.168:554/0', rtspsuburl='rtsp://192.168.1.168:554/1'  where channelNo=%d", i);

			if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
			{
				Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
				ret = DB_OPR_ERROR;
			}
			memset(&ch_cfg, 0, sizeof(sdk_channel_t));
			ch_cfg.chann_no = i;
			appStartDecodeChannel(&ch_cfg);
			memset(&g_DbChanCfg[i],0,sizeof(sdk_channel_t));
			//ptzSetStatus(i,1);
		}
		//recordStopAllDevChannel();
		recordSetUpdateFlag(RECORD_UPDATE_FLAG_PARAM, 0, RECORD_UPDATE_FLAG_PARAM);
		recordSetUpdateFlag(MOTION_UPDATE_FLAG_PARAM, 0, 0);
	}
	return ret;
}
/******************************************
函数功能:录像配置---更新


******************************************/
int DbUpdate_RecordCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int i = 0, j = 0;
	int ret = 0;
	char sql_cmd[1024] = {0};
	char *err_msg = NULL;
	sdk_record_cfg_t *p_rec_cfg = (sdk_record_cfg_t *)pRecvData;
	int channel = *(int *)data;
	
	if(channel == CHANN_TYPE_ALL)
	{
		for(j = 0; (j < MAX_DISPLAY_NUM) && (j < configGetDisplayNum()); j++)
		{
			sprintf(sql_cmd,"update RecordChannelInfo set enable=%d, preRecord=%d where channelNo=%d", p_rec_cfg->enable, p_rec_cfg->pre_record_time, j);
			if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
			{
				Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
				ret = DB_OPR_ERROR;
			}
			for(i = 0; i < MAX_WEEK_NUM - 1; i++)
			{
				sprintf(sql_cmd,"update RecordChannelTime set \
						recCurSegNum=%d,\
						enable0=%d, recType0=%d, startHour0=%d, startMin0=%d, stopHour0=%d, stopMin0=%d, \
						enable1=%d, recType1=%d, startHour1=%d, startMin1=%d, stopHour1=%d, stopMin1=%d, \
						enable2=%d, recType2=%d, startHour2=%d, startMin2=%d, stopHour2=%d, stopMin2=%d, \
						enable3=%d, recType3=%d, startHour3=%d, startMin3=%d, stopHour3=%d, stopMin3=%d, \
						enable4=%d, recType4=%d, startHour4=%d, startMin4=%d, stopHour4=%d, stopMin4=%d, \
						enable5=%d, recType5=%d, startHour5=%d, startMin5=%d, stopHour5=%d, stopMin5=%d\
						where channelNo=%d and weekday=%d",

						p_rec_cfg->record_sched[i].record_time_num,
						p_rec_cfg->record_sched[i].sched_time[0].enable,
						p_rec_cfg->record_sched[i].sched_time[0].record_type,
						p_rec_cfg->record_sched[i].sched_time[0].start_hour, p_rec_cfg->record_sched[i].sched_time[0].start_min,
						p_rec_cfg->record_sched[i].sched_time[0].stop_hour, p_rec_cfg->record_sched[i].sched_time[0].stop_min,

						p_rec_cfg->record_sched[i].sched_time[1].enable,
						p_rec_cfg->record_sched[i].sched_time[1].record_type,
						p_rec_cfg->record_sched[i].sched_time[1].start_hour, p_rec_cfg->record_sched[i].sched_time[1].start_min,
						p_rec_cfg->record_sched[i].sched_time[1].stop_hour, p_rec_cfg->record_sched[i].sched_time[1].stop_min,

						p_rec_cfg->record_sched[i].sched_time[2].enable,
						p_rec_cfg->record_sched[i].sched_time[2].record_type,
						p_rec_cfg->record_sched[i].sched_time[2].start_hour, p_rec_cfg->record_sched[i].sched_time[2].start_min,
						p_rec_cfg->record_sched[i].sched_time[2].stop_hour, p_rec_cfg->record_sched[i].sched_time[2].stop_min,

						p_rec_cfg->record_sched[i].sched_time[3].enable,
						p_rec_cfg->record_sched[i].sched_time[3].record_type,
						p_rec_cfg->record_sched[i].sched_time[3].start_hour, p_rec_cfg->record_sched[i].sched_time[3].start_min,
						p_rec_cfg->record_sched[i].sched_time[3].stop_hour, p_rec_cfg->record_sched[i].sched_time[3].stop_min,

						p_rec_cfg->record_sched[i].sched_time[4].enable,
						p_rec_cfg->record_sched[i].sched_time[4].record_type,
						p_rec_cfg->record_sched[i].sched_time[4].start_hour, p_rec_cfg->record_sched[i].sched_time[4].start_min,
						p_rec_cfg->record_sched[i].sched_time[4].stop_hour, p_rec_cfg->record_sched[i].sched_time[4].stop_min,

						p_rec_cfg->record_sched[i].sched_time[5].enable,
						p_rec_cfg->record_sched[i].sched_time[5].record_type,
						p_rec_cfg->record_sched[i].sched_time[5].start_hour, p_rec_cfg->record_sched[i].sched_time[5].start_min,
						p_rec_cfg->record_sched[i].sched_time[5].stop_hour, p_rec_cfg->record_sched[i].sched_time[5].stop_min,

						j, i);
				if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
				{
					Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
					ret = DB_OPR_ERROR;
				}
			}
			memset(&g_DbRecCfg[j],0,sizeof(sdk_record_cfg_t));
			memcpy(&g_DbRecCfg[j],p_rec_cfg,sizeof(sdk_record_cfg_t));
		}

		if(ret != DB_OPR_ERROR)
		{
			recordSetUpdateFlag(RECORD_UPDATE_FLAG_PARAM, 0, RECORD_UPDATE_FLAG_PARAM);
		}
	}
	else
	{
		if(!memcmp(p_rec_cfg, &g_DbRecCfg[channel],sizeof(sdk_record_cfg_t)))
		{
			//Printf("rec Para no change!\n");
			return sizeof(sdk_record_cfg_t);
		}
		//Printf("rec Para change!\n");
		sprintf(sql_cmd,"update RecordChannelInfo set enable=%d, preRecord=%d where channelNo=%d", p_rec_cfg->enable, p_rec_cfg->pre_record_time, channel);
		if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			ret = DB_OPR_ERROR;
		}

		for(i = 0; i < MAX_WEEK_NUM - 1; i++)
		{
			sprintf(sql_cmd,"update RecordChannelTime set \
					recCurSegNum=%d,\
					enable0=%d, recType0=%d,startHour0=%d, startMin0=%d, stopHour0=%d, stopMin0=%d, \
					enable1=%d, recType1=%d,startHour1=%d, startMin1=%d, stopHour1=%d, stopMin1=%d, \
					enable2=%d, recType2=%d,startHour2=%d, startMin2=%d, stopHour2=%d, stopMin2=%d, \
					enable3=%d, recType3=%d,startHour3=%d, startMin3=%d, stopHour3=%d, stopMin3=%d, \
					enable4=%d, recType4=%d,startHour4=%d, startMin4=%d, stopHour4=%d, stopMin4=%d, \
					enable5=%d, recType5=%d,startHour5=%d, startMin5=%d, stopHour5=%d, stopMin5=%d\
					where channelNo=%d and weekday=%d",

					p_rec_cfg->record_sched[i].record_time_num,
					p_rec_cfg->record_sched[i].sched_time[0].enable,
					p_rec_cfg->record_sched[i].sched_time[0].record_type,
					p_rec_cfg->record_sched[i].sched_time[0].start_hour, p_rec_cfg->record_sched[i].sched_time[0].start_min,
					p_rec_cfg->record_sched[i].sched_time[0].stop_hour, p_rec_cfg->record_sched[i].sched_time[0].stop_min,

					p_rec_cfg->record_sched[i].sched_time[1].enable,
					p_rec_cfg->record_sched[i].sched_time[1].record_type,
					p_rec_cfg->record_sched[i].sched_time[1].start_hour, p_rec_cfg->record_sched[i].sched_time[1].start_min,
					p_rec_cfg->record_sched[i].sched_time[1].stop_hour, p_rec_cfg->record_sched[i].sched_time[1].stop_min,

					p_rec_cfg->record_sched[i].sched_time[2].enable,
					p_rec_cfg->record_sched[i].sched_time[2].record_type,
					p_rec_cfg->record_sched[i].sched_time[2].start_hour, p_rec_cfg->record_sched[i].sched_time[2].start_min,
					p_rec_cfg->record_sched[i].sched_time[2].stop_hour, p_rec_cfg->record_sched[i].sched_time[2].stop_min,

					p_rec_cfg->record_sched[i].sched_time[3].enable,
					p_rec_cfg->record_sched[i].sched_time[3].record_type,
					p_rec_cfg->record_sched[i].sched_time[3].start_hour, p_rec_cfg->record_sched[i].sched_time[3].start_min,
					p_rec_cfg->record_sched[i].sched_time[3].stop_hour, p_rec_cfg->record_sched[i].sched_time[3].stop_min,

					p_rec_cfg->record_sched[i].sched_time[4].enable,
					p_rec_cfg->record_sched[i].sched_time[4].record_type,
					p_rec_cfg->record_sched[i].sched_time[4].start_hour, p_rec_cfg->record_sched[i].sched_time[4].start_min,
					p_rec_cfg->record_sched[i].sched_time[4].stop_hour, p_rec_cfg->record_sched[i].sched_time[4].stop_min,

					p_rec_cfg->record_sched[i].sched_time[5].enable,
					p_rec_cfg->record_sched[i].sched_time[5].record_type,
					p_rec_cfg->record_sched[i].sched_time[5].start_hour, p_rec_cfg->record_sched[i].sched_time[5].start_min,
					p_rec_cfg->record_sched[i].sched_time[5].stop_hour, p_rec_cfg->record_sched[i].sched_time[5].stop_min,

					channel, i);
			if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
			{
				Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
				ret = DB_OPR_ERROR;
			}
		}
		if(ret != DB_OPR_ERROR)
		{
			memset(&g_DbRecCfg[channel],0,sizeof(sdk_record_cfg_t));
			memcpy(&g_DbRecCfg[channel],p_rec_cfg,sizeof(sdk_record_cfg_t));
			recordSetUpdateFlag(RECORD_UPDATE_FLAG_PARAM, 0, RECORD_UPDATE_FLAG_PARAM);
		}
	}
	return ret;
}
/******************************************
函数功能:录像配置---更新


******************************************/
int GetGlobal_RecordCfg(void *data, int len, void *pRecvData, int recvDataLen)
{
	int ret = 0;
	sdk_record_cfg_t *p_rec_cfg = (sdk_record_cfg_t *)pRecvData;
	int channel = *(int *)data;
	//Printf("Get chann rec  from global para!:%d \n",channel);
	memcpy(p_rec_cfg, &g_DbRecCfg[channel], sizeof(sdk_record_cfg_t));
	ret = sizeof(sdk_record_cfg_t);
	return ret;
}

/******************************************
函数功能:录像配置---更新


******************************************/
int DbSelect_RecordCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int i = 0, j = 0;
	int ret = 0;
	char sql_cmd[1024] = {0};
	int result = 0;
	char **dbResult;
	char *err_msg = NULL;
	int nRow = 0, nColumn = 0;
	sdk_record_cfg_t *p_rec_cfg = (sdk_record_cfg_t *)pRecvData;
	int channel = *(int *)data;	
	//Printf("Get  rec  from database!:%d \n",channel);
	
	sprintf(sql_cmd, "select * from RecordChannelInfo");
	result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, NULL);
	if(SQLITE_OK != result)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		sqlite3_free_table(dbResult);
		return DB_OPR_ERROR;
	}
	for(i = 0; i < configGetDisplayNum(); i++)
	{
		memset(&g_DbRecCfg[i], 0, sizeof(sdk_record_cfg_t));
		g_DbRecCfg[i].enable = atoi(dbResult[(i + 1) * nColumn + 0]);
		g_DbRecCfg[i].pre_record_time = atoi(dbResult[(i + 1) * nColumn + 2]);
	}
	


	for(j = 0; j < configGetDisplayNum(); j++)
	{
		sprintf(sql_cmd, "select * from RecordChannelTime where channelNo=%d",j);
		result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, NULL);
		if(SQLITE_OK != result)
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			sqlite3_free_table(dbResult);
			return DB_OPR_ERROR;
		}
		for(i = 0; i < MAX_WEEK_NUM - 1; i++)
		{
			int weekday = atoi(dbResult[(i + 1) * nColumn + 3]);
			g_DbRecCfg[j].record_sched[weekday].record_time_num = atoi(dbResult[(i + 1) * nColumn + 4]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[0].enable = atoi(dbResult[(i + 1) * nColumn + 5]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[0].record_type = atoi(dbResult[(i + 1) * nColumn + 6]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[0].start_hour = atoi(dbResult[(i + 1) * nColumn + 7]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[0].start_min = atoi(dbResult[(i + 1) * nColumn + 8]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[0].stop_hour = atoi(dbResult[(i + 1) * nColumn + 9]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[0].stop_min = atoi(dbResult[(i + 1) * nColumn + 10]);

			g_DbRecCfg[j].record_sched[weekday].sched_time[1].enable = atoi(dbResult[(i + 1) * nColumn + 11]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[1].record_type = atoi(dbResult[(i + 1) * nColumn + 12]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[1].start_hour = atoi(dbResult[(i + 1) * nColumn + 13]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[1].start_min = atoi(dbResult[(i + 1) * nColumn + 14]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[1].stop_hour = atoi(dbResult[(i + 1) * nColumn + 15]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[1].stop_min = atoi(dbResult[(i + 1) * nColumn + 16]);

			g_DbRecCfg[j].record_sched[weekday].sched_time[2].enable = atoi(dbResult[(i + 1) * nColumn + 17]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[2].record_type = atoi(dbResult[(i + 1) * nColumn + 18]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[2].start_hour = atoi(dbResult[(i + 1) * nColumn + 19]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[2].start_min = atoi(dbResult[(i + 1) * nColumn + 20]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[2].stop_hour = atoi(dbResult[(i + 1) * nColumn + 21]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[2].stop_min = atoi(dbResult[(i + 1) * nColumn + 22]);

			g_DbRecCfg[j].record_sched[weekday].sched_time[3].enable = atoi(dbResult[(i + 1) * nColumn + 23]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[3].record_type = atoi(dbResult[(i + 1) * nColumn + 24]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[3].start_hour = atoi(dbResult[(i + 1) * nColumn + 25]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[3].start_min = atoi(dbResult[(i + 1) * nColumn + 26]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[3].stop_hour = atoi(dbResult[(i + 1) * nColumn + 27]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[3].stop_min = atoi(dbResult[(i + 1) * nColumn + 28]);

			g_DbRecCfg[j].record_sched[weekday].sched_time[4].enable = atoi(dbResult[(i + 1) * nColumn + 29]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[4].record_type = atoi(dbResult[(i + 1) * nColumn + 30]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[4].start_hour = atoi(dbResult[(i + 1) * nColumn + 31]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[4].start_min = atoi(dbResult[(i + 1) * nColumn + 32]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[4].stop_hour = atoi(dbResult[(i + 1) * nColumn + 33]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[4].stop_min = atoi(dbResult[(i + 1) * nColumn + 34]);

			g_DbRecCfg[j].record_sched[weekday].sched_time[5].enable = atoi(dbResult[(i + 1) * nColumn + 35]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[5].record_type = atoi(dbResult[(i + 1) * nColumn + 36]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[5].start_hour = atoi(dbResult[(i + 1) * nColumn + 37]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[5].start_min = atoi(dbResult[(i + 1) * nColumn + 38]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[5].stop_hour = atoi(dbResult[(i + 1) * nColumn + 39]);
			g_DbRecCfg[j].record_sched[weekday].sched_time[5].stop_min = atoi(dbResult[(i + 1) * nColumn + 40]);
		}
		
	}
	sqlite3_free_table(dbResult);
	ret = sizeof(sdk_record_cfg_t);
	memcpy(p_rec_cfg, &g_DbRecCfg[channel], sizeof(sdk_record_cfg_t));
	if(ret != DB_OPR_ERROR)
	{
		recordSetUpdateFlag(RECORD_UPDATE_FLAG_PARAM, 0, RECORD_UPDATE_FLAG_PARAM);
	}
	return ret;
}
/******************************************
函数功能:录像配置恢复默认


******************************************/
int DbUpdate_DefaultRecord(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int i = 0, j = 0;
	int ret = 0;
	char sql_cmd[1024] = {0,};
	char *err_msg = NULL;
	int channel = *(int *)data;
	//Printf("Record Default!:%d\n",channel);
	if(channel == CHANN_TYPE_ALL)
	{
		for(j = 0; (j < MAX_DISPLAY_NUM) && (j < configGetDisplayNum()); j++)
		{

			sprintf(sql_cmd,"update RecordChannelInfo set enable=0, preRecord=0 where channelNo=%d", j);
			if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
			{
				Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
				ret = DB_OPR_ERROR;
			}
			g_DbRecCfg[j].enable = 0;
			g_DbRecCfg[j].pre_record_time = 0;

			for(i = 0; i < MAX_WEEK_NUM - 1; i++)
			{
				sprintf(sql_cmd,"update RecordChannelTime set  \
						recCurSegNum=1,\
						enable0=0, recType0=1, startHour0=0, startMin0=0, stopHour0=24, stopMin0=0, \
						enable1=0, recType1=0, startHour1=0, startMin1=0, stopHour1=0, stopMin1=0, \
						enable2=0, recType2=0, startHour2=0, startMin2=0, stopHour2=0, stopMin2=0, \
						enable3=0, recType3=0, startHour3=0, startMin3=0, stopHour3=0, stopMin3=0, \
						enable4=0, recType4=0, startHour4=0, startMin4=0, stopHour4=0, stopMin4=0, \
						enable5=0, recType5=0, startHour5=0, startMin5=0, stopHour5=0, stopMin5=0 \
						where channelNo=%d and weekday=%d",
						j, i);
				if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
				{
					Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
					ret = DB_OPR_ERROR;
				}

				g_DbRecCfg[j].record_sched[i].record_time_num = 1;
				g_DbRecCfg[j].record_sched[i].sched_time[0].enable = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[0].record_type = 1;
				g_DbRecCfg[j].record_sched[i].sched_time[0].start_hour = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[0].start_min = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[0].stop_hour = 24;
				g_DbRecCfg[j].record_sched[i].sched_time[0].stop_min = 0;

				g_DbRecCfg[j].record_sched[i].sched_time[1].enable = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[1].record_type = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[1].start_hour =0 ;
				g_DbRecCfg[j].record_sched[i].sched_time[1].start_min = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[1].stop_hour = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[1].stop_min = 0;

				g_DbRecCfg[j].record_sched[i].sched_time[2].enable = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[2].record_type = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[2].start_hour = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[2].start_min = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[2].stop_hour = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[2].stop_min = 0;

				g_DbRecCfg[j].record_sched[i].sched_time[3].enable = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[3].record_type = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[3].start_hour = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[3].start_min = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[3].stop_hour =0 ;
				g_DbRecCfg[j].record_sched[i].sched_time[3].stop_min = 0;

				g_DbRecCfg[j].record_sched[i].sched_time[4].enable = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[4].record_type = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[4].start_hour = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[4].start_min = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[4].stop_hour = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[4].stop_min = 0;

				g_DbRecCfg[j].record_sched[i].sched_time[5].enable = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[5].record_type = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[5].start_hour = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[5].start_min = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[5].stop_hour = 0;
				g_DbRecCfg[j].record_sched[i].sched_time[5].stop_min = 0;

			}
		}
	}
	else
	{
		sprintf(sql_cmd,"update RecordChannelInfo set enable=0, preRecord=0 where channelNo=%d", channel);
		if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			ret = DB_OPR_ERROR;
		}
		for(i = 0; i < MAX_WEEK_NUM - 1; i++)
		{
			memset(sql_cmd,0,sizeof(sql_cmd));
			sprintf(sql_cmd,"update RecordChannelTime set \
					recCurSegNum=1,\
					enable0=0, recType0=1, startHour0=0, startMin0=0, stopHour0=24, stopMin0=0, \
					enable1=0, recType1=0, startHour1=0, startMin1=0, stopHour1=0, stopMin1=0, \
					enable2=0, recType2=0, startHour2=0, startMin2=0, stopHour2=0, stopMin2=0, \
					enable3=0, recType3=0, startHour3=0, startMin3=0, stopHour3=0, stopMin3=0,\
					enable4=0, recType4=0, startHour4=0, startMin4=0, stopHour4=0, stopMin4=0, \
					enable5=0, recType5=0, startHour5=0, startMin5=0, stopHour5=0, stopMin5=0 \
					where channelNo=%d and weekday=%d",
					channel, i);
			if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
			{
				Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
				ret = DB_OPR_ERROR;
			}
			g_DbRecCfg[channel].record_sched[i].record_time_num = 1;
			g_DbRecCfg[channel].record_sched[i].sched_time[0].enable = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[0].record_type = 1;
			g_DbRecCfg[channel].record_sched[i].sched_time[0].start_hour = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[0].start_min = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[0].stop_hour = 24;
			g_DbRecCfg[channel].record_sched[i].sched_time[0].stop_min = 0;

			g_DbRecCfg[channel].record_sched[i].sched_time[1].enable = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[1].record_type = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[1].start_hour =0 ;
			g_DbRecCfg[channel].record_sched[i].sched_time[1].start_min = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[1].stop_hour = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[1].stop_min = 0;

			g_DbRecCfg[channel].record_sched[i].sched_time[2].enable = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[2].record_type = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[2].start_hour = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[2].start_min = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[2].stop_hour = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[2].stop_min = 0;

			g_DbRecCfg[channel].record_sched[i].sched_time[3].enable = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[3].record_type = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[3].start_hour = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[3].start_min = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[3].stop_hour =0 ;
			g_DbRecCfg[channel].record_sched[i].sched_time[3].stop_min = 0;

			g_DbRecCfg[channel].record_sched[i].sched_time[4].enable = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[4].record_type = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[4].start_hour = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[4].start_min = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[4].stop_hour = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[4].stop_min = 0;

			g_DbRecCfg[channel].record_sched[i].sched_time[5].enable = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[5].record_type = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[5].start_hour = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[5].start_min = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[5].stop_hour = 0;
			g_DbRecCfg[channel].record_sched[i].sched_time[5].stop_min = 0;

		}
	}	
	if(ret != DB_OPR_ERROR)
	{
		recordSetUpdateFlag(RECORD_UPDATE_FLAG_PARAM, 0, RECORD_UPDATE_FLAG_PARAM);
	}
	return ret;
}
/******************************************
函数功能:预览配置---更新


******************************************/
int DbUpdate_PreviewCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int i = 0;
	int ret = 0;
	char sql_cmd[512] = {0};
	char *err_msg = NULL;
	sdk_preview_t *p_preview_cfg = (sdk_preview_t *)pRecvData;
	char tmpValue[MAX_DISPLAY_NUM] = {0};
	if(!memcmp(p_preview_cfg, &g_DbPreviewCfg,sizeof(sdk_preview_t)))
	{
		//Printf("preview Para no change!\n");
		return sizeof(sdk_preview_t);
	}
	//Printf("preview Para change!\n");
	memcpy(&g_DbPreviewCfg,p_preview_cfg, sizeof(sdk_preview_t));
	sprintf(sql_cmd,"update displayInfo set displayNum=%d, enableViceScreen=%d", p_preview_cfg->init_mode, p_preview_cfg->enable);
	if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		ret = DB_OPR_ERROR;
	}
	for(i = 0; (i < MAX_DISPLAY_NUM) && (i < configGetDisplayNum()); i++)
	{
		if(tmpValue[p_preview_cfg->windows[p_preview_cfg->init_mode][i]] == 1)
		{
			ret = -1;
			break;
		}
		tmpValue[p_preview_cfg->windows[p_preview_cfg->init_mode][i]] = 1;
	}
	if(ret == -1)
	{
		return ret;
	}
	if(configGetDisplayNum() == 32)	p_preview_cfg->init_mode = 7;
	else if(configGetDisplayNum() == 24)	p_preview_cfg->init_mode = 6;
	else if(configGetDisplayNum() == 16)	p_preview_cfg->init_mode = 5;
	else if(configGetDisplayNum() == 9)	p_preview_cfg->init_mode = 4;
	else if(configGetDisplayNum() == 8)	p_preview_cfg->init_mode = 3;
	else if(configGetDisplayNum() == 6)	p_preview_cfg->init_mode = 2;
	else if(configGetDisplayNum() == 4)	p_preview_cfg->init_mode = 1;
	else if(configGetDisplayNum() == 1)	p_preview_cfg->init_mode = 0;
	else				p_preview_cfg->init_mode = 0;
	for(i = 0; (i < MAX_DISPLAY_NUM) && (i <= configGetDisplayNum()); i++)
	{
		if(p_preview_cfg->init_mode == 0)
		{
			sprintf(sql_cmd,"update viewPosInfo set pos1=%d where channelNo=%d", p_preview_cfg->windows[p_preview_cfg->init_mode][i], i);
		}
		else if(p_preview_cfg->init_mode == 1)
		{
			sprintf(sql_cmd,"update viewPosInfo set pos4=%d where channelNo=%d", p_preview_cfg->windows[p_preview_cfg->init_mode][i], i);
		}
		else if(p_preview_cfg->init_mode == 2)
		{
			sprintf(sql_cmd,"update viewPosInfo set pos6=%d where channelNo=%d", p_preview_cfg->windows[p_preview_cfg->init_mode][i], i);
		}
		else if(p_preview_cfg->init_mode == 3)
		{
			sprintf(sql_cmd,"update viewPosInfo set pos8=%d where channelNo=%d", p_preview_cfg->windows[p_preview_cfg->init_mode][i], i);
		}
		else if(p_preview_cfg->init_mode == 4)
		{
			sprintf(sql_cmd,"update viewPosInfo set pos9=%d where channelNo=%d", p_preview_cfg->windows[p_preview_cfg->init_mode][i], i);
		}
		else if(p_preview_cfg->init_mode == 5)
		{
			sprintf(sql_cmd,"update viewPosInfo set pos16=%d where channelNo=%d", p_preview_cfg->windows[p_preview_cfg->init_mode][i], i);
		}
		else if(p_preview_cfg->init_mode == 6)
		{
			sprintf(sql_cmd,"update viewPosInfo set pos25=%d where channelNo=%d", p_preview_cfg->windows[p_preview_cfg->init_mode][i], i);
		}
		else if(p_preview_cfg->init_mode == 7)
		{
			sprintf(sql_cmd,"update viewPosInfo set pos32=%d where channelNo=%d", p_preview_cfg->windows[p_preview_cfg->init_mode][i], i);
		}
		if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			ret = DB_OPR_ERROR;
		}

	}

	return ret;
}
int GetGlobal_PreviewCfg(void *data, int len, void *pRecvData, int recvDataLen)
{
	int ret = 0;
	sdk_preview_t *p_preview_cfg = (sdk_preview_t *)pRecvData;	
	//Printf("Get preview value from global para! \n");
	memcpy(p_preview_cfg, &g_DbPreviewCfg,sizeof(sdk_preview_t));
	ret = sizeof(sdk_preview_t);
	return ret;
}
/******************************************
函数功能:预览配置---查询


******************************************/
int DbSelect_PreviewCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int i = 0, j = 0;
	int ret = 0;
	char sql_cmd[512] = {0};
	int result = 0;
	char *err_msg = NULL;
	char **dbResult;
	int nRow = 0, nColumn = 0;
	sdk_preview_t *p_preview_cfg = (sdk_preview_t *)pRecvData;	
	//Printf("Get preview value from database! \n");
	
	sprintf(sql_cmd, "select * from displayInfo");
	result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
	if(SQLITE_OK != result)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		sqlite3_free_table(dbResult);
		return DB_OPR_ERROR;
	}
	for(i = 0; i < nRow; i++)
	{
		p_preview_cfg->init_mode = atoi(dbResult[nColumn + 0]);
		p_preview_cfg->enable = atoi(dbResult[nColumn + 1]);
	}
	
	sprintf(sql_cmd, "select * from viewPosInfo");
	result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
	if(SQLITE_OK != result)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		sqlite3_free_table(dbResult);
		return DB_OPR_ERROR;
	}
	for(i = 0; (i < nRow) && (i < MAX_DISPLAY_NUM) && (i <= configGetDisplayNum()); i++)
	{
		for(j = 0; j < MAX_PREVIEW_MODE; j++)
		{
			p_preview_cfg->windows[j][i] = atoi(dbResult[(i + 1) * nColumn + 1 + j]);
		}
	}
	ret = sizeof(sdk_preview_t);
	sqlite3_free_table(dbResult);
	memset(&g_DbPreviewCfg,0, sizeof(sdk_preview_t));
	memcpy(&g_DbPreviewCfg,p_preview_cfg, sizeof(sdk_preview_t));
	return ret;
}

/******************************************
函数功能:预览配置恢复默认


******************************************/
int DbUpdate_DefaultPreview(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int i = 0, j = 0;
	int ret = 0;
	char sql_cmd[1024] = {0,};
	char *err_msg = NULL;
	int init_mode = 0;
	sprintf(sql_cmd,"update displayInfo set displayNum=%d, enableViceScreen=%d", 255, 0);
	if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		ret = DB_OPR_ERROR;
	}
	g_DbPreviewCfg.init_mode = 255;
	g_DbPreviewCfg.enable = 0;
	if(configGetDisplayNum() == 32)		init_mode = 7;
	else if(configGetDisplayNum() == 24)	init_mode = 6;
	else if(configGetDisplayNum() == 16)	init_mode = 5;
	else if(configGetDisplayNum() == 9)	init_mode = 4;
	else if(configGetDisplayNum() == 8)	init_mode = 3;
	else if(configGetDisplayNum() == 6)	init_mode = 2;
	else if(configGetDisplayNum() == 4)	init_mode = 1;
	else if(configGetDisplayNum() == 1)	init_mode = 0;
	else					init_mode = 0;
	
	for(i = 0; (i < MAX_DISPLAY_NUM) && (i <= configGetDisplayNum()); i++)
	{
		if(init_mode == 0)
		{
			sprintf(sql_cmd,"update viewPosInfo set pos1=%d where channelNo=%d", i, i);
		}
		else if(init_mode == 1)
		{
			sprintf(sql_cmd,"update viewPosInfo set pos4=%d where channelNo=%d", i, i);
		}
		else if(init_mode == 2)
		{
			sprintf(sql_cmd,"update viewPosInfo set pos6=%d where channelNo=%d", i, i);
		}
		else if(init_mode == 3)
		{
			sprintf(sql_cmd,"update viewPosInfo set pos8=%d where channelNo=%d", i, i);
		}
		else if(init_mode == 4)
		{
			sprintf(sql_cmd,"update viewPosInfo set pos9=%d where channelNo=%d", i, i);
		}
		else if(init_mode == 5)
		{
			sprintf(sql_cmd,"update viewPosInfo set pos16=%d where channelNo=%d", i, i);
		}
		else if(init_mode == 6)
		{
			sprintf(sql_cmd,"update viewPosInfo set pos25=%d where channelNo=%d", i, i);
		}
		else if(init_mode == 7)
		{
			sprintf(sql_cmd,"update viewPosInfo set pos32=%d where channelNo=%d", i, i);
		}
		if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			ret = DB_OPR_ERROR;
		}
		for(j = 0; j < MAX_PREVIEW_MODE; j++)
		{
			g_DbPreviewCfg.windows[j][i] = i;
		}
	}
	return ret;
}

/******************************************
函数功能:普通配置---更新


******************************************/
int DbUpdate_CommCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[1024] = {0};
	char *err_msg = NULL;
	sdk_comm_cfg_t *p_comm_cfg = (sdk_comm_cfg_t*)pRecvData;
	if(!memcmp(p_comm_cfg, &g_DbCommCfg,sizeof(sdk_comm_cfg_t)))
	{
		//Printf("comm Para no change!\n");
		return sizeof(sdk_comm_cfg_t);
	}
	//Printf("comm Para change!\n");

	sprintf(sql_cmd,"update CommCfgInfo set language=%d, recycleRecord=%d, dvrId=%d,\
			outDevice=%d, videoMode=%d, resolution=%d, standbyTime=%d,\
			devName='%s', wizardEnable=%d, transparent=%d  where idx=0",
			p_comm_cfg->language,p_comm_cfg->recycle_record,p_comm_cfg->dvr_id,p_comm_cfg->out_device, 
			p_comm_cfg->video_mode,p_comm_cfg->resolution,p_comm_cfg->standby_time,p_comm_cfg->dvr_name, 
			p_comm_cfg->wizardEnable,p_comm_cfg->transparent);
	if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		ret = DB_OPR_ERROR;
	}
	else
	{
		memcpy(&g_DbCommCfg, p_comm_cfg, sizeof(sdk_comm_cfg_t));
	}
	return ret;
}
/******************************************
函数功能:普通配置---查询全局变量


******************************************/
int GetGlobal_CommCfg(void *data, int len, void *pRecvData, int recvDataLen)
{
	int ret = 0;
	sdk_comm_cfg_t *p_comm_cfg = (sdk_comm_cfg_t*)pRecvData;
	//Printf("Get  comm from global para! \n");
	memcpy(p_comm_cfg, &g_DbCommCfg,sizeof(sdk_comm_cfg_t));
	ret = sizeof(sdk_comm_cfg_t);
	return ret;
}

/******************************************
函数功能:普通配置---查询


******************************************/
int DbSelect_CommCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0};
	int result = 0;
	char **dbResult;
	int nRow = 0, nColumn = 0;
	char *err_msg = NULL;
	sdk_comm_cfg_t *p_comm_cfg = (sdk_comm_cfg_t*)pRecvData;
	
	//Printf("Get comm value from  database !\n");
	memset(&g_DbCommCfg, 0, sizeof(sdk_comm_cfg_t));

	sprintf(sql_cmd, "select * from CommCfgInfo");
	result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
	if(SQLITE_OK != result)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		sqlite3_free_table(dbResult);
		return DB_OPR_ERROR;
	}
	g_DbCommCfg.language=atoi(dbResult[nColumn + 1]);
	g_DbCommCfg.recycle_record=atoi(dbResult[nColumn + 2]);
	g_DbCommCfg.dvr_id=atoi(dbResult[nColumn + 3]);
	g_DbCommCfg.out_device=atoi(dbResult[nColumn + 4]);
	g_DbCommCfg.video_mode=atoi(dbResult[nColumn + 5]);
	g_DbCommCfg.resolution=atoi(dbResult[nColumn + 6]);
	g_DbCommCfg.standby_time=atoi(dbResult[nColumn + 7]);
	strcpy((char *)g_DbCommCfg.dvr_name,dbResult[nColumn + 8]);
	g_DbCommCfg.wizardEnable =atoi(dbResult[nColumn + 9]);
	g_DbCommCfg.transparent=atoi(dbResult[nColumn + 10]);
	sqlite3_free_table(dbResult);
	ret = sizeof(sdk_comm_cfg_t);
	memcpy(p_comm_cfg, &g_DbCommCfg, sizeof(sdk_comm_cfg_t));
	return ret;
}

/******************************************
函数功能:普通配置恢复默认


******************************************/
int DbUpdate_DefaultComm(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0,};
	char *err_msg = NULL;
	sprintf(sql_cmd, "update CommCfgInfo set language=%d, recycleRecord=%d, dvrId=%d,\
				                 outDevice=%d, videoMode=%d,  standbyTime=%d,\
				                 devName='%s', wizardEnable=%d where idx=0",
				                 1, 0, 0, 2, 1, 30,"NVR",1);
	if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		ret = DB_OPR_ERROR;
	}
	g_DbCommCfg.language = 1;
	g_DbCommCfg.recycle_record= 0;
	g_DbCommCfg.dvr_id= 0;
	g_DbCommCfg.out_device= 2;
	g_DbCommCfg.video_mode= 1;
	g_DbCommCfg.standby_time= 30;
	memset(g_DbCommCfg.dvr_name, 0, sizeof(g_DbCommCfg.dvr_name));
	strcpy((char *)g_DbCommCfg.dvr_name,"NVR");
	g_DbCommCfg.wizardEnable = 1;
	
	//NTP默认配置。
	memset(sql_cmd,0,sizeof(sql_cmd));
	sprintf(sql_cmd,"update ntpCfg set timezone=%d, ntpserver='%s', enable=0, \
		interval=60, dateFormat=0, dateSeparator=0, timeFormat=0, port=123\
		where idx2=0",TIME_ZONE_ZERO,"clock.via.net");

	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		ret = DB_OPR_ERROR;
	}
	else
	{
		g_DbNtpCfg.time_zone = TIME_ZONE_ZERO;
		g_DbNtpCfg.enable = 0;
		g_DbNtpCfg.interval = 60;
		g_DbNtpCfg.date_format = 0;
		g_DbNtpCfg.date_separator = 0;
		g_DbNtpCfg.time_format = 0;
		g_DbNtpCfg.port = 123;
		memset(g_DbNtpCfg.ntp_server, 0, sizeof(g_DbNtpCfg.ntp_server));
		strcpy(g_DbNtpCfg.ntp_server,"clock.via.net");
		SetNtpTime(TIME_ZONE_ZERO,"clock.via.net", NTP_TIME_PORT);
	}
	return ret;
}



/******************************************
函数功能:网络配置---更新

******************************************/
int DbUpdate_NetLinkCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0};
	char **dbResult;
	char *err_msg = NULL;
	sdk_eth_cfg_t *p_eth_cfg = (sdk_eth_cfg_t *)pRecvData;
	int pIsDhcpSetPara=*(int*)(data);	////用于标记是否DHCP时设置的参数

	sprintf(sql_cmd,"update networkInfo set ip_addr='%s', netmask='%s', gateway='%s', mac='%s', dns1='%s', dns2='%s', enable_dhcp=%d, dns_auto_en=%d where rowId=1",
	p_eth_cfg->ip_info.ip_addr, p_eth_cfg->ip_info.mask, p_eth_cfg->ip_info.gateway, p_eth_cfg->ip_info.mac, p_eth_cfg->ip_info.dns1, p_eth_cfg->ip_info.dns2, p_eth_cfg->ip_info.enable_dhcp,p_eth_cfg->ip_info.enable_p2p);
	if (SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		return DB_OPR_ERROR;
	}
	if(p_eth_cfg->ip_info.enable_dhcp & (pIsDhcpSetPara!=1))// 使能了DHCP
	{	
		memcpy((char*)&g_DbEthLinkCfg,(char*)p_eth_cfg,sizeof(sdk_eth_cfg_t));
		
		if(dhcpNetFun() <0)
		{
			Printf("PlayRealStream CreatThread erro!\n");
			return -1;
		}
	}
	else // 没有使能dhcp
	{
		netSetIp("eth0", inet_addr((char *)p_eth_cfg->ip_info.ip_addr));
		netSetMask("eth0", inet_addr((char *)p_eth_cfg->ip_info.mask));
		netSetGw("eth0", inet_addr((char *)p_eth_cfg->ip_info.gateway));
		netSetRoute("eth0", inet_addr((char *)p_eth_cfg->ip_info.gateway));
		#ifdef ENABLE_MODIFY_MAC
		//netSetMac("eth0", &p_eth_cfg->ip_info.mac);
		#endif
		netSetDns(inet_addr((char *)p_eth_cfg->ip_info.dns1), inet_addr((char *)p_eth_cfg->ip_info.dns2));
		memcpy((char*)&g_DbEthLinkCfg,(char*)p_eth_cfg,sizeof(sdk_eth_cfg_t));
	}
	ret = sizeof(sdk_eth_cfg_t);
	return ret;
}
/******************************************
函数功能:网络配置---查询

******************************************/
int GetGlobal_NetLinkCfg(void *data, int len, void *pRecvData, int recvDataLen)
{
	int ret = 0;
	sdk_eth_cfg_t *p_eth_cfg = (sdk_eth_cfg_t *)pRecvData;
	//Printf("Get  ethlink from global para! \n");
	memcpy(p_eth_cfg, &g_DbEthLinkCfg,sizeof(sdk_eth_cfg_t));
	ret = sizeof(sdk_eth_cfg_t);
	return ret;
}
/******************************************
函数功能:网络配置---查询

******************************************/
int DbSelect_NetLinkCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0};
	int result = 0;
	char **dbResult;
	int nRow = 0, nColumn = 0;
	char *err_msg = NULL;
	unsigned char mac[16] = { 0 };
	sdk_eth_cfg_t *p_eth_cfg = (sdk_eth_cfg_t *)pRecvData;
	//Printf("Get ethlink value from  database !\n");
	memset(&g_DbEthLinkCfg, 0, sizeof(sdk_eth_cfg_t));
	sprintf(sql_cmd, "select * from networkInfo");
	result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
	if(SQLITE_OK != result)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		sqlite3_free_table(dbResult);
		return DB_OPR_ERROR;
	}		      
	strcpy((char *)g_DbEthLinkCfg.ip_info.if_name, dbResult[ nColumn + 0]);
	strcpy((char *)g_DbEthLinkCfg.ip_info.ip_addr, dbResult[ nColumn + 1]);	
	strcpy((char *)g_DbEthLinkCfg.ip_info.mask, dbResult[ nColumn + 2]);
	strcpy((char *)g_DbEthLinkCfg.ip_info.gateway, dbResult[ nColumn + 3]);
	#ifdef ENABLE_MODIFY_MAC
	strcpy((char *)g_DbEthLinkCfg.ip_info.mac, dbResult[ nColumn + 4]);
	#else
	netGetMac("eth0", mac);
	sprintf((char *)g_DbEthLinkCfg.ip_info.mac, "%02X-%02X-%02X-%02X-%02X-%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	#endif
	if(dbResult[ nColumn + 5])
	{
		strcpy((char *)g_DbEthLinkCfg.ip_info.dns1, dbResult[ nColumn + 5]);
	}
	if(dbResult[ nColumn + 6])
	{
		strcpy((char *)g_DbEthLinkCfg.ip_info.dns2, dbResult[ nColumn + 6]);
	}
	g_DbEthLinkCfg.ip_info.enable_dhcp = (int)atoi(dbResult[ nColumn + 7]);
	g_DbEthLinkCfg.ip_info.enable_p2p= (int)atoi(dbResult[ nColumn + 8]);
	g_DbEthLinkCfg.if_mode = 0;
	ret = sizeof(sdk_eth_cfg_t);
	sqlite3_free_table(dbResult);	
	memcpy(p_eth_cfg, &g_DbEthLinkCfg, sizeof(sdk_eth_cfg_t));
	return ret;
}

/******************************************
函数功能:网络配置恢复默认

******************************************/
int DbUpdate_DefaultNetLink(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0,};
	char *err_msg = NULL;
	unsigned char mac[16] = { 0 };
	
	sprintf(sql_cmd,"update networkInfo set ip_addr='%s', netmask='%s', gateway='%s', dns1='%s', dns2='%s', enable_dhcp=%d where rowId=1",
			DEFAULT_IP, DEFAULT_MASK, DEFAULT_GW, DEFAULT_DNS1, DEFAULT_DNS2, 0);
	if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		ret = DB_OPR_ERROR;
	}
	else
	{
		//memset(&g_DbEthLinkCfg,0, sizeof(g_DbEthLinkCfg));
		strcpy((char *)g_DbEthLinkCfg.ip_info.ip_addr, DEFAULT_IP);
		strcpy((char *)g_DbEthLinkCfg.ip_info.mask, DEFAULT_MASK);
		strcpy((char *)g_DbEthLinkCfg.ip_info.gateway, DEFAULT_GW);
		strcpy((char *)g_DbEthLinkCfg.ip_info.dns1, DEFAULT_DNS1);
		strcpy((char *)g_DbEthLinkCfg.ip_info.dns2, DEFAULT_DNS2);
		//netGetMac("eth0", mac);
		//sprintf((char *)g_DbEthLinkCfg.ip_info.mac, "%02X-%02X-%02X-%02X-%02X-%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		g_DbEthLinkCfg.ip_info.enable_dhcp = 0;
		netSetIp("eth0", inet_addr((char *)DEFAULT_IP));
		netSetMask("eth0", inet_addr((char *)DEFAULT_MASK));
		netSetGw("eth0", inet_addr((char *)DEFAULT_GW));
		netSetRoute("eth0", inet_addr((char *)DEFAULT_GW));
		#ifndef ENABLE_MODIFY_MAC
		//netSetMac("eth0", DEFAULT_MAC);
		#endif
		netSetDns(inet_addr((char *)DEFAULT_DNS1), inet_addr((char *)DEFAULT_DNS2));
	}
	sprintf(sql_cmd,"update netManageInfo set def_if_no=%d, multicast_addr='%s', net_mm_server='%s', net_mm_port=%d, http_port=%d, dvr_cmd_port=%d, dvr_data_port=%d where rowId=1",
			DEFAULT_ETH0_NO, DEFAULT_MULTICAST_ADDR, DEFAULT_SERVER_ADDR, DEFAULT_PLAYBACK_PORT, DEFAULT_HTTP_PORT, DEFAULT_PLAYBACK_PORT, DEFAULT_RTSP_PORT);

	if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		ret = DB_OPR_ERROR;
	}
	else
	{
		memset(&g_DbNetMngCfg, 0 ,sizeof(g_DbNetMngCfg));
		g_DbNetMngCfg.def_if_no = DEFAULT_ETH0_NO;
		strcpy(g_DbNetMngCfg.multicast, DEFAULT_MULTICAST_ADDR);
		strcpy(g_DbNetMngCfg.multicast, DEFAULT_SERVER_ADDR);
		g_DbNetMngCfg.net_manager_port = DEFAULT_PLAYBACK_PORT;
		g_DbNetMngCfg.dvr_cmd_port = DEFAULT_PLAYBACK_PORT;
		g_DbNetMngCfg.dvr_data_port = DEFAULT_RTSP_PORT;
		g_DbNetMngCfg.http_port = DEFAULT_HTTP_PORT;
		#ifndef ENABLE_MODIFY_MAC
		netGetMac("eth0", mac);
		sprintf((char *)g_DbEthLinkCfg.ip_info.mac, "%02X-%02X-%02X-%02X-%02X-%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
#endif
		playbackRemoteUpdateParam();
		rtspServerUpdateParam();
		commonSetUpdataBoaParamFlag(1);
		BoaSocketClose();
		rtspServerClose();
		BoaResetPort(DEFAULT_HTTP_PORT);
		rtspServerReset(DEFAULT_RTSP_PORT);
	}
	return ret;
}
/******************************************
  函数功能:网络配置---更新

 ******************************************/

static PortChanInfo portinfo;
static int changingPortInfo = 0;
static void * NetMngChangeThread(void *para)
{
	//PortChanInfo portinfo;
	//memcpy(&portinfo,)
	//if(p_net_mng->dvr_cmd_port != g_DbNetMngCfg.dvr_cmd_port)
	if(portinfo.cur_tcp_port != portinfo.prev_tcp_port)
	{
		playbackRemoteUpdateParam();
	}
	//if(p_net_mng->dvr_data_port != g_DbNetMngCfg.dvr_data_port)
	if(portinfo.cur_rtsp_port != portinfo.prev_rtsp_port)
	{
		rtspServerUpdateParam();
		rtspServerClose();
	}
	//if(p_net_mng->http_port != g_DbNetMngCfg.http_port)
	if(portinfo.cur_http_port != portinfo.prev_http_port)
	{
		//boaUpdateParam();
		commonSetUpdataBoaParamFlag(1);
		BoaSocketClose();
	}


	//if(p_net_mng->dvr_data_port != g_DbNetMngCfg.dvr_data_port)
	if(portinfo.cur_rtsp_port != portinfo.prev_rtsp_port)
	{
		fprintf(stderr,"[%s:%s:%d]:reset rtsp port \n",__FILE__,__FUNCTION__,__LINE__);
		rtspServerReset(portinfo.cur_rtsp_port);
	}
	//if(p_net_mng->http_port != g_DbNetMngCfg.http_port)
	if(portinfo.cur_http_port != portinfo.prev_http_port)
	{
		fprintf(stderr,"[%s:%s:%d]:reset http port \n",__FILE__,__FUNCTION__,__LINE__);
		BoaResetPort(portinfo.cur_http_port);
	}
	changingPortInfo = 0;
	return NULL;
}

static void CreateNetMngChangeThread()
{
	pthread_t NetMngID;
	changingPortInfo = 1;
	if(pthread_create(&NetMngID, NULL, (void *)NetMngChangeThread, NULL) < 0)
	{
		fprintf(stderr,"[%s:%s:%d]:create reset netport thread failed \n",__FILE__,__FUNCTION__,__LINE__);
	}
}

int DbUpdate_NetMngCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{ 
	int ret = 0;
	char sql_cmd[512] = {0};
	char *err_msg = NULL;
	sdk_net_mng_cfg_t *p_net_mng = (sdk_net_mng_cfg_t*)pRecvData;
	unsigned int Brocast[2] = {0};
	struct in_addr addr;
	if(changingPortInfo == 1)
		return DB_OPR_ERROR;
	if(!memcmp(p_net_mng, &g_DbNetMngCfg,sizeof(sdk_net_mng_cfg_t)))
	{
		//Printf("netmng Para no change!\n");
		return sizeof(sdk_net_mng_cfg_t);
	}
	//Printf("netmng Para change!\n");
	
       if(g_DbEthLinkCfg.ip_info.enable_dhcp == 1) // DHCP使能，从网卡中获取广播地址
	{
		if(GetIpResult  == 0)
		{
		  // 获取广播地址
		  netGetBro("eth0", Brocast);
		  memcpy(&addr, &Brocast, 4); 
		  sprintf((char *)p_net_mng->multicast,"%s",inet_ntoa((struct in_addr)addr));
		}
	} 
	sprintf(sql_cmd,"update netManageInfo set def_if_no=%d, multicast_addr='%s', net_mm_server='%s', net_mm_port=%d, http_port=%d, dvr_cmd_port=%d, dvr_data_port=%d where rowId=1",
			p_net_mng->def_if_no, p_net_mng->multicast, p_net_mng->net_manager, p_net_mng->net_manager_port, p_net_mng->http_port, p_net_mng->dvr_cmd_port, p_net_mng->dvr_data_port);
	if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		ret = DB_OPR_ERROR;
	}
	else
	{
		memset(&portinfo,0,sizeof(portinfo));
		portinfo.prev_tcp_port = g_DbNetMngCfg.dvr_cmd_port;
		portinfo.cur_tcp_port = p_net_mng->dvr_cmd_port;
		portinfo.prev_rtsp_port = g_DbNetMngCfg.dvr_data_port;
		portinfo.cur_rtsp_port = p_net_mng->dvr_data_port;
		portinfo.prev_http_port = g_DbNetMngCfg.http_port;
		portinfo.cur_http_port = p_net_mng->http_port;
		
		CreateNetMngChangeThread();
		memcpy((char*)&g_DbNetMngCfg,(char*)p_net_mng,sizeof(sdk_net_mng_cfg_t));
	}
	return ret;
}
/******************************************
函数功能:网络配置---查询全局变量

******************************************/
int GetGlobal_NetMngCfg(void *data, int len, void *pRecvData, int recvDataLen)
{
	int ret = 0;
	sdk_net_mng_cfg_t *p_net_mng = (sdk_net_mng_cfg_t*)pRecvData;	
	//Printf("Get  ethmng from global para! \n");
	memcpy(p_net_mng, &g_DbNetMngCfg,sizeof(sdk_net_mng_cfg_t));
	ret = sizeof(sdk_net_mng_cfg_t);
	return ret;
}

/******************************************
函数功能:网络配置---查询

******************************************/
int DbSelect_NetMngCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0};
	int result = 0;
	int nRow = 0, nColumn = 0;
       char **dbResult;
	char *err_msg = NULL;
	sdk_net_mng_cfg_t *p_net_mng = (sdk_net_mng_cfg_t*)pRecvData;	
	//Printf("Get  ethmng from database! \n");

	memset(&g_DbNetMngCfg, 0, sizeof(sdk_net_mng_cfg_t));
	sprintf(sql_cmd, "select * from netManageInfo");
	result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
	if(SQLITE_OK != result)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		sqlite3_free_table(dbResult);
		return DB_OPR_ERROR;
	}
	g_DbNetMngCfg.def_if_no = (uint16_t)atoi(dbResult[ nColumn + 0]);	
	strcpy((char *)g_DbNetMngCfg.multicast, dbResult[ nColumn + 1]);
	strcpy((char *)g_DbNetMngCfg.net_manager, dbResult[ nColumn + 2]);
	g_DbNetMngCfg.net_manager_port = (uint16_t)atoi(dbResult[ nColumn + 3]);	

	g_DbNetMngCfg.http_port = (uint16_t)atoi(dbResult[ nColumn + 4]);
	g_DbNetMngCfg.dvr_cmd_port = (uint16_t)atoi(dbResult[ nColumn + 5]);
	g_DbNetMngCfg.dvr_data_port = (uint16_t)atoi(dbResult[ nColumn + 6]);

	ret = sizeof(sdk_net_mng_cfg_t);
	sqlite3_free_table(dbResult);
	memcpy(p_net_mng, &g_DbNetMngCfg, sizeof(sdk_net_mng_cfg_t));
	return ret;
}
/******************************************
函数功能:PTZ配置---更新

******************************************/
int DbUpdate_CruiseCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{		
	int i = 0;
	int ret = 0;
	char sql_cmd[3000] = {0};
	char *err_msg = NULL;
	int channel = *(int*)data;	
	sdk_cruise_param_t *p_cruise_cfg = (sdk_cruise_param_t *)pRecvData;
	int CruLinNum = p_cruise_cfg->line;
	char str[248] = {0};
	if(!memcmp(p_cruise_cfg, g_DbCruiseCfg[channel], MAX_CRUISE_LINE_NUM * sizeof(sdk_cruise_param_t)))
	{
		//Printf("cruise Para no change!:%d\n",channel);
		return  MAX_CRUISE_LINE_NUM * sizeof(sdk_cruise_param_t);
	}
	//Printf("cruise Para change:%d!\n",channel);
	
	memset(sql_cmd,0,sizeof(sql_cmd));
	sprintf(sql_cmd,"update ptzCruiseInfo set line_en=%d",p_cruise_cfg->enable );
	for(i = 0; i < MAX_CRUISE_PRESET_NUM; i++)
	{
		memset(str,0,sizeof(str));
		sprintf(str,",cru_en%d=%d,cru_sp%d=%d,cru_dt%d=%d,cru_pre%d_num=%d,cru_pre%d_en=%d",
			   i,p_cruise_cfg->cruise_preset[i].enable,
			   i,p_cruise_cfg->cruise_preset[i].speed,
			   i,p_cruise_cfg->cruise_preset[i].dwell_time,
			   i,p_cruise_cfg->cruise_preset[i].preset.num,
			   i,p_cruise_cfg->cruise_preset[i].preset.enable);
		strcat(sql_cmd,str);
	}
	sprintf(str," where ch=%d and line_num=%d",channel,CruLinNum);
	strcat(sql_cmd,str);
	if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		ret = DB_OPR_ERROR;
	}
	else
	{
		memset(&g_DbCruiseCfg[channel][CruLinNum],0,sizeof(sdk_cruise_param_t));
		memcpy(&g_DbCruiseCfg[channel][CruLinNum], p_cruise_cfg,sizeof(sdk_cruise_param_t));
	}
	return ret;
}
/******************************************
函数功能:PTZ配置---查询全局变量

******************************************/
int GetGlobal_CruiseCfg(void *data, int len, void *pRecvData, int recvDataLen)
{
	int ret = 0;
	int channel = *(int*)data;
	sdk_cruise_param_t *p_cruise_cfg = (sdk_cruise_param_t *)pRecvData;

	//Printf("Get ch%dcruise from global para! \n",channel);
	memcpy(p_cruise_cfg, g_DbCruiseCfg[channel],MAX_CRUISE_LINE_NUM * sizeof(sdk_cruise_param_t));
	ret = MAX_CRUISE_LINE_NUM * sizeof(g_DbCruiseCfg);
	return ret;
}
/******************************************
函数功能:PTZ配置---查询

******************************************/
int DbSelect_CruiseCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int i = 0, j = 0, k = 0;
	int ret = 0;
	char sql_cmd[128] = {0};
	int result = 0;
	char **dbResult;
	char *err_msg = NULL;
	int nRow = 0, nColumn = 0;
	int channel = *(int*)data;
	sdk_cruise_param_t *p_cruise_cfg = (sdk_cruise_param_t *)pRecvData;
	
	//Printf("Get ch%d cruise from database! \n",channel);
	memset(g_DbCruiseCfg, 0, sizeof(g_DbCruiseCfg));
	for(k = 0; (k < MAX_DISPLAY_NUM)&&(k<configGetDisplayNum()); k++)
	{
		for(i = 0; i < MAX_CRUISE_LINE_NUM; i++)
		{
			memset(sql_cmd,0,sizeof(sql_cmd));
			sprintf(sql_cmd, "select * from ptzCruiseInfo where ch=%d and line_num=%d", k,i);
			result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
			if(SQLITE_OK != result)
			{
				Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
				sqlite3_free_table(dbResult);
				return DB_OPR_ERROR;
			}
			if((nRow==0) ||(nColumn ==0))
			{
				memset(sql_cmd,0,sizeof(sql_cmd));
				sprintf(sql_cmd, "drop table ptzCruiseInfo");
				result = sqlite3_get_table(db, sql_cmd, &dbResult, NULL, NULL, &err_msg);
				if(SQLITE_OK != result)
				{
					Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
				}
				sqlite3_free_table(dbResult);
				return DB_OPR_ERROR;
			}
			g_DbCruiseCfg[k][i].line = atoi(dbResult[ nColumn + 2]);
			g_DbCruiseCfg[k][i].enable= atoi(dbResult[nColumn + 3]);
			for(j = 0; j < MAX_CRUISE_PRESET_NUM; j++)
			{
				g_DbCruiseCfg[k][i].cruise_preset[j].num =  atoi(dbResult[ nColumn + 4 + 6 * j ]);
				g_DbCruiseCfg[k][i].cruise_preset[j].enable =  atoi(dbResult[nColumn + 5 + 6 * j ]);
				g_DbCruiseCfg[k][i].cruise_preset[j].speed =  atoi(dbResult[nColumn + 6 + 6 * j ]);
				g_DbCruiseCfg[k][i].cruise_preset[j].dwell_time =  atoi(dbResult[nColumn + 7 + 6 * j ]);
				g_DbCruiseCfg[k][i].cruise_preset[j].preset.num=  atoi(dbResult[nColumn + 8 + 6 * j ]);
				g_DbCruiseCfg[k][i].cruise_preset[j].preset.enable=  atoi(dbResult[nColumn + 9 + 6 * j ]);
			}
			sqlite3_free_table(dbResult);
		}
	}
	
	if(ret != DB_OPR_ERROR)
	{
		memcpy(p_cruise_cfg, g_DbCruiseCfg[channel], MAX_CRUISE_LINE_NUM * sizeof(sdk_cruise_param_t));
	}
	return ret;
}


/******************************************
函数功能:用户配置---修改用户

******************************************/
int DbUpdate_UserCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0};
	char *err_msg = NULL;
	int result = 0;
	char **dbResult;
	int nRow = 0, nColumn = 0;
	int i = 0;
	sdk_user_right_t *p_usr_cfg = (sdk_user_right_t *)data;
	sprintf(sql_cmd, "update userInfo set userPwd='%s', localRight=%u, remoteRight=%u where userName='%s'",
			p_usr_cfg->user.user_pwd, p_usr_cfg->local_right, p_usr_cfg->remote_right, p_usr_cfg->user.user_name);
	if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		ret = DB_OPR_ERROR;
	}
	else
	{
		memset(g_DbUsrCfg, 0, sizeof(g_DbUsrCfg));
		sprintf(sql_cmd, "select * from userInfo");
		result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
		if(SQLITE_OK != result)
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			sqlite3_free_table(dbResult);
			return DB_OPR_ERROR;
		}
		if(nRow > MAX_USER_NUM)
		{
			nRow  = MAX_USER_NUM;
		}
		for (i = 0; i < nRow; i++)
		{
			strcpy((char *)g_DbUsrCfg[i].user.user_name, dbResult[(i + 1) * nColumn + 0]);
			strcpy((char *)g_DbUsrCfg[i].user.user_pwd, dbResult[(i + 1) * nColumn + 1]);
			g_DbUsrCfg[i].local_right = atoi(dbResult[(i + 1) * nColumn + 2]);
			g_DbUsrCfg[i].remote_right = atoi(dbResult[(i + 1) * nColumn + 3]);
			ret += sizeof(sdk_user_right_t);											
		}
		sqlite3_free_table(dbResult);
		g_DbUsrTatolNum  = nRow;
	}
	return ret;
}
/******************************************
函数功能:用户配置---查询

******************************************/
int GetGlobal_UserCfg(void *data, int len, void *pRecvData, int recvDataLen)
{
	int ret = 0;
	sdk_user_right_t *p_usr_cfg = (sdk_user_right_t *)pRecvData;

	//Printf("Get usr value from global para! \n");
	memcpy(p_usr_cfg, g_DbUsrCfg, g_DbUsrTatolNum * sizeof(sdk_user_right_t));
	ret = g_DbUsrTatolNum * sizeof(sdk_user_right_t);
	return ret;
}
/******************************************
函数功能:用户配置---查询

******************************************/
int DbSelect_UserCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int i = 0;
	int ret = 0;
	char sql_cmd[512] = {0};
	int result = 0;
	char **dbResult;
	char *err_msg = NULL;
	int nRow = 0, nColumn = 0;
	sdk_user_right_t *p_usr_cfg = (sdk_user_right_t *)pRecvData;
	
	memset(g_DbUsrCfg, 0, nRow * sizeof(sdk_user_right_t));
	sprintf(sql_cmd, "select * from userInfo");
	result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
	if(SQLITE_OK != result)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		sqlite3_free_table(dbResult);
		return DB_OPR_ERROR;
	}
	if(nRow > MAX_USER_NUM)
	{
		nRow  = MAX_USER_NUM;
	}
	for (i = 0; i < nRow; i++)
	{
		strcpy((char *)g_DbUsrCfg[i].user.user_name, dbResult[(i + 1) * nColumn + 0]);
		strcpy((char *)g_DbUsrCfg[i].user.user_pwd, dbResult[(i + 1) * nColumn + 1]);
		g_DbUsrCfg[i].local_right = atoi(dbResult[(i + 1) * nColumn + 2]);
		g_DbUsrCfg[i].remote_right = atoi(dbResult[(i + 1) * nColumn + 3]);
		memcpy(p_usr_cfg++, &g_DbUsrCfg[i], sizeof(sdk_user_right_t));
		ret += sizeof(sdk_user_right_t);											
	}
	sqlite3_free_table(dbResult);
	g_DbUsrTatolNum  = nRow;
	return ret;
}

/******************************************
函数功能:用户配置---插入

******************************************/
int DbInsert_UserCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0};
	char *err_msg = NULL;
	sdk_user_right_t *p_usr_cfg = (sdk_user_right_t *)data;
	int result = 0;
	char **dbResult;
	int nRow = 0, nColumn = 0;
	int i = 0;
	sprintf(sql_cmd, "insert into userInfo (userName, userPwd, localRight, remoteRight) values ('%s', '%s', %u, %u)",
			p_usr_cfg->user.user_name, p_usr_cfg->user.user_pwd, p_usr_cfg->local_right, p_usr_cfg->remote_right);
	if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		ret = DB_OPR_ERROR;
	}
	else
	{
		memset(g_DbUsrCfg, 0, sizeof(g_DbUsrCfg));
		sprintf(sql_cmd, "select * from userInfo");
		result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
		if(SQLITE_OK != result)
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			sqlite3_free_table(dbResult);
			return DB_OPR_ERROR;
		}
		if(nRow > MAX_USER_NUM)
		{
			nRow  = MAX_USER_NUM;
		}
		for (i = 0; i < nRow; i++)
		{
			strcpy((char *)g_DbUsrCfg[i].user.user_name, dbResult[(i + 1) * nColumn + 0]);
			strcpy((char *)g_DbUsrCfg[i].user.user_pwd, dbResult[(i + 1) * nColumn + 1]);
			g_DbUsrCfg[i].local_right = atoi(dbResult[(i + 1) * nColumn + 2]);
			g_DbUsrCfg[i].remote_right = atoi(dbResult[(i + 1) * nColumn + 3]);
			ret += sizeof(sdk_user_right_t);											
		}
		sqlite3_free_table(dbResult);
		g_DbUsrTatolNum  = nRow;
	}
	return ret;
}
/******************************************
函数功能:用户配置---删除用户

******************************************/
int DbDelete_UserCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0};
	char *err_msg = NULL;
	sdk_user_t *p_usr_cfg = (sdk_user_t *)data;
	int result = 0;
	char **dbResult;
	int nRow = 0, nColumn = 0;
	int i = 0;
	
	sprintf(sql_cmd, "delete from userInfo where userName='%s'", p_usr_cfg->user_name);
	if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		ret = DB_OPR_ERROR;
	}
	else
	{
		memset(g_DbUsrCfg, 0, sizeof(g_DbUsrCfg));
		sprintf(sql_cmd, "select * from userInfo");
		result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
		if(SQLITE_OK != result)
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			sqlite3_free_table(dbResult);
			return DB_OPR_ERROR;
		}
		if(nRow > MAX_USER_NUM)
		{
			nRow  = MAX_USER_NUM;
		}
		for (i = 0; i < nRow; i++)
		{
			strcpy((char *)g_DbUsrCfg[i].user.user_name, dbResult[(i + 1) * nColumn + 0]);
			strcpy((char *)g_DbUsrCfg[i].user.user_pwd, dbResult[(i + 1) * nColumn + 1]);
			g_DbUsrCfg[i].local_right = atoi(dbResult[(i + 1) * nColumn + 2]);
			g_DbUsrCfg[i].remote_right = atoi(dbResult[(i + 1) * nColumn + 3]);
			ret += sizeof(sdk_user_right_t);											
		}
		sqlite3_free_table(dbResult);
		g_DbUsrTatolNum  = nRow;
	}
	return ret;
}
/******************************************
函数功能:用户配置恢复默认

******************************************/
int DbUpdate_DefaultUser(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int i = 0;
	int ret = 0;
	char sql_cmd[512] = {0,};
	int result = 0;
	char *err_msg = NULL;
	char **dbResult;
	int nRow = 0, nColumn = 0;
	sprintf(sql_cmd, "select * from userInfo");
	result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
	if(SQLITE_OK != result)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		sqlite3_free_table(dbResult);
		return DB_OPR_ERROR;
	}
	for (i = 0; i < nRow; i++)
	{
		if(!strcmp(dbResult[(i + 1) * nColumn + 0], "admin") )
		{
			sprintf(sql_cmd, "update userInfo set userPwd='12345', localRight=4294967295, remoteRight=4294967295 where userName='%s'", "admin");
		}
		else
		{
			sprintf(sql_cmd, "delete from userInfo where userName='%s'", dbResult[(i + 1) * nColumn + 0]);
		}

		if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			ret = DB_OPR_ERROR;
		}
	}
	sqlite3_free_table(dbResult);
	memset(&g_DbUsrCfg, 0, sizeof(g_DbUsrCfg));
	strcpy((char *)g_DbUsrCfg[0].user.user_name, "admin");
	strcpy((char *)g_DbUsrCfg[0].user.user_pwd, "12345");
	g_DbUsrCfg[0].local_right = 0xFFFFFFFF;
	g_DbUsrCfg[0].remote_right = 0xFFFFFFFF;
	return ret;
}
/******************************************
函数功能:DDNS配置---更新

******************************************/
int DbUpdate_DdnsCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[1024] = {0};
	sdk_ddns_cfg_t *p_ddns_cfg = (sdk_ddns_cfg_t *)pRecvData;
	int i;
	char *err_msg = NULL;
	if(!memcmp(p_ddns_cfg,&g_DbDdnsCfg[p_ddns_cfg->type],sizeof(sdk_ddns_cfg_t)))
	{
		//Printf("ddns para no change!\n");
		return sizeof(sdk_ddns_cfg_t);
	}
	//Printf("ddns Para change!\n");
	
	if(p_ddns_cfg->interval < 60) // 重连时间应至少为60S，频繁的连接会让DNS服务器拒绝服务!
	{
		p_ddns_cfg->interval = 60;
	}
	memset(sql_cmd,0,sizeof(sql_cmd));
#if 0	
	for(i = 0; i < MAX_DDNS_TYPE; i++)// 将所有DDNS类型的刷新时间和使能保持一致。
	{
		sprintf(sql_cmd, "update ddns set interTime=%d, enable=%d where type=%d", p_ddns_cfg->interval,p_ddns_cfg->enable, i);
		if (SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			ret = DB_OPR_ERROR;
		}
	}
#endif
	sprintf(sql_cmd, "update ddns set interTime=%d, enable=%d, domain='%s', userName='%s', userPwd='%s', userType=%d where type=%d",\
			p_ddns_cfg->interval, p_ddns_cfg->enable, p_ddns_cfg->domain, p_ddns_cfg->user, p_ddns_cfg->pwd, p_ddns_cfg->user_type, p_ddns_cfg->type);

	if (SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		ret = DB_OPR_ERROR;
	}
	else
	{
		DdnsUpdate(1);
		changDdnsSetting(1);		
		memcpy(&g_DbDdnsCfg[p_ddns_cfg->type],p_ddns_cfg, sizeof(sdk_ddns_cfg_t));
		ret = 0;
	}
	return ret;
}
/******************************************
函数功能:DDNS配置---更新

******************************************/
int GetGlobal_DdnsCfg(void *data, int len, void *pRecvData, int recvDataLen)
{
	int ret = 0;
	sdk_ddns_cfg_t *p_ddns_cfg = (sdk_ddns_cfg_t *)pRecvData;
	unsigned char DdnsType  = *(unsigned char *)data;

	//Printf("Get ddns value from global para! \n");
	memcpy(p_ddns_cfg, &g_DbDdnsCfg[DdnsType],sizeof(sdk_ddns_cfg_t));
	ret = sizeof(sdk_ddns_cfg_t);
	return ret;
}
/******************************************
函数功能:DDNS配置---更新

******************************************/
int DbSelect_DdnsCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	int i = 0;
	char sql_cmd[1024] = {0};
	int result = 0;
	char **dbResult;
	int nRow = 0, nColumn = 0;
	sdk_ddns_cfg_t *p_ddns_cfg = (sdk_ddns_cfg_t *)pRecvData;
	unsigned char DdnsType  = *(unsigned char *)data;
	char *err_msg = NULL;
	//Printf("Get ddns value from  database !\n");
	
	sprintf(sql_cmd, "select * from ddns");
	result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
	if(SQLITE_OK != result)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		sqlite3_free_table(dbResult);
		return DB_OPR_ERROR;
	}
	for(i = 0; i < MAX_DDNS_TYPE; i++)
	{

		memset(&g_DbDdnsCfg[i], 0, sizeof(sdk_ddns_cfg_t));
		g_DbDdnsCfg[i].enable = atoi(dbResult[(i + 1) * nColumn + 1]);
		g_DbDdnsCfg[i].type = atoi(dbResult[(i + 1) * nColumn + 2]);
		g_DbDdnsCfg[i].interval = atoi(dbResult[(i + 1) * nColumn + 3]);
		if(g_DbDdnsCfg[i].interval<60)
		{
			g_DbDdnsCfg[i].interval=60;
		}
		
		if(dbResult[nColumn + 3] != NULL)
			strcpy((char *)g_DbDdnsCfg[i].domain, dbResult[(i + 1) * nColumn + 4]);
		if(dbResult[nColumn + 4] != NULL)
			strcpy((char *)g_DbDdnsCfg[i].user, dbResult[(i + 1) * nColumn + 5]);
		if(dbResult[nColumn + 5] != NULL)
			strcpy((char *)g_DbDdnsCfg[i].pwd, dbResult[(i + 1) * nColumn + 6]);
		g_DbDdnsCfg[i].user_type = atoi(dbResult[(i + 1) * nColumn + 7]);
	}
	sqlite3_free_table(dbResult);
	memcpy(p_ddns_cfg, &g_DbDdnsCfg[DdnsType],sizeof(sdk_ddns_cfg_t));
	ret = sizeof(sdk_ddns_cfg_t);	
	return ret;
}

/******************************************
函数功能:DDNS配置恢复默认

******************************************/
int DbUpdate_DefaultDdns(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	int i = 0;
	char sql_cmd[512] = {0,};
	char *err_msg = NULL;
	unsigned char DdnsType = 0;
	//Printf("DDNS default....\n\n");
	for(DdnsType = 0; DdnsType< MAX_DDNS_TYPE; DdnsType++)
	{
		memset(sql_cmd,0,sizeof(sql_cmd));
		sprintf(sql_cmd, "update ddns set enable=0, interTime=60, domain='', userName='', userPwd='', userType=0 where type=%d",DdnsType);
		if (SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			ret = DB_OPR_ERROR;
		}
	}
	memset(&g_DbDdnsCfg, 0, sizeof(g_DbDdnsCfg));
	for(i = 0; i < MAX_DDNS_TYPE; i++)
	{
		g_DbDdnsCfg[i].enable = 0;
		g_DbDdnsCfg[i].interval = 60;
		g_DbDdnsCfg[i].type= i;
		strcpy((char *)g_DbDdnsCfg[i].domain, "");
		strcpy((char *)g_DbDdnsCfg[i].user, "");
		strcpy((char *)g_DbDdnsCfg[i].pwd, "");

	}
	return ret;
}
/******************************************
函数功能:Ntp配置---更新

******************************************/
int DbUpdate_NtpCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0,};
	char *err_msg = NULL;
	sdk_ntp_conf_t *p_ntp_conf = (sdk_ntp_conf_t*)pRecvData;
	if(memcmp(p_ntp_conf, &g_DbNtpCfg,sizeof(sdk_ntp_conf_t)))
	{	
		//Printf("ntp Para change!\n");
		sprintf(sql_cmd,"update ntpCfg set timezone=%d, ntpserver='%s',enable=%d, \
			interval=%d, dateFormat=%d, dateSeparator=%d, timeFormat=%d, port=%d \
			where idx2=%d ",p_ntp_conf->time_zone,p_ntp_conf->ntp_server,p_ntp_conf->enable,\
			p_ntp_conf->interval, p_ntp_conf->date_format, p_ntp_conf->date_separator, \
			p_ntp_conf->time_format, p_ntp_conf->port, 0);
		
		/////Printf("+++%s+++", sql_cmd);
		
		if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			ret = DB_OPR_ERROR;
		}
	}
	if(1==p_ntp_conf->enable)
	{
		Printf("\n\n==========SetNtpTime!==========\n\n");
		SetNtpTime(p_ntp_conf->time_zone,p_ntp_conf->ntp_server, p_ntp_conf->port);
	}
	memset(&g_DbNtpCfg, 0, sizeof(sdk_ntp_conf_t));
	memcpy(&g_DbNtpCfg, p_ntp_conf, sizeof(sdk_ntp_conf_t));
	
	return ret;
}

/******************************************
函数功能:Ntp配置---查询

******************************************/
int GetGlobal_NtpCfg(void *data, int len, void *pRecvData, int recvDataLen)
{
	int ret = 0;
	sdk_ntp_conf_t *p_ntp_conf = (sdk_ntp_conf_t*)pRecvData;

	//Printf("Get ntp value from global para! \n");
	memcpy(p_ntp_conf, &g_DbNtpCfg,sizeof(sdk_ntp_conf_t));
	ret = sizeof(sdk_ntp_conf_t);
	return ret;
}
/******************************************
函数功能:Ntp配置---查询

******************************************/
int DbSelect_NtpCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0,};
	int result = 0;
	char **dbResult; 
	int nRow = 0, nColumn = 0;
	char *err_msg = NULL;
	sdk_ntp_conf_t *p_ntp_conf = (sdk_ntp_conf_t*)pRecvData;
	
	//Printf("Get ntp value from  database !\n");
	memset(&g_DbNtpCfg,0,sizeof(sdk_ntp_conf_t));

	sprintf(sql_cmd, "select * from ntpCfg");
	result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
	if(SQLITE_OK != result)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		sqlite3_free_table(dbResult);
		return DB_OPR_ERROR;
	}
	g_DbNtpCfg.time_zone = atoi(dbResult[nColumn+1]);
	strcpy(g_DbNtpCfg.ntp_server,dbResult[nColumn+2]);
	g_DbNtpCfg.enable = atoi(dbResult[nColumn+3]);
	g_DbNtpCfg.interval = atoi(dbResult[nColumn+4]);
	g_DbNtpCfg.date_format = atoi(dbResult[nColumn+5]);
	g_DbNtpCfg.date_separator = atoi(dbResult[nColumn+6]);
	g_DbNtpCfg.time_format = atoi(dbResult[nColumn+7]);
	g_DbNtpCfg.port = atoi(dbResult[nColumn+8]);

	ret = sizeof(sdk_ntp_conf_t);
	sqlite3_free_table(dbResult);
	memcpy(p_ntp_conf, &g_DbNtpCfg, sizeof(sdk_ntp_conf_t));
	return ret;
}
/******************************************
函数功能:Smtp配置---更新

******************************************/
int DbUpdate_SmtpCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0};
	char *err_msg = NULL;
	sdk_email_conf_t *p_smtp_conf = (sdk_email_conf_t*)pRecvData;
	if(!memcmp(p_smtp_conf, &g_DbEmailCfg,sizeof(sdk_email_conf_t)))
	{
		//Printf("smtp Para no change!\n");
		return sizeof(sdk_email_conf_t);
	}
	//Printf("smtp Para change!\n");
	sprintf(sql_cmd,"update smtpCfg set anonymity=%d, encryption_type=%d, file_enable=%d, health_enable=%d, health_interval=%d, interval=%d, port=%d, email_enable=%d,\
				  passwd='%s' , receiver='%s', sender='%s', smtp_server='%s', theme='%s', user='%s' ", 
		p_smtp_conf->anonymity,p_smtp_conf->encryption_type,p_smtp_conf->file_enable, p_smtp_conf->health_enable, p_smtp_conf->health_interval,p_smtp_conf->interval,p_smtp_conf->port,p_smtp_conf->email_enable,
		p_smtp_conf->passwd,p_smtp_conf->receiver,p_smtp_conf->sender,p_smtp_conf->smtp_server,p_smtp_conf->theme,p_smtp_conf->user);
	//Printf("sql_cmd =%s\n",sql_cmd);
	if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		ret = DB_OPR_ERROR;
	}
	else
	{
		memset(&g_DbEmailCfg, 0, sizeof(sdk_email_conf_t));
		memcpy(&g_DbEmailCfg,p_smtp_conf,sizeof(sdk_email_conf_t));

		changEmailSetting(1);
		updateSmtpSetting(1);
	}
	return ret;
}
/******************************************
函数功能:Smtp配置---查询全局变量

******************************************/
int GetGlobal_SmtpCfg(void *data, int len, void *pRecvData, int recvDataLen)
{
	int ret = 0;
	sdk_email_conf_t *p_smtp_conf = (sdk_email_conf_t*)pRecvData;
	//Printf("Get smtp value from global para! \n");
	memcpy(p_smtp_conf, &g_DbEmailCfg,sizeof(sdk_email_conf_t));
	ret = sizeof(sdk_email_conf_t);
	return ret;
}

/******************************************
函数功能:Smtp配置---查询

******************************************/
int DbSelect_SmtpCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0};
	int result = 0;
	char **dbResult; 
	int nRow = 0, nColumn = 0;
	char *err_msg = NULL;
	sdk_email_conf_t *p_smtp_conf = (sdk_email_conf_t*)pRecvData;
	
	//Printf("Get smtp value from  database !\n");
	memset(&g_DbEmailCfg,0,sizeof(sdk_email_conf_t));

	sprintf(sql_cmd, "select * from smtpCfg");
	result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
	if(SQLITE_OK != result)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		sqlite3_free_table(dbResult);
		return DB_OPR_ERROR;
	}
	g_DbEmailCfg.anonymity= (uint8_t)atoi(dbResult[nColumn+1]);
	g_DbEmailCfg.encryption_type= (uint8_t)atoi(dbResult[nColumn+2]);
	g_DbEmailCfg.file_enable= (uint8_t)atoi(dbResult[nColumn+3]);
	g_DbEmailCfg.health_enable= (uint8_t)atoi(dbResult[nColumn+4]);
	g_DbEmailCfg.health_interval= (uint16_t)atoi(dbResult[nColumn+5]);
	g_DbEmailCfg.interval= (uint16_t)atoi(dbResult[nColumn+6]);
	g_DbEmailCfg.port= (uint16_t)atoi(dbResult[nColumn+7]);
	g_DbEmailCfg.email_enable= (uint8_t)atoi(dbResult[nColumn+8]);
	strcpy(g_DbEmailCfg.passwd,dbResult[nColumn+9]);
	strcpy(g_DbEmailCfg.receiver,dbResult[nColumn+10]);
	strcpy(g_DbEmailCfg.sender,dbResult[nColumn+11]);
	strcpy(g_DbEmailCfg.smtp_server,dbResult[nColumn+12]);
	strcpy(g_DbEmailCfg.theme,dbResult[nColumn+13]);
	strcpy(g_DbEmailCfg.user,dbResult[nColumn+14]);
	ret = sizeof(sdk_email_conf_t);
	sqlite3_free_table(dbResult);
	memcpy(p_smtp_conf,&g_DbEmailCfg, sizeof(sdk_email_conf_t));
	return ret;
}

/******************************************
函数功能:Smtp恢复默认值

******************************************/
int DbUpdate_DefaultEmail(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0,};
	char *err_msg = NULL;
	//Printf("Email default....\n\n");
	memset(sql_cmd,0,sizeof(sql_cmd));
	sprintf(sql_cmd,
		"update smtpCfg set anonymity=0, encryption_type=1, file_enable=0, health_enable=0, \
		health_interval=1, interval=1, port=465, email_enable=0,passwd='' , receiver='',\
		sender='', smtp_server='smtp.naver.com', theme='Notice_Netcury_NVR', user='' "
		);
	if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
	}
	else
	{
		memset(&g_DbEmailCfg, 0, sizeof(sdk_email_conf_t));
		g_DbEmailCfg.anonymity = 0;
		g_DbEmailCfg.encryption_type = 0;
		g_DbEmailCfg.file_enable = 0;
		g_DbEmailCfg.health_enable = 0;
		g_DbEmailCfg.health_interval = 10;
		g_DbEmailCfg.interval = 1;
		g_DbEmailCfg.port = 465;
		g_DbEmailCfg.email_enable = 0;
		strcpy(g_DbEmailCfg.passwd, "");
		strcpy(g_DbEmailCfg.receiver, "");
		strcpy(g_DbEmailCfg.sender, "");
		strcpy(g_DbEmailCfg.smtp_server, "smtp.naver.com");
		strcpy(g_DbEmailCfg.theme, "Notice_Netcury_NVR");
		strcpy(g_DbEmailCfg.user, "");
	}
	return ret;
}
/******************************************
函数功能:P2P配置---更新

******************************************/
int DbUpdate_P2PCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0};
	char *err_msg = NULL;
	sdk_p2p_conf_t *p_p2p_cfg =(sdk_p2p_conf_t*)pRecvData;
	int i = 0;
	if(!memcmp(p_p2p_cfg,g_DbP2pCfg,MAX_P2P_TYPE * sizeof(sdk_p2p_conf_t)))
	{
		//Printf("p2p para no change!\n");
		return 0;
	}
	//Printf("====p2p Para change!\n");
	P2pExchFlg(p_p2p_cfg);
	for(i = 0; i < MAX_P2P_TYPE;i++)
	{
		memset(sql_cmd,0,sizeof(sql_cmd));
		sprintf(sql_cmd,"update p2pCfgTbl set P2pEn=%d, SerialNum='%s' where idx=%d", p_p2p_cfg->P2pEn,p_p2p_cfg->SerialNum,i);
		if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			ret = DB_OPR_ERROR;
		}
		else
		{
			P2pCtrl(i,p_p2p_cfg->P2pEn);
			memset(&g_DbP2pCfg[i], 0, sizeof(sdk_p2p_conf_t));
			memcpy(&g_DbP2pCfg[i], p_p2p_cfg ,sizeof(sdk_p2p_conf_t));
			p_p2p_cfg++;
		}
	}
	return ret;
}
/******************************************
  函数功能:P2P配置---查询

******************************************/
int GetGlobal_P2PCfg(void *data, int len, void *pRecvData, int recvDataLen)
{
	int ret = 0;
	sdk_p2p_conf_t *p_p2p_cfg = (sdk_p2p_conf_t*)pRecvData;

	//Printf("Get p2p value from global para! \n");
	memcpy(p_p2p_cfg, g_DbP2pCfg,MAX_P2P_TYPE *  sizeof(sdk_p2p_conf_t));
	ret = MAX_P2P_TYPE * sizeof(sdk_p2p_conf_t);
	return ret;
}
/******************************************
  函数功能:P2P配置---查询

******************************************/
int DbSelect_P2PCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0};
	int result = 0;
	char **dbResult; 
	int nRow = 0, nColumn = 0;
	char *err_msg = NULL;
	sdk_p2p_conf_t *p_p2p_cfg = (sdk_p2p_conf_t*)pRecvData;
	int i = 0;
	
	//Printf("Get p2p value from  database !\n");
	memset(sql_cmd,0,sizeof(sql_cmd));
	sprintf(sql_cmd, "select * from p2pCfgTbl");
	result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
	if(SQLITE_OK != result)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		sqlite3_free_table(dbResult);
		return DB_OPR_ERROR;
	}
	for(i = 0;i < MAX_P2P_TYPE;i++)
	{
		memset(&g_DbP2pCfg[i], 0 ,sizeof(sdk_p2p_conf_t));
		g_DbP2pCfg[i].P2pEn= (uint8_t)atoi(dbResult[(i+1)*nColumn+2]);
		strcpy( g_DbP2pCfg[i].SerialNum,dbResult[(i+1)*nColumn+3]); 
		strcpy( g_DbP2pCfg[i].AndroidAppUrl,dbResult[(i+1)*nColumn+4]); 
		strcpy( g_DbP2pCfg[i].IosAppUrl,dbResult[(i+1)*nColumn+5]);
		memcpy(p_p2p_cfg++ ,&g_DbP2pCfg[i], sizeof(sdk_p2p_conf_t));
		ret += sizeof(sdk_p2p_conf_t);
	}
	sqlite3_free_table(dbResult);
	return ret;
}
/******************************************
函数功能:图像参数配置---更新

******************************************/
int DbUpdate_ImgAttrCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0};
	char *err_msg = NULL;
	sdk_image_attr_t *p_ImgArr_cfg =  (sdk_image_attr_t *)pRecvData;
	if(!memcmp(p_ImgArr_cfg,&g_DbImgAttrCfg,sizeof(sdk_image_attr_t)))
	{
		//Printf("ImgArr para no change!\n");
		return sizeof(sdk_image_attr_t);
	}
	//Printf("=====ImgArr Para change!\n");
	sprintf(sql_cmd, "update ImgAttrTbl set Bright=%d, Contrast=%d, Saturation=%d, Color=%d ", 
		p_ImgArr_cfg->brightness,p_ImgArr_cfg->contrast,p_ImgArr_cfg->saturation,p_ImgArr_cfg->hue);
	
	if (SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		ret = DB_OPR_ERROR;
	}
	else
	{
		memset(&g_DbImgAttrCfg,0,sizeof(sdk_image_attr_t));
		memcpy(&g_DbImgAttrCfg, p_ImgArr_cfg, sizeof(sdk_image_attr_t));
	}
	return ret;
}
/******************************************
函数功能:图像参数配置---查询全局变量

******************************************/
int GetGlobal_ImgAttrCfg(void *data, int len, void *pRecvData, int recvDataLen)
{
	int ret = 0;
	sdk_image_attr_t *p_ImgArr_cfg = (sdk_image_attr_t *)pRecvData;

	//Printf("Get immArr value from global para! \n");
	memcpy(p_ImgArr_cfg, &g_DbImgAttrCfg,sizeof(sdk_image_attr_t));
	ret = sizeof(sdk_image_attr_t);
	return ret;
}

/******************************************
函数功能:图像参数配置---查询

******************************************/
int DbSelect_ImgAttrCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0};
	int result = 0;
	char *err_msg = NULL;
	char **dbResult; 
	int nRow = 0, nColumn = 0;
	sdk_image_attr_t *p_ImgArr_cfg = (sdk_image_attr_t *)pRecvData;
	
	//Printf("####Get immArr value from  database !\n");
	
	memset(&g_DbImgAttrCfg, 0, sizeof(sdk_image_attr_t));
	memset(sql_cmd,0,sizeof(sql_cmd));
	sprintf(sql_cmd, "select * from ImgAttrTbl");
	result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
	if(SQLITE_OK != result)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		sqlite3_free_table(dbResult);
		return DB_OPR_ERROR;
	}
	g_DbImgAttrCfg.brightness = atoi(dbResult[nColumn+1]);
	g_DbImgAttrCfg.contrast = atoi(dbResult[nColumn+2]);
	g_DbImgAttrCfg.saturation = atoi(dbResult[nColumn+3]);
	g_DbImgAttrCfg.hue= atoi(dbResult[nColumn+4]);
 	sqlite3_free_table(dbResult);
	memcpy(p_ImgArr_cfg, &g_DbImgAttrCfg, sizeof(sdk_image_attr_t));
	ret = sizeof(sdk_image_attr_t);
	return ret;
}


int DbUpdate_pollCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0};
	char *err_msg = NULL;
	sdk_poll_t *p_poll_cfg =  (sdk_poll_t *)pRecvData;
	if(!memcmp(p_poll_cfg,&g_pollCfg,sizeof(sdk_poll_t)))
	{
		//Printf("ImgArr para no change!\n");
		return sizeof(sdk_poll_t);
	}
	//Printf("=====ImgArr Para change!\n");
	sprintf(sql_cmd, "update autoPoll set enable=%d, time=%d, mode=%d", p_poll_cfg->enable,p_poll_cfg->time,p_poll_cfg->mode);
	//Printf("---------sql_cmd =%s\n",sql_cmd);
	
	if (SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		ret = DB_OPR_ERROR;
	}
	else
	{
		memset(&g_pollCfg,0,sizeof(sdk_poll_t));
		memcpy(&g_pollCfg, p_poll_cfg, sizeof(sdk_poll_t));
	}
	return ret;
}
/******************************************
函数功能:自动维护参数配置-----全局变量查询

******************************************/
int GetGlobal_pollCfg(void *data, int len, void *pRecvData, int recvDataLen)
{
	int ret = 0;
	sdk_poll_t *p_poll_cfg = (sdk_poll_t *)pRecvData;

	//Printf("Get p_AutoMainArr_cfg value from global para! \n");
	memcpy(p_poll_cfg, &g_pollCfg,sizeof(sdk_poll_t));
	ret = sizeof(sdk_poll_t);
	return ret;
}

/******************************************
函数功能:自动维护参数配置查询

******************************************/
int DbSelect_pollCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0};
	int result = 0;
	char *err_msg = NULL;
	char **dbResult; 
	int nRow = 0, nColumn = 0;
	sdk_poll_t *p_poll_cfg = (sdk_poll_t *)pRecvData;
	
	//Printf("####Get p_AutoMainArr_cfg value from  database !\n");
	
	memset(&g_pollCfg, 0, sizeof(sdk_poll_t));
	memset(sql_cmd,0,sizeof(sql_cmd));
	sprintf(sql_cmd, "select * from autoPoll");
	result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
	if(SQLITE_OK != result)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		sqlite3_free_table(dbResult);
		return DB_OPR_ERROR;
	}
	g_pollCfg.enable= atoi(dbResult[nColumn+1]);
	g_pollCfg.time= atoi(dbResult[nColumn+2]);
	g_pollCfg.mode= atoi(dbResult[nColumn+3]);
 	sqlite3_free_table(dbResult);
	memcpy(p_poll_cfg, &g_pollCfg, sizeof(sdk_poll_t));
	ret = sizeof(sdk_poll_t);
	return ret;
}

/******************************************
函数功能:自动维护参数配置------更新

******************************************/
int DbUpdate_AutoMaintainCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0};
	char *err_msg = NULL;
	sdk_autoMaintain_attr_t *p_AutoMainArr_cfg =  (sdk_autoMaintain_attr_t *)pRecvData;
	if(!memcmp(p_AutoMainArr_cfg,&g_DbAutoMaintainAttrCfg,sizeof(sdk_autoMaintain_attr_t)))
	{
		//Printf("ImgArr para no change!\n");
		return sizeof(sdk_autoMaintain_attr_t);
	}
	//Printf("=====ImgArr Para change!\n");
	sprintf(sql_cmd, "update autoMaintain set interval=%d, hour=%d ", p_AutoMainArr_cfg->interval,p_AutoMainArr_cfg->hour);
	//Printf("---------sql_cmd =%s\n",sql_cmd);
	
	if (SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		ret = DB_OPR_ERROR;
	}
	else
	{
		memset(&g_DbAutoMaintainAttrCfg,0,sizeof(sdk_autoMaintain_attr_t));
		memcpy(&g_DbAutoMaintainAttrCfg, p_AutoMainArr_cfg, sizeof(sdk_autoMaintain_attr_t));
	}
	return ret;
}
/******************************************
函数功能:自动维护参数配置-----全局变量查询

******************************************/
int GetGlobal_AutoMaintainCfg(void *data, int len, void *pRecvData, int recvDataLen)
{
	int ret = 0;
	sdk_autoMaintain_attr_t *p_AutoMainArr_cfg = (sdk_autoMaintain_attr_t *)pRecvData;

	//Printf("Get p_AutoMainArr_cfg value from global para! \n");
	memcpy(p_AutoMainArr_cfg, &g_DbAutoMaintainAttrCfg,sizeof(sdk_autoMaintain_attr_t));
	ret = sizeof(sdk_autoMaintain_attr_t);
	return ret;
}

/******************************************
函数功能:自动维护参数配置查询

******************************************/
int DbSelect_AutoMaintainCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0};
	int result = 0;
	char *err_msg = NULL;
	char **dbResult; 
	int nRow = 0, nColumn = 0;
	sdk_autoMaintain_attr_t *p_AutoMainArr_cfg = (sdk_autoMaintain_attr_t *)pRecvData;
	
	//Printf("####Get p_AutoMainArr_cfg value from  database !\n");
	
	memset(&g_DbAutoMaintainAttrCfg, 0, sizeof(sdk_autoMaintain_attr_t));
	memset(sql_cmd,0,sizeof(sql_cmd));
	sprintf(sql_cmd, "select * from autoMaintain");
	result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
	if(SQLITE_OK != result)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		sqlite3_free_table(dbResult);
		return DB_OPR_ERROR;
	}
	g_DbAutoMaintainAttrCfg.interval= atoi(dbResult[nColumn+1]);
	g_DbAutoMaintainAttrCfg.hour = atoi(dbResult[nColumn+2]);
 	sqlite3_free_table(dbResult);
	memcpy(p_AutoMainArr_cfg, &g_DbImgAttrCfg, sizeof(sdk_autoMaintain_attr_t));
	ret = sizeof(sdk_autoMaintain_attr_t);
	return ret;
}

/******************************************
函数功能:图像参数配置恢复默认

******************************************/
int DbUpdate_DefaultDisplay(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0,};
	char *err_msg = NULL;
	
	//Printf("Resolution default....\n\n");
	memset(sql_cmd,0,sizeof(sql_cmd));
	sprintf(sql_cmd, "update CommCfgInfo set resolution=1,transparent=0 ");
	if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		ret = DB_OPR_ERROR;
	}
	else
	{
		g_DbCommCfg.resolution = 1;
		g_DbCommCfg.transparent= 18;
	}
	return ret;
	
}
/******************************************
函数功能:预置点---更新
说明:但是每次只传一个预置点的数据。

******************************************/
int DbUpdate_AllPresetCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[128] = {0};
	char *err_msg = NULL;
	sdk_preset_t *p_preset_cfg = (sdk_preset_t *)pRecvData;
	int channel = *(int *)data;
	int PresetNum = p_preset_cfg->num-1;
	//Printf( "==22222222 ptzAllPreset set preset%d_en=%d where ch=%d\n\n", PresetNum, p_preset_cfg->enable, channel);

	if(!memcmp(p_preset_cfg,&g_DbAllPreset[channel].preset[PresetNum],sizeof(sdk_preset_t)))
	{
		//Printf("all_preset para no change!\n");
		return sizeof(sdk_preset_t);
	}
	//Printf("all_preset Para change!\n");
	sprintf(sql_cmd, "update ptzAllPreset set preset%d_en=%d where ch=%d", PresetNum, p_preset_cfg->enable, channel);
	if (SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		ret = DB_OPR_ERROR;
	}
	else
	{
		g_DbAllPreset[channel].preset[PresetNum].enable = p_preset_cfg->enable;
	}
	return ret;
}
/******************************************
  函数功能:预置点---获取
  说明:每次获取某个通道中所有预置点的值。

******************************************/
int GetGlobal_AllPresetCfg(void *data, int len, void *pRecvData, int recvDataLen)
{
	int ret = 0;
	int channel = *(int *)data;
	sdk_preset_all_t *p_all_preset = (sdk_preset_all_t *)pRecvData;

	//Printf("Get all preset from global para! \n");
	memcpy(p_all_preset, &g_DbAllPreset[channel], sizeof(sdk_preset_all_t));
	ret = sizeof(sdk_preset_all_t);
	return ret;
}
/******************************************
  函数功能:预置点---获取
  说明:每次获取某个通道中所有预置点的值。

******************************************/
int DbSelect_AllPresetCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0, i = 0,j = 0;
	char sql_cmd[64] = {0};
	int result = 0;
	char *err_msg = NULL;
	char **dbResult; 
	int nRow = 0, nColumn = 0;
	int channel = *(int *)data;
	sdk_preset_all_t *p_all_preset = (sdk_preset_all_t *)pRecvData;
	
	memset(&g_DbAllPreset, 0, sizeof(g_DbAllPreset));
	for(j = 0; (j<MAX_DISPLAY_NUM)&&(j < configGetDisplayNum()); j ++)
	{
		memset(sql_cmd,0,sizeof(sql_cmd));
		sprintf(sql_cmd, "select * from ptzAllPreset where ch=%d",j);
		result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
		if(SQLITE_OK != result)
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			sqlite3_free_table(dbResult);
			return DB_OPR_ERROR;
		}
		for(i = 0; i < MAX_PRESET_NUM; i++)
		{
			g_DbAllPreset[j].preset[i].num = i + 1;
			g_DbAllPreset[j].preset[i].enable = atoi(dbResult[nColumn + 2 + i]);
		}
	}
	memcpy(p_all_preset, &g_DbAllPreset[channel], sizeof(sdk_preset_all_t));
	return ret;
}

//////FTP参数设置相关功能
int GetGlobal_FtpCfg(void *data, int len, void *pRecvData, int recvDataLen)
{
	int ret = 0;
	sdk_ftp_config *p_ftp_cfg = (sdk_ftp_config *)pRecvData;

	memcpy(p_ftp_cfg, &g_DbFtpCfg,sizeof(sdk_ftp_config));
	ret = sizeof(sdk_ftp_config);
	return ret;
}


int DbUpdate_FtpCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0};
	char *err_msg = NULL;
	sdk_ftp_config *p_ftp_cfg = (sdk_ftp_config *)pRecvData;
	if(!memcmp(p_ftp_cfg,&g_DbFtpCfg, sizeof(sdk_ftp_config)))
	{
		///完全相同
		return sizeof(sdk_ftp_config);
	}

	memset(sql_cmd, 0, sizeof(sql_cmd));
	sprintf(sql_cmd, "update ftpConf set bFtpEnable=%d, servier_ip='%s', username='%s', password='%s', foldername='%s', port=%d",\
		p_ftp_cfg->bFtpEnable,p_ftp_cfg->servier_ip, p_ftp_cfg->username, p_ftp_cfg->password, p_ftp_cfg->foldername, p_ftp_cfg->port);
	///Printf("++++++++-------sql_cmd =%s\n",sql_cmd);
	
	if (SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		ret = DB_OPR_ERROR;
	}
	else
	{
		memset(&g_DbFtpCfg,0,sizeof(sdk_ftp_config));
		memcpy(&g_DbFtpCfg, p_ftp_cfg, sizeof(sdk_ftp_config));
	}
	
	UpdateFtpDate(1);

	return ret;
}


int DbSelect_FtpCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0};
	int result = 0;
	char *err_msg = NULL;
	char **dbResult; 
	int nRow = 0, nColumn = 0;
	sdk_ftp_config *p_ftp_cfg = (sdk_ftp_config *)pRecvData;
	
	//Printf("####Get p_AutoMainArr_cfg value from  database !\n");
	
	memset(sql_cmd,0,sizeof(sql_cmd));
	sprintf(sql_cmd, "select * from ftpConf");
	result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
	if(SQLITE_OK != result)
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		sqlite3_free_table(dbResult);
		return DB_OPR_ERROR;
	}

	memset(&g_DbFtpCfg, 0, sizeof(sdk_ftp_config));
	g_DbFtpCfg.bFtpEnable= atoi(dbResult[nColumn+1]);
	if(dbResult[nColumn + 2] != NULL)
		strcpy(g_DbFtpCfg.servier_ip, dbResult[nColumn + 2]);
	if(dbResult[nColumn + 3] != NULL)
		strcpy(g_DbFtpCfg.username, dbResult[nColumn + 3]);
	if(dbResult[nColumn + 4] != NULL)
		strcpy(g_DbFtpCfg.password, dbResult[nColumn + 4]);
	if(dbResult[nColumn + 5] != NULL)
		strcpy(g_DbFtpCfg.foldername, dbResult[nColumn + 5]);

	g_DbFtpCfg.port = atoi(dbResult[nColumn+6]);

 	sqlite3_free_table(dbResult);
	memcpy(p_ftp_cfg, &g_DbFtpCfg, sizeof(sdk_ftp_config));
	ret = sizeof(sdk_ftp_config);
	
	return ret;
}


int DbUpdate_DefaultFtp(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0,};
	char *err_msg = NULL;
	
	//Printf("Resolution default....\n\n");
	memset(sql_cmd,0,sizeof(sql_cmd));
	sprintf(sql_cmd, "update ftpConf set bFtpEnable=0, servier_ip="", username="", password="", foldername="", port=21 ");
	
	if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		ret = DB_OPR_ERROR;
	}
	else
	{
		memset(&g_DbFtpCfg, 0, sizeof(sdk_ftp_config));
		g_DbFtpCfg.port = 21;
	}
	UpdateFtpDate(1);
	return ret;
}


/******************************************
  函数功能:默认配置--查询


 ******************************************/
int DbUpdate_DefaultCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int i = 0;
	int ret = 0;
	sdk_default_param_t *p_def_cfg = (sdk_default_param_t *)pRecvData;
	DefaultCfg DefaultCfgArr[] = 
	{
		{SDK_PARAM_MASK_ENCODE,		NULL},
		//{SDK_PARAM_MASK_PTZ,			DbUpdate_DefaultAllPreset}, // ALL_PRESET
		{SDK_PARAM_MASK_RECORD,		DbUpdate_DefaultRecord},
		{SDK_PARAM_MASK_NET,			DbUpdate_DefaultNetLink},
		{SDK_PARAM_MASK_CHANNEL,		DbUpdate_DefaultChan},
		{SDK_PARAM_MASK_IMAGE_ATTR,	NULL},
		{SDK_PARAM_MASK_SERIAL,			NULL},
		{SDK_PARAM_MASK_OVERLAY,		NULL},
		{SDK_PARAM_MASK_MOTION,		DbUpdate_DefaultMotion},
		{SDK_PARAM_MASK_COMM,			DbUpdate_DefaultComm},
		{SDK_PARAM_MASK_OSD,			NULL},
		{SDK_PARAM_MASK_USER,			DbUpdate_DefaultUser},
		{SDK_PARAM_MASK_HIDE,			NULL},
		{SDK_PARAM_MASK_VIDEO_LOST,	NULL},
		{SDK_PARAM_MASK_TV,			NULL},
		{SDK_PARAM_MASK_PREVIEW,		DbUpdate_DefaultPreview},
		//{SDK_PARAM_MASK_TOUR,			DbUpdate_DefaultCruise},// CRUISE 
		{SDK_PARAM_MASK_EMAIL,			DbUpdate_DefaultEmail},
		{SDK_PARAM_MASK_DDNS,			DbUpdate_DefaultDdns},
		{SDK_PARAM_MASK_DISPLAY,		DbUpdate_DefaultDisplay}
	};
	if(p_def_cfg->param_mask == PARAM_MASK_ALL)// 恢复出厂默认设置
	{
		sdk_eth_cfg_t  pEthInfo;
		databaseLibInit();
		databaseOperate(SDK_MAIN_MSG_NET_LINK_CFG,SDK_PARAM_GET,NULL,0,&pEthInfo,sizeof(pEthInfo));
		system("cp /mnt/system.db /data/");
		usleep(100);
		strcpy((char *)pEthInfo.ip_info.ip_addr, DEFAULT_IP);
		strcpy((char *)pEthInfo.ip_info.mask, DEFAULT_MASK);
		strcpy((char *)pEthInfo.ip_info.gateway, DEFAULT_GW);
		strcpy((char *)pEthInfo.ip_info.dns1, DEFAULT_DNS1);
		strcpy((char *)pEthInfo.ip_info.dns2, DEFAULT_DNS2);
		databaseOperate(SDK_MAIN_MSG_NET_LINK_CFG,SDK_PARAM_SET,NULL,0,&pEthInfo,sizeof(pEthInfo));
		system("reboot");
	}
	for(i = 0;i < TBL_DEFAULT_CFG_SIZE; i++)
	{
		if((p_def_cfg->param_mask & (0x01<<DefaultCfgArr[i].DefaultEnum)))
		{
			if(DefaultCfgArr[i].DbUpdate_DefaultCfg != NULL)
			{
				ret = DefaultCfgArr[i].DbUpdate_DefaultCfg(data, len, pRecvData, recvDataLen, db);
			}
		}
	}
	return ret;
}

// 获取端口号
int GetPort(unsigned int *RtspPort, unsigned int *HttpPort,unsigned int *remotePort)
{
	unsigned int *Rport = RtspPort;
	unsigned int *Hport = HttpPort;
	unsigned int *RMPort = remotePort;
	*Rport = g_DbNetMngCfg.dvr_data_port;
	*Hport = g_DbNetMngCfg.http_port;
	*RMPort = g_DbNetMngCfg.dvr_cmd_port;
	return 0;
}
int DbMainMsgChannelInfo(int channel,char * ipaddr ,int *port ,char * username,char *password)
{
	if (channel < 0 || channel > MAX_DISPLAY_NUM)
	{
	    return -1;
	}
	strncpy((char *)ipaddr,(char *)g_DbChanCfg[channel].ip_addr,63);
	strncpy((char *)username,(char *)g_DbChanCfg[channel].user.user_name,63);
	strncpy((char *)password,(char *)g_DbChanCfg[channel].user.user_pwd,63);
	*port = g_DbChanCfg[channel].port;
	return 0;

}


int DbGetTimeZone()
{
	return g_DbNtpCfg.time_zone;
}




#if 0


static sdk_alarm_in_cfg_t g_alarm_cfg[MAX_ALARM_IN];
static unsigned char g_alarm_cfg_flag[MAX_ALARM_IN] = {0};

static sdk_upnp_t g_upnp_cfg;
static unsigned char g_upnp_cfg_flag = 0;

static sdk_28181_conf_t g_28181_cfg;
static unsigned char g_28181_cfg_flag = 0;

/******************************************
函数功能:PTZ配置---恢复默认

******************************************/
int DbUpdate_DefaultCruise(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	char sql_cmd[3000] = {0};
	int i, j, k;
	char *err_msg = NULL;
	char str[256] = {0};
	int ret = 0;
	int channel = *(int *)data;
	//Printf("Cruise default....\n\n");
	if(channel == CHANN_TYPE_ALL)
	{
		for(i = 0; (i < MAX_DISPLAY_NUM)&&(configGetDisplayNum()); i++)// 32个通道
		{
			for(j = 0; j < MAX_CRUISE_LINE_NUM; j++)
			{
				memset(sql_cmd,0,sizeof(sql_cmd));
				sprintf(sql_cmd,"update ptzCruiseInfo set line_en=0");
				for(k = 0; k < MAX_CRUISE_PRESET_NUM; k++)
				{
					memset(str,0,sizeof(str));
					sprintf(str,",cru_en%d=0,cru_sp%d=0,cru_dt%d=0,cru_pre%d_num=0,cru_pre%d_en=0", k, k, k,k,k);
					strcat(sql_cmd,str);
				}
				sprintf(str," where ch=%d and line_num=%d",i,j);
				strcat(sql_cmd,str);
				if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
				{
					Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
					ret = DB_OPR_ERROR;
				}
			}
		}
	}
	else
	{
		for(j = 0; j < MAX_CRUISE_LINE_NUM; j++)
		{
			memset(sql_cmd,0,sizeof(sql_cmd));
			sprintf(sql_cmd,"update ptzCruiseInfo set line_en=0");
			for(i = 0; i < MAX_CRUISE_PRESET_NUM; i++)
			{
				memset(str,0,sizeof(str));
				sprintf(str,",cru_en%d=0,cru_sp%d=0,cru_dt%d=0,cru_pre%d_num=0,cru_pre%d_en=0", i, i, i,i,i);
				strcat(sql_cmd,str);
			}
			sprintf(str," where ch=%d and line_num=%d",channel,j);
			strcat(sql_cmd,str);
			if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
			{
				Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
				ret = DB_OPR_ERROR;
			}
		}

	}
	//g_DbCruiseUpdateFlg = 1;
	return ret;
}

/******************************************
函数功能:预置点---恢复默认
说明:每次获取某个通道中所有预置点的值。

******************************************/
int DbUpdate_DefaultAllPreset(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
 	char *sql_cmd = (char *)malloc(4 * 1024 * sizeof(char));
	char *err_msg = NULL;
	char str[128] = {0};
	int i, j;
	int ret = 0;
	int channel = *(int *)data;
	//Printf("AllPreset default....\n\n");
	if(channel == CHANN_TYPE_ALL)
	{
		for(i = 0; (i < MAX_DISPLAY_NUM)&&(configGetDisplayNum()); i++)// 32个通道
		{
			sprintf(sql_cmd, "update ptzAllPreset set ");
			for(j = 0; j < MAX_PRESET_NUM; j++)
			{
				memset(str,0,sizeof(str));
				sprintf(sql_cmd, " ,preset%d_en=0 ", j);
				strcat(sql_cmd,str);
			}
			memset(str,0,sizeof(str));
			sprintf(sql_cmd, " where ch=%d", i);
			strcat(sql_cmd,str);
			if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
			{
				Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
				ret = DB_OPR_ERROR;
			}	
		}
	}
	else
	{
		sprintf(sql_cmd, "update ptzAllPreset set ");
		for(j = 0; j < MAX_PRESET_NUM; j++)
		{
			memset(str,0,sizeof(str));
			sprintf(sql_cmd, " ,preset%d_en=0 ", j);
			strcat(sql_cmd,str);
		}
		memset(str,0,sizeof(str));
		sprintf(sql_cmd, " where ch=%d", channel);
		strcat(sql_cmd,str);
		if(sqlite3_exec(db,sql_cmd,NULL,NULL,&err_msg) != SQLITE_OK)
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			ret = DB_OPR_ERROR;
		}	
	}
	free(sql_cmd);
	//g_DbAllPresetUpdateFlg = 1;
	return ret;
}

// 以下功能暂时未实现!
/******************************************
函数功能:Upnp配置---查询

******************************************/
int DbSelect_UpnpCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int i = 0;
	int ret = 0;
	char sql_cmd[512] = {0};
	int result = 0;
	char **dbResult;
	char *err_msg = NULL;
	int nRow = 0, nColumn = 0;
	sdk_upnp_t *pUpnp = (sdk_upnp_t *)pRecvData;
	if(g_upnp_cfg_flag == 0)
	{
		sprintf(sql_cmd, "select * from upnp");
		result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
		if(SQLITE_OK != result)
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			sqlite3_free_table(dbResult);
			return DB_OPR_ERROR;
		}
		for(i = 0; i < nRow; i++)
		{ 
			memset(pUpnp, 0, sizeof(sdk_upnp_t));
			pUpnp->enable = atoi(dbResult[nColumn + 0]);
			pUpnp->net_type = atoi(dbResult[nColumn + 1]);
			pUpnp->en_auto = atoi(dbResult[nColumn + 2]);
			pUpnp->time = atoi(dbResult[nColumn + 3]);
			strcpy((char *)pUpnp->server_ip, dbResult[nColumn + 4]);
			pUpnp->cmd_port = atoi(dbResult[nColumn + 5]);
			pUpnp->data_port = atoi(dbResult[nColumn + 6]);
			pUpnp->web_port = atoi(dbResult[nColumn + 7]);
			pUpnp->res_port = atoi(dbResult[nColumn + 8]);

			memcpy(&g_upnp_cfg, pUpnp, sizeof(sdk_upnp_t));
			g_upnp_cfg_flag = 1;
			ret = sizeof(sdk_upnp_t);

			break;
		}
		sqlite3_free_table(dbResult);
	}
	else
	{
		memcpy(pUpnp, &g_upnp_cfg, sizeof(sdk_upnp_t));
		ret = sizeof(sdk_upnp_t);
	}
	return ret;
}
/******************************************
函数功能:Upnp配置---更新

******************************************/
int DbUpdate_UpnpCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0};
	sdk_upnp_t *pUpnp = (sdk_upnp_t *)pRecvData;
	char *err_msg = NULL;
	
	if((g_upnp_cfg_flag == 0) || (memcmp(pUpnp, &g_upnp_cfg, sizeof(sdk_upnp_t)) != 0))
	{
		sprintf(sql_cmd, "update upnp set enable=%d, netType=%d, auto=%d, updateTime=%d, serverAddr='%s', cmdPort=%d, dataPort=%d, webPort=%d, resPort=%d",
				pUpnp->enable, pUpnp->net_type, pUpnp->en_auto, pUpnp->time, pUpnp->server_ip, pUpnp->cmd_port, pUpnp->data_port, pUpnp->web_port, pUpnp->res_port);
		if (SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			ret = DB_OPR_ERROR;
		}
		else
		{
			memcpy(&g_upnp_cfg, pUpnp, sizeof(sdk_upnp_t));
			g_upnp_cfg_flag = 1;
		}
	}
	return ret;
}

/******************************************
函数功能:28181配置---查询

******************************************/
int DbSelect_28181Cfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int i = 0, j = 0;
	int ret = 0;
	char sql_cmd[512] = {0};
	int result = 0;
	char **dbResult;
	char *err_msg = NULL;
	int nRow = 0, nColumn = 0;

	sdk_28181_conf_t *p_cfg = (sdk_28181_conf_t*)pRecvData;
	
	if(g_28181_cfg_flag == 0)
	{
		memset(&g_28181_cfg, 0, sizeof(sdk_28181_conf_t));
		sprintf(sql_cmd, "select * from gb28181");
		result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
		if(SQLITE_OK != result)
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			sqlite3_free_table(dbResult);
			return DB_OPR_ERROR;
		}
		for(i = 0; i < nRow; i++)
		{
			g_28181_cfg.enable = atoi(dbResult[nColumn + 0]);
			if(dbResult[nColumn + 1] != NULL)
				strcpy(g_28181_cfg.server_ip, dbResult[nColumn + 1]);
			g_28181_cfg.server_port = atoi(dbResult[nColumn + 2]);
			if(dbResult[nColumn + 3] != NULL)
				strcpy(g_28181_cfg.server_id, dbResult[nColumn + 3]);
			if(dbResult[nColumn + 4] != NULL)
				strcpy(g_28181_cfg.device_id, dbResult[nColumn + 4]);
			for(j = 0; j < 32; j++)
			{
				if(dbResult[nColumn + 5 + j] != NULL)
				{
					strcpy(g_28181_cfg.channel_nvr_id[j], dbResult[nColumn + 5 + j]);
				}
			}
			if(dbResult[nColumn + 37] != NULL)
				strcpy(g_28181_cfg.pwd, dbResult[nColumn + 37]);
			if(dbResult[nColumn + 38] != NULL)
				strcpy(g_28181_cfg.alarm_id, dbResult[nColumn + 38]);
		}
		memcpy(p_cfg, &g_28181_cfg, sizeof(sdk_28181_conf_t));
		ret = sizeof(sdk_28181_conf_t);
		g_28181_cfg_flag = 1;
		sqlite3_free_table(dbResult);
	}
	else
	{
		memcpy(p_cfg, &g_28181_cfg, sizeof(sdk_28181_conf_t));
		ret = sizeof(sdk_28181_conf_t);
	}
	return ret;
}

/******************************************
函数功能:28181配置---更新

******************************************/
int DbUpdate_28181Cfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int ret = 0;
	char sql_cmd[512] = {0};
	char *err_msg = NULL;

	sdk_28181_conf_t *p_cfg = (sdk_28181_conf_t*)pRecvData;
	if((g_28181_cfg_flag == 0) || (memcmp(p_cfg, &g_28181_cfg, sizeof(sdk_28181_conf_t)) != 0))
	{
		sprintf(sql_cmd,"update gb28181 set enable=%d, serverIp='%s', serverPort=%d, serverId='%s', deviceId='%s', \
				channelId1='%s', channelId2='%s', channelId3='%s', channelId4='%s', channelId5='%s', channelId6='%s', channelId7='%s', channelId8='%s', channelId9='%s', channelId10='%s', \
				channelId11='%s', channelId12='%s', channelId13='%s', channelId14='%s', channelId15='%s', channelId16='%s', channelId17='%s', channelId18='%s', channelId19='%s', \
				channelId20='%s', channelId21='%s', channelId22='%s', channelId23='%s', channelId24='%s', channelId25='%s', channelId26='%s', channelId27='%s', channelId28='%s', \
				channelId29='%s', channelId30='%s', channelId31='%s', channelId32='%s', \
				pwd='%s', alarmId='%s'",
				p_cfg->enable, p_cfg->server_ip, p_cfg->server_port, p_cfg->server_id, p_cfg->device_id,
				p_cfg->channel_nvr_id[0], p_cfg->channel_nvr_id[1], p_cfg->channel_nvr_id[2], p_cfg->channel_nvr_id[3], p_cfg->channel_nvr_id[4],
				p_cfg->channel_nvr_id[5], p_cfg->channel_nvr_id[6], p_cfg->channel_nvr_id[7], p_cfg->channel_nvr_id[8], p_cfg->channel_nvr_id[9],
				p_cfg->channel_nvr_id[10], p_cfg->channel_nvr_id[11], p_cfg->channel_nvr_id[12], p_cfg->channel_nvr_id[13], p_cfg->channel_nvr_id[14],
				p_cfg->channel_nvr_id[15], p_cfg->channel_nvr_id[16], p_cfg->channel_nvr_id[17], p_cfg->channel_nvr_id[18], p_cfg->channel_nvr_id[19],
				p_cfg->channel_nvr_id[20], p_cfg->channel_nvr_id[21], p_cfg->channel_nvr_id[22], p_cfg->channel_nvr_id[23], p_cfg->channel_nvr_id[24],
				p_cfg->channel_nvr_id[25], p_cfg->channel_nvr_id[26], p_cfg->channel_nvr_id[27], p_cfg->channel_nvr_id[28], p_cfg->channel_nvr_id[29],
				p_cfg->channel_nvr_id[30], p_cfg->channel_nvr_id[31],
				p_cfg->pwd, p_cfg->alarm_id
				);
		if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			ret = DB_OPR_ERROR;
		}
		else
		{
			memcpy(&g_28181_cfg, p_cfg, sizeof(sdk_28181_conf_t));
			g_28181_cfg_flag = 1;
			//TE_platUpdateParam();
		}
	}
	return ret;
}

/******************************************
函数功能:告警配置---更新

******************************************/
int DbUpdate_AlarmInCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int i = 0, j = 0;
	int ret = 0;
	char sql_cmd[512] = {0};
	char *err_msg = NULL;
	sdk_alarm_in_cfg_t *alarm_in_cfg = (sdk_alarm_in_cfg_t *)pRecvData;
	int channel = *(int *)data;
	unsigned int tmpValueAlarm = 0, tmpValueRecord = 0;
	if(channel == CHANN_TYPE_ALL)
	{
		for(j = 0; j < configGetAlarmInNum(); j++)
		{
			channel = j;
			if((g_alarm_cfg_flag[channel] == 0) || (memcmp(alarm_in_cfg, &g_alarm_cfg[channel], sizeof(sdk_alarm_in_cfg_t)) != 0))
			{
				for(i = 0; i < configGetDisplayNum(); i++)
				{
					if(alarm_in_cfg->alarm_handle.alarm_out_mask[i] == 1)
					{
						tmpValueAlarm |= (1 << i);
					}
					if(alarm_in_cfg->alarm_handle.record_mask[i] == 1)
					{
						tmpValueRecord |= (1 << i);
					}
				}

				sprintf(sql_cmd,"update AlarmInfo set enable=%d, type=%d, enableAlarmOut=%d, alarmOutChannel=%d, alarmOutTime=%d, enableRecord=%d, recordChannel=%d, recordTime=%d where alarmInNo=%d",
						alarm_in_cfg->enable, alarm_in_cfg->type,
						alarm_in_cfg->alarm_handle.alarm_out_enable, tmpValueAlarm, alarm_in_cfg->alarm_handle.alarm_out_time,
						alarm_in_cfg->alarm_handle.record_enable, tmpValueRecord, alarm_in_cfg->alarm_handle.record_time, channel);
				if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
				{
					Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
					ret = DB_OPR_ERROR;
				}
				else
				{
					for(i = 0; i < 7; i++)
					{
						sprintf(sql_cmd, "update AlarmTime set \
								enable0=%d, startHour0=%d, startMin0=%d, stopHour0=%d, stopMin0=%d, \
								enable1=%d, startHour1=%d, startMin1=%d, stopHour1=%d, stopMin1=%d, \
								enable2=%d, startHour2=%d, startMin2=%d, stopHour2=%d, stopMin2=%d, \
								enable3=%d, startHour3=%d, startMin3=%d, stopHour3=%d, stopMin3=%d \
								where alarmInNo=%d and weekday=%d",
								alarm_in_cfg->sched_time[i][0].enable,
								alarm_in_cfg->sched_time[i][0].start_hour, alarm_in_cfg->sched_time[i][0].start_min,
								alarm_in_cfg->sched_time[i][0].stop_hour, alarm_in_cfg->sched_time[i][0].stop_min,
								alarm_in_cfg->sched_time[i][1].enable,
								alarm_in_cfg->sched_time[i][1].start_hour, alarm_in_cfg->sched_time[i][1].start_min,
								alarm_in_cfg->sched_time[i][1].stop_hour, alarm_in_cfg->sched_time[i][1].stop_min,
								alarm_in_cfg->sched_time[i][2].enable,
								alarm_in_cfg->sched_time[i][2].start_hour, alarm_in_cfg->sched_time[i][2].start_min,
								alarm_in_cfg->sched_time[i][2].stop_hour, alarm_in_cfg->sched_time[i][2].stop_min,
								alarm_in_cfg->sched_time[i][3].enable,
								alarm_in_cfg->sched_time[i][3].start_hour, alarm_in_cfg->sched_time[i][3].start_min,
								alarm_in_cfg->sched_time[i][3].stop_hour, alarm_in_cfg->sched_time[i][3].stop_min,
								channel, i);
						if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
						{
							Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
							ret = DB_OPR_ERROR;
						}
					}
				}

				if(ret != DB_OPR_ERROR)
				{
					memcpy(&g_alarm_cfg[channel], alarm_in_cfg, sizeof(sdk_alarm_in_cfg_t));
					alarmSetUpdateParamFlag();
					g_alarm_cfg_flag[channel] = 1;
				}
			}
		}
	}
	else
	{
		if((g_alarm_cfg_flag[channel] == 0) || (memcmp(alarm_in_cfg, &g_alarm_cfg[channel], sizeof(sdk_alarm_in_cfg_t)) != 0))
		{
			for(i = 0; i < configGetDisplayNum(); i++)
			{
				if(alarm_in_cfg->alarm_handle.alarm_out_mask[i] == 1)
				{
					tmpValueAlarm |= (1 << i);
				}
				if(alarm_in_cfg->alarm_handle.record_mask[i] == 1)
				{
					tmpValueRecord |= (1 << i);
				}
			}

			sprintf(sql_cmd,"update AlarmInfo set enable=%d, type=%d, enableAlarmOut=%d, alarmOutChannel=%d, alarmOutTime=%d, enableRecord=%d, recordChannel=%d, recordTime=%d where alarmInNo=%d",
					alarm_in_cfg->enable, alarm_in_cfg->type,
					alarm_in_cfg->alarm_handle.alarm_out_enable, tmpValueAlarm, alarm_in_cfg->alarm_handle.alarm_out_time,
					alarm_in_cfg->alarm_handle.record_enable, tmpValueRecord, alarm_in_cfg->alarm_handle.record_time, channel);
			if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
			{
				Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
				ret = DB_OPR_ERROR;
			}
			else
			{
				for(i = 0; i < 7; i++)
				{
					sprintf(sql_cmd, "update AlarmTime set \
							enable0=%d, startHour0=%d, startMin0=%d, stopHour0=%d, stopMin0=%d, \
							enable1=%d, startHour1=%d, startMin1=%d, stopHour1=%d, stopMin1=%d, \
							enable2=%d, startHour2=%d, startMin2=%d, stopHour2=%d, stopMin2=%d, \
							enable3=%d, startHour3=%d, startMin3=%d, stopHour3=%d, stopMin3=%d \
							where alarmInNo=%d and weekday=%d",
							alarm_in_cfg->sched_time[i][0].enable,
							alarm_in_cfg->sched_time[i][0].start_hour, alarm_in_cfg->sched_time[i][0].start_min,
							alarm_in_cfg->sched_time[i][0].stop_hour, alarm_in_cfg->sched_time[i][0].stop_min,
							alarm_in_cfg->sched_time[i][1].enable,
							alarm_in_cfg->sched_time[i][1].start_hour, alarm_in_cfg->sched_time[i][1].start_min,
							alarm_in_cfg->sched_time[i][1].stop_hour, alarm_in_cfg->sched_time[i][1].stop_min,
							alarm_in_cfg->sched_time[i][2].enable,
							alarm_in_cfg->sched_time[i][2].start_hour, alarm_in_cfg->sched_time[i][2].start_min,
							alarm_in_cfg->sched_time[i][2].stop_hour, alarm_in_cfg->sched_time[i][2].stop_min,
							alarm_in_cfg->sched_time[i][3].enable,
							alarm_in_cfg->sched_time[i][3].start_hour, alarm_in_cfg->sched_time[i][3].start_min,
							alarm_in_cfg->sched_time[i][3].stop_hour, alarm_in_cfg->sched_time[i][3].stop_min,
							channel, i);
					if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
					{
						Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
						ret = DB_OPR_ERROR;
					}
				}
			}

			if(ret != DB_OPR_ERROR)
			{
				memcpy(&g_alarm_cfg[channel], alarm_in_cfg, sizeof(sdk_alarm_in_cfg_t));
				alarmSetUpdateParamFlag();
			}
		}
	}
	return ret;
}


/******************************************
函数功能:告警配置---查询


******************************************/
int DbSelect_AlarmInCfg(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int i = 0, j = 0;
	int ret = 0;
	char sql_cmd[512] = {0};
	int result = 0;
	char **dbResult;
	char *err_msg = NULL;
	int nRow = 0, nColumn = 0;
	sdk_alarm_in_cfg_t *alarm_in_cfg = (sdk_alarm_in_cfg_t *)pRecvData;
	int channel = *(int *)data;
	unsigned int tmpValue = 0;
	if(g_alarm_cfg_flag[channel] == 0)
	{
		sprintf(sql_cmd, "select * from AlarmInfo where alarmInNo=%d", channel);
		result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
		if(SQLITE_OK != result)
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			sqlite3_free_table(dbResult);
			return DB_OPR_ERROR;
		}
		for(i = 0; i < nRow; i++)
		{
			g_alarm_cfg[channel].enable = atoi(dbResult[(i + 1) * nColumn + 1]);
			g_alarm_cfg[channel].type = atoi(dbResult[(i + 1) * nColumn + 2]);

			g_alarm_cfg[channel].alarm_handle.alarm_out_enable = atoi(dbResult[(i + 1) * nColumn + 3]);
			tmpValue = atoi(dbResult[(i + 1) * nColumn + 4]);
			for(j = 0; j < configGetDisplayNum(); j++)
			{
				if((tmpValue & (1 << j)) > 0)
				{
					g_alarm_cfg[channel].alarm_handle.alarm_out_mask[j] = 1;
				}
				else
				{
					g_alarm_cfg[channel].alarm_handle.alarm_out_mask[j] = 0;
				}
			}
			g_alarm_cfg[channel].alarm_handle.alarm_out_time = atoi(dbResult[(i + 1) * nColumn + 5]);

			g_alarm_cfg[channel].alarm_handle.record_enable = atoi(dbResult[(i + 1) * nColumn + 6]);
			tmpValue = atoi(dbResult[(i + 1) * nColumn + 7]);
			for(j = 0; j < configGetDisplayNum(); j++)
			{
				if((tmpValue & (1 << j)) > 0)
				{
					g_alarm_cfg[channel].alarm_handle.record_mask[j] = 1;
				}
				else
				{
					g_alarm_cfg[channel].alarm_handle.record_mask[j] = 0;
				}
			}
			g_alarm_cfg[channel].alarm_handle.record_time = atoi(dbResult[(i + 1) * nColumn + 8]);
		}

		sqlite3_free_table(dbResult);

		sprintf(sql_cmd, "select * from AlarmTime where alarmInNo=%d", channel);
		result = sqlite3_get_table(db, sql_cmd, &dbResult, &nRow, &nColumn, &err_msg);
		if(SQLITE_OK != result)
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			sqlite3_free_table(dbResult);
			return DB_OPR_ERROR;
		}
		for(i = 0; (i < nRow) && (i < 7); i++)
		{
			g_alarm_cfg[channel].sched_time[i][0].enable = atoi(dbResult[(i + 1) * nColumn + 2]);
			g_alarm_cfg[channel].sched_time[i][0].start_hour = atoi(dbResult[(i + 1) * nColumn + 3]);
			g_alarm_cfg[channel].sched_time[i][0].start_min = atoi(dbResult[(i + 1) * nColumn + 4]);
			g_alarm_cfg[channel].sched_time[i][0].stop_hour = atoi(dbResult[(i + 1) * nColumn + 5]);
			g_alarm_cfg[channel].sched_time[i][0].stop_min = atoi(dbResult[(i + 1) * nColumn + 6]);
			g_alarm_cfg[channel].sched_time[i][1].enable = atoi(dbResult[(i + 1) * nColumn + 7]);
			g_alarm_cfg[channel].sched_time[i][1].start_hour = atoi(dbResult[(i + 1) * nColumn + 8]);
			g_alarm_cfg[channel].sched_time[i][1].start_min = atoi(dbResult[(i + 1) * nColumn + 9]);
			g_alarm_cfg[channel].sched_time[i][1].stop_hour = atoi(dbResult[(i + 1) * nColumn + 10]);
			g_alarm_cfg[channel].sched_time[i][1].stop_min = atoi(dbResult[(i + 1) * nColumn + 11]);
			g_alarm_cfg[channel].sched_time[i][2].enable = atoi(dbResult[(i + 1) * nColumn + 12]);
			g_alarm_cfg[channel].sched_time[i][2].start_hour = atoi(dbResult[(i + 1) * nColumn + 13]);
			g_alarm_cfg[channel].sched_time[i][2].start_min = atoi(dbResult[(i + 1) * nColumn + 14]);
			g_alarm_cfg[channel].sched_time[i][2].stop_hour = atoi(dbResult[(i + 1) * nColumn + 15]);
			g_alarm_cfg[channel].sched_time[i][2].stop_min = atoi(dbResult[(i + 1) * nColumn + 16]);
			g_alarm_cfg[channel].sched_time[i][3].enable = atoi(dbResult[(i + 1) * nColumn + 17]);
			g_alarm_cfg[channel].sched_time[i][3].start_hour = atoi(dbResult[(i + 1) * nColumn + 18]);
			g_alarm_cfg[channel].sched_time[i][3].start_min = atoi(dbResult[(i + 1) * nColumn + 19]);
			g_alarm_cfg[channel].sched_time[i][3].stop_hour = atoi(dbResult[(i + 1) * nColumn + 20]);
			g_alarm_cfg[channel].sched_time[i][3].stop_min = atoi(dbResult[(i + 1) * nColumn + 21]);
		}
		sqlite3_free_table(dbResult);
		if(ret != DB_OPR_ERROR)
		{
			g_alarm_cfg_flag[channel] = 1;
			memcpy(alarm_in_cfg, &g_alarm_cfg[channel], sizeof(sdk_alarm_in_cfg_t));
			ret = sizeof(sdk_alarm_in_cfg_t);
		}
	}
	else
	{
		memcpy(alarm_in_cfg, &g_alarm_cfg[channel], sizeof(sdk_alarm_in_cfg_t));
		ret = sizeof(sdk_alarm_in_cfg_t);
	}
	return ret;
}


/******************************************
函数功能:告警配置恢复默认


******************************************/
int DbUpdate_DefaultAlarm(void *data, int len, void *pRecvData, int recvDataLen, sqlite3 *db)
{
	int i = 0, j = 0;
	int ret = 0;
	char sql_cmd[512] = {0,};
	char *err_msg = NULL;
	int channel = *(int *)data;
	for(j = 0; j < configGetAlarmInNum(); j++)
	{
		channel = j;
		sprintf(sql_cmd,"update AlarmInfo set enable=%d, type=%d, enableAlarmOut=%d, alarmOutChannel=%d, alarmOutTime=%d, enableRecord=%d, recordChannel=%d, recordTime=%d where alarmInNo=%d",
				0, 0,
				0, 0, 10,
				0, 0, 10, channel);
		if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
		{
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			ret = DB_OPR_ERROR;
		}
		else
		{
			for(i = 0; i < 7; i++)
			{
				sprintf(sql_cmd, "update AlarmTime set \
						enable0=%d, startHour0=%d, startMin0=%d, stopHour0=%d, stopMin0=%d, \
						enable1=%d, startHour1=%d, startMin1=%d, stopHour1=%d, stopMin1=%d, \
						enable2=%d, startHour2=%d, startMin2=%d, stopHour2=%d, stopMin2=%d, \
						enable3=%d, startHour3=%d, startMin3=%d, stopHour3=%d, stopMin3=%d \
						where alarmInNo=%d and weekday=%d",
						0, 0, 0, 0, 0,
						0, 0, 0, 0, 0,
						0, 0, 0, 0, 0,
						0, 0, 0, 0, 0,
						channel, i);
				if(SQLITE_OK != sqlite3_exec(db, sql_cmd, NULL, NULL, &err_msg))
				{
					Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
					ret = DB_OPR_ERROR;
				}
			}
		}

		if(ret != DB_OPR_ERROR)
		{
			alarmSetUpdateParamFlag();
			g_alarm_cfg_flag[channel] = 0;
		}
	}
	return ret;
}
#endif
