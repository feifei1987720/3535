#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <glob.h>
#include <errno.h>
#ifndef __GLIBC__
	#include <term.h>
#endif
#include "ftp.h"
#include "text.h"
#include "rc.h"
#include "Exmod_Ftp.h"

#define VERSION			"1.0.7"

#define DIRECTION_NONE		0
#define DIRECTION_OUT		1
#define DIRECTION_PUT		1
#define DIRECTION_IN		2
#define DIRECTION_GET		2
#define DIRECTION_FXP		3

#define COMMAND_NONE		0

#define STATREGEX1		"^ *([-a-z]+) +([0-9]+) +([a-z]+) +([a-z]+) +([0-9]+) (.{12}) (.*)$"


/* Increase this for a steadier but less accurate ETA and Speed readout */
#define SPEEDS_AVERAGE		5

/* Maximum number of concurrent connections */
#define MAX_CONNECTIONS		3

/* Define a couple of very handy macros */
#define CURRENT_CONNECTION	connection[atoi(config(variables, "CURRENT_CONNECTION"))]
#define LAST_CONNECTION		connection[atoi(config(variables, "LAST_CONNECTION"))]

struct ftpconnection *connection[MAX_CONNECTIONS];
struct qitem *leftqueue = NULL;
struct variable_list *variables = NULL;
struct alias_list *aliases = NULL;


struct qitem {
	int  id;
	int  permissions;
	char direction;
	char command;
	char status;
	char remotedir[256];
	char localdir[256];
	char localfilename[256];
	char remotefilename[256];
	struct ftpconnection *remoteconnection;
	struct ftpconnection *localconnection;
	unsigned long size;
	struct qitem *next;
};


int getconnected(struct ftpconnection *connection) {
	char *dir = NULL;
	if (!connection) return 0;
	if (!connection->hostname) return 0;
	if (connection->status > STATUS_WAIT_CONNECT) return 1;
	if (connection->remotedir[0] != '\0') {
		dir = strdup(connection->remotedir);
	}
	if (ftp_connect(connection)) {
		FREE(dir);
		return 0;
	}
	if (ftp_login(connection)) {
		FREE(dir);
		ftp_disconnect(connection);
		return 0;
	}
	if (dir) {
		if (!ftp_cwd(connection, dir)) 
			printf("Can't change directory to %s\n", dir);
		FREE(dir);
	}
	connection->status = STATUS_IDLE;
	setconfig(&variables, "USERNAME", connection->username);
	return 1;
}



int FTP_Connect(char *ip)
{
	if(NULL == ip)
	{
		fprintf(stderr,"arguments error!\n");
		return -1;
	}
	
	if (!parseurl(ip, CURRENT_CONNECTION)) {
		printf("Can't open %s\n", ip);
		return -1;
	}
	if (strlen(CURRENT_CONNECTION->password) == 0) {
		if (config(variables, "email"))
			strncpy(CURRENT_CONNECTION->password, config(variables, "email"), 32);
		else
			strncpy(CURRENT_CONNECTION->password, "user@quftp.com", 32);
	}
	if (CURRENT_CONNECTION->remotedir[0] != '\0') getconnected(CURRENT_CONNECTION);
	setconfig(&variables, "HOSTNAME", CURRENT_CONNECTION->hostname);
	
	return 0;
}


int FTP_Login(char *UserName, char *Password)
{
	if(NULL == UserName || NULL == Password)
	{
		fprintf(stderr,"arguments error!\n");
		return -1;
	}
	
	if (CURRENT_CONNECTION->status >= STATUS_IDLE) {
		ftp_disconnect(CURRENT_CONNECTION);
	}
	strncpy(CURRENT_CONNECTION->username, UserName, 32);
	strncpy(CURRENT_CONNECTION->password, Password, 32);

	CURRENT_CONNECTION->status = STATUS_WAIT_CONNECT;

	return 0;
}


int FTP_Close()
{
	int i;
	ftp_disconnect(CURRENT_CONNECTION);
	for (i = 0; i < MAX_CONNECTIONS; i++) {
		FREE(connection[i]);
	}
	while(variables){
		setconfig(&variables, variables->key, NULL);
	}
	config(variables, NULL);
	return 0;
}


