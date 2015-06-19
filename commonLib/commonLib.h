/*
*******************************************************************************
**  Copyright (c) 2012, All rights reserved.
**  
**  : epoll,epoll
*******************************************************************************
*/
#ifndef __COMMON_LIB_H__
#define __COMMON_LIB_H__


#include <unistd.h>
#include <sys/types.h>       /* basic system data types */
#include <sys/socket.h>      /* basic socket definitions */
#include <netinet/in.h>      /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>       /* inet(3) functions */
#include <sys/epoll.h> /* epoll function */
#include <fcntl.h>     /* nonblocking */
#include <sys/resource.h> /*setrlimit */

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>


#include <unistd.h>
#include <string.h>
//#include <epoll.h>
#include <sys/epoll.h>

#include "logLib.h"

#ifdef __cplusplus
extern "C" {
#endif


//#define Printf(format, arg...) myPrintf( __FILE__, __LINE__, format, ##arg)
#define Printf(format, arg...) dump_printf( __FILE__, __LINE__, format, ##arg)



/* commonLib.c */
/* 获取系统已经运行的时间 */
unsigned int getSystemBootTime();
/* 延时函数 , 单位 ms*/
inline void myMsleep(int sleepTime);
/* 打印信息 带文件名和行数 */
void myPrintf(const char *file, int line, const char *format, ... );
int readn(int fd, void *vptr, size_t n);
int writen(int fd, const void *vptr, size_t n);

int commonGetUpdataBoaParamFlag();
void commonSetUpdataBoaParamFlag(int value);


/* hash.c */
#define MAX_HASH_KEY_LEN 32//hash关键字的长度

/* 元素值 */
typedef struct elem_value_t
{
	char key[MAX_HASH_KEY_LEN];//hash关键字
	int keyLen;//hash关键字的长度
	void *pValue;//值
	struct elem_value_t *pre;
	struct elem_value_t *next;
}elem_value_t;

/* 元素类型 */
typedef struct elem_type_t
{
	int elemNum;//每个hash数组中的元素的数目
	struct elem_value_t *head;
	struct elem_value_t *last;
}elem_type_t;

/* hash表 */
typedef struct hash_table_t
{
	int hashNum;//hash数组的个数
	int userHashNum;//已经有的hash数目
	elem_type_t *pElem;
}hash_table_t;

/* 初始化hash表 */
hash_table_t * initHash(int num);
/* 插入hash表中 */
int insertHash(hash_table_t *pHashTable, char *key, int keyLen, elem_value_t *pElemValue);
/* 从hash表中删除 */
int delHash(hash_table_t *pHashTable, char *key, int keyLen);
/* 从hash表中查找 */
elem_value_t * searchHash(hash_table_t *pHashTable, char *key, int keyLen);
/* 显示hash表的详细信息 */
void showHashInfo(hash_table_t *pHashTable);
/* 销毁hash表 */
int deinitHash(hash_table_t *pHashTable);



/* socketFunc.c */
/* 设置非阻塞socket */
int netSocketSetNonBlock(int sockfd);
/* 获取socket接收缓冲区 */
int netSocketGetBufferRecvSize(int sockFd);
/* 获取socket发送缓冲区 */
int netSocketGetBufferSendSize(int sockFd);
/* 设置socket接收缓冲区 */
int netSocketSetBufferRecvSize(int sockFd, int recvSize);
/* 设置socket发送缓冲区 */
int netSocketSetBufferSendSize(int sockFd, int sendSize);
/* select超时等待 */
int netSocketSelectWait(int sock, int maxMsec);
/* 获取sock上的IP和端口 */
int netSocketGetSockName( int socket, char *pIp, int *pPort );


/* socketTcpFunc.c */
/* 初始化创建TCP监听服务器sock */
int netSocketCreateTcpListenServer(int port);
/* 创建一个TCP连接 */
int netSocketTcpConnect(char *pIp, int port);
/* accept一个tcp链接，并返回链接好的socket */
int netSocketTcpAccept(int listenSocket);


/* socketUdpFunc.c */
/* 初始化创建UDP监听服务器sock */
int netSocketCreateUdpListenServer(int port);
/* 初始化创建UDP客户端sock */
int netSocketCreateUdpClient();


/* networkIntface.c */
/* 设置网卡状态  status == 1为Up， status == 0为Down */
int netSetStatus(char *pInterface, int status);
/* 获取Ip */
int netGetIp(char *pInterface, unsigned int *ip);
/* 设置Ip */
int netSetIp(char *pInterface, unsigned int ip);
/* 获取网关 */
int netGetGw(char *gateway);
/* 设置网关 */
int netSetGw(char *pInterface, unsigned int gw);
/* 设置子网掩码 */
int netSetMask(char *pInterface, unsigned int mask);
/* 设置mtu单元 */
int netSetMtu(char *pInterface, unsigned int mtu);
/* 设置路由 */
int netGetRoute(char *pInterface, unsigned int *route);
/* 设置路由 */
int netSetRoute(char *pInterface, unsigned int route);
/* 删除路由 */
int netDelRoute(char *pInterface, int gw);
/* 设置DNS */
int netSetDns(unsigned int dns1, unsigned int dns2);
/* 设置主机名 */
int netSetHostName(char *pHostName);
/* 获取MAC地址 */
int netGetMac(char *pInterface, unsigned char *pMac);
/*设置 MAC地址*/
int netSetMac(char *pInterface,char* pMac);
/* 检测ip是否合法	返回值：>0:合法 */
int netCheckIpv4(char *pIp);
/* 检测子网掩码是否合法	返回值：> 0:合法 */
int netCheckNetMask(char *pMask);
/*network status*/
int checkNetworkStatus(char* ethname);
/* DNS*/
int GetDNSAddr(char *Dns1, char *Dns2);
/*Mask*/
int GetNetMask(char *EthName,unsigned char *NetMask);

int netGetBro(char *pInterface, unsigned int *ip);
/* epollFunc.c */
int netEpollCreat( int size );
int netEpollClose( int epfd );
int netEpollCtl( int epfd, int op, int fd, struct epoll_event *event );
int netEpollWait( int epfd, struct epoll_event *events, int maxevents, int timeout );

int netEpollCtlAdd( int epfd, int sockfd, uint events );
int netEpollCtlDel( int epfd, int sockfd, uint events );
int netEpollCtlMod( int epfd, int sockfd, uint events );

int GetNetworkFlow(char * interface ,double * recv_rate , double * send_rate);


/* daemon.c */
/* 守护进程 */
void daemonInit();


/* semFunc.c */
/**************************************************************************
 * function: sem_init
 * description: create and initial semaphore
 * input:       int key
 * int sem_num
 * unsigned short int value_array
 * output:      null
 * return:      >0 SUCCEED  other: FAILED
 **************************************************************************/
int Sem_init(int key);

/****************************************************************
 * function:    sem_open
 * discription: open semaphore
 * input:       int key
 * output:      null
 * return:      0: SUCCEED  other: FAILED
 ****************************************************************/
int Sem_open(int key);

/****************************************************************
 * function:    sem_lock
 * description: semaphore lock
 * input:       int sem_id
 *              int sem_num
 * output:      null
 * return:      0:SUCCEED  other: FAILED
 ****************************************************************/
int Sem_lock(int sem_id);

/******************************************************************
 * function:    sem_unlock
 * description: semaphore unlock
 * input:       int sem_id
 * int sem_num
 * output:      null
 * return:      0:SUCCEED  other: FAILED
 ******************************************************************/
int Sem_unlock(int sem_id);

/****************************************************************
 * function:    sem_exit
 * description: semaphore exit
 * input:       int sem_id
 * output:      null
 * return:      0:SUCCEED  other: FAILED
 ***************************************************************/
int Sem_exit(int sem_id);



/* msg_util.c */
int Msg_Init( int msgKey );

int Msg_Send( int qid, void *pdata , int size );

int Msg_Rsv( int qid, int msg_type, void *pdata , int size );

int Msg_Send_Rsv( int qid, int msg_type, void *pdata , int size );

int Msg_Kill( int qid );


/* share_mem.c */
#define PROC_MEM_SIZE 4096*3
#define MAX_SHARE_PROC	12

int ShareMemInit(int key); ///< Initial shared memory.
int pShareMemInit(int key); ///< Initial shared memory for private use .
void ShareMemRead(int offset,void *buf, int length); ///< Read shared memory.
void ShareMemWrite(int offset,void *buf, int length); ///< Write shared memory.




#ifdef __cplusplus
}
#endif


#endif//__COMMON_LIB_H__

