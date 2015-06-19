/*
 *  Boa, an http server
 *  Copyright (C) 1995 Paul Phillips <paulp@go2net.com>
 *  Some changes Copyright (C) 1996 Charles F. Randall <crandall@goldsys.com>
 *  Some changes Copyright (C) 1996 Larry Doolittle <ldoolitt@boa.org>
 *  Some changes Copyright (C) 1996-2002 Jon Nelson <jnelson@boa.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 1, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/* $Id: boa.c,v 1.99.2.2 2002/07/23 15:50:29 jnelson Exp $*/

#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include<sys/types.h>
#include <sys/resource.h>


#include "boa.h"
#include "Appro_interface.h"
#include "appro_api.h"
#include "web_translate.h"
#include "net_config.h"
#include "commonLib.h"
#include "databaseLib.h"
#include "sdk_global.h"
#include "sdk_struct.h"
#include "main.h"
#include "global.h"
#include "param.h"
#include "myThreadLib.h"

static pthread_t  HiBoaId;
static pthread_mutex_t HiBoaMutex;


/* globals */
int backlog = SO_MAXCONN;
time_t start_time;

int sighup_flag = 0;            /* 1 => signal has happened, needs attention */
int sigchld_flag = 0;           /* 1 => signal has happened, needs attention */
int sigalrm_flag = 0;           /* 1 => signal has happened, needs attention */
int sigterm_flag = 0;           /* lame duck mode */
time_t current_time;
int max_fd = 0;
int pending_requests = 0;

/* static to boa.c */
static void fixup_server_root(void);
static int create_server_socket(void);
static void drop_privs(void);

static int sock_opt = 1;
static int do_fork = 0;
//int devnullfd = -1;
static int server_s = -1;
//static int server_port = 0;

int BOA_GetServerSoc()
{
	return server_s;
}

//int main(int argc, char **argv)
void  *BoaThread(void)
{
	while(1)
	{
		/* main loop */
		select_loop(server_s);
		g_boaRun = 0;
		usleep(100*1000);
	}

	return NULL;
}

static int create_server_socket(void)
{
	int server_s;
	
	server_s = socket(SERVER_AF, SOCK_STREAM, IPPROTO_TCP);
	if (server_s == -1)
	{
		DIE("unable to create socket");
	}

	/* server socket is nonblocking */
	if (set_nonblock_fd(server_s) == -1)
	{
		DIE("fcntl: unable to set server socket to nonblocking");
	}

	if(fcntl(server_s, F_SETFL, O_NONBLOCK) == -1)
	{
		perror("set nonblock");
	}

	/* close server socket on exec so cgi's can't write to it */
	if (fcntl(server_s, F_SETFD, 1) == -1)
	{
		DIE("can't set close-on-exec on server socket!");
	}

	/* reuse socket addr */
	if ((setsockopt(server_s, SOL_SOCKET, SO_REUSEADDR, (void *) &sock_opt,sizeof (sock_opt))) == -1)
	{
		DIE("setsockopt");
	}
	server_ip = NULL;
	if(server_ip != NULL)
		Printf("server_s = %d,ip = %s \n",server_s,server_ip);
	/* internet family-specific code encapsulated in bind_server()  */
	if (bind_server(server_s, server_ip) == -1)
	{
		DIE("unable to bind");
	}

	/* listen: large number just in case your kernel is nicely tweaked */
	if (listen(server_s, backlog) == -1)
	{
		DIE("unable to listen");
	}
	return server_s;
}

static void drop_privs(void)
{
	/* give away our privs if we can */
	if (getuid() == 0)
	{
		struct passwd *passwdbuf;
		passwdbuf = getpwuid(server_uid);
		if (passwdbuf == NULL)
		{
			DIE("getpwuid");
		}
		if (initgroups(passwdbuf->pw_name, passwdbuf->pw_gid) == -1)
		{
			DIE("initgroups");
		}
		if (setgid(server_gid) == -1)
		{
			DIE("setgid");
		}
		if (setuid(server_uid) == -1)
		{
			DIE("setuid");
		}
		/* test for failed-but-return-was-successful setuid
		 * http://www.securityportal.com/list-archive/bugtraq/2000/Jun/0101.html
		 */
		/*        if (setuid(0) != -1) {
			  DIE("icky Linux kernel bug!");
			  }*/
	} else {
		if (server_gid || server_uid)
		{
			log_error_time();
			fprintf(stderr, "Warning: "
					"Not running as root: no attempt to change"
					" to uid %d gid %d\n", server_uid, server_gid);
		}
		server_gid = getgid();
		server_uid = getuid();
	}
}

