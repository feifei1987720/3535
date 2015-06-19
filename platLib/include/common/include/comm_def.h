/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Zhong caiwang <caiwang213@gmail.com>
 * Apr 2011-04-16
 *
 * Common defines.
 */

#ifndef __COMM_DEF_H__
#define __COMM_DEF_H__

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <sys/types.h>   
#include <sys/stat.h>    

#include <sys/socket.h>  
#include <linux/rtnetlink.h> /* for NETLINK_ROUTE  */
#include <arpa/inet.h>       /* for inet_ntoa */
#include <net/if.h>          /* for "struct ifconf","struct ifreq" */
#include <sys/un.h>

#include <sys/wait.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>

#include "type.h"
#include "comm_dbg.h"
//#include "commondebug.h"
#include "comm_errno.h"

#define LISTENQ     500

#define TRUE     (1)
#define FALSE    (0)

//#define MaxIpLen	16
//#define MaxNameLen	36

#ifndef C_ASSERT
#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]
#endif

typedef struct sockaddr_storage SA_STORAGE;
typedef struct sockaddr_in      SA_IN;
typedef struct sockaddr         SA;


typedef struct
{
	WORD Year; //2011
	BYTE Month; //1 ~ 12
	BYTE Day; //1 ~ 31
	BYTE Hour; //0 ~ 23
	BYTE Minute; //0 ~ 59
	BYTE Second; //0 ~ 59
	BYTE Reserved;
}DateTime;

typedef enum tag_FRAMETYPE_E
{        
	FRAME_TYPE_I = 0,
	FRAME_TYPE_P,
	FRAME_TYPE_A,
	FRAME_TYPE_ALL,
	FRAME_TYPE_OTHERS
}FRAMETYPE_E;

typedef struct tag_REC_TIME_S
{           
    S32 s32Year;                 /**<08/09/10...*/
    S32 s32Month;
    S32 s32Day;
    S32 s32Hour;
    S32 s32Min;
    S32 s32Sec;
}REC_TIME_S;

typedef struct tag_REC_OUTPUT_S
{
    REC_TIME_S  stStart;
    REC_TIME_S  stEnd;
    U64 u64RecSize;    /**<BYTE*/
    U32 U32AlarmCount;
    U32 u32RecType;     /**<N/P*/
    U32 u32RecFormat;   /**<D1/HD1/CIF*/
    U32 u32RecAttr;         /**<REC_NORMAL/REC_ALARM/REC_MD, the top 2bit is REC_UNLOCK/REC_LOCK */
    U8  u8RecDevPath[80];/**<the disk path of the disk*/
    U32 u32DiskGroupNum;
    U32 u32AlarmOffset;
    S32 s32ChnNO;      // the channel_id;
    U32 u32DiskId;
    U32 u32EndSector;
}REC_OUTPUT_DAY_S;
#endif
