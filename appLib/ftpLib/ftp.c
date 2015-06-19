/* FTP Functions
   $Revision: 1.5 $
   $Date: 2003/03/10 22:31:26 $
   vim: sw=4 ts=4
*/

#define _XOPEN_SOURCE 500

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


#include <stdlib.h>
#include <features.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <arpa/inet.h>
#include <fnmatch.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "ftp.h"






/*
 * Global Variables
 */

/* STAT or LIST Formats
 * %P	UNIX style permissions
 * %L	Number of hard links
 * %O	Owner
 * %G	Group
 * %S	Size
 * %N	File Name
 *
 * %Y	Year
 * %M	Month name
 * %n	Month number
 * %D	Day of month
 * %h	Hour
 * %m	Minute
 * %s	Second
 */

char *stat_formats[] = {
	/* drwxr-xr-x   2 ftp      ftp          1024 Jan 23 21:37 incoming */
	"%P %L %O %G %S %M %D %h:%m %N",

	/* d--x--x--x   2 ftp      ftp          1024 Oct 17  1998 bin */
	"%P %L %O %G %S %M %D %Y %N"
};

struct ftpconnection *ftp_new(void) {
	struct ftpconnection *connection;
	connection = malloc(sizeof(struct ftpconnection));
	if(NULL == connection)
	{
		return NULL;
	}

	memset(connection->hostname, 0, 64);
	memset(connection->system, 0, 128);
	memset(connection->username, 0, 32);
	memset(connection->password, 0, 32);
	memset(connection->remotedir, 0, 256);
	memset(connection->localdir, 0, 256);
	connection->port = 0;
	connection->status = STATUS_NEW;
	connection->controlfd = 0;
	connection->datafd = 0;
	connection->dataconfd = 0;
	return connection;
}

