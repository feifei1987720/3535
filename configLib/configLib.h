/*
*******************************************************************************
** 
*******************************************************************************
*/

#ifndef __CONFIG_LIB_H__
#define __CONFIG_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif


/*  */
typedef struct system_config_t
{
	unsigned char cpuType;
	unsigned char sataNum;
	unsigned char esataNum;
	unsigned char alarmInNum;
	unsigned char alarmOutNum;
	unsigned char displayNum;
	unsigned char printfEnable;
}system_config_t;



int configGetCpuType();
int configGetSataNum();
int configGetEsataNum();
int configGetAlarmInNum();
int configGetAlarmOutNum();
int configGetDisplayNum();
int configGetPrintfEnable();

void configLibInit();
void configLibDestroy();


#ifdef __cplusplus
}
#endif


#endif//__CONFIG_LIB_H__

