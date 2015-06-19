/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Zhong caiwang <caiwang213@gmail.com>
 * Apr 2011-04-16
 *
 * Protocol common defines.
 */

#ifndef __PROTO_STDDEF_H__
#define __PROTO_STDDEF_H__

//#ifndef C_ASSERT
//#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]
//#endif

#include "RSNetProtocol.h"
#include "RS_Upgrade.h"


typedef struct tag_PACKAGE_S
{   
	PackHead stHead;
	S8       s8Data[MaxPackSize];
}PACKAGE_S;


#endif
