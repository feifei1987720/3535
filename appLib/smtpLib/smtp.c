/********************************************
  SMTP程序说明:
  1. 本程序实现了邮件发送的基本功能，如SSLj加密、带附件发送、抄送等。
  2. 由于本程序中使用了SSL加密技术，会用到OpenSSL的头文件和静态库文件，
  所以，在编译此程序前，应该正确编译OpenSSL，OpenSSL的编译请参照相关说明。
  3. 当有多个抄送人时，每个抄送人之间必须使用分号;相隔,不然将无法解析
 ********************************************/
#include <stdlib.h>    
#include <stdio.h>    
#include <string.h>    
#include <netdb.h>    
#include <sys/types.h>    
#include <sys/stat.h>    
#include <unistd.h>     
#include <fcntl.h>    
#include <sys/ioctl.h>    
#include <netinet/in.h>    
#include <sys/socket.h>    
#include <linux/if_ether.h>    
#include <net/if.h>    
#include <errno.h>  
#include <time.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>    
#include <signal.h>
// SSL的头文件，这些头文件包含在OpenSSL的源码中。
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>

#include "smtp.h"


#define SSL_SEND(Content, SSL)			\
	do								\
	{								\
		if(SslSend(Content,SSL) == -1) 	\
		{ 							\
									\
			return -1; 				\
		}							\
	} while(0);						\

#define SSL_SEND_FILE(Content, SSL, fp)			\
	do								\
	{								\
		if(SslSend(Content,SSL) == -1) 	\
		{ 							\
			if(fp != NULL)				\
			{						\
				fclose(fp);			\
				fp = NULL;			\	
			} 						\
			return -1; 				\
		}							\
	} while(0);						\



static int SendDataN(int fd, char* data, int len)
{
	int leftlen = len;
	int pos = 0;
	int sendlen;
	int ret = -1;
	int ret11count = 0;

	fd_set writefds;   
	struct timeval 	tv;

	FD_ZERO(&writefds);
	FD_SET(fd, &writefds);
	tv.tv_sec = 5; 
	tv.tv_usec= 10;	
	
	ret = select(fd + 1, NULL, &writefds, NULL, &tv);
	if(ret <= 0)
	{
		return -1;
	}

	if (FD_ISSET(fd, &writefds))
    	{
		do	
		{
			ret= send(fd, data + pos, leftlen, MSG_DONTWAIT | MSG_NOSIGNAL);

			if(ret <= 0)
			{
				
				if(errno == 11)
				{
					ret11count++;
					printf("%s %d: ret %d <= 0 !errno = %d\n", __func__, __LINE__, ret, errno);

				if(ret11count >= 200000000)
				{
					printf("%s %d: ret %d <= 0 !ret11count = %d\n", __func__, __LINE__, ret, ret11count);
					return -1;
				}
				
				usleep(30000);
				continue;
			}
	
			return -1;
		}

		ret11count = 0;
		
		if(ret != leftlen)
		{
			printf("%s %d: ret != sendlen !%d != %d\n", __func__, __LINE__, ret, sendlen);
			//usleep(1);
		}

		leftlen -= ret;
		pos += ret;

			//usleep(1);
		}while(leftlen > 0);
	}

	return 0;
}


char  BaseToChr( unsigned char n )   
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



