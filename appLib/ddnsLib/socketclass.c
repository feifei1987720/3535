/********************************************************************

	socketclass.cpp
	
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

/********************************************************************
	Sub functions
********************************************************************/

// Convert integer IP to string IP
void IpToDomain (char *domain, long IP) {
	sprintf (domain, "%d.%d.%d.%d",	((unsigned char *)(&IP))[0], ((unsigned char *)(&IP))[1], ((unsigned char *)(&IP))[2], ((unsigned char *)(&IP))[3]);
}

// Convert string IP to integer IP
unsigned long _inet_addr_ (char *cp) {
	long len, i;
	char *s;
	
	len = strlen (cp);

	// ´ë°ýÈ£·Î ½×¿©Á® ÀÖÀ¸¸é ¶©´Ù.
	if (cp[0] == '[') {
		cp++;
		len--;
	}
	if (cp[len-1] == ']') {
		len--;
		cp[len] = 0;
	}

	s = cp;
	for (i=0; i<len; i++) {
		if (((*s < '0') || ('9' < *s)) && (*s != '.')) return (unsigned long)INADDR_NONE;
		s++;
	}
	return inet_addr (cp);
}

// Convert domain or string IP to integer IP
unsigned long GetIPbyDomain (char *Domain) {
	unsigned long haddr;
	struct hostent *hent;

	if ((haddr = _inet_addr_ (Domain)) == (unsigned long)INADDR_NONE) {
		hent = gethostbyname (Domain);
		if ((hent != NULL) && (hent->h_addr_list[0] != NULL))
			haddr = *(unsigned long *)(hent->h_addr_list[0]);
		else return (unsigned long)INADDR_NONE;
	}
	return haddr;
}


void SOCK_Init (SOCKET *sock) {
	*sock = INVALID_SOCKET;
}

void SOCK_Disconnect (SOCKET *sock) {
	if (*sock == INVALID_SOCKET) return;
	closesocket (*sock);
	*sock = INVALID_SOCKET;
}



/********************************************************************
	TCP functions
********************************************************************/

// Make TCP server socket
long TCPSOCK_MakeServer (SOCKET *sock, long TCP_Port, long QueueSize) {
	SOCKADDR_IN sin;
	long optvalue;
	*sock = socket (PF_INET, SOCK_STREAM, 0);
	if (*sock == INVALID_SOCKET)
	{
		return FALSE;
	}
	
	optvalue = -1;
	setsockopt (*sock, SOL_SOCKET, SO_REUSEADDR, (char *)(&optvalue), sizeof (optvalue));

	sin.sin_family = PF_INET;
	sin.sin_port = htons ((unsigned short)(TCP_Port));
	sin.sin_addr.s_addr = INADDR_ANY;
	if (bind (*sock, (struct sockaddr *)&sin, sizeof(sin)) != 0) {
		SOCK_Disconnect (sock);
		return FALSE;
	}
	if (listen (*sock, QueueSize) != 0) {
		SOCK_Disconnect (sock);
		return FALSE;
	}
	return TRUE;
}

// Listen from TCP server socket
SOCKET TCPSOCK_Listen (SOCKET *sock, char *hostaddress) {
	SOCKADDR_IN sin;
	socklen_t addrlen;
	SOCKET acceptsock;

	addrlen = sizeof(sin);
	acceptsock = accept (*sock, (struct sockaddr *)&sin, (socklen_t *)(&addrlen));
	if (hostaddress) strcpy (hostaddress, inet_ntoa(sin.sin_addr));
	return acceptsock;
}

// Connect to TCP server socket
long TCPSOCK_Connect (SOCKET *sock, char *IP_Address, long TCP_Port, long nonblocking) {
	SOCKADDR_IN sin;
	unsigned long haddr;
	u_long option;

	if ((haddr = GetIPbyDomain (IP_Address)) == (unsigned long)INADDR_NONE) return FALSE;

	if ((*sock = socket (PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) return FALSE;

	sin.sin_family = PF_INET;
	sin.sin_port = htons ((unsigned short)(TCP_Port));
	sin.sin_addr.s_addr = haddr;

	if (nonblocking) {
		option = 1;
		ioctlsocket (*sock, FIONBIO, &option);
	}

	if (connect (*sock, (struct sockaddr *)&sin, sizeof sin) == SOCKET_ERROR)
		if (errno != EINPROGRESS) {
			SOCK_Disconnect (sock);
			return FALSE;
		}

	if (nonblocking) {
		option = 0;
		ioctlsocket (*sock, FIONBIO, &option);
	}

	return TRUE;
}

// Send stream through TCP socket
long TCPSOCK_Write (SOCKET *sock, char *data, long leng) {
	return send (*sock, data, leng, 0);
}

// Receive stream through TCP socket
long TCPSOCK_Read (SOCKET *sock, char *data, long leng) {
	return recv (*sock, data, leng, 0);
}

// Receive stream through TCP socket for preset size
long TCPSOCK_SafeRead (SOCKET *sock, char *data, long leng) {
	long len;

	while (0 < leng) {
		if ((len = recv (*sock, data, leng, 0)) <= 0) return FALSE;
		leng -= len;
		data += len;
	}
	return TRUE;
}



/********************************************************************
	UDP functions
********************************************************************/

// Make client UDP socket
long UDPSOCK_MakeClientSocket (SOCKET *sock) {
	if ((*sock = socket (AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) return FALSE;
	return TRUE;
}

// Send packet through UDP socket
long UDPSOCK_WriteIP (SOCKET *sock, unsigned long IP_Address, long UDP_Port, char *data, long leng) {
	SOCKADDR_IN udp_sin;

	if (IP_Address == (long)INADDR_NONE) return SOCKET_ERROR;
	udp_sin.sin_family = AF_INET;
	udp_sin.sin_addr.s_addr = IP_Address;
	udp_sin.sin_port = htons ((unsigned short)(UDP_Port));
	return sendto (*sock, data, leng, 0, (SOCKADDR *)&udp_sin, sizeof(SOCKADDR_IN));
}

// Receive packet through UDP socket
long UDPSOCK_Read (SOCKET *sock, char *data, long leng) {
	SOCKADDR_IN udp_sinCli;
	socklen_t nSize;

	nSize = sizeof (SOCKADDR_IN);
	return recvfrom (*sock, data, leng, 0, (SOCKADDR *)&udp_sinCli, (socklen_t *)(&nSize));
}


