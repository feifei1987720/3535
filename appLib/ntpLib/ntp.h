#ifndef __NTP_H_
#define __NTP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <netdb.h>

#define  WEST  -1 
#define  EAST  1
#define  UTC   0
int getNtpTime(struct hostent* phost,struct timeval *pTimeVal);
int SetSystemTime(int timeZone,struct hostent* phost);

int SetNtpTime(int time_zone,char *ntp_server, int port);

int GetTimeZone( char * timezone,int * hour ,int * minute);



#endif

