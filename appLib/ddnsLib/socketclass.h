/********************************************************************

	socketclass.h

********************************************************************/

#define	TRUE		1
#define	FALSE		0
#define INADDR_NONE -1

#define SOCKET				long
#define SOCKADDR_IN			struct sockaddr_in
#define SOCKADDR			struct sockaddr
#define INVALID_SOCKET		-1
#define SOCKET_ERROR		-1
#define closesocket			close
#define ioctlsocket			ioctl


void IpToDomain (char *domain, long IP);
unsigned long _inet_addr_ (char *cp);
unsigned long GetIPbyDomain (char *Domain);

void SOCK_Init (SOCKET *sock);
void SOCK_Disconnect (SOCKET *sock);

long TCPSOCK_MakeServer (SOCKET *sock, long TCP_Port, long QueueSize);
SOCKET TCPSOCK_Listen (SOCKET *sock, char *hostaddress);
long TCPSOCK_Connect (SOCKET *sock, char *IP_Address, long TCP_Port, long nonblocking);
long TCPSOCK_Write (SOCKET *sock, char *data, long leng);
long TCPSOCK_Read (SOCKET *sock, char *data, long leng);
long TCPSOCK_SafeRead (SOCKET *sock, char *data, long leng);

long UDPSOCK_MakeClientSocket (SOCKET *sock);
long UDPSOCK_WriteIP (SOCKET *sock, unsigned long IP_Address, long UDP_Port, char *data, long leng);
long UDPSOCK_Read (SOCKET *sock, char *data, long leng);