/*
 * Name: fixup_server_root
 *
 * Description: Makes sure the server root is valid.
 *
 */

static void fixup_server_root()
{
	char *dirbuf;

	if (!server_root)
	{
#ifdef SERVER_ROOT
		server_root = strdup(SERVER_ROOT);
		if (!server_root)
		{
			perror("strdup (SERVER_ROOT)");
			exit(1);
		}
#else
		fputs("boa: don't know where server root is.  Please #define "
				"SERVER_ROOT in boa.h\n"
				"and recompile, or use the -c command line option to "
				"specify it.\n", stderr);
		exit(1);
#endif
	}

	if (chdir(server_root) == -1)
	{
		fprintf(stderr, "Could not chdir to \"%s\": aborting\n",server_root);
		exit(1);
	}

	dirbuf = normalize_path(server_root);
	free(server_root);
	server_root = dirbuf;
}

int BoaSocketClose()
{
	while(g_boaRun == 1)
		usleep(10000);
	if(server_s >0)
		close(server_s);
	server_s = 0;
	return 0;	
}
int BoaResetPort(int port)
{
	server_port = port;
	server_s = create_server_socket();
	commonSetUpdataBoaParamFlag(0);
	g_boaRun = 1;
	return 0;
}

int BoaInit()
{
	int c;                      /* command line arg */
	sdk_net_mng_cfg_t netcfg;
	memset(&netcfg,0,sizeof(netcfg));

	/* set umask to u+rw, u-x, go-rwx */
	c = umask(~0600);
	if (c == -1) {
		perror("umask");
		exit(1);
	}

	//devnullfd = open("/dev/null", 0);

	/* make STDIN and STDOUT point to /dev/null */
	//if (devnullfd == -1) {
	//	DIE("can't open /dev/null");
	//}

	//if (dup2(devnullfd, STDIN_FILENO) == -1) {
	//	DIE("can't dup2 /dev/null to STDIN_FILENO");
	//}

	//if (dup2(devnullfd, STDOUT_FILENO) == -1) {
	//	DIE("can't dup2 /dev/null to STDOUT_FILENO");
	//}

	/* but first, update timestamp, because log_error_time uses it */
	(void) time(&current_time);
		
	if (hash_table_init() < 0) {
			exit(1);
	}
	
	if(arg_hash_table_init() < 0) {
		exit(1);
	}

	if(uri_hash_table_init() < 0) {
		exit(1);
	}	
	pthread_mutex_init(&HiBoaMutex, NULL);


	fixup_server_root();
	read_config_files();// lsl
	
	if(databaseOperate(SDK_MAIN_MSG_NET_MNG_CFG, SDK_PARAM_GET, NULL, 0, &netcfg, sizeof(sdk_net_mng_cfg_t)) != sizeof(sdk_net_mng_cfg_t))
	{
		server_port = 80;
	}
	else
	{
		server_port = netcfg.http_port;
	}
	server_s = create_server_socket();

	init_signals();
	//drop_privs();  //xg 20150203  for haisi gdb 
	
	create_common_env();
	//build_needs_escape();

	if (max_connections < 1)
	{
		struct rlimit rl;

		/* has not been set explicitly */
		c = getrlimit(RLIMIT_NOFILE, &rl);
		if (c < 0)
		{
			perror("getrlimit");
			exit(1);
		}
		max_connections = rl.rlim_cur;
	}
	status.requests = 0;
	status.errors = 0;
	timestamp();
	start_time = current_time;
	return 0;
}


int  startBoaThread()
{
	g_boaRun = 1;

	BoaInit();
	if(pthread_create(&HiBoaId, NULL, (void *)BoaThread, NULL) < 0)
	{
		fprintf(stderr,"BoaThread error\r\n");
		return -1;
	}

	return 0;
}

int stopBoaThread()
{
	g_boaRun = 0;

	pthread_join(HiBoaId, NULL);
	return 0;
}


