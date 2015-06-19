/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Zhong caiwang <caiwang213@gmail.com>
 * Apr 2011-04-16
 *
 * Common debug info define.
 */

#ifndef __COMM_DEG_H__
#define __COMM_DEG_H__

#include <string.h>
#include "stdio.h"
#include <stdarg.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define DBG_ON
#define ASSERT_ON
//#define PRINT_POST            /* print the positions in the file */

/* */
#define LEVEL_EMERG     0
#define LEVEL_ALERT     1
#define LEVEL_CRIT      2
#define LEVEL_ERR       3
#define LEVEL_WARNING   4
#define LEVEL_NOTICE    5
#define LEVEL_INFO      6
#define LEVEL_LINE		7
#define LEVEL_DEBUG     7

#define LEVEL_TRACE     8

#define LEVEL_ALL       7

#define LEVEL_NONE      0


/*:  0,; 0,(), */
#define DBG_LANGTAO  7       /*  */
#define DBG_COMM     7       /*  */
#define DBG_NET      7       /*  */
#define DBG_RTSP     7      /* RTSP */
#define DBG_NPW      7       /*  */
#define DBG_NPB      7       /* */
#define DBG_HTTP     7       /* HTTP*/
#define DBG_BCAST    7       /* BCAST*/
#define DBG_PLATF	 7       /* PLATFORM */
#define DBG_FTP      6       /* FTP*/
#define DBG_P2P      7      /* P2p */


/**/


#define PRINT(pszModeName, pszFmt, ...) \
	do {\
		fprintf(stderr,"[%s] [%s] "pszFmt,pszModeName,__func__, ##__VA_ARGS__);\
		fflush(stderr);\
	}while(0)
	
#define PRINT_TRACE(pszModeName, pszFmt, ...) \
	do {\
		fprintf(stderr,"[%s] func: [%s], file: [%s], line: [%d] "pszFmt,pszModeName, __func__, __FILE__,__LINE__,  ##__VA_ARGS__);\
		fflush(stderr);\
	}while(0)
#define PRINT_LINE(pszModeName, pszFmt, ...) \
	do {\
		fprintf(stderr,"[%s]--[%s]:[%d] "pszFmt,pszModeName, __func__, __LINE__,  ##__VA_ARGS__);\
		fflush(stderr);\
	}while(0)

#if defined(ASSERT_ON)
#define ASSERT(exp) ( (void)((exp)?1:( printf("ASSERT failed: func: [%s], line [%d], file: [%s]\n", \
				__func__, __LINE__,__FILE__),abort(), 0)) )
			 
#else
#define ASSERT(exp) 
#endif

/*  */
#if (DBG_COMM == 0) || !defined(DBG_ON)
    #define DBG_PRINT(u32Level,pszFmt, ...)
