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

#include "socketclass.h"
#include "ddnsNoti.h"

#define	DDNSSRVIP		"noti.ddns.co.kr"
#define	DDNSSRVPORT		5309
#define BUF_LEN         1024
#define NOTIVERSION 	"5.0.0"
#define DOMAIN_LEN    	64


// DDNS server address
struct sockaddr_in ddnsINETAddress;      // Server Address UDP.
struct sockaddr_in ddnsINETAddressTCP;   // Server Address TCP.

union CHARLONG {
	long l;
	char c[4];
};



// Noti client socket
long notiFd;
fd_set notiReadFds, notiTestFds;

// Buffer for making UDP Noti packet
unsigned char buf[BUF_LEN];

// IP of DDNS server
char DDNSIP[65];

// Parameters for making UDP Noti packet
char USR[65];		// Domain
char KEY[17];			// Key (MAC 16Byte)
char DOMAIN_NAME[128];

// Storage for Noti reply
char _NOTIMSG[257];		// Result message
char _MYIP[65];			// My IP

const char int2base[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
char byteorder[3];


/********************************************************************
	Common functions
********************************************************************/

// Safety strcpy (d:destination, size:destination size, s:source)
void safestrcpy (char *d, long size, const char *s) {
	if (d) {
		if (s) {
			memcpy (d, s, size);
			d[size-1] = 0;
		} else d[0] = 0;
	}
}

long safestrlen (const char *s) {
	if (s == NULL) return 0;
	return strlen (s);
}

char *safestrchr (char *a, char b) {
	if (a == NULL) return NULL;
	return strchr (a, b);
}


// Trim white spaces of string
char *trim (char *str) {
	long len, index;

	while ((*str==' ') || (*str==9)) str++;
	len = strlen(str);
	if (len==0) return str;
	index = len-1;
	while ((str[index]==' ') || (str[index]==9)) index--;
	str[index+1] = 0;
	return str;
}

// Set Byte order for Base64 transform by CPU type
void SerByteOrder () {
	long cputype;

	cputype = 1;
	if (*((char *)(&cputype))) {
		byteorder[0] = 2;
		byteorder[1] = 1;
		byteorder[2] = 0;
	} else {
		byteorder[0] = 1;
		byteorder[1] = 2;
		byteorder[2] = 3;
	}
}

// Convert normal string to Base64
char *Text2Base64 (char *rp, int len, char *buff) {
	char *wp;
	union CHARLONG cb;

	if (buff == NULL) return NULL;
	if (rp == NULL) {
		*buff = 0;
		return buff;
	}
	wp = buff;

	SerByteOrder ();

	while (0 < len) {
		cb.c[byteorder[0]] = *rp++;
		if (1 < len) cb.c[byteorder[1]] = *rp++;
		else cb.c[byteorder[1]] = 0;
		if (2 < len) cb.c[byteorder[2]] = *rp++;
		else cb.c[byteorder[2]] = 0;

		*wp++ = int2base[(cb.l>>18)&0x3F];
		*wp++ = int2base[(cb.l>>12)&0x3F];
		if (len < 2) *wp++ = '=';
		else *wp++ = int2base[(cb.l>>6)&0x3F];
		if (len < 3) *wp++ = '=';
		else *wp++ = int2base[cb.l&0x3F];

		len -= 3;
	}
	*wp = 0;
	return buff;
}

// Convert numeric IP to dotted IP
char *getDotIP(long ip, char *ipStr) {
	long a, b, c, d;

	a = (ip>>24)&255;
	b = (ip>>16)&255;
	c = (ip>>8)&255;
	d = ip&255;
	sprintf(ipStr, "%d.%d.%d.%d", a, b, c, d);
	return ipStr;
}

// Convert domain or string IP to integer IP
unsigned long getIPbyDomain (char *Domain) {
	unsigned long haddr;
	struct hostent *hent;

	if ((haddr = inet_addr (Domain)) == (unsigned long)(-1)) {
		hent = gethostbyname (Domain);
		if ((hent != NULL) && (hent->h_addr_list[0] != NULL))
		{
			haddr = *(unsigned long *)(hent->h_addr_list[0]);
		}
		else 
		{
			int nError = errno;
			return (unsigned long)(-1);
		}
			
	}
	return htonl (haddr);
}


// Open Noti UDP socket
long initNotiSock(char* ip) {
	struct hostent *hent;
	unsigned long haddr;
	int serverLen;

	serverLen = sizeof(ddnsINETAddress);
	memset(&ddnsINETAddress, 0, serverLen);
    ddnsINETAddress.sin_family = AF_INET;
	haddr = inet_addr(ip);
	if (haddr==(unsigned long)(-1)) {
		hent = gethostbyname(ip);
		if ((hent!=NULL) && (hent->h_addr_list[0]!=NULL)) {
			haddr = *(unsigned long*)(hent->h_addr_list[0]);
		}
		else {
			return -1;
		}
	}
	
	ddnsINETAddress.sin_addr.s_addr = haddr;
    ddnsINETAddress.sin_port = htons(DDNSSRVPORT);
    notiFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (notiFd<0) {
		return -1;
	}
	return 0;
}

// Get ID from Noti replay packet
long getID() {
	long result;

	result = (buf[0]<<8)+(buf[1]);
	return result;
}

// Get QR from Noti replay packet
long getQR() {
	long result;

	result = (buf[2]&0x80)>>7;
	return result;
}

// Get OPCODE from Noti replay packet
long getOPCODE() {
	long result;

	result = (buf[2]&0x78)>>3;
	return result;
}

// Get USERCODE from Noti replay packet
long getUSERCODE() {
	long result;

	result = buf[2]&0x07;
	return result;
}

// Get Z from Noti replay packet
long getZ() {
	long result;

	result = (buf[3]&0x70)>>4;
	return result;
}

// Get RCODE from Noti replay packet
long getRCODE() {
	long result;

	result = (buf[3]&0x0F);
	return result;
}

// Get my IP from Noti replay packet
long getMyIP() {
	long result;

	strcpy(_MYIP, "0.0.0.0");
	result = (buf[4]<<24)+(buf[5]<<16)+(buf[6]<<8)+buf[7];
	getDotIP(result, _MYIP);
	return result;
}

// Set ID into Noti repuest packet
void setID(long value) {
	buf[0] = (unsigned char)((value>>8)&0xFF);
	buf[1] = (unsigned char)(value&0xFF);
}

// Set QR into Noti repuest packet
void setQR(long value) {
	buf[2] = (unsigned char)(buf[2]|((value<<7)&0x80));
}

// Set OPCODE into Noti repuest packet
void setOPCODE(long value) {
	buf[2] = (unsigned char)(buf[2]|((value<<3)&0x78));
}

// Set USERCODE into Noti repuest packet
void setUSERCODE(long value) {
	buf[2] = (unsigned char)(buf[2]|(value&0x07));
}

// Set Z into Noti repuest packet
void setZ(long value) {
	buf[3] = (unsigned char)(buf[3]|((value<<4)&0x70));
}

// Set domain into Noti repuest packet
void setDomain(long ptr, char* value) {
	memset(&buf[ptr], 32, DOMAIN_LEN);
	memcpy(&buf[ptr], value, strlen(value));
}

// Set key into Noti repuest packet
void setKey(long ptr, char* value) {
	memset(&buf[ptr], 32, 16);
	memcpy(&buf[ptr], value, strlen(value));
}

// Set reserved field into Noti repuest packet
void setSpace(long ptr, long size) {
	memset(&buf[ptr], 32, size);
}

// Set normal string into Noti request packet
void setString(long ptr, long len, char* value) {
	memset(&buf[ptr], 32, len);
	memcpy(&buf[ptr], value, strlen(value));
}

// Treat Noti reply packet from DDNS server
long procEcho(long n) {
	long qr;
	long opCode;
	long z;
	long rCode;
	long ip;

	qr = getQR();
	opCode = getOPCODE();
	z = getZ();
	rCode = getRCODE();
	if (qr!=1) {
		return -1;
	}
	if (opCode!=15) {
		return -1;
	}
	if (z!=4) {
		return -1;
	}
	if (rCode!=0) {
		return rCode;
	}
	return 0;
}

// Start treatment for Noti reply packet from DDNS server
long procResponse(long n) {
	long qr;
	long result;

	qr = getQR();
	if (qr==1) {
		result = procEcho(n);
		return result;
	}
	return -1;
}

// Receive Noti reply from DDNS server (Socket Polling Loop)
long pollSocket() {
	unsigned int serverLen;
	struct timeval timeOut;
	long retryCount;
	long result;
	long n;

	memset(&buf[0], 0, BUF_LEN);
	retryCount = 0;
	while (1) {
		notiTestFds = notiReadFds;
		timeOut.tv_sec = 10;
		timeOut.tv_usec = 0;
		result = select(FD_SETSIZE, &notiTestFds, (fd_set*)0, (fd_set*)0, &timeOut);
		if (result<0) {
			return -2;
		}
		if (FD_ISSET(notiFd, &notiTestFds)) {
			serverLen = sizeof(ddnsINETAddress);
			n = recvfrom(notiFd, (char*)&buf[0], BUF_LEN, 0, (struct sockaddr*)&ddnsINETAddress, &serverLen);
			if (n>0) {
				result = procResponse(n);
				return result;
			}
			return -3;
		}
		retryCount++;
		if (retryCount==2) {
			return -4;
		}
	}
}

// Retry by TCP Noti in case of UDP Noti failure
void retryDynDNS () {
	char msg_dyndns[1024];
	char dom[64];
	char auth[128];
	char auth_base64[256];
	SOCKET dyndnssock;
	char *p;

	safestrcpy (dom, sizeof (dom), USR);
	if (p = safestrchr (dom, '.')) {
		*p = 0;
	}
	sprintf (auth, "%s:%s", dom, KEY);
	if (Text2Base64 (auth, safestrlen (auth), auth_base64)) {
		sprintf (msg_dyndns, "GET /nic/update?system=dyndns&hostname=%s&myip=&wildcard=OFF&mx=NO&backmx=NO&offline=NO HTTP/1.0\r\nHost: members.dyndns.org\r\nAuthorization: Basic %s\r\nUser-Agent: DDNS-Noti-%s\r\n", DOMAIN_NAME, auth_base64, NOTIVERSION);
		if (TCPSOCK_Connect (&dyndnssock, DDNSSRVIP, DDNSSRVPORT, 0)) {
			TCPSOCK_Write (&dyndnssock, msg_dyndns, safestrlen (msg_dyndns));
			SOCK_Disconnect (&dyndnssock);
		}
	}
}

// Treat result of Noti reply
long recvReply(long code) {
	long result;

	result = pollSocket();
	if (result==0) {
		strcpy (_NOTIMSG, "Success");
	}
	else if (result==1) {
		strcpy (_NOTIMSG, "Not good by network noise");
	}
	else if (result==3) {
		strcpy (_NOTIMSG, "Domain not found");
	}
	else if (result==5) {
		strcpy (_NOTIMSG, "Request deny");
	} else {
		if (result==-2) {
			DDNSIP[0] = 0;
			strcpy (_NOTIMSG, "Cannot receive reply by socket fault");
			if (code == 7) {
				retryDynDNS ();
			}
		}
		else if (result==-3) {
			DDNSIP[0] = 0;
			strcpy (_NOTIMSG, "Received reply from DDNS server but cannot read reply by socket fault");
			if (code == 7) {
				retryDynDNS ();
			}
		}
		else {
			DDNSIP[0] = 0;
			strcpy (_NOTIMSG, "Cannot receive reply by network busy");
			if (code == 7) {
				retryDynDNS ();
			}
		}
	}
	if (code == 7) {
		strcat (_NOTIMSG, " - IP notification(7)");
	}

	return result;
}

// UDP Noti process
long lSuc = 0;
void castNotify(ddns_cfg_t *ddns) {
	long ptr;
	long result, code=7;
	long i;
	char s[128];
	
	///strcpy(USR,"iwlee6605");
	///strcpy(USR,"yhqly");
	///strcpy(KEY,"yh5629");
	strcpy(USR,ddns->user);
	strcpy(KEY,ddns->pwd);
	strcpy(DOMAIN_NAME, ddns->domain);

	if (DDNSIP[0] == 0) {
		getDotIP (getIPbyDomain (DDNSSRVIP), s);
		safestrcpy (DDNSIP, 65, s);
	}
	result = initNotiSock(DDNSIP);
	strcpy (_NOTIMSG, "");
	strcpy (_MYIP, "0.0.0.0");
	if (result>=0) {
		FD_ZERO(&notiReadFds);
		FD_SET(notiFd, &notiReadFds);
		memset(buf, 0, BUF_LEN);
		setID(0);
		setQR(0);
		setOPCODE(15);
		setUSERCODE(code);
		setZ(4);
		ptr = 4;
		///sprintf (s, "%s.skddns.com", USR);
		setDomain(ptr, DOMAIN_NAME);
		ptr = ptr+DOMAIN_LEN;
		setKey(ptr, KEY);
		ptr = ptr+16;
		setSpace(ptr, 32);
		ptr = ptr+32;
		sendto(notiFd, (char*)&buf[0], ptr, 0, (struct sockaddr*)&ddnsINETAddress, sizeof(ddnsINETAddress));
//		result = recvReply(0, code);
		result = recvReply(code);
		if ( result == 0)
		{
			lSuc++;
			printf("lSuc = %d\n", lSuc);
		}

		closesocket(notiFd);
		usleep(100*1000);
	}
	else {
		strcpy (_NOTIMSG, "Cannot send packet by socket fault");
	}
}