const int  BASE64_MAXLINE = 2048;   
const char EOL[] = "\r\n";   
const char BASE64_TAB[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"   
      "abcdefghijklmnopqrstuvwxyz0123456789+/";   


//int Base64ToAnsi( char *dst,int nOutLen, char *src,int nInLen);   
int ANSIToBase64(const char *szInANSI, int nInLen, char *szOutBase64, int nOutLen)   
{   
 //Input Parameter validation   
 if ((szInANSI == NULL) || (nInLen == 0) || (szOutBase64 == NULL) || (nOutLen == 0))   
  return 0;   
 if (nOutLen < (nInLen*4/3 + 1 + nInLen*4/3/BASE64_MAXLINE*2 + 1 + 4))   
  return 0;   
   
 //Set up the parameters prior to the main encoding loop   
 int nInPos  = 0;   
 int nOutPos = 0;   
 int nLineLen = 0;   
 int c1, c2, c3;   
 int i;   
   
 // Get three characters at a time from the input buffer and encode them   
 for (i=0; i<nInLen/3; ++i)   
 {   
  //Get the next 2 characters   
  c1 = szInANSI[nInPos++] & 0xFF;   
  c2 = szInANSI[nInPos++] & 0xFF;   
  c3 = szInANSI[nInPos++] & 0xFF;   
   
  //Encode into the 4 6 bit characters   
  szOutBase64[nOutPos++] = BASE64_TAB[c1 >> 2];   
  szOutBase64[nOutPos++] = BASE64_TAB[((c1 << 4) | (c2 >> 4)) & 0x3F];   
  szOutBase64[nOutPos++] = BASE64_TAB[((c2 << 2) | (c3 >> 6)) & 0x3F];   
  szOutBase64[nOutPos++] = BASE64_TAB[c3 & 0x3F];   
  nLineLen += 4;   
    
  //Handle the case where we have gone over the max line boundary   
  if (nLineLen > BASE64_MAXLINE - 4)   
  {   
   szOutBase64[nOutPos++] = EOL[0];   
   szOutBase64[nOutPos++] = EOL[1];   
   nLineLen = 0;   
  }   
 }   
// Encode the remaining one or two characters in the input buffer   
 switch (nInLen % 3)   
 {   
  case 0:   
  {   
   szOutBase64[nOutPos++] = EOL[0];   
   szOutBase64[nOutPos++] = EOL[1];   
   break;   
  }   
  case 1:   
  {   
   c1 = szInANSI[nInPos] & 0xFF;   
   szOutBase64[nOutPos++] = BASE64_TAB[(c1 & 0xFC) >> 2];   
   szOutBase64[nOutPos++] = BASE64_TAB[((c1 & 0x03) << 4)];   
   szOutBase64[nOutPos++] = '=';   
   szOutBase64[nOutPos++] = '=';   
   szOutBase64[nOutPos++] = EOL[0];   
   szOutBase64[nOutPos++] = EOL[1];   
   break;   
  }   
  case 2:   
  {   
   c1 = szInANSI[nInPos++] & 0xFF;   
   c2 = szInANSI[nInPos] & 0xFF;   
   szOutBase64[nOutPos++] = BASE64_TAB[(c1 & 0xFC) >> 2];   
   szOutBase64[nOutPos++] = BASE64_TAB[((c1 & 0x03) << 4) | ((c2 & 0xF0) >> 4)];   
   szOutBase64[nOutPos++] = BASE64_TAB[((c2 & 0x0F) << 2)];   
   szOutBase64[nOutPos++] = '=';   
   szOutBase64[nOutPos++] = EOL[0];   
   szOutBase64[nOutPos++] = EOL[1];   
   break;   
  }   
  default:   
  {   
   return 0;   
  }   
 }   
   
 szOutBase64[nOutPos] = 0;   
   
 return nOutPos;   
}   


/******************************
函数功能: Base64加密，用于附件内容的加密


*******************************/
static int Base64Encode( char * const aDest, const unsigned char * aSrc, int aLen )   
{   
    char   *p = aDest;   
    int     i;   
    unsigned char t;   
   
    for ( i = 0; i < aLen; i++ )   
    {   
        switch ( i % 3 )   
        {   
        case 0 :   
            *p++ = BaseToChr( *aSrc >> 2 );   
            t = ( *aSrc++ << 4 ) & 0x3F;   
            break;   
        case 1 :   
            *p++ = BaseToChr( t | ( *aSrc >> 4 ) );   
            t = ( *aSrc++ << 2 ) & 0x3F;   
            break;   
        case 2 :   
            *p++ = BaseToChr( t | ( *aSrc >> 6 ) );   
            *p++ = BaseToChr( *aSrc++ );   
            break;   
        }   
    }   
    if ( aLen % 3 != 0 )   
    {   
        *p++ = BaseToChr( t );   
        if ( aLen % 3 == 1 )   
            *p++ = '=';   
        *p++ = '=';   
    }   
    *p = 0;
    return ( p - aDest );
}   


/******************************
函数功能: Base64加密，主要用于用户名和密码的加密。


*******************************/
static int Base64_Code(unsigned char *s,unsigned char *d)   
{   
	char CharSet[64]=   
	{   
        'A','B','C','D','E','F','G','H',   
        'I','J','K','L','M','N','O','P',   
        'Q','R','S','T','U','V','W','X',   
        'Y','Z','a','b','c','d','e','f',   
        'g','h','i','j','k','l','m','n',   
        'o','p','q','r','s','t','u','v',   
        'w','x','y','z','0','1','2','3',   
        '4','5','6','7','8','9','+','/'   
	};   
	unsigned char In[3];   
	unsigned char Out[4];   
	int cnt=0;   
	if(!s||!d) return 0;   
	for(;*s!=0;)   
	{   
        if(cnt+4>76)   
        {   
            *d++='\n';   
            cnt=0;   
        }   
        if(strlen((char*)s) >=3)   
        {   
            In[0]=*s;   
            In[1]=*(s+1);   
            In[2]=*(s+2);   
            Out[0]=In[0]>>2;   
            Out[1]=(In[0]&0x03)<<4|(In[1]&0xf0)>>4;   
            Out[2]=(In[1]&0x0f)<<2|(In[2]&0xc0)>>6;   
            Out[3]=In[2]&0x3f;   
            *d=CharSet[Out[0]];   
            *(d+1)=CharSet[Out[1]];   
            *(d+2)=CharSet[Out[2]];   
            *(d+3)=CharSet[Out[3]];   
            s+=3;   
            d+=4;   
        }   
        else if(strlen((char*)s)==1)   
        {   
            In[0]=*s;   
            Out[0]=In[0]>>2;   
            Out[1]=(In[0]&0x03)<<4|0;   
            *d=CharSet[Out[0]];   
            *(d+1)=CharSet[Out[1]];   
            *(d+2)='=';   
            *(d+3)='=';   
            s+=1;   
            d+=4;   
        }   
        else if(strlen((char*)s)==2)   
        {   
            In[0]=*s;   
            In[1]=*(s+1);   
            Out[0]=In[0]>>2;   
            Out[1]=(In[0]&0x03)<<4|(In[1]&0xf0)>>4;   
            Out[2]=(In[1]&0x0f)<<2|0;   
            *d=CharSet[Out[0]];   
            *(d+1)=CharSet[Out[1]];   
            *(d+2)=CharSet[Out[2]];   
            *(d+3)='=';   
            s+=2;   
            d+=4;   
        }   
        cnt+=4;   
	}   
	*d='\0';   
	return 1;   
}  


/*BASE64加解密通用常量字符串数组，此常量用于网络通用加解密算法*/
const char G_BASE_NORMALBASE64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";


/************************************************************
函数:BaseEncode
功能:Base64加密
参数:
            data   待加密数据的首地址
            data_len	 待加密的数据长度
            dst			 密文存放的首地址
返回：加密后的数据长度，与strlen(dst)是相等的
************************************************************/
size_t BaseEncode(const void *data,size_t data_len,void *dst)
{
	if((data == NULL)||(dst == NULL)||(data_len == 0))
			return 0;
	int i = 0,prepare = 0,temp = 0,tmp = 0;
	size_t length = 0;
	char changed[4];
	char *f = NULL;

	f=(char *)dst;
	while(tmp < data_len)
	{
		temp = 0;
		prepare = 0;
		memset(changed,0x00,sizeof(changed));
		while(temp < 3)
		{
			if(tmp >= data_len)
				break;
			prepare = ((prepare<<8)|(*((char *)data+tmp)&0xFF));
			tmp++;
			temp++;
		}
		prepare=(prepare << ((3-temp)*8));
		for(i = 0; i < 4; i++)
		{
			if(temp < i)
				changed[i] = 0x40;
			else
				changed[i] = (prepare>>((3-i)*6))&0x3F;
			*f = G_BASE_NORMALBASE64[(int)changed[i]];
			f++;
			length++;
		}
	}
	*f = '\0';
	return length;
}


/******************************
函数功能: 获取SSL证书



*******************************/
void ShowCerts(SSL *ssl)
{
 	X509 *cert = NULL;
 	char *line = NULL;//(char *)malloc(1024);
	cert = SSL_get_peer_certificate(ssl);
 	if (cert != NULL) 
	{
 	 	printf("Get certification OK!\n");
		line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
		//printf("certification:%s\n", line);
 		//free(line);
 		line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
 		//printf("Issuer:%s\n", line);
 		//free(line);
 		X509_free(cert);
 	} 
	else
	{
	   // free(line);
 		//X509_free(cert);
		printf("No certification!\n");
	}
}


/******************************
函数功能: 初始化SSL库



*******************************/
SSL_CTX *SslInit()
{
	SSL_CTX *ctx = NULL;
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();
		
	if(ctx == NULL)
	{
		ctx = SSL_CTX_new(SSLv23_client_method());
		if (ctx == NULL) 
		{
			ERR_print_errors_fp(stdout);
			return NULL;
		}
	}
	
	return ctx;
}


/******************************
函数功能:SSL连接


*******************************/
SSL * SslConnect(int sockfd,SSL_CTX *ctx)
{
#if 1
	SSL * ssl = NULL;
	//ctx = SslInit();
	ssl = SSL_new(ctx);

	if(ssl == NULL)
	{
		return NULL;
	}
	
	SSL_set_fd(ssl, sockfd);
	if (SSL_connect(ssl) == -1)
	{
		SSL_free(ssl);
		ssl = NULL;
		ERR_print_errors_fp(stderr);
		return NULL;
	}
	else 
	{
		printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
		//ShowCerts(ssl);
	}
	return ssl;
#else
	SSL * ssl;

	fd_set writefds;
	struct timeval wait_time = {5, 0};

	ctx = SslInit();
	ssl = SSL_new(ctx);
	SSL_set_fd(ssl, sockfd);

	FD_ZERO(&writefds);
	FD_CLR(sockfd, &writefds);
	FD_SET(sockfd, &writefds);

	
	if (SSL_connect(ssl) != -1)
	{
		printf("%s %d: Connect immediately!\n", __func__, __LINE__);
		return ssl;
	}
	else
	{
		int iRet1 = select(sockfd + 1, NULL, &writefds, NULL, &wait_time);  
		if (iRet1 < 0)  
		{  
			printf("%s %d: connect error\n", __func__, __LINE__);  
			return NULL;  
		}  
		else if (!iRet1)  
		{  
			printf("%s %d: timeout error\n", __func__, __LINE__);  
			return NULL;  
		}  
		else  
		{  
			if (FD_ISSET(sockfd, &writefds))  
			{  
				//int ret = SSL_connect(ssl);
				printf("%s %d: ret = %d connect success\n", __func__, __LINE__, ret);  
				printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
				ShowCerts(ssl);

				return ssl;
			}  
			else
			{
				ERR_print_errors_fp(stderr);
				return NULL;  
			}
		}  
	}


#endif
}


/******************************
函数功能:SSL数据发送


*******************************/
int SslSend(const char* cmd, SSL * ssl)
{
	int err;
	//printf("Cmd: %s",cmd);
	if(SSL_write (ssl, cmd, strlen(cmd)) == -1)
	{
		printf("SSL send data error!!\n");
		return -1;
	}

//	printf("\n\n\n%s %d Send SMTP LEN = %d\n", __func__, __LINE__, strlen(cmd));
	return 0;
}


/******************************
函数功能:SSL数据接收



*******************************/
int SslRecv(SSL * ssl)
{
	int ret;
	char recv_data[512];
	memset(recv_data,0,sizeof(recv_data));
	ret = SSL_read(ssl, recv_data, sizeof(recv_data));
	if (ret <= 0)
	{
		printf("SSL get data error!!\n");
		return -1;       
	}

	recv_data[ret] = '\0'; 

	if(*recv_data == '5') 
	{ 
		printf("ssl response error:%s\n",recv_data); 
		return -1;
	} 

	return 0;
}


/******************************
函数功能: 以SSL加密的方式与SMTP服务器连接



*******************************/
SSL *  Ssl_ConnetHost(int sockfd, SSL_CTX *ctx)
{
	SSL * ssl;

	if((ssl = SslConnect(sockfd,ctx)) == NULL)
	{
		printf("SSL connect error\n");
		return NULL;  
	}
	
	if(SslRecv(ssl) == -1)
	{
		return NULL;   
	}
	return ssl;
}


/******************************
函数功能: 将Cc通过RCPT TO的格式发送



*******************************/
int CarbonCopyRCPT(char *SendCc,SSL * ssl)
{
	char  CcTemp[80] = {0}, Cc[40] = {0};
	char *p;
	sprintf(CcTemp, "%s", SendCc);
	p = strtok(CcTemp, ";");
	while(p !=NULL)
	{
		sprintf(Cc,"RCPT TO: <%s>\r\n", p);
		SSL_SEND(Cc,ssl);
		if(SslRecv(ssl) == -1)   
		{			 
			return -1;
		}
		p = strtok(NULL, ";");
	}
}


/******************************
函数功能: 将Cc通过Cc的格式发送



*******************************/
int CarbonCopyCc(char *SendCc,SSL * ssl)
{
	char  CcTemp[80] = {0}, Cc[40] = {0};
	char *p;
	sprintf(CcTemp, "%s", SendCc);
	p = strtok(CcTemp, ";");
	while(p !=NULL)
	{
		sprintf(Cc,"Cc: <%s>\r\n", p); 
		SSL_SEND(Cc,ssl);
		p = strtok(NULL, ";");
	}
}


/******************************
函数功能: 接收服务器应答



*******************************/
int GetResponse(int sockfd)   
{     
	char buffer[512];
	int ret;

	fd_set fdread;
	struct timeval tv;

	FD_ZERO(&fdread);
	FD_SET(sockfd, &fdread);
	tv.tv_sec = 5;
	tv.tv_usec= 10;

	memset(buffer,0,sizeof(buffer));

	if((ret = select(sockfd + 1, &fdread, NULL, NULL, &tv)) <=0)
	{
		printf("select socket error.\n");
		return -1;
	}

	if (FD_ISSET(sockfd, &fdread))
	{
		ret = recv(sockfd,buffer,sizeof(buffer),0); 
		if(ret == -1) 
		{ 
			printf("receive nothing\n"); 
			close(sockfd);
			return -1; 
		} 
		buffer[ret] = '\0'; 

		if(*buffer == '5') 
		{ 
			printf("response error:%s\n",buffer); 
			close(sockfd);
			return -1;
		} 
		printf("response:%s\n",buffer);	
	}
	
	return 0;   
}  


/******************************
函数功能:接连SMTP服务器



*******************************/
int  Smtp_ConnectHost(const char *hostname,unsigned short port)   
{   
	struct sockaddr_in sockaddr;
	struct hostent *host;
	int sockfd;

	if(hostname == NULL)   
	{
		return -1;  
	}

	if((sockfd = socket(AF_INET,SOCK_STREAM,0) )== -1)   
	{	
		printf("CreateSocket error\n");   
		//close(sockfd);	 
		return -1;	 
	}	
	
	if((host = gethostbyname(hostname)) == NULL)
	{
		printf("gethostbyname error !\n");   
		close(sockfd);
		return -1;
	}

	if(NULL == host || host->h_addr_list == NULL || host->h_addr_list[0] == NULL)
	{
		printf("%s %d: gethostbyname fail\n", __func__, __LINE__);
		close(sockfd);
		return -1;
	}
	
	memset(&sockaddr,0,sizeof(struct sockaddr));   
	sockaddr.sin_family = AF_INET;   
	sockaddr.sin_port   = htons(port);   
	memcpy(&sockaddr.sin_addr,host->h_addr,sizeof(struct in_addr));

	if((connect(sockfd,(struct sockaddr *)&sockaddr,sizeof(struct sockaddr))) == -1)   
	{ 
		printf("connect error\n");   
		close(sockfd);   
		return -1;   
	}
	return sockfd;   
} 


/******************************
函数功能: 普通邮件登录
*******************************/

int Smtp_Login(int sockfd, char *username,char *password)   
{   
	char *pSendCmd = NULL;
	char tmpbuf[128];
	char UserDst[64],PassDst[64];	
	memset(tmpbuf,0,sizeof(tmpbuf));
	memset(UserDst,0,sizeof(UserDst));
	memset(PassDst,0,sizeof(PassDst));
	
	char *pUser,*pPass;   
	if(username == NULL || password == NULL)   
	{
		return -1;   
	}
	pUser = username;   
	pPass = password;   
	
	Base64_Code((unsigned char *)pUser,(unsigned char *)UserDst);   // Base64编码
	Base64_Code((unsigned char *)pPass,(unsigned char *)PassDst);   

	pSendCmd = "HELO Localhost\r\n";   //客户向对方邮件服务器发出的标识自己的身份的命令
	if((SendDataN(sockfd,pSendCmd,strlen(pSendCmd))) == -1)   
	{
		close(sockfd);   
		return -1;   
	}      
	if(GetResponse(sockfd) == -1) 
		return -1;   

	pSendCmd = "AUTH LOGIN\r\n";   // 登录请求
	if((SendDataN(sockfd,pSendCmd,strlen(pSendCmd))) == -1)   
	{   
		close(sockfd);   
		return -1;   
	}   
	if(GetResponse(sockfd) == -1)   
		return -1;   
	sprintf(tmpbuf,"%s\r\n",UserDst); // 用户名
	if((SendDataN(sockfd,(char *)tmpbuf,strlen(tmpbuf))) == -1)   
	{   
		close(sockfd);   
		return -1;   
	}   

	if(GetResponse(sockfd) == -1)   
		return -1;   

	sprintf(tmpbuf,"%s\r\n",PassDst); // 登录密码
	if((SendDataN(sockfd,(char *)tmpbuf,strlen(tmpbuf))) == -1)   
	{   
		close(sockfd);   
		return -1;   
	}   
	if(GetResponse(sockfd) == -1)  
	{		
		close(sockfd);   
		return -1;
	}
	return 0;   
}  


/******************************
函数功能: 发送邮件的内容和格式



*******************************/
int Smtp_SendMessage(int sockfd, const char *from,const char *to,const char *date,const char *subject,const char *data)   
{   
	char *pSendCmd = NULL;
	char Date[64],Subject[64],tmpbuf[1024],From[128],To[128];
	char tmepbase64[1024] = {0};

	memset(Date,0,sizeof(Date));
	memset(Subject,0,sizeof(Subject));	
	memset(From,0,sizeof(From));	
	memset(To,0,sizeof(To));	
	memset(tmpbuf,0,sizeof(tmpbuf));	

	if(from == NULL || to == NULL || date == NULL || subject == NULL)   
	{   
		close(sockfd);   
		return -1;   
	} 

	/*MAIL FROM*/
	sprintf(From,"MAIL FROM: <%s>\r\n",from); 
	//printf("%s\n",From);
	if(SendDataN(sockfd,From,strlen(From)) == -1)
	{
	    close(sockfd);
	    return -1;
	}

	if(GetResponse(sockfd) < 0)
	    return -1;

	/*RCPT TO*/
	sprintf(To,"RCPT TO: <%s>\r\n",to);
	//printf(To);
	if(SendDataN(sockfd,To,strlen(To)) == -1)
	{
	    close(sockfd);
	    return -1;
	}
	//sleep(1);
	if(GetResponse(sockfd) < 0)
	    return -1;

	/*DATA*/    
	pSendCmd = "DATA\r\n";
	//pSendCmd = "DATA\r\n\r\n";
	if(SendDataN(sockfd,pSendCmd,strlen(pSendCmd)) == -1)
	{
	    close(sockfd);
	    return -1;
	}
	
#if 1
	if(GetResponse(sockfd) < 0)
	    return -1;
#endif

	memset(tmpbuf,0x00,sizeof(tmpbuf));
#if 0
	
	printf("%s %d: here! strlen = %d\n", __func__, __LINE__, strlen(tmpbuf));
	if(SendDataN(sockfd,tmpbuf,strlen(tmpbuf))== -1)
	{
	    close(sockfd);
	    return -1;
	}
#else
	sprintf(tmpbuf,"From: \"longse\"<%s>\r\n", from);

	//printf("%s %d: here! strlen = %d\n", __func__, __LINE__, strlen(tmpbuf));
	if(SendDataN(sockfd,tmpbuf,strlen(tmpbuf))== -1)
	{
	    close(sockfd);
	    return -1;
	}

	sprintf(tmpbuf,"To: %s\r\n", to);
	//printf("%s %d: here! strlen = %d\n", __func__, __LINE__, strlen(tmpbuf));
	if(SendDataN(sockfd,tmpbuf,strlen(tmpbuf))== -1)
	{
	    close(sockfd);
	    return -1;
	}

	sprintf(tmpbuf,"Subject: %s\r\n", subject);
	//printf("%s %d: here! strlen = %d\n", __func__, __LINE__, strlen(tmpbuf));
	if(SendDataN(sockfd,tmpbuf,strlen(tmpbuf))== -1)
	{
	    close(sockfd);
	    return -1;
	}

	

	sprintf(tmpbuf,"MIME-Version:1.0\r\n");
	//printf("%s %d: here! strlen = %d\n", __func__, __LINE__, strlen(tmpbuf));
	if(SendDataN(sockfd,tmpbuf,strlen(tmpbuf))== -1)
	{
	    close(sockfd);
	    return -1;
	}

	sprintf(tmpbuf,"Content-Type: multipart/mixed;boundary=\"boundary=_zx\"\r\n\r\n\r\n");
	//printf("%s %d: here! strlen = %d\n", __func__, __LINE__, strlen(tmpbuf));
	if(SendDataN(sockfd,tmpbuf,strlen(tmpbuf))== -1)
	{
	    close(sockfd);
	    return -1;
	}


	sprintf(tmpbuf,"Content-Transfer-Encoding: 7bit\r\n\r\n");
	//printf("%s %d: here! strlen = %d\n", __func__, __LINE__, strlen(tmpbuf));
	if(SendDataN(sockfd,tmpbuf,strlen(tmpbuf))== -1)
	{
	    close(sockfd);
	    return -1;
	}

	sprintf(tmpbuf,"This is a MIME Encoded Message\r\n\r\n");
	//printf("%s %d: here! strlen = %d\n", __func__, __LINE__, strlen(tmpbuf));
	if(SendDataN(sockfd,tmpbuf,strlen(tmpbuf))== -1)
	{
	    close(sockfd);
	    return -1;
	}

	
	sprintf(tmpbuf,"--boundary=_zx\r\n");
	//printf("%s %d: here! strlen = %d\n", __func__, __LINE__, strlen(tmpbuf));
	if(SendDataN(sockfd,tmpbuf,strlen(tmpbuf))== -1)
	{
	    close(sockfd);
	    return -1;
	}

	sprintf(tmpbuf,"Content-Type: text/plain;charset=us-ascii\r\n");
	//printf("%s %d: here! strlen = %d\n", __func__, __LINE__, strlen(tmpbuf));
	if(SendDataN(sockfd,tmpbuf,strlen(tmpbuf))== -1)
	{
		close(sockfd);
		return -1;
	}
	
	sprintf(tmpbuf,"Content-Transfer-Encoding: 7bit\r\n\r\n");
	//printf("%s %d: here! strlen = %d\n", __func__, __LINE__, strlen(tmpbuf));
	if(SendDataN(sockfd,tmpbuf,strlen(tmpbuf))== -1)
	{
	    close(sockfd);
	    return -1;
	}	

	
	if(SendDataN(sockfd,(char *)data,strlen((char *)data))== -1)
	{
	    close(sockfd);
	    return -1;
	}	

#endif

#if 0
	printf("%s %d: here!\n", __func__, __LINE__);
	/*Content*/
	memset(tmpbuf,0x00,sizeof(tmpbuf));
	strcat(tmpbuf,data);
	memset(tmepbase64,0x00,sizeof(tmepbase64));
	
	printf("%s %d: here!\n", __func__, __LINE__);
	BaseEncode(tmpbuf,strlen(tmpbuf),tmepbase64);

	printf("%s %d: here!\n", __func__, __LINE__);
	strcat(tmepbase64,"\r\n");
	if(SendDataN(sockfd,tmepbase64,strlen(tmepbase64))== -1)
	{
		close(sockfd);
		return -1;
	}
#endif


	//printf("%s %d: here!\n", __func__, __LINE__);
	return 0;   
}  


/******************************
函数功能: 发送附件



*******************************/
int Smtp_SendFile(int sockfd, const char *filename )   
{   
	
	static int file_index = 0;

	file_index++;
	int  len;   
	char *pSendCmd = NULL;
	int stringlen = 0;   
	int timedelay,timedelay1;	
	char tmpbuf[1024];
	char datasrc[1024],datades[1024];

	int sendlen = 0;
	
	memset(tmpbuf,0,sizeof(tmpbuf));
	memset(datasrc,0,sizeof(datasrc));
	memset(datades,0,sizeof(datades));
	
	FILE *fp=NULL;           
	fp = fopen(filename,"rb");   
	if(fp==NULL)   
	{   
		printf("can not open file");   
		return -1;   
	}   

	fseek(fp,0,SEEK_END);   //文件结尾
	len = ftell(fp);   // 计算文件的大小
	fseek(fp,0,SEEK_SET);   // 文件头

	//printf("\n\n\n\n@@@@@Smtp file index = %d len = %d\n\n\n\n", file_index, len);

	memset(tmpbuf,0x00,sizeof(tmpbuf));
	sprintf(tmpbuf, "\r\n--boundary=_zx\r\nContent-Type: application/octet-stream;charset=\"gb18030\";name=\"%s\"\r\nContent-Disposition: attachment; filename=\"%s\"\r\nContent-Transfer-Encoding: base64\r\n\r\n", ";" , filename , filename);

#if 0
	sprintf(tmpbuf, "--%s\r\nContent-Type: application/octet-stream;charset=\"gb18030\";name=\"%s\"\r\nContent-Disposition: attachment; filename=\"%s\"\r\nContent-Transfer-Encoding: base64\r\n\r\n", \
				";" , "MotionDetect.avi" , "MotionDetect.avi");
#endif
	

	if(SendDataN(sockfd,tmpbuf,strlen(tmpbuf)) == -1)
	{
		close(sockfd);
		if(fp != NULL)
		{
			fclose(fp);
			fp = NULL;
		}
		return -1;
	}

	printf("Sending file,please wait ...\n");   
	int inlen = 0;
	int read_len = 0;
	int read_ret = 0;
	while(!feof(fp))   
	{    
#if 0
		if(len>=57)   
		{   
			fread(datasrc,57,1,fp);   
			datasrc[57]='\0';   
			stringlen = 57;   
			len -= 57;   
			
		}   
#else
		if(len>=75)   
		{   
			read_ret = fread(datasrc,75,1,fp);

			if(read_ret <= 0)
			{
				printf("%s %d: read_len = %d\n", __func__, __LINE__, read_ret);
			}

			read_len += read_ret * 75;
			
			datasrc[75]='\0';   
			stringlen = 75;   
			len -= 75;   
			inlen = 75;
		}   
#endif
		else if(len>0)   
		{   
			read_ret = fread(datasrc,len,1,fp);   


			if(read_ret <= 0)
			{
				printf("%s %d: read_len = %d\n", __func__, __LINE__, read_ret);
			}

			read_len += read_ret * len;

			
			datasrc[len]='\0';   
			stringlen = len;   
			inlen = 75;
			len = 0;   
		}   
		else 
		{
			break;
		}
		
	//	int base64ret = Base64Encode((unsigned char *)datades,(unsigned char *)datasrc,stringlen);  

		int base64ret = ANSIToBase64(datasrc, stringlen, datades, (inlen+1)*2);
		

		//printf("%s %d: strlen(datades) = %d \n", __func__, __LINE__, strlen(datades));
		if((SendDataN(sockfd, datades, base64ret)) == -1)   
		{   
			//printf("send base64data error , errno = %d\n", errno);   
			close(sockfd);  
			if(fp != NULL)
			{
				fclose(fp);
				fp = NULL;
			}
			return -1;   
		}  

		sendlen += base64ret;

		usleep(0);
		
	}   
	if(fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}	

	//printf("\n\n\n\n%s %d: index = %d sendlen = %d retlen = %d\n\n\n\n", __func__, __LINE__, file_index, sendlen, read_len);
		
	pSendCmd = "\r\n";
	if((SendDataN(sockfd,pSendCmd,strlen(pSendCmd))) == -1)   
	{   
		close(sockfd);   
		return -1;   
	} 	
	sprintf(tmpbuf,"--boundary=_zx--");
	if((SendDataN(sockfd,tmpbuf,strlen(tmpbuf))) == -1)   
	{   
		close(sockfd);   
		return -1;   
	}

	//printf("Send file ok\n");  
	
	return 0;   
}


/******************************
函数功能: 退出邮箱服务器


*******************************/ 
int Smtp_Quit(int sockfd)   
{   
	char *pSendCmd = NULL;
	pSendCmd = "\r\n";
	if((SendDataN(sockfd,pSendCmd,strlen(pSendCmd))) == 1)   
	{   
		close(sockfd);   
		return -1;   
	}   
	pSendCmd = "\r\n.\r\n";
	if((SendDataN(sockfd,pSendCmd,strlen(pSendCmd))) == -1)   
	{  
		close(sockfd);   
		return -1;   
	}   
	if(GetResponse(sockfd) == -1)   
	{		
		close(sockfd);	 
		return -1;
	}

	pSendCmd = "QUIT\r\n";
	if((SendDataN(sockfd,pSendCmd,strlen(pSendCmd))) == -1)   
	{   
		close(sockfd);   
		return -1;   
	}   
	if(GetResponse(sockfd) == -1)   
	{		
		close(sockfd);	 
		return -1;
	}
	if(sockfd != -1)   
	{   
		close(sockfd);   
	}   
	return 0;   
}  


/********************************
函数功能:发送邮件，包括抄送、附件等功能。



*********************************/
int Smtp_SendEmail(st_smtp *s)
{
	int sockfd;
	
	sockfd = Smtp_ConnectHost(s->pHost,s->port);   // 连接
	if(sockfd < 0)
	{   
		//printf("can not connect the host\n");   
		return -1;   
	}   
	//printf("connect the smtp host\n");   

	if(Smtp_Login(sockfd, s->pLoginName,s->pLoginPassword) == -1)  // 登录 
	{   
		//printf("login error\n");   
		return -1;   
	}   
	//printf("login\n"); 

	if(Smtp_SendMessage(sockfd, s->pSendFrom,s->pSendTo,s->pDate,s->pEmailSubject,s->pMessage) == -1)   // 发送邮件
	{   
		printf("send mail error\n");   
		return -1;   
	}   
	//printf("send message succ! \n");

	if(NULL != s->pFileName)
	{
		if(Smtp_SendFile(sockfd, s->pFileName) == -1)	 // 发送附件
		{	
			printf("send file error\n");
			//return -1;
		}	
		//printf("send file succ! \n");
	}
	
	if(Smtp_Quit(sockfd) == -1)   // 退出
	{   
		printf("Quit error\n");   
		return -1;   
	}   
	//printf("send over\n");   
	return 0;
}



/******************************
函数功能: SSL 邮件登录



*******************************/
int Smtp_SslLogin(char *username, char *password, SSL * ssl)   
{   
	char *pSendCmd = NULL;
	char tmpbuf[100] = {0};
	char UserDst[40] = {0}, PassDst[40] = {0};	
	char i;
	char *CmdStr[2] = {
       "HELO Localhost\r\n",
	"AUTH LOGIN\r\n"
	};
	char *pUser = NULL,*pPass = NULL;   
	
	if(username == NULL || password == NULL)   
	{
		return -1;
	}
	// 发送查询命令
	for(i = 0;i < 2; i++)
	{
		SSL_SEND(CmdStr[i],ssl);
		if(SslRecv(ssl) == -1) 
		{
			return -1;
		}
	}
	// 发送登录信息
	pUser = username;   
	pPass = password;   
	Base64_Code((unsigned char *)pUser,(unsigned char *)UserDst);   
	Base64_Code((unsigned char *)pPass,(unsigned char *)PassDst);   
	sprintf(tmpbuf,"%s\r\n",UserDst);
	SSL_SEND(tmpbuf,ssl);
	
	if(SslRecv(ssl) == -1)  
	{
		return -1;
	}
	sprintf(tmpbuf,"%s\r\n",PassDst);
	SSL_SEND(tmpbuf,ssl);
	
	if(SslRecv(ssl) == -1)  
	{		
		return -1;
	}
	return 0;   
}  


/******************************
函数功能: 邮件抄送、附件发送功能的实现，已经邮件发送的实现。


*******************************/
int Smtp_SslSendMessage(st_smtp *s,SSL * ssl)   
{   
	char *pSendCmd = NULL;
	char Date[40] = {0},Subject[40] = {0},tmpbuf[1024] = {0},From[128] = {0},To[128] = {0};
	if(s->pSendFrom == NULL || s->pSendTo == NULL || s->pDate == NULL || s->pEmailSubject == NULL)   
	{   
		return -1;   
	} 
	sprintf(From,"MAIL FROM: <%s>\r\n",s->pSendFrom);
	SSL_SEND(From,ssl);
	
	if(SslRecv(ssl) == -1)   
	{		 
		return -1;
	}
	sprintf(To,"RCPT TO: <%s>\r\n",s->pSendTo);   
	SSL_SEND(To,ssl);
	if(SslRecv(ssl) == -1)   
	{			 
		return -1;
	}
#if 0
	// 抄送
	if(s->pSendCc != NULL)
	{
        CarbonCopyRCPT(s->pSendCc,ssl);
	}
#endif
	pSendCmd = "DATA\r\n"; 
	SSL_SEND(pSendCmd,ssl);
	//SslSend(pSendCmd,ssl);
	if(SslRecv(ssl) == -1)   
	{			 
		return -1;
	}
	sprintf(From,"From: \"longse\"<%s>\r\n",s->pSendFrom); 
	SSL_SEND(From,ssl);
	
	//SslSend(From,ssl);
	sprintf(To,"To: %s\r\n",s->pSendTo);
	SSL_SEND(To,ssl);
	//SslSend(To,ssl);
#if 0
	if(s->pSendCc != NULL)
	{
       		CarbonCopyCc(s->pSendCc,ssl);
	}
#endif
#if 0
	sprintf(Date,"Date: %s\r\n",s->pDate);
	SSL_SEND(Date,ssl);
#endif
	sprintf(Subject,"Subject: %s\r\n",s->pEmailSubject);
	SSL_SEND(Subject,ssl);
	sprintf(tmpbuf,"MIME-Version: 1.0\r\n");
	SSL_SEND(tmpbuf,ssl);
	sprintf(tmpbuf,"Content-Type: multipart/mixed;boundary=\"boundary=_zx\"\r\n\r\n\r\n"); 
	SSL_SEND(tmpbuf,ssl);
	sprintf(tmpbuf,"Content-Transfer-Encoding: 7bit\r\n\r\n");
	SSL_SEND(tmpbuf,ssl);
	pSendCmd = "This is a MIME Encoded Message\r\n\r\n";
	SSL_SEND(pSendCmd,ssl);
	sprintf(tmpbuf,"--boundary=_zx\r\n");
	SSL_SEND(tmpbuf,ssl);
	sprintf(tmpbuf,"Content-Type: text/plain; charset=us-ascii\r\n");
	SSL_SEND(tmpbuf,ssl);
	sprintf(tmpbuf,"Content-Transfer-Encoding: 7bit\r\n\r\n");
	SSL_SEND(tmpbuf,ssl);
	pSendCmd = "\r\n";
	SSL_SEND(pSendCmd,ssl);
	SSL_SEND(s->pMessage,ssl);
	if(NULL != s->pFileName)// 发送附件
	{
		if(Smtp_SslSendFile(s->pFileName,ssl) == -1)	
		{	
			printf("send file error\n");
			//return -1;
		}	
		//printf("send file succ! \n");
	}

	return 0;   
}  


/******************************
函数功能:发送附件



*******************************/
int Smtp_SslSendFile(const char *filename, SSL * ssl )   
{   
	int  len;   
	char *pSendCmd = NULL;
	int stringlen = 0;   
	int timedelay,timedelay1;	
	char attachname[1000],Filename[100];
	unsigned char datasrc[1024],datades[1024];
	char tmpbuf[1024];
	int inlen = 0;
	int read_len = 0;
	int read_ret = 0;
	memset(attachname,0,sizeof(attachname));
	memset(Filename,0,sizeof(Filename));
	memset(tmpbuf,0,sizeof(tmpbuf));
	memset(datasrc,0,sizeof(datasrc));
	memset(datades,0,sizeof(datades));
	
	FILE *fp=NULL;           
	fp = fopen(filename,"rb");   
	if(fp==NULL)   
	{   
		printf("can not open file");   
		return -1;   
	}   

	fseek(fp,0,SEEK_END);   
	len = ftell(fp);   
	fseek(fp,0,SEEK_SET);   

	//printf("\n\n\n\n@@@@@Smtp file len = %d\n\n\n\n", len);

	pSendCmd = "\r\n";
	SSL_SEND_FILE(pSendCmd,ssl, fp);
	pSendCmd = "--boundary=_zx\r\n";
	SSL_SEND_FILE(pSendCmd,ssl, fp);

	memset(tmpbuf,0x00,sizeof(tmpbuf));
	sprintf(tmpbuf, "\r\n--boundary=_zx\r\nContent-Type: application/octet-stream;charset=\"gb18030\";name=\"%s\"\r\nContent-Disposition: attachment; filename=\"%s\"\r\nContent-Transfer-Encoding: base64\r\n\r\n", ";" , filename , filename);
	SSL_SEND_FILE(tmpbuf,ssl, fp);
	
#if 0
	sprintf(Filename,"Content-Type: text/plain;name=\"%s\"\r\n",filename);
	SSL_SEND(Filename,ssl);
	pSendCmd = "Content-Transfer-Encoding: base64\r\n";
	SSL_SEND(pSendCmd,ssl);
	sprintf(attachname,"Content-Disposition: attachment;filename=\"%s\"\r\n",filename);
	SSL_SEND(attachname,ssl);
	pSendCmd = "\r\n";
#endif
	//printf("Sending file,please wait ...\n");   
//    	SSL_SEND(pSendCmd,ssl);
	while(!feof(fp))   
	{                   
		if(len>=75)   
		{   
			read_ret = fread(datasrc,75,1,fp);

			if(read_ret <= 0)
			{
				printf("%s %d: read_len = %d\n", __func__, __LINE__, read_ret);
			}

			read_len += read_ret * 75;
			
			datasrc[75]='\0';   
			stringlen = 75;   
			len -= 75;   
			inlen = 75;   
		}   
		else if(len>0)   
		{   
			read_ret = fread(datasrc,len,1,fp);   


			if(read_ret <= 0)
			{
				printf("%s %d: read_len = %d\n", __func__, __LINE__, read_ret);
			}

			read_len += read_ret * len;

			
			datasrc[len]='\0';   
			stringlen = len;   
			inlen = 75;
			len = 0;  
		}   
		else 
		{
			break;
		}		
//		int base64ret = Base64Encode((unsigned char *)datades,(unsigned char *)datasrc,stringlen);  

		int base64ret = ANSIToBase64(datasrc, stringlen, datades, (inlen+1)*2);
		
		SSL_SEND_FILE(datades,ssl, fp);

		//sendlen += base64ret;
#if 0
		for( timedelay=0;timedelay<1000;timedelay++)   
		for( timedelay1=0;timedelay1<1000;timedelay1++);
		{
			SSL_SEND(datades,ssl);
		}
		usleep(5000);
#endif
	}   
	if(fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	} 	
	pSendCmd = "\r\n";
	SSL_SEND_FILE(pSendCmd,ssl, fp);
	sprintf(tmpbuf,"--boundary=_zx--");
	SSL_SEND_FILE(tmpbuf,ssl, fp);
	//printf("Send file ok\n");   
	return 0;   
}    

  
/******************************
函数功能: 退出邮箱服务器


*******************************/ 
int Smtp_SslQuit(SSL * ssl)   
{   
	char *pSendCmd = NULL;
	pSendCmd = "\r\n";
	SSL_SEND(pSendCmd,ssl);
	pSendCmd = "\r\n.\r\n";
	SSL_SEND(pSendCmd,ssl);
	if(SslRecv(ssl) == -1)   
	{		
		return -1;
	}

	pSendCmd = "QUIT\r\n";
	SSL_SEND(pSendCmd,ssl);
	if(SslRecv(ssl) == -1)   
	{		 
		return -1;
	}
	return 0;   
}  

int ReleaseSsl()
{
	ERR_remove_state(0);
	// thread-safe cleanup
	ENGINE_cleanup();
	CONF_modules_unload(1);

	// global application exit cleanup (after all SSL activity is shutdown)
	ERR_free_strings();
	EVP_cleanup();
	CRYPTO_cleanup_all_ex_data();

}

/********************************
函数功能:发送邮件，包括SSL加密、抄送、附件等功能。



*********************************/
int Smtp_SslSendEmail(st_smtp *s)
{
	int sockfd;
	SSL * ssl;
	SSL_CTX *ctx;
	
	ctx = SslInit(); // 初始化SSL库

	//printf("%s %d: before Smtp_ConnectHost\n", __func__, __LINE__);
	
	if((sockfd = Smtp_ConnectHost(s->pHost,s->port))== -1)   // SMTP连接
	{   
		//printf("can not connect the host\n");
		SSL_CTX_free(ctx);
		ReleaseSsl();
		//close(sockfd);
		return -1;   
	}   
	//printf("Connect the smtp host\n");  
	//fcntl(sockfd,F_SETFL,O_NONBLOCK);

	if((ssl = Ssl_ConnetHost(sockfd,ctx)) == NULL)   // SSL握手
	{
		//printf("SSL connet  error\n");
		//SSL_free(ssl);
		SSL_CTX_free(ctx);
		ReleaseSsl();
		close(sockfd);
		return -1;   
	}
	
	//printf("SSL connect Ok!\n");  
	
       if(Smtp_SslLogin(s->pLoginName,s->pLoginPassword,ssl) == -1)   // 邮箱登录
	{   
		//printf("login error\n"); 
		SSL_free(ssl);
		SSL_CTX_free(ctx);
		ReleaseSsl();
		close(sockfd);
		return -1;   
	}   
	//printf("login\n"); 

	if(Smtp_SslSendMessage(s,ssl) == -1)   // 发送邮件
	{   
		//printf("send mail error\n");   
		SSL_free(ssl);
		SSL_CTX_free(ctx);
		ReleaseSsl();
		close(sockfd);
		return -1;   
	}   
	//printf("send message success! \n");
	if(Smtp_SslQuit(ssl) == -1)   // 退出
	{   
		printf("Quit error\n");
		SSL_free(ssl);
		SSL_CTX_free(ctx);
		ReleaseSsl();
		close(sockfd);
		return -1;   
	}   
	//printf("send over\n");  
	
	// SMTP和SSL的结束处理
	SSL_free(ssl);
	SSL_CTX_free(ctx);
	ReleaseSsl();

	close(sockfd);
	return 0;
}


/*****************************************************************
函数功能:发送邮件，支持普通邮件和SSL加密邮件



******************************************************************/
int SendEmail(st_smtp *s)
{
	signal(SIGPIPE, SIG_IGN);
	int ret = -1;
	unsigned short port = 0;

	if(NULL == s)
	{
		return ret;
	}

	port = s->port;

	//printf("%s %d: port = %d\n", __func__, __LINE__, port);

	if(port == 25)
	{
		ret = Smtp_SendEmail(s);	
	}
	else if(port == 465)
	{
		ret = Smtp_SslSendEmail(s);
	}
	else 
	{
		ret = Smtp_SslSendEmail(s);
	}

	return ret;
}




