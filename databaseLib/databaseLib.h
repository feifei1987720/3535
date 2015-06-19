/*
*******************************************************************************
*
**  : 
*******************************************************************************
*/

#ifndef __DATABASE_LIB_H__
#define __DATABASE_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "sqlite3.h"

#define ENABLE_MODIFY_MAC     //0自动获取MAC地址 1手动修改MAC地址

#define TBL_CFG_SIZE	(sizeof(DbMainMsgCfgArr)/sizeof(DbMainCfg))
#define TBL_INIT_SIZE	(sizeof(TblInitArr)/sizeof(DbTblInit))
#define ADD_FIELD_EMPTY   0
#define SELECT_PARA_TURE 		1
#define SELECT_PARA_FALSE 		0	

typedef void (*DbTblCreate)(sqlite3 *db,char *TblName);
typedef void (*DbTblValueInit)(sqlite3 *db,char *TblName);

typedef struct DbTblInit{
    char *TblName;  // 表名
    char *AddField;  // 新增的字段名，不能为空。如果没有新增字段，则应为表中某个已存在字段
    DbTblCreate TblCreate;  // 创建数据库表单
    DbTblValueInit  TblValueInit; // 初始化表单
}DbTblInit;

typedef int (*DbUpdate)(void *data, int len, void *pRecvData, int recvDataLen,sqlite3 *db);
typedef int (*DbSelect)(void *data, int len, void *pRecvData, int recvDataLen,sqlite3 *db);
typedef int (*DbInsert)(void *data, int len, void *pRecvData, int recvDataLen,sqlite3 *db);
typedef int (*DbDelete)(void *data, int len, void *pRecvData, int recvDataLen,sqlite3 *db);
typedef int (*GetGlobalVal)(void *data, int len, void *pRecvData, int recvDataLen);
typedef struct DbMainCfg{
    int MainType; // 主类型
    DbUpdate DbMainCfgUpdate;  // 数据库更新
    DbSelect DbMainCfgSelect;     // 数据库选择
    DbInsert DbMainCfgInsert;     // 数据库插入
    DbDelete DbMainCfgDelete;	// 数据库删除	
    int SelectFlg;				// 数据库获取标志
    GetGlobalVal DbGetGlobalPara;    // 获取全局变量值
}DbMainCfg;


/*  */
void databaseLibInit();

/*  */
//void databaseLibDestroy();

/*
 * 
 * type:	
 * data:	
 * len:		
 * pRecvData:	buf
 * recvDataLen:	
 * */
int databaseOperate(int type, int subType, void *data, int len, void *pRecvData, int recvDataLen);


#ifdef __cplusplus
}
#endif


#endif//__DATABASE_LIB_H__

