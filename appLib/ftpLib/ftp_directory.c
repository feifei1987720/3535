/* FTP Directory Functions
   $Revision: 1.4 $
   $Date: 2003/03/10 22:31:27 $
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
#include "ftp.h"


char *ftp_pwd(struct ftpconnection *connection) {
	char line[MAX_LINE_SIZE];
	char *temp, *temp2;
	int length, response;
	if (!ftp_check_ready(connection, 1)) return NULL;
	ftp_sendline(connection, "PWD\r\n");
	response = ftp_getrc(connection, line, MAX_LINE_SIZE, 0);
	switch (response) {
		case 257 : break;
		case 500 : printf("Server doesn't understand PWD\n");
			   return NULL;
			   break;
		case 501 : printf("Server doesn't understand PWD parameters\n");
			   return NULL;
			   break;
		case 502 : printf("Server doesn't understand PWD\n");
			   return NULL;
			   break;
		case 421 : printf("Service unavailable\n");
			   ftp_disconnect(connection);
			   return NULL;
			   break;
		case 550 : printf("Directory information unavailable: %d\n", response);
			   return NULL;
			   break;
		default  : printf("Unknown response to PWD: %d\n", response);
			   return NULL;
			   break;
	}
	temp = strstr(line, "\"");
	temp++;
	temp2 = strstr(temp, "\"");
	length = temp2-temp;
	strncpy(connection->remotedir, temp, length);
	connection->remotedir[length] = 0;
	return connection->remotedir;
}

int ftp_cwd(struct ftpconnection *connection, char *path) {
	char line[MAX_LINE_SIZE];
	int response;
	if (!ftp_check_ready(connection, 1)) return 0;
	if (strcmp(path, connection->remotedir) == 0) return 1;
	sprintf(line, "CWD %s\r\n", path);
	ftp_sendline(connection, line);
	response = ftp_getrc(connection, NULL, 0, 0);
	switch (response) {
		case 250 : ftp_pwd(connection);
			   printf("Changed directory on %s to %s\n",connection->hostname, connection->remotedir);
			   return 1;
			   break;
		case 530 : printf("Not logged in\n");
			   return 0;
			   break;
		case 500 : printf("Server doesn't understand CWD\n");
			   return 0;
			   break;
		case 501 : printf("Server doesn't understand CWD parameters\n");
			   return 0;
			   break;
		case 502 : printf("Server doesn't understand CWD\n");
			   return 0;
			   break;
		case 421 : printf("Service unavailable\n");
			   ftp_disconnect(connection);
			   return 0;
			   break;
		case 550 : printf("Directory \"%s\" does not exist\n", path);
			   return 0;
			   break;
		default  : printf("Unknown response to CWD: %d\n", response);
			   return 0;
			   break;
	}
	return 0;
}

int ftp_mkd(struct ftpconnection *connection, char *path) {
	char line[MAX_LINE_SIZE];
	int response;
	if (!ftp_check_ready(connection, 1)) return 0;
	sprintf(line, "MKD %s\r\n", path);
	ftp_sendline(connection, line);
	response = ftp_getrc(connection, NULL, 0, 0);
	switch (257) {
		case 257 : printf("Created %s on %s\n",path, connection->remotedir);
			   return 1;
			   break;
		case 530 : printf("Not logged in\n");
			   return 0;
			   break;
		case 500 : printf("Server doesn't understand \"MKD %s\"\n", path);
			   return 0;
			   break;
		case 501 : printf("Server doesn't understand MKD parameters\n");
			   return 0;
			   break;
		case 502 : printf("Server doesn't understand MKD\n");
			   return 0;
			   break;
		case 421 : printf("Service unavailable\n");
			   ftp_disconnect(connection);
			   return 0;
			   break;
		case 550 : printf("Not allowed to create directory %s\n", path);
			   return 0;
			   break;
		default  : printf("Unknown response to MKD: %d\n", response);
			   return 0;
			   break;
	}
	return 0;
}

int ftp_rmd(struct ftpconnection *connection, char *path) {
       char line[MAX_LINE_SIZE];
       int response;
       if (!ftp_check_ready(connection, 1)) return 0;
       sprintf(line, "RMD %s\r\n", path);
       ftp_sendline(connection, line);
       response = ftp_getrc(connection, NULL, 0, 0);
       switch (response) {
               case 250 : printf("Removed %s in %s\n",path, connection->remotedir);
                          return 1;
                          break;
               case 530 : printf("Not logged in\n");
                          return 0;
                          break;
               case 500 : printf("Server doesn't understand \"RMD %s\"\n", path);
                          return 0;
                          break;
               case 501 : printf("Server doesn't understand RMD parameters\n");
                          return 0;
                          break;
               case 502 : printf("Server doesn't understand RMD\n");
                          return 0;
                          break;
               case 421 : printf("Service unavailable\n");
                          ftp_disconnect(connection);
                          return 0;
                          break;
               case 550 : printf("Not allowed to remove directory %s\n", path);
                          return 0;
                          break;
               default  : printf("Unknown response to RMD: %d\n", response);
                          return 0;
                          break;
       }
       return 0;
}