#else
#if defined(PRINT_POST)
#define DBG_PRINT(u32Level,pszFmt, ...)     \
	    do  \
    {   \
		        if (DBG_COMM >= u32Level)  \
		        {   \
					            PRINT_TRACE("COMM",pszFmt, ##__VA_ARGS__);   \
					        }   \
		    }   \
    while(0)
#else
#define DBG_PRINT(u32Level, pszFmt, ...)     \
	    do  \
    {   \
		        if (DBG_COMM > u32Level)  \
		        {   \
					            PRINT("COMM",pszFmt,##__VA_ARGS__);   \
					        }   \
				else if(DBG_COMM == u32Level) \
				{ \
					            PRINT_LINE("COMM",pszFmt,##__VA_ARGS__);   \
							}	\
		    }   \
    while(0)
#endif
#endif

/* LANGTAO */
#if (DBG_LANGTAO == 0) || !defined(DBG_ON)
    #define DBG_LANGTAO_PRINT(u32Level,pszFmt, ...)
#else
#if defined(PRINT_POST)
#define DBG_LANGTAO_PRINT(u32Level,pszFmt, ...)     \
	    do  \
    {   \
		        if (DBG_LANGTAO >= u32Level)  \
		        {   \
					            PRINT_TRACE("LANGTAO",pszFmt, ##__VA_ARGS__);   \
					        }   \
		    }   \
    while(0)
#else
#define DBG_LANGTAO_PRINT(u32Level, pszFmt, ...)     \
	    do  \
    {   \
		        if (DBG_LANGTAO >= u32Level)  \
		        {   \
					            PRINT("LANGTAO",pszFmt,##__VA_ARGS__);   \
					        }   \
		    }   \
    while(0)
#endif
#endif

/* BCAST */
#if (DBG_BCAST == 0) || !defined(DBG_ON)
    #define DBG_BCAST_PRINT(u32Level,pszFmt, ...)
#else
#if defined(PRINT_POST)
#define DBG_BCAST_PRINT(u32Level,pszFmt, ...)     \
	    do  \
    {   \
		        if (DBG_BCAST >= u32Level)  \
		        {   \
					            PRINT_TRACE("BCAST",pszFmt, ##__VA_ARGS__);   \
					        }   \
		    }   \
    while(0)
#else
#define DBG_BCAST_PRINT(u32Level, pszFmt, ...)     \
	    do  \
    {   \
		        if (DBG_BCAST >= u32Level)  \
		        {   \
					            PRINT("BCAST",pszFmt,##__VA_ARGS__);   \
					        }   \
		    }   \
    while(0)
#endif
#endif

/* HTTP */
#if (DBG_HTTP == 0) || !defined(DBG_ON)
    #define DBG_HTTP_PRINT(u32Level,pszFmt, ...)
#else
#if defined(PRINT_POST)
#define DBG_HTTP_PRINT(u32Level,pszFmt, ...)     \
	    do  \
    {   \
		        if (DBG_HTTP >= u32Level)  \
		        {   \
					            PRINT_TRACE("HTTP",pszFmt, ##__VA_ARGS__);   \
					        }   \
		    }   \
    while(0)
#else
#define DBG_HTTP_PRINT(u32Level, pszFmt, ...)     \
	    do  \
    {   \
		        if (DBG_HTTP >= u32Level)  \
		        {   \
					            PRINT("HTTP",pszFmt,##__VA_ARGS__);   \
					        }   \
		    }   \
    while(0)
#endif
#endif

/*  */
#if (DBG_NPB == 0) || !defined(DBG_ON)
    #define DBG_NPB_PRINT(u32Level,pszFmt, ...)
#else
#if defined(PRINT_POST)
#define DBG_NPB_PRINT(u32Level,pszFmt, ...)     \
	    do  \
    {   \
		        if (DBG_NPB >= u32Level)  \
		        {   \
					            PRINT_TRACE("NPB",pszFmt, ##__VA_ARGS__);   \
					        }   \
		    }   \
    while(0)
#else
#define DBG_NPB_PRINT(u32Level, pszFmt, ...)     \
	    do  \
    {   \
		        if (DBG_NPB >= u32Level)  \
		        {   \
					            PRINT("NPB",pszFmt,##__VA_ARGS__);   \
					        }   \
		    }   \
    while(0)
#endif
#endif

#if (DBG_NET == 0) || !defined(DBG_ON)
    #define DBG_NET_PRINT(u32Level,pszFmt, ...)
#else
#if defined(PRINT_POST)
#define DBG_NET_PRINT(u32Level,pszFmt, ...)     \
	    do  \
    {   \
		        if (DBG_NET >= u32Level)  \
		        {   \
					            PRINT_TRACE("NET",pszFmt, ##__VA_ARGS__);   \
					        }   \
		    }   \
    while(0)
#else
#define DBG_NET_PRINT(u32Level, pszFmt, ...)     \
	    do  \
    {   \
		        if (DBG_NET >= u32Level)  \
		        {   \
					            PRINT("NET",pszFmt,##__VA_ARGS__);   \
					        }   \
		    }   \
    while(0)
#endif
#endif

#if (DBG_RTSP == 0) || !defined(DBG_ON)
    #define DBG_RTSP_PRINT(u32Level,pszFmt, ...)
#else
#if defined(PRINT_POST)
#define DBG_RTSP_PRINT(u32Level,pszFmt, ...)     \
	    do  \
    {   \
		        if (DBG_RTSP >= u32Level)  \
		        {   \
					            PRINT_TRACE("RTSP",pszFmt, ##__VA_ARGS__);   \
					        }   \
		    }   \
    while(0)
#else
#define DBG_RTSP_PRINT(u32Level, pszFmt, ...)     \
	    do  \
    {   \
		        if (DBG_RTSP >= u32Level)  \
		        {   \
					            PRINT_LINE("RTSP",pszFmt,##__VA_ARGS__);   \
					        }   \
		    }   \
    while(0)
#endif
#endif

#if (DBG_P2P == 0) || !defined(DBG_ON)
    #define DBG_P2P_PRINT(u32Level,pszFmt, ...)
#else
#if defined(PRINT_POST)
#define DBG_P2P_PRINT(u32Level,pszFmt, ...)     \
	    do  \
    {   \
		        if (DBG_P2P >= u32Level)  \
		        {   \
					            PRINT_TRACE("TUTKP2P",pszFmt, ##__VA_ARGS__);   \
					        }   \
		    }   \
    while(0)
#else
#define DBG_P2P_PRINT(u32Level, pszFmt, ...)     \
	    do  \
    {   \
		        if (DBG_P2P >= u32Level)  \
		        {   \
					            PRINT_LINE("TUTKP2P",pszFmt,##__VA_ARGS__);   \
					        }   \
		    }   \
    while(0)
#endif
#endif
/**/
#if (DBG_PLATF == 0) || !defined(DBG_ON)
#define DBG_PLATF_PRINT(u32Level,pszFmt, ...)
#else
#define DBG_PLATF_PRINT(u32Level,pszFmt, ...)     \
	do  \
{   \
	if (u32Level >= 8)  \
	{   \
		PRINT_TRACE("PLATFORM",pszFmt, ##__VA_ARGS__);   \
	}   \
	else if(u32Level == 6) \
	{   \
		PRINT_LINE("PLATFORM",pszFmt, ##__VA_ARGS__);   \
	}   \
	else    \
	{   \
		PRINT("PLATFORM",pszFmt,##__VA_ARGS__);   \
	}   \
}   \
while(0)
#endif

/*FTP*/
#if (DBG_FTP == 0) || !defined(DBG_ON)
    #define DBG_FTP_PRINT(u32Level,pszFmt, ...)
#else
#define DBG_FTP_PRINT(u32Level,pszFmt, ...)     \
    do  \
    {   \
        if (DBG_FTP < u32Level)  \
        {   \
            PRINT_TRACE("FTP",pszFmt, ##__VA_ARGS__);   \
        }   \
		else if(DBG_FTP == u32Level) \
		{   \
			PRINT_LINE("FTP", pszFmt, ##__VA_ARGS__);   \
		}   \
		else    \
		{   \
			PRINT("FTP",pszFmt,##__VA_ARGS__);   \
		}   \
	}   \
    while(0)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif 

