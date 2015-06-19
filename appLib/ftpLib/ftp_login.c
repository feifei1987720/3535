/* FTP Functions
   $Revision: 1.3 $
   $Date: 2002/06/24 04:04:32 $
*/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "rc.h"
#include "ftp.h"


int ftp_login(struct ftpconnection *connection) {
	int response, show_motd = 0;
	char line[4096];
	if (connection->status == STATUS_IDLE) {
		ftp_sendline(connection, "REIN\r\n");
		response = ftp_getrc(connection, NULL, 0, 0);
		switch (response) {
			case 220 : break; /* OK REIN */
			case 120 : printf("Service not ready... try again later\n");
				   ftp_disconnect(connection);
				   return response;
				   break;
			case 421 : printf("Service unavailable\n");
				   ftp_disconnect(connection);
				   return response;
				   break;
			case 500 : printf("This server doesn't allow you to re-login\n");
				   return response;
				   break;
			case 502 : printf("This server doesn't allow you to re-login\n");
				   return response;
				   break;
			default	 : printf("Unknown response %d\n", response);
				   return response;
				   break;
		}
	} else if (connection->status != STATUS_WAIT_LOGIN) {
		printf("ftp_login: Server is not waiting for login\n");
		return -1;
	}
	printf("Send username %s\n", connection->username);
	sprintf(line, "USER %s\r\n", connection->username);
	response = ftp_sendline(connection, line);
	response = ftp_getrc(connection, NULL, 0, 0);
	switch (response) {
		case 230 : printf("Don't need to send password\n");
			   connection->status = STATUS_IDLE;
			   break;
		case 530 : printf("Not logged in as %s\n", connection->username);
			   return response;
			   break;
		case 500 : printf("Server doesn't understand USER\n");
			   ftp_disconnect(connection);
			   return response;
			   break;
		case 501 : printf("Server can't understand USER command\n");
			   ftp_disconnect(connection);
			   return response;
			   break;
		case 421 : printf("Service unavailable\n");
			   ftp_disconnect(connection);
			   return response;
			   break;
		case 332 : printf("This program does not support user accounts\n");
			   return response;
			   break;
		case 331 : printf("Send password xxxxxxx\n");
			   sprintf(line, "PASS %s\r\n", connection->password);
			   response = ftp_sendline(connection, line);
			   //if (log_get_level() >= 5) show_motd = 1;
			   response = ftp_getrc(connection, NULL, 0, show_motd);
			   switch (response) {
				   case 230 :
				   case 202 : break;
				   case 530 : printf("Login incorrect\n");
					      ftp_disconnect(connection);
					      connection->status = STATUS_WAIT_CONNECT;
					      return response;
					      break;
				   case 500 :
				   case 503 :
				   case 501 : printf("Server doesn't understand PASS\n");
					      ftp_disconnect(connection);
					      return response;
					      break;
				   case 421 : printf("ftp_login: Service is unavailable\n");
					      ftp_disconnect(connection);
					      return response;
					      break;
				   case 332 :
				   default  : printf("Unknown response to PASS: %d\n", response);
					      return response;
					      break;
			   }
			   break;
		default  : printf("Unknown response to USER: %d\n", response);
			   return response;
			   break;
	}
	connection->status = STATUS_IDLE;
	printf("Getting remote system type\n");
	ftp_sendline(connection, "SYST\r\n");
	response = ftp_getrc(connection, connection->system, 128, 0);
	if (response == 215) {
		printf("Remote system type is %s\n", connection->system);
	} else {
		memset(connection->system, 0, sizeof(connection->system));
	}
	printf("Logged in to %s as %s\n", connection->hostname,connection->username);
	connection->status = STATUS_IDLE;
	ftp_pwd(connection);
	return 0;
}

