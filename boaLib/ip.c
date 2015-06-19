/*

 */

#include "boa.h"
#include <arpa/inet.h>          /* inet_ntoa */


int bind_server(int server_s, char *server_ip)
{
#ifdef INET6
	struct sockaddr_in6 server_sockaddr;
	server_sockaddr.sin6_family = AF_INET6;
	memcpy(&server_sockaddr.sin6_addr, &in6addr_any, sizeof (in6addr_any));
	server_sockaddr.sin6_port = htons(server_port);
#else
	struct sockaddr_in server_sockaddr;
	memset(&server_sockaddr, 0, sizeof server_sockaddr);
#ifdef HAVE_SIN_LEN             /* uncomment for BSDs */
	server_sockaddr.sin_len = sizeof server_sockaddr;
#endif
	server_sockaddr.sin_family = AF_INET;
	if (server_ip != NULL) {
		inet_aton(server_ip, &server_sockaddr.sin_addr);
	} else {
		server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		fprintf(stderr,"server_sockaddr.sin_addr.s_addr =%d\n",server_sockaddr.sin_addr.s_addr);
	}
	server_sockaddr.sin_port = htons(server_port);
#endif

	return bind(server_s, (struct sockaddr *) &server_sockaddr,
			sizeof (server_sockaddr));
}

char *ascii_sockaddr(struct SOCKADDR *s, char *dest, int len)
{
#ifdef INET6
	if (getnameinfo((struct sockaddr *) s,
				sizeof(struct SOCKADDR),
				dest, len, NULL, 0, NI_NUMERICHOST)) {
		fprintf(stderr, "[IPv6] getnameinfo failed\n");
		*dest = '\0';
	}
#ifdef WHEN_DOES_THIS_APPLY
	if ((s->__ss_family == AF_INET6) &&
			IN6_IS_ADDR_V4MAPPED(&(((struct sockaddr_in6 *) s)->sin6_addr))) {
		memmove(dest, dest+7, NI_MAXHOST);
	}
#endif
#else
	memmove(dest, inet_ntoa(s->sin_addr), len);
#endif
	return dest;
}

int net_port(struct SOCKADDR *s)
{
	int p = -1;
#ifdef INET6
	char serv[NI_MAXSERV];

	if (getnameinfo((struct sockaddr *) s,
				sizeof(struct SOCKADDR),
				NULL, 0, serv, sizeof(serv), NI_NUMERICSERV)) {
		fprintf(stderr, "[IPv6] getnameinfo failed\n");
	} else {
		p = atoi(serv);
	}
#else
	p = ntohs(s->sin_port);
#endif
	return p;
}