int FTP_UploadJpg(char *FilePath, char *FileName)
{
	FILE *file;
	char *sFileData;
	
	struct ftpconnection *remoteconnection;
	int response,ret;
	char *param1,*buffer,line[MAX_LINE_SIZE];
	unsigned long file_size,index = 0,n = 0,temp;
	char uploadfile[128] = {0};
	unsigned long left,pos = 0;
	int ret11count = 0;

	if(NULL == FilePath || NULL == FileName)
	{
		fprintf(stderr,"arguments error!\n");
		return -1;
	}
	
	sprintf(uploadfile, "%s", FilePath);

	file = fopen(uploadfile, "rb");
	if(NULL == file)
	{
		return -1;
	}
	
	fseek(file,0,SEEK_END);
	file_size = ftell(file);
	fseek(file,0,0);
	printf("len:%ld\n",file_size);
	sFileData = (char *)malloc(file_size);
	if(NULL == sFileData)
	{
		return -1;
	}
	fread(sFileData,1,file_size,file);
	fclose(file);
	file = NULL;

	if (!getconnected(CURRENT_CONNECTION)) {
		printf("ujpeg: Not connected\n");
		return -1;
	}
	remoteconnection = CURRENT_CONNECTION;

	temp = file_size - 4096;
	left = file_size;

	ftp_sendline(remoteconnection, "TYPE I\r\n");
	ftp_getrc(remoteconnection, NULL, 0, 0);

	ftp_open_data_pasv(remoteconnection, NULL, 0);

	#if 0
	sprintf(line, "ALLO %lu\r\n", file_size);
	ftp_sendline(remoteconnection, line);
	response = ftp_getrc(remoteconnection, NULL, 0, 0);
	switch (202) {
		case 200 :
		case 202 : break;
		case 504 : printf("ALLO command does not implement parameter\n");
			   break;
		case 500 : printf("Server doesn't understand ALLO command\n");
			   break;
		case 501 : printf("Syntax error in ALLO command\n");
			   break;
		case 421 : printf("Server shutting down\n");
			   ftp_close_data(remoteconnection);
			   ftp_disconnect(remoteconnection);
			   return -1;
		case 530 : printf("Server says not logged in!\n");
			   ftp_close_data(remoteconnection);
			   ftp_disconnect(remoteconnection);
			   return -1;
		default  : printf("ALLO: Unrecognised error %d\n", response);
			   ftp_close_data(remoteconnection);
			   ftp_disconnect(remoteconnection);
			   break;
	}

	buffer = config(remoteconnection->variables, "passive");
	
	ftp_open_data(remoteconnection);
	#endif

	sprintf(line, "STOR %s\r\n", FileName);
	ftp_sendline(remoteconnection, line);
	response = ftp_getrc(remoteconnection, NULL, 0, 0);
	switch (response) {
		case 125 :
		case 150 : 
			#if 0
			if (remoteconnection->status == STATUS_WAITING)
				   if (!ftp_wait_data(remoteconnection)) {
				   	   printf("ftp_wait_data\n");
					   ftp_close_data(remoteconnection);
					   return -response;
				   }
			#endif
			 break;
		case 532 : printf("Server requires an account.. not supported yet\n");
			   ftp_close_data(remoteconnection);
			   return -response;
			   break;
		case 450 : printf("Remote file is busy\n");
			   ftp_close_data(remoteconnection);
			   return -response;
			   break;
		case 452 : printf("Insufficient storage space\n");
			   ftp_close_data(remoteconnection);
			   return -response;
			   break;
		case 553 : printf("Permission denied to upload \"%s\"\n", FileName);
			   ftp_close_data(remoteconnection);
			   return -response;
			   break;
		case 500 : printf("Server doesn't understand STOR command\n");
			   ftp_close_data(remoteconnection);
			   return -response;
			   break;
		case 501 : printf("Syntax error in STOR command\n");
			   ftp_close_data(remoteconnection);
			   return -response;
			   break;
		case 421 : printf("Server shutting down\n");
			   ftp_close_data(remoteconnection);
			   ftp_disconnect(remoteconnection);
			   return -response;
			   break;
		case 530 : printf("Server says not logged in!\n");
			   ftp_close_data(remoteconnection);
			   ftp_disconnect(remoteconnection);
			   return -response;
			   break;
		default  : printf("Unrecognised error %d\n", response);
			   ftp_close_data(remoteconnection);
			   ftp_disconnect(remoteconnection);
			   break;
	}

	//ftp_wait_data(remoteconnection);
	
	printf("[%s %d] Start Send Data\n", __func__, __LINE__);

	//buffer = sFileData;

	while(left > 0)
	{
		if(left > 4096)
		{
			n = 4096;
		}
		else
		{
			n = left;
		}

		n = ftp_write_data(remoteconnection, sFileData + pos, n);
		if(n == -1)
		{
			if(errno == 11)
			{
				ret11count++;
				printf("%s %d: n = %d <= 0 !errno = %d\n", __func__, __LINE__, n, errno);

				if(ret11count >= 2000)
				{
					printf("%s %d: n = %d <= 0 !ret11count = %d\n", __func__, __LINE__, n, ret11count);
					return -1;
				}
				
				usleep(30000);
				continue;
			}
			break;
		}

		pos += n;
		left -=n;
	}

	printf("[%s %d] Send Data  End\n", __func__, __LINE__);

#if 0
	n = 4096;

	while( index < temp ){
			ftp_write_data(remoteconnection, &buffer[index], n);
			index += 4096;
	}
	n = file_size % 4096;
	ftp_write_data(remoteconnection, &buffer[index], n );
#endif

	ftp_close_data(remoteconnection);
	ftp_getrc(remoteconnection, NULL, 0, 0);
	FREE(sFileData);
	//FREE(buffer);
	return 0;
}


