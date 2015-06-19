/********************************************************************

	noti.cpp

********************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "ddnsCom.h"


#define     DDNS_CMD_LEN  	1024
#define     DDNS_RCV_LEN   	2048
#define 	Printf printf


static char  baseToChr( unsigned char n )   
{   
	n &= 0x3F;   
	if ( n < 26 )   
		return ( char )( n + 'A' );   
	else if ( n < 52 )   
		return ( char )( n - 26 + 'a' );   
	else if ( n < 62 )   
		return ( char )( n - 52 + '0' );   
	else if ( n == 62 )   
		return '+';   
	else   
		return '/';   
}   

/******************************
  函数功能: Base64加密，用于附件内容的加密


 *******************************/
static int base64Encode( char * const aDest, const unsigned char * aSrc, int aLen )   
{   
	char        * p = aDest;   
	int           i;   
	unsigned char t;   

	for ( i = 0; i < aLen; i++ )   
	{   
		switch ( i % 3 )   
		{   
			case 0 :   
				*p++ = baseToChr( *aSrc >> 2 );   
				t = ( *aSrc++ << 4 ) & 0x3F;   
				break;   
			case 1 :   
				*p++ = baseToChr( t | ( *aSrc >> 4 ) );   
				t = ( *aSrc++ << 2 ) & 0x3F;   
				break;   
			case 2 :   
				*p++ = baseToChr( t | ( *aSrc >> 6 ) );   
				*p++ = baseToChr( *aSrc++ );   
				break;   
		}   
	}   
	if ( aLen % 3 != 0 )   
	{   
		*p++ = baseToChr( t );   
		if ( aLen % 3 == 1 )   
			*p++ = '=';   
		*p++ = '=';   
	}   
	*p = 0;
	return ( p - aDest );
}  

static int ConnectServer(int connectFd, struct sockaddr *pServerSockAddr ,int sockAddrLen, unsigned long timeOut)
{
	int iSave = fcntl(connectFd,F_GETFL);
	fcntl(connectFd, F_SETFL, iSave | O_NONBLOCK);

	if( 0 != connect( connectFd, pServerSockAddr, sockAddrLen) )
	{
		// 使用select设置超时时间
		struct timeval timeOutVal;
		timeOutVal.tv_sec = timeOut;
		timeOutVal.tv_usec = 0;

		fd_set readdSet;
		fd_set writeSet;
		FD_ZERO(&readdSet);
		FD_ZERO(&writeSet);
		FD_SET(connectFd, &readdSet);
		FD_SET(connectFd, &writeSet);
		if(0 < select(connectFd+1, &readdSet, &writeSet, NULL, &timeOutVal))
		{
			//要重新设置为阻塞模式
			fcntl(connectFd, F_SETFL, iSave );
			Printf("Connect to the ddns server fail!Connetting again!\n");
			Printf("error select socket:%s\n\n",strerror(errno));
			return 0;
		}
		//Printf("Connect to the ddns server ok!\n");

		return -1;
	}
	return 0;
	
}

static int GetMessageFromServer(int sockfd, char *pbuf, int len )
{
	int i = 0,  retval = 0;
	int recvLen = 0;
	struct timeval timeout;
	fd_set ReadSet;

	for( i=0; i<10; i++ )
	{
		// FD_SET()应该在每次循环时都被调用， select()超时readSet会被清空
		// 初始化套接字
		FD_ZERO(&ReadSet);
		FD_SET( sockfd, &ReadSet );
		timeout.tv_sec  = 3;
		timeout.tv_usec = 0;
		retval = select(sockfd+1, &ReadSet, NULL, NULL, &timeout );
		if(retval == -1)//错误，退出程序
		{
			perror("select()");
			return -1;
		}
		else if( retval == 0 )//再次轮询
		{
			continue;
		}

		if( FD_ISSET( sockfd, &ReadSet ) )// 测试是否有网络数据
		{
			if ( recvLen > len)// 数据过多
			{
				Printf("%s:%s:%d, recvLen=%d, len=%d", __FUNCTION__, __FILE__ ,__LINE__, recvLen, len);
				return -1;
			}
			retval = recv( sockfd, pbuf + recvLen, len - recvLen, 0 );// 接收数据
			if(retval== -1 )
			{
				perror("recv()");
				Printf("pbuf = %s\n",pbuf);
				return -1;
			}
			if( strstr(pbuf, "\r\n\r\n"))// 搜索接收的数据中是否有"\r\n\r\n"字符串
			{
				recvLen += retval;
				pbuf[recvLen] = '\0';
				len = recvLen;
				return len;
			}

			recvLen += retval;

		}
	}
	return -1;
}

