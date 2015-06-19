/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Zhong caiwang <caiwang213@gmail.com>
 * Apr 2011-04-16
 *
 * Session list.
 */

#ifndef __SESSIONLIST_H__
#define __SESSIONLIST_H__
#include "session.h"

using namespace std;
#include <list>
#include <algorithm>  

class SessionList
{
	/*  */
	public:
		SessionList();
		~SessionList();
		Session *AddSession( S32 s32SessID );
		Session *FirstEntry();
		Session *NextEntry();
		S32      DelSession( S32 s32SessID );
		S32      DelSession( Session *pSession );
		S32      Release();
		S32      GetSessionNum();

		BOOL     IsEmpty();
		Session *FindSession( S32 s32SessID );
		
	/*  */
//	protected:
		inline  VOID Lock()   { pthread_mutex_lock( &m_Mutex ); }
		inline  VOID UnLock() { pthread_mutex_unlock( &m_Mutex ); }

	private:
		U32                       m_u32SessionNum;
		list<Session *>           m_List;
		list<Session *>::iterator m_CurIter;
		pthread_mutex_t           m_Mutex;
};
#endif
