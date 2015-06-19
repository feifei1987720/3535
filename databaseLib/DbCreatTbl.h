#ifndef __DB_CREATE_TBL_H__
#define __DB_CREATE_TBL_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "sqlite3.h"

void DbCreate_DbBkTestTbl(sqlite3 *db,char *TblName);
void DbInit_DbBkTestTbl(sqlite3 *db,char *TblName);
void DbCreate_MotionTbl(sqlite3 *db,char *TblName);
void DbInit_MotionTbl(sqlite3 *db,char *TblName);
void DbCreate_NtpTbl(sqlite3 *db,char *TblName);
void DbInit_NtpTbl(sqlite3 *db,char *TblName);
void DbCreate_LogInfoTbl(sqlite3 *db,char *TblName);
void DbCreate_SmtpTbl(sqlite3 *db,char *TblName);
void DbInit_SmtpTbl(sqlite3 *db,char *TblName);
void DbCreate_CommTbl(sqlite3 *db,char *TblName);
void DbInit_CommTbl(sqlite3 *db,char *TblName);
void DbCreate_ChannelTbl(sqlite3 *db,char *TblName);
void DbInit_ChannelTbl(sqlite3 *db,char *TblName);
void DbCreate_DdnsTbl(sqlite3 *db,char *TblName);
void DbInit_DdnsTbl(sqlite3 *db,char *TblName);
void DbCreate_P2pTbl(sqlite3 *db,char *TblName);
void DbInit_P2pTbl(sqlite3 *db,char *TblName);
void DbCreate_RecTimeTbl(sqlite3 *db,char *TblName);
void DbInit_RecTimeTbl(sqlite3 *db,char *TblName);
void DbCreate_ImgAttrTbl(sqlite3 *db,char *TblName);
void DbInit_ImgAttrTbl(sqlite3 *db,char *TblName);
void DbCreate_CruiseTbl(sqlite3 *db,char *TblName);
void DbInit_CruiseTbl(sqlite3 *db,char *TblName);
void DbCreate_AllPresetTbl(sqlite3 *db,char *TblName);
void DbInit_AllPresetTbl(sqlite3 *db,char *TblName);
void DbCreate_AutoMaintainTbl(sqlite3 *db,char *TblName);
void DbInit_AutoMaintainTbl(sqlite3 *db,char *TblName);
void DbCreate_pollTbl(sqlite3 *db,char *TblName);
void DbInit_pollTbl(sqlite3 *db,char *TblName);
void DbCreate_FtpTbl(sqlite3 *db,char *TblName);
void DbInit_FtpTbl(sqlite3 *db,char *TblName);

#ifdef __cplusplus
}
#endif
#endif//__DB_CREATE_TBL_H__
