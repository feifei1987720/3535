#ifndef PING_H
#define PING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <setjmp.h>
#include <errno.h>
#include <stdbool.h>
#include <netinet/in.h>


#define PACKET_SIZE     2048
#define MAX_WAIT_TIME   5
#define MAX_NO_PACKETS  3
#define MAX_OVER_TIME   2                        //
#define ADDR_LENGTH   32
#define APPLY_LENGTH  4
//icmp
typedef struct
{
    int icmpSeq;
    int icmpLen;
    int ipTtl;
    double rtt;
    char fromAddr[ADDR_LENGTH];
    bool isReply;
}IcmpEchoReply;
//ping
typedef struct
{
    int dataLen;
    int nsend;
    int nreceived;
    char ip[ADDR_LENGTH];
    char error[ADDR_LENGTH];
    IcmpEchoReply icmpEchoReplys[APPLY_LENGTH];
}PingResult;



bool ping(const char * host, int count, PingResult * pingResult);
unsigned short getChksum(unsigned short *addr,int len);
//int packIcmp(int pack_no, struct icmp* icmp);
int packIcmp(int pack_no, struct icmp* icmp ,pid_t m_pid);
//bool unpackIcmp(char *buf,int len,  IcmpEchoReply *icmpEchoReply);
bool unpackIcmp(char *buf,int len,  IcmpEchoReply *icmpEchoReply,pid_t m_pid);
struct timeval tvSub(struct timeval timeval1,struct timeval timval2);
bool getsockaddr(const char * hostOrIp, struct sockaddr_in* sockaddr);
//bool sendPacket();
bool sendPacket(int m_sockfd ,int m_nsend,int m_icmp_seq,int count ,struct sockaddr_in * m_dest_addr,pid_t m_pid);
//bool recvPacket(PingResult * pingResult);
bool recvPacket(int m_sockfd,int count ,PingResult *pingResult,struct sockaddr_in * m_from_addr,pid_t m_pid);

#endif

