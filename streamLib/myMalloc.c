/*
*******************************************************************************
**  Copyright (c) 2012, All rights reserved.
**  lsl
**  v1.0
**  : 2012.01.30
**  : malloc ; 
*******************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "myMalloc.h"
#include "commonLib.h"

static pthread_mutex_t mallocMutex;

void initMeMalloc()
{
	pthread_mutex_init(&mallocMutex, NULL);
}
void deinitMeMalloc()
{
	pthread_mutex_destroy(&mallocMutex);
}

/* ===== malloc  ===== */
void *MeAlloc( long nbytes, const char *file, int line )
{	
	void *ptr = ( void * )malloc( nbytes );
	if ( ptr == NULL )
		Printf( "Malloc Memory Failed, file: %s, line: %d !\r\n", file, line );
		
	return ptr;
}

void *MeCalloc( long count, long nbytes, const char *file, int line )
{	
	void *ptr = ( void * )calloc( count, nbytes );
	if ( ptr == NULL )
		Printf( "Calloc Memory Failed, file: %s, line: %d !\r\n", file, line );
		
	return ptr;
}

void MeFree( void *ptr, const char *file, int line )
{
	if ( ptr != NULL ) free( ptr );
}

void *MeRealloc( void *ptr, long nbytes, const char *file, int line )
{	
	ptr = ( void * )realloc( ptr, nbytes );
	if ( ptr == NULL )
		Printf( "Realloc Memory Failed, file: %s, line: %d !\r\n", file, line );
		
	return ptr;
}
/* ===== end malloc  ===== */

/* =====  ===== */
#define		SHARE_MEMORY_FLAG		0xCCBB2828

void *ShareMeAlloc( long nbytes, const char *file, int line )
{	
	long size = nbytes + sizeof(long)*2;
	long *ptr = ( long * )malloc( size );
	if ( ptr == NULL )
	{
		Printf( "Malloc Share Memory Failed, file: %s, line: %d !\r\n", file, line );
	}
	else
	{
		*ptr = SHARE_MEMORY_FLAG;
		*(ptr + 1) = 1;
	}		
	
	return (void *)(ptr + 2);
}

void *ShareMeCalloc( long count, long nbytes, const char *file, int line )
{	
	long size = nbytes + sizeof(long)*2;
	long *ptr = ( long * )calloc( count, size );
	if ( ptr == NULL )
	{
		Printf( "Calloc Share Memory Failed, file: %s, line: %d !\r\n", file, line );
	}
	else
	{
		*ptr = SHARE_MEMORY_FLAG;
		*(ptr + 1) = 1;
	}		
	return (void *)(ptr + 2);
}

void ShareMeFree( void *ptr, const char *file, int line )
{
	if ( ptr != NULL )
	{
		long *pmem = (long *)ptr - 2;
		if ( *pmem == (long)SHARE_MEMORY_FLAG )
		{
			pthread_mutex_lock(&mallocMutex);
			*(pmem + 1) -= 1;
			if ( *(pmem + 1) == 0 ) 
			{				
				free( pmem );
			}
			pthread_mutex_unlock(&mallocMutex);
		}
		else
		{
			Printf( "Free Share Memory Failed, file: %s, line: %d !\r\n", file, line );
			
		}
	}	
}

void *ShareMeCopy( void *ptr, const char *file, int line )
{
	if ( ptr == NULL )
	{
#if 0
		Printf( "Copy Share Memory Failed, Can't Copy NULL ptr; "
				"file: %s, line: %d !\r\n", file, line );
#endif
		return NULL;
	}
		
	long *pmem = (long *)ptr - 2;
	if ( *pmem == (long)SHARE_MEMORY_FLAG )
	{
		pthread_mutex_lock(&mallocMutex);
		*(pmem + 1) += 1;
		pthread_mutex_unlock(&mallocMutex);
		return ptr;
	}
	else
	{
		Printf( "Copy Share Memory Failed, Copy Error ptr; "
				"file: %s, line: %d !\r\n", file, line );
		
	}
	return NULL;
}
/* ===== end  ===== */

/*
* free
* ptr: free 
* bShareMem: PDATA_FROM_E
*/
void FreeMs( void *ptr, int bShareMem )
{
	if( PDATA_FROM_SHAREMALLOC == bShareMem )
	{
		ShareFree( ptr );
	}
	else
	{
		Free( ptr );
	}
}