int cmd_quit (char *command_line) {
	printf("execute quit command in quftp\n");
	return -1;
}

int cmd_cd(char *command_line) {
	char *param;
	if (!getconnected(CURRENT_CONNECTION)) {
		printf("cd: Not connected\n");
		return 0;
	}
	getnextword(command_line, GNW_START);
	param = getnextword(command_line, GNW_NEXT);
	if (!param || !*param) {
		printf("Must specify a directory to change\n");
		return 0;
	} else {
		if (!ftp_cwd(CURRENT_CONNECTION, param)) return 0;
	}
	return 1;
}


int cmd_mkdir(char *command_line) {
	char *param;
	if (!getconnected(CURRENT_CONNECTION)) {
		printf("mkdir: Not connected\n");
		return 0;
	}
	getnextword(command_line, GNW_START);
	param = getnextword(command_line, GNW_NEXT);
	if (!param || !*param) {
		printf("Must specify a directory to create\n");
		return 0;
	} else {
		if (!ftp_mkd(CURRENT_CONNECTION, param)) 
			return 0;
		else 
			printf("Created directory %s on %s in %s\n", param, CURRENT_CONNECTION->hostname, CURRENT_CONNECTION->remotedir);
	}
	return 1;
}


int ftp_upload(char *ServerName, int ServerPort, char *FolderPath, char *UserName, char *Password, char *FilePath, char *FileName) 
{
	int i;
	int ret;
	char line[MAX_LINE_SIZE];

	if(NULL == ServerName || NULL == FolderPath || NULL == UserName || NULL == Password || NULL == FilePath || NULL == FileName)
	{
		fprintf(stderr,"arguments error!\n");
		return -1;
	}

	setconfig(&variables, "PROGRAM_NAME", "quftp");
	setconfig(&variables, "VERSION", VERSION);
	setconfig(&variables, "CURRENT_CONNECTION", "0");
	setconfig(&variables, "LAST_CONNECTION", "1");

	for (i = 0; i < MAX_CONNECTIONS; i++) {
		connection[i] = ftp_new();
		connection[i]->variables = variables;
		getcwd((char *)connection[i]->localdir, 256);
	}
	
	ret = FTP_Connect(ServerName);
	if(ret == -1)
	{
		printf("ftp connect fail\n");
		FTP_Close();
		return -1;
	}

	ret = FTP_Login(UserName, Password);
	if(ret == -1)
	{
		printf("ftp login fail\n");
		FTP_Close();
		return -1;
	}


	sprintf(line,"cd %s",FolderPath);
	if( cmd_cd(line)==0 )
	{
		sprintf(line,"mkdir %s",FolderPath);
		if(cmd_mkdir(line)==0)
		{
			FTP_Close();
			cmd_quit("");
			return -1;
		}else{
			sprintf(line,"cd %s",FolderPath);
			if( cmd_cd(line) == 0 )
			{
				FTP_Close();
				return -1;
			}
		}
	}

	ret = FTP_UploadJpg(FilePath, FileName); 
	if(ret == -1)
	{
		printf("ftp uploadjpg fail\n");
		FTP_Close();
		return -1;
	}
	
	FTP_Close();
	return 0;
}