// 获取公网IP
static int  GetExternalIp(char *pIPAddr )
{
	struct sockaddr_in Server;
	int serverfd = 0;
	struct hostent *pServerhe = NULL;
	char package[100]={0};
	char *pReceiveBuf  = NULL;
	int recvLen = 4096;
	int ret;

	pServerhe = gethostbyname("checkip.dyndns.org");
	if( NULL == pServerhe )
	{
		Printf("gethostbyname() error\n");
		//printf("%s:%s:%d, 不能解析域名:%s\n", __FUNCTION__, __FILE__, __LINE__,"checkip.dyndns.org");
		return -1;
	}

	memset(&Server, 0, sizeof(struct sockaddr_in));
	Server.sin_family = AF_INET;
	Server.sin_port =  htons(80);
	memcpy(&(Server.sin_addr), *((struct in_addr **)pServerhe->h_addr_list), sizeof(struct in_addr));

	serverfd = socket(AF_INET, SOCK_STREAM, 0);
	if(serverfd== -1 )
	{
		perror("socket()");
		return -1;
	}
	if(ConnectServer(serverfd, (struct sockaddr*)&Server, sizeof(struct sockaddr), 3) != 0)
	{
		close(serverfd);
		perror("connect():");
		return -1;
	}	

	strcpy(package,"GET http://checkip.dyndns.org/HTTP/1.0\r\n\r\n");
	if ( -1 == send(serverfd,(char*)package,strlen(package),0))
	{
		close(serverfd);
		perror("send():");
		return -1;
	}
	pReceiveBuf = malloc(4096);
	memset(pReceiveBuf, 0, 4096);

	//接收从服务器发来的IP信息
	ret = GetMessageFromServer(serverfd, pReceiveBuf, recvLen);
	if( ret == -1 )
 	{
 		close(serverfd);
		free(pReceiveBuf);
		pReceiveBuf = NULL;
 		return -1;
 	}

	if ( 0 < ret ) 
	{
		char *pIP = strstr(pReceiveBuf ,"Current IP Address:");
		if(pIP != NULL)
		{
			pIP += 19;
			int a=0, b=0, c=0, d=0;
			sscanf(pIP, "%d.%d.%d.%d", &a, &b,&c,&d);
			snprintf(pIPAddr, 16,"%d.%d.%d.%d", a, b, c, d);
		}	
	}	
	free(pReceiveBuf);
	pReceiveBuf = NULL;
	close( serverfd );
	return 0;

}
// DDNS的socket通信

//组装DDNS的URS命令
static int DdnsCmdInfo(char *DdnsCmdBuf,ddns_cfg_t *ddns)
{
	char SrcData[128] = {0};
    char EnData[256] = {0};
	char pClientIp[20] = {0};
	memset(DdnsCmdBuf,0,DDNS_CMD_LEN);
	memset(ddns->server,0,sizeof(ddns->server));
	sprintf(SrcData,"%s:%s",ddns->user,ddns->pwd);
	fprintf(stderr,"SrcData:%s\n",SrcData);
	base64Encode(EnData,SrcData,strlen(SrcData));
	if(GetExternalIp(pClientIp) == -1)
	{
		fprintf(stderr,"Get external IP fail!\n");
		return -1;
	}
	//fprintf(stderr,"ipaddr:%s\n" ,pClientIp);

	///switch(_DDNS_MYQSEE)
	switch(ddns->type)
	{
		case _DDNS_ORAY: 
			strcpy((char *)ddns->server,"ddns.oray.com");
			snprintf(DdnsCmdBuf,(DDNS_CMD_LEN-1),
				"GET /ph/update?hostname=%s&myip=%s HTTP/1.0\r\n"
				"Host: ddns.oray.com\r\n"
				"Authorization: Basic %s\r\n"
				"User-Agent: Oray\r\n\r\n" //
				,ddns->domain, pClientIp, EnData);
			break;
		case _DDNS_NOIP:
			strcpy((char *)ddns->server,"dynupdate.no-ip.com");
			snprintf(DdnsCmdBuf,(DDNS_CMD_LEN-1),
				"GET /nic/update?hostname=%s&myip=%s HTTP/1.0\r\n"
				"Host: dynupdate.no-ip.com\r\n"
				"Authorization: Basic %s\r\n"
				"User-Agent: Bobs/1.2 bob@somedomain.com\r\n\r\n" 
				,ddns->domain,pClientIp, EnData);
			break;
		case _DDNS_DYNDNS:
			strcpy((char *)ddns->server,"members.dyndns.org");
			snprintf(DdnsCmdBuf,(DDNS_CMD_LEN - 1),
				"GET /nic/update?hostname=%s&myip=%s&wildcard=NO&mx=NOCHG&backmx=NOCHG&offline=NO HTTP/1.0\r\n"
				"Host: www.dyndns.com\r\n"  // 之前使用members.dyndns.org不行
				"Authorization: Basic %s\r\n"
				"User-Agent: futura - arduinodydns - 1.1 \r\n\r\n"
				,ddns->domain,pClientIp,EnData);
			break;
		case _DDNS_CHANGEIP:
			strcpy((char *)ddns->server,"changeip.com");
			snprintf(DdnsCmdBuf,(DDNS_CMD_LEN - 1),
				"GET /nic/update?hostname=%s&myip=%s HTTP/1.0\r\n"
				"Host: www.changeip.com\r\n"  
				"Authorization: Basic %s\r\n"
				"User-Agent: ChangeIP \r\n\r\n" 
				,ddns->domain,pClientIp,EnData);
			break;
		case _DDNS_APRESS:
			strcpy((char *)ddns->server,"a-press.org");
			snprintf(DdnsCmdBuf,(DDNS_CMD_LEN - 1),
				"GET /dns?hostname=%s&username=%s&password=%s HTTP/1.1\r\n"
				 "Host: www.a-press.org\r\n\r\n"
				,ddns->domain,ddns->user,ddns->pwd);
			break;
		case _DDNS_MYQSEE:
			strcpy((char *)ddns->server,"www.myq-see.com");
			snprintf(DdnsCmdBuf,(DDNS_CMD_LEN - 1),
				"GET /nic/update?hostname=%s&myip=%s HTTP/1.0\r\n"
				"Host: www.myq-see.com\r\n"  
				"Authorization: Basic %s\r\n"
				"User-Agent: TongLi Inc. - Router 25001 - 1.1\r\n\r\n" 
				,ddns->domain,pClientIp,EnData);
			break;	
		case _DDNS_SMARTEYES:
			strcpy((char *)ddns->server,"www.smart-eyes.com");
			snprintf(DdnsCmdBuf,(DDNS_CMD_LEN - 1),
				"GET /dns?hostname=%s&username=%s&password=%s HTTP/1.1\r\n"
				 "Host:www.smart-eyes.com\r\n\r\n",ddns->domain,ddns->user,ddns->pwd);
			break;
		default:
			break;
	}
	fprintf(stderr,"DDNS_CMD:\n%s",DdnsCmdBuf);
	return 0;
}
static int DdnsRspCode(char *RecvBuf)
{
	char RspSusCode[][16] = {"good","nochg"};
	int i = 0;
	char GoodCode = 0;
	for(i = 0; i < sizeof(RspSusCode); i++) // 杩??姝ｅ父
	{   
	    if(NULL != strstr(RecvBuf,RspSusCode[i]))
	    {   
	        GoodCode = 1;
	        break;
	    }   
	}   
	if (GoodCode)
	{
	    fprintf(stderr,"( %s:%s:%d )    _DDNS ip update:ok!\n", __FILE__, __FUNCTION__, __LINE__ );
	    return 0;
	}
	else
	{
	    fprintf(stderr,"( %s:%s:%d )    _DDNS ip update:fail!!!!\n", __FILE__, __FUNCTION__, __LINE__ );
	    return -1;
	}
	return 0;

}


