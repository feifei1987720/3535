#ifndef __EMAILLIB_H__
#define __EMAILLIB_H__

typedef struct Smtp_Email
{
	char *pHost;				// 服务器域名 
	char *pLoginName;			// 发送 账号 
	char *pLoginPassword;		// 登陆密码 
	char *pSendFrom;			// 发送 账号 
	char *pSendTo;				// 接收账号 
	char *pSendCc;				//  抄送,每个邮件地址之间必须要以分号;相隔，也可为空
	char *pDate;				//   发送时间，时区+800 表示北京时间 
	char *pEmailSubject;		//  邮件名  
	char *pMessage;				//  邮件显示的内容 
	char *pFileName;			// 邮件附件 ，不发附件则为 NULL 
	unsigned short port;		//当SMTP使用SSL时，其端口号必须为465，不使用时是25.本程序只使用SSL，故其端口号须为465
}st_smtp;


int SendEmail(st_smtp *s);


#endif

