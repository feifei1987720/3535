/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Zhong caiwang <caiwang213@gmail.com>
 * Apr 2011-04-16
 *
 * type defines.
 */

#ifndef __TYPE_H__
#define __TYPE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef unsigned char           U8;
typedef unsigned char           UCHAR;
typedef unsigned short          U16;
typedef unsigned int            U32;
typedef unsigned long           ULONG;
typedef unsigned long long      U64;

typedef char                    S8;
typedef short                   S16;
typedef int                     S32;
typedef long                    LONG;

typedef long long               S64;

typedef char                    CHAR;
typedef char*                   PCHAR;

typedef float                   FLOAT;
typedef double                  DOUBLE;
typedef void                    VOID;
typedef bool                    BOOL;

typedef U64                     PTS_TIME;


typedef unsigned long           SIZE_T;
typedef unsigned long           LENGTH_T;

typedef unsigned char           BYTE;
typedef unsigned short          WORD;
typedef unsigned int            UINT;

typedef LONG *                  PLONG;
typedef PLONG                   LPLONG;
typedef unsigned long*		    HANDLE;

typedef U8                      ULONG8;
typedef U32                     ULONG32;
typedef S32                     INT32;
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif 
