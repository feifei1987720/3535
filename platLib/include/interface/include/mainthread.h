#ifndef __MAINTHREAD_H__
#define __MAINTHREAD_H__
struct CMAVEnc
{
};

struct CFsManage 
{
};


typedef struct 
{
	U16 u16Port;
}setupparameter_t;

typedef setupparameter_t SYSPARAMEMTER_S;

typedef struct
{
	SYSPARAMEMTER_S *m_pSysPara;
	CMAVEnc   *m_cMAVEnc;
	CFsManage *m_pFileManager;
}CMainThread;
#endif
