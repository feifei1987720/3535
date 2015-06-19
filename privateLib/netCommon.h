#ifndef NETCOM_H
#define NETCOM_H

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Windows */
#if IS_PLATFORM_WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <errno.h>
#include <string.h>
#define  closeSocket closesocket
/* Linux */
#elif IS_PLATFORM_LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <ctype.h>
#include <stdint.h>
#include <fcntl.h>
#define  closeSocket close
#endif

/*无效socket*/
#define NET_SOCKET_INVALID (-1)
/*检测socket是否有效*/
#define NET_SOCKET_IS_VALID(fd) ((0>fd)?0:1)
/*关闭socket*/
#define NET_SOCKET_CLOSE(fd) do{if(0>fd)break; closeSocket(fd);fd=-1;}while(0)

#ifdef __cplusplus
}
#endif
#endif