int ftp_connect(struct ftpconnection *connection) {
	struct hostent *hp = NULL;
	int response, len = 128;
	int readynow = 0;
	char *ipaddr;
	char resultstring[4096];
	struct sockaddr_in controladdr;
	int flags;
	struct timeval timeout;

	if (connection->status < STATUS_WAIT_CONNECT) {
		return -1;
	}
	if (!connection->hostname || !*connection->hostname) {
		printf("Must initialize struct ftpconnection first\n");
		return -1;
	}
	if (!(hp = (struct hostent *)gethostbyname(connection->hostname))) {
		printf("Can't resolve %s: %d\n", connection->hostname, HERROR);
		return -1;
	}

	

	if ((connection->controlfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		return -1;
	}
	printf("Resolving %s\n", connection->hostname);
	fflush(stdout);
	memset(&controladdr, 0, sizeof(controladdr));
	controladdr.sin_family = AF_INET;
	controladdr.sin_port = htons(connection->port);

	if(hp == NULL || hp->h_addr_list == NULL || hp->h_addr_list[0] == NULL)
	{
		return -1;
	}

//	memcpy(&(controladdr.sin_addr), *((struct in_addr **)hp->h_addr_list), sizeof(struct in_addr));
	printf("%s %d: hp->h_addr_list[0] = %p\n", __func__, __LINE__, hp->h_addr_list[0]);
	controladdr.sin_addr = *(struct in_addr *)hp->h_addr_list[0];
	
	//memcpy((char *)&controladdr.sin_addr, hp->h_addr_list[0], hp->h_length);
	ipaddr = inet_ntoa(controladdr.sin_addr);
	printf("%s resolves to %s\n", connection->hostname, ipaddr);

	timeout.tv_sec  = 3;  
	timeout.tv_usec = 0;

	setsockopt(connection->controlfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
	setsockopt(connection->controlfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

	if (connect(connection->controlfd, (SOCKADDR *)&controladdr, sizeof(controladdr)) < 0) {
		printf("Connection refused to %s\n", connection->hostname);
		shutdown(connection->controlfd,SHUT_RDWR);
		close(connection->controlfd);
		return -1;
	}

	if (getsockname(connection->controlfd, (SOCKADDR *)&controladdr,&len)< 0) {
		perror("getsockname");
		return -1;
	}
	
	connection->localip = controladdr.sin_addr.s_addr;
	connection->status = STATUS_WAIT_LOGIN;
	while (!readynow) {
		response = ftp_getrc(connection, resultstring, 4096, 0);
		switch (response) {
			case 120 : printf("Service not yet ready.. waiting for 15 seconds\n");
				   sleep(15);
				   break;
			case 220 : readynow = 1;
				   printf("Connected to %s on port %d\n",connection->hostname, connection->port);
				   break;
			default  : printf("%s\n", ftp_error_message(response));
				   return response;
				   break;
		}
	}
	return 0;
}

int ftp_disconnect(struct ftpconnection *connection) {
	if (connection->status >= STATUS_IDLE) {
		int response;
		response = ftp_sendline(connection, "QUIT\r\n");
		response = ftp_getrc(connection, NULL, 0, 0);
		if (response == 500)
			printf("Braindead server doesn't understand QUIT.. closing connection\n");
		if (close(connection->controlfd) < 0) {
			perror("close");
			return -1;
		}
	}
	if (connection->hostname)
		printf("Disconnected from %s\n", connection->hostname);
	connection->controlfd = 0;
	return 0;
}

int ftp_reset(struct ftpconnection *connection) {
	if (connection->status >= STATUS_WAIT_LOGIN) {
		printf("Still connected. Call ftp_disconnect() first!\n");
		return 1;
	}
	memset(connection, 0, sizeof(struct ftpconnection));
	connection->status = STATUS_NEW;
	return 0;
}

char *ftp_readline(struct ftpconnection *connection) {
	int n = 0;
	int length = 0;
	char *temp, *buffer;
	static char *read_line = 0;

	FREE(read_line);
	read_line = (char *)NULL;

	buffer = (char *)malloc(MAX_LINE_SIZE);
	if(NULL == buffer)
	{
		return NULL;
	}

	if (connection->controlfd <= 0) {
		printf("ftp_readline: Not connected\n");
		FREE(buffer);
		return 0;
	}
	temp = buffer;
	while ((n = read(connection->controlfd, temp, 1)) > 0) {
		if (*temp == '\r') continue;
		if (*temp == '\n') break;
		if (*temp == '\0') break;
		if (length == MAX_LINE_SIZE) break;
		temp++; length++;
	}
	if (n < 0) {
		perror("read");
		FREE(buffer);
		return NULL;
	}
	if (n == 0)
	{
		printf("FTP server disconnected\n");
		FREE(buffer);
		return NULL;
	}
	read_line = (char *)malloc(length + 1);
	if(NULL == read_line)
	{
		FREE(buffer);
		return NULL;
	}
	strncpy(read_line, buffer, length);
	read_line[length] = '\0';
	//log_hex(LOG_DATA, "Read", read_line, length);
	FREE(buffer);
	return read_line;
}

int ftp_sendline(struct ftpconnection *connection, char *line) {
	int n = 0;
	int size;
	if (!line || !*line) return 0;
	size = strlen(line);
	if (size == 0) return 0;
	if (connection->status <= STATUS_WAIT_CONNECT) {
		printf("ftp_sendline: Not connected\n");
		return 0;
	}
	//log_hex(LOG_DATA, "Sent", line, size);
	if ((n = write(connection->controlfd, line, size)) < 0) {
		perror("ftp_sendline: write");
	}
	return n;
}

int ftp_getrc(struct ftpconnection *connection, char *data, int max_size, int print_line) {
	char *line;
	int response;

	if (connection->status <= STATUS_WAIT_CONNECT) {
		printf("ftp_getrc: Not connected\n");
		return 0;
	}
	while ((line = ftp_readline(connection))) {
		if (!*line) {
			printf("Big bad error.. empty string returned by ftp_readline\n");
			break;
		}
		if (print_line)
		{
			if (line[0] == ' ')
				printf("%s\n", (char *)(line + 1));
			else
				printf("%s\n", (char *)(line + 4));
		}
		if (IS_NUMBER(line[0]) && IS_NUMBER(line[1]) && IS_NUMBER(line[2]) && line[3] == ' ')
			break;
	}
	if (!line || !*line) return 0;
	sscanf(line, "%d ", &response);
	printf("ftp_getrc() = %d\n", response);
	if (data)
		strncpy(data, (line + 4), max_size);
	if (response >= 100)
		return response;

	FREE(line);
	
	return 0;
}

int ftp_parse_pasv(char *line, char *address) {
	int h1, h2, h3, h4, p1, p2, port;
	if (line == NULL) return 0;
	sscanf(line, "%i,%i,%i,%i,%i,%i", &h1, &h2, &h3, &h4, &p1, &p2);
	port = (p1 << 8) + p2;
	sprintf(address, "%d.%d.%d.%d", h1, h2, h3, h4);
	return port;
}

int ftp_parse_permissions(char *perm) {
	int permissions = 0;
	if (perm[1] != 'r' && perm[1] != '-') return -1;
	if (perm[0] == 'd') permissions |= 2048;
	if (perm[1] == 'r') permissions |= 256;
	if (perm[2] == 'w') permissions |= 128;
	if (perm[3] == 'x') permissions |= 64;
	if (perm[4] == 'r') permissions |= 32;
	if (perm[5] == 'w') permissions |= 16;
	if (perm[6] == 'x') permissions |= 8;
	if (perm[7] == 'r') permissions |= 4;
	if (perm[8] == 'w') permissions |= 2;
	if (perm[9] == 'x') permissions |= 1;
	return permissions;
}

int ftp_check_ready(struct ftpconnection *connection, int print) {
	if (connection->status <= STATUS_WAIT_CONNECT) {
		if (print) printf("Not connected\n");
		return 0;
	}
	if (connection->status == STATUS_WAIT_LOGIN) {
		if (print) printf("Not logged in\n");
		return 0;
	}
	if (connection->status == STATUS_ERROR) {
		if (print) printf("Connection is in error state!\n");
		return 0;
	}
	if (connection->status >= STATUS_WAITING) {
		if (print) printf("Server is busy. Please wait\n");
		return 0;
	}
	return 1;
}

int ftp_stat(struct ftpconnection *connection, char *filename, struct filedetails *details) {
	char *buffer = NULL;
	int result = 0;
	if (!details) return -1;
	//log(LOG_CALLS, "\"%s\"", filename);
	if (!ftp_check_ready(connection, 1)) return -1;
	if (details) memset(details, 0, sizeof(struct filedetails));
	buffer = (char *)malloc(strlen(filename) + 15);
	if(NULL == buffer)
	{
		return -1;
	}
	sprintf(buffer, "STAT %s\r\n", filename);
	ftp_sendline(connection, buffer);
	FREE(buffer);
	buffer = ftp_readline(connection);
	result = atoi(buffer);
	switch (result) {
		case 211 :
		case 212 :
		case 213 :
		case 250 : break;
		case 500 : printf("Server doesn't understand STAT\n");
				return result; break;
		case 501 : printf("Bad parameters to STAT\n");
				return result; break;
		case 502 : printf("Server doesn't understand STAT\n"); 
				return result; break;
		case 421 : printf("Service not available\n");
				return result; break;
		default  : printf("Unknown response to STAT: %d\n", result); 
				return result; break;
	}
	if (buffer[3] == '-') {
		/* Read a line at a time */
		while ((buffer = ftp_readline(connection))) {
			if (strstr(buffer, "total ") == buffer) continue;
			break;
		}
		if (buffer[0] == '2' && buffer[1] == '1') return 1;
		if (!buffer) {
			printf("Disconnected!\n");
			ftp_disconnect(connection);
			return 1;
		}
		if (strstr(buffer, "/bin/ls") != 0) {
			ftp_readline(connection);
			return 10;
		}
		if (ftp_split_ls(buffer, details) == 1) {;
			ftp_strip_illegal(details->filename);
			ftp_getrc(connection, NULL, 0, 0);
			return 0;
		} else {
			ftp_getrc(connection, NULL, 0, 0);
			return 1;
		}
	} else {
		printf("Bad response to STAT: %s\n", buffer);
	}

	FREE(buffer);
	
	return 0;
}

int ftp_split_ls(char *buffer, struct filedetails *details) {
	int stat_index, complete_match = 0;

	if (!buffer || !*buffer) return 0;

	/* Try matching STAT output */
	stat_index = 0;
	while (stat_formats[stat_index]) {
		char *format_p, *buffer_p;
		complete_match = 1;
		buffer_p = buffer;
		format_p = stat_formats[stat_index];

		while (*format_p) {
			while (*buffer_p == ' ' || *buffer_p == '\t')
				buffer_p++;				/* Skip whitespace */
			if (*format_p == '%') {
				switch (*++format_p) {
					case '%' : if (*buffer_p == '%') {	/* %% means a literal % character */
								   buffer_p++;
							   } else {
								   complete_match = 0;
							   }
							   break;
					case 'P' : { char *permissions_p; /* UNIX style permissions */
							   char *permissions = (char *)malloc(4096);
							   if(NULL == permissions)
							   {	
									return 0;
							   }
							   memset(permissions, 0, 4096);
							   permissions_p = permissions;
							   format_p++;
							   while (*buffer_p && (*buffer_p != *format_p)) 
								   *permissions_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
							   details->permissions = ftp_parse_permissions(permissions);
							   if (details->permissions == 0) complete_match = 0;
							   FREE(permissions);
							   break; }
					case 'L' : { char *links_p; /* UNIX style links */
							   char *links = (char *)malloc(4096);
							   if(NULL == links)
							   {	
									return 0;
							   }
							   memset(links, 0, 4096);
							   links_p = links;
							   format_p++;
							   while (*buffer_p && (*buffer_p != *format_p)) 
								   *links_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
							   for (links_p = links; *links_p; links_p++)
								   if (*links_p < '0' || *links_p > '9') complete_match = 0;
							   details->links = atoi(links);
							   FREE(links);
							   break; }
					case 'O' : { char *owner_p; /* UNIX style owner */
							   char *owner = (char *)malloc(4096);
							   if(NULL == owner)
							   {	
							   		return 0;
							   }
							   memset(owner, 0, 4096);
							   owner_p = owner;
							   format_p++;
							   while (*buffer_p && (*buffer_p != *format_p)) 
								   *owner_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
							   strncpy(details->owner, owner, 63);
							   FREE(owner);
							   break; }
					case 'G' : { char *group_p; /* UNIX style group */
							   char *group = (char *)malloc(4096);
							   if(NULL == group)
							   {	
									return 0;
							   }
							   memset(group, 0, 4096);
							   group_p = group;
							   format_p++;
							   while (*buffer_p && (*buffer_p != *format_p)) 
								   *group_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
							   strncpy(details->group, group, 63);
							   FREE(group);
							   break; }
					case 'S' : { char *size_p; /* UNIX style size */
							   char *size = (char *)malloc(4096);
							   if(NULL == size)
							   {	
									return 0;
							   }
							   memset(size, 0, 4096);
							   size_p = size;
							   format_p++;
							   while (*buffer_p && (*buffer_p != *format_p)) 
								   *size_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
							   for (size_p = size; *size_p; size_p++)
								   if (*size_p < '0' || *size_p > '9') complete_match = 0;
							   details->size = atol(size);
							   FREE(size);
							   break; }
					case 'N' : { char *filename_p; /* UNIX style filename */
							   char *filename = (char *)malloc(4096);
							   if(NULL == filename)
							   {	
									return 0;
							   }
							   memset(filename, 0, 4096);
							   filename_p = filename;
							   format_p++;
							   while (*buffer_p && (*buffer_p != *format_p)) 
								   *filename_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
							   strncpy(details->filename, filename, 254);
							   FREE(filename);
							   break; }
					case 'Y' : { char *year_p; /* UNIX style year */
							   char *year = (char *)malloc(4096);
							   if(NULL == year)
							   {	
									return 0;
							   }
							   memset(year, 0, 4096);
							   year_p = year;
							   format_p++;
							   while (*buffer_p && (*buffer_p != *format_p)) 
								   *year_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
							   for (year_p = year; *year_p; year_p++)
								   if (*year_p < '0' || *year_p > '9') complete_match = 0;
							   FREE(year);
							   break; }
					case 'M' : { char *month_p; /* UNIX style month name */
							   char *month = (char *)malloc(4096);
							   if(NULL == month)
							   {	
									return 0;
							   }
							   memset(month, 0, 4096);
							   month_p = month;
							   format_p++;
							   while (*buffer_p && (*buffer_p != *format_p)) 
								   *month_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
							   FREE(month);
							   break; }
					case 'n' : { char *month_p; /* UNIX style month number */
							   char *month = (char *)malloc(4096);
							   if(NULL == month)
							   {	
									return 0;
							   }
							   memset(month, 0, 4096);
							   month_p = month;
							   format_p++;
							   while (*buffer_p && (*buffer_p != *format_p)) 
								   *month_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
							   for (month_p = month; *month_p; month_p++)
								   if (*month_p < '0' || *month_p > '9') complete_match = 0;
							   FREE(month);
							   break; }
					case 'm' : { char *minute_p; /* UNIX style minute number */
							   char *minute = (char *)malloc(4096);
							   if(NULL == minute)
							   {	
									return 0;
							   }
							   memset(minute, 0, 4096);
							   minute_p = minute;
							   format_p++;
							   while (*buffer_p && (*buffer_p != *format_p)) 
								   *minute_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
							   for (minute_p = minute; *minute_p; minute_p++)
								   if (*minute_p < '0' || *minute_p > '9') complete_match = 0;
							   FREE(minute);
							   break; }
					case 'D' : { char *mday_p; /* UNIX style mday number */
							   char *mday = (char *)malloc(4096);
							   if(NULL == mday)
							   {	
									return 0;
							   }
							   memset(mday, 0, 4096);
							   mday_p = mday;
							   format_p++;
							   while (*buffer_p && (*buffer_p != *format_p)) 
								   *mday_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
							   for (mday_p = mday; *mday_p; mday_p++)
								   if (*mday_p < '0' || *mday_p > '9') complete_match = 0;
							   FREE(mday);
							   break; }
					case 's' : { char *second_p; /* UNIX style second number */
							   char *second = (char *)malloc(4096);
							   if(NULL == second)
							   {	
									return 0;
							   }
							   memset(second, 0, 4096);
							   second_p = second;
							   format_p++;
							   while (*buffer_p && (*buffer_p != *format_p)) 
								   *second_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
							   for (second_p = second; *second_p; second_p++)
								   if (*second_p < '0' || *second_p > '9') complete_match = 0;
							   FREE(second);
							   break; }
					case 'h' : { char *hour_p; /* UNIX style hour number */
							   char *hour = (char *)malloc(4096);
							   if(NULL == hour)
							   {	
									return 0;
							   }
							   memset(hour, 0, 4096);
							   hour_p = hour;
							   format_p++;
							   while (*buffer_p && (*buffer_p != *format_p)) 
								   *hour_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
							   for (hour_p = hour; *hour_p; hour_p++)
								   if (*hour_p < '0' || *hour_p > '9') complete_match = 0;
							   FREE(hour);
							   break; }
					default  : printf("Unknown character %c in STAT format description\n", *format_p);
							   break;
				}
			}
			if (complete_match == 0) break;
			if (!*format_p++) break;
		}
		if (complete_match == 1) break;
		stat_index++;
	}
	return complete_match;
}

int ftp_delete(struct ftpconnection *connection, char *filename)  {
	int result;
	char line[4096];
	char resultstring[4096];
	if (!filename) return 0;
	if (!ftp_check_ready(connection, 1)) return 0;
	sprintf(line, "DELE %s\r\n", filename);
	ftp_sendline(connection, line);
	result = ftp_getrc(connection, resultstring, 4096, 0);
	switch (result) {
		case 250 : printf("Deleted %s\n", filename); 
				return 1; break;
		case 450 : printf("Can't delete %s: %s\n", filename, resultstring);
				break;
		case 421 : printf("%s\n", resultstring); 
				break;
		case 500 : printf("This server does not support the DELE command\n");
				break;
		case 501 : printf("Invalid parameters to DELE command\n");
				break;
		case 502 : printf("This server does not support the DELE command\n");
				break;
		case 530 : printf("Not logged in correctly\n"); 
				break;
		case 550 : printf("Can't delete %s: %s\n", filename, resultstring);
				break;
		case 553 : printf("Not allowed to delete %s\n", filename); 
				break;
		default  : printf("Unrecognised response to DELE: %d %s\n", result, resultstring);
			    break;
	}
	return 0;
}

char *ftp_glob(struct ftpconnection *connection, char *wildcard, int status) {
	static int index = 0;
	static char pathname[4096], working[4096];
	int response;
	static char *return_list[500];
	char *retstring;
	if (!ftp_check_ready(connection, 1)) return NULL;
	if (status == 0) {
		memset(pathname, 0, 4096);
		memset(working, 0, 4096);
		memset(return_list, 0, sizeof(*return_list) * 500);
		// Separate the path and filename
		index = strlen(wildcard);
		while (index) {
			if (wildcard[index] == '/') {
				strncpy(pathname, wildcard, index + 1);
				strncpy(working, &wildcard[index + 1], 4096);
				break;
			}
			index--;
		}
		if (strlen(working) == 0) strncpy(working, wildcard, 4096);
		response = ftp_nlst(connection, pathname, return_list, 500);
		if (response <= 0) return NULL;
		index = 0;
	}
	while (return_list[index] != NULL) {
		int eflags = FNM_PATHNAME | FNM_PERIOD | (1 << 3);
		if (fnmatch(wildcard, return_list[index], eflags) != 0) {
			index++;
			continue;
		}
		retstring = (char *)strdup(return_list[index]);
		return_list[index] = 0;
		index++;
		return retstring;
	}
	return NULL;
}

int ftp_exists(struct ftpconnection *connection, char *filename) {
	/* Check if a file exists on the server */
	/* Return codes
	   < 0		Don't know
	     0		Doesn't exist
	   > 0		Does exist
	 */
	char line[4096];
	int result;

	if (!filename) return 0;
	if (!ftp_check_ready(connection, 1)) return 0;

	//log(LOG_CALLS, "(%p, %s)", connection, filename);
	sprintf(line, "STAT %s\r\n", filename);
	ftp_sendline(connection, line);
	result = ftp_getrc(connection, NULL, 0, 0);
	switch (result) {
		case 213 : return 1;
		case 211 : return 1;
		case 212 : return 1;
		case 421 : printf("STAT error.. %d\n", result); 
					return -1; break;
		case 450 : return 0;
		case 500 : break;
		case 501 : break;
		case 502 : break;
		case 530 : printf("Not logged in correctly\n"); 
					return -1; break;
		default  : break;
	 }
	return -1;
}

char *ftp_error_message(int error) {
	switch(error) {
		case 110 : return "Restart marker reply"; break;
		case 120 : return "Service ready in nnn minutes."; break;
		case 125 : return "Data connection already open; transfer starting."; break;
		case 150 : return "File status okay; about to open data connection."; break;
		case 200 : return "Command okay."; break;
		case 202 : return "Command not implemented, superfluous at this site."; break;
		case 211 : return "System status, or system help reply."; break;
		case 212 : return "Directory status."; break;
		case 213 : return "File status."; break;
		case 214 : return "Help message."; break;
		case 215 : return "NAME system type."; break;
		case 220 : return "Service ready for new user."; break;
		case 221 : return "Service closing control connection."; break;
		case 225 : return "Data connection open; no transfer in progress."; break;
		case 226 : return "Closing data connection successfully."; break;
		case 227 : return "Entering Passive Mode (h1,h2,h3,h4,p1,p2)."; break;
		case 230 : return "User logged in, proceed."; break;
		case 250 : return "Requested file action okay, completed."; break;
		case 257 : return "PATHNAME created."; break;
		case 331 : return "User name okay, need password."; break;
		case 332 : return "Need account for login."; break;
		case 350 : return "Requested file action pending further information."; break;
		case 421 : return "Service not available, closing control connection."; break;
		case 425 : return "Can't open data connection."; break;
		case 426 : return "Connection closed; transfer aborted."; break;
		case 450 : return "Requested file action not taken - File unavailable."; break;
		case 451 : return "Requested action aborted: local error in processing."; break;
		case 452 : return "Requested action not taken - Insufficient storage space in system."; break;
		case 500 : return "Syntax error, command unrecognized."; break;
		case 501 : return "Syntax error in parameters or arguments."; break;
		case 502 : return "Command not implemented."; break;
		case 503 : return "Bad sequence of commands."; break;
		case 504 : return "Command not implemented for that parameter."; break;
		case 530 : return "Not logged in."; break;
		case 532 : return "Need account for storing files."; break;
		case 550 : return "Requested action not taken - File unavailable."; break;
		case 551 : return "Requested action aborted: page type unknown."; break;
		case 552 : return "Requested file action aborted - Exceeded storage allocation"; break;
		case 553 : return "Requested action not taken - File name not allowed."; break;
		default  : return "Unknown error";
	}
}