static int DdnsSockComm(ddns_cfg_t *ddns)
{
	struct sockaddr_in ser_addr;
	int sockfd = 0;
	int  ret = 0;
	char   str[32] = {0};
	char **pptr = NULL;
	char DdnsCmdBuf[DDNS_CMD_LEN] = {0};
	char ReceiveBuf[DDNS_RCV_LEN] = {0};
       struct hostent *phost = NULL;
	
	DdnsCmdInfo(DdnsCmdBuf,ddns);
	phost = gethostbyname((char *)ddns->server);
       if (NULL == phost)
	{
	       fprintf(stderr,"gethostbyname() error\n");
		return -1;//
	}
	else
	{
		 pptr=phost->h_addr_list;
		 fprintf(stderr,"gethostbyname() sucssess! address:%s\n",  inet_ntop(phost->h_addrtype, *pptr, str, sizeof(str)));
	}
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port =  htons(80);
	memcpy(&(ser_addr.sin_addr), *((struct in_addr **)phost->h_addr_list), sizeof(struct in_addr));
	memset(ser_addr.sin_zero,0, sizeof(ser_addr.sin_zero));
       sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if( -1 == sockfd )
	{
		perror(" socket() error\n");
		return -1;
	}	 
	if(ConnectServer(sockfd, (struct sockaddr*)&ser_addr, sizeof(struct sockaddr), 10) != 0)
	{
		perror("connect()");		
		close(sockfd);
		return -1;
	}
	if(send(sockfd, DdnsCmdBuf, strlen(DdnsCmdBuf), 0) == -1)
	{
		perror("send() data to server");
		close(sockfd);
		return -1;
	}
	memset(ReceiveBuf,0,sizeof(ReceiveBuf));
	ret=GetMessageFromServer(sockfd,ReceiveBuf,sizeof(ReceiveBuf));
	close(sockfd);
	if(ret == -1)
	{
		fprintf(stderr,"GetMessagErr!!!\n\n");
		return ret;
	}
	//fprintf(stderr,"recieve:\n%s\n",ReceiveBuf);
	DdnsRspCode(ReceiveBuf);
	return 0;
}


int StartDdns(ddns_cfg_t *ddns)
{
	if(ddns->type == _DDNS_NOTI)
	{
		castNotify(ddns);
	}
	else
	{
		DdnsSockComm(ddns);
	}
	return 0;
}


