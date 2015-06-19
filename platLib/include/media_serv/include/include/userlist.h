/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Zhong caiwang <caiwang213@gmail.com>
 * Apr 2011-04-16
 *
 * User list.
 */
#ifndef __USERLIST_H__
#define __USERLIST_H__
#include "user.h"

using namespace std;
#include <list>
#include <algorithm>  

class UserList
{
	/*  */
	public:
		UserList();
		~UserList();
		User *AddUser( U32 u32Key = 11 );
		S32  DelUser( U32 u32Key );
		S32  DelUser( User *pUser );
		User *FindUser( U32 u32Key );
		User *FirstEntry();
		User *NextEntry();
		S32  GetUserNum();
		S32  Release();
		S32  ReleaseDead();
		S32  StartCollector();
		bool IsUserExit( U32 u32Key );
		
		inline VOID Lock() { pthread_mutex_lock( &m_Mutex ); }
		inline VOID UnLock() { pthread_mutex_unlock( &m_Mutex ); }
	
		BOOL   IsEmpty();
		
		/*  */
	protected:
		S32    Collector();
	
		static VOID *ThreadEntry(VOID *pArg);
	private:
		U32                    m_u32UserNum;
		U32                    m_u32UserSeq;
		list<User *>           m_List;
		list<User *>::iterator m_CurIter;
		pthread_mutex_t        m_Mutex;
};

#endif
