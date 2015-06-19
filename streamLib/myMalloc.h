#ifndef __MALLOCW_H__
#define __MALLOCW_H__

// 
typedef enum _PdataFrom_
{
	PDATA_FROM_MALLOC = 0,		// malloc 
	PDATA_FROM_SHAREMALLOC		// ShareMalloc 
} PDATA_FROM_E;

void initMeMalloc();
void deinitMeMalloc();


void *MeAlloc ( long nbytes, const char *file, int line );
void *MeCalloc( long count, long nbytes, const char *file, int line );
void MeFree( void *ptr, const char *file, int line );
void *MeRealloc( void *ptr, long nbytes, const char *file, int line );
#define Malloc( nbytes )			MeAlloc((nbytes), __FILE__, __LINE__)
#define Calloc( count, nbytes )		MeCalloc((count), (nbytes), __FILE__, __LINE__)
#define Realloc( ptr, nbytes )		MeRealloc((ptr), (nbytes), __FILE__, __LINE__)
#define Free( ptr )					((void)(MeFree((ptr), __FILE__, __LINE__), (ptr) = NULL))


void *ShareMeAlloc( long nbytes, const char *file, int line );
void *ShareMeCalloc( long count, long nbytes, const char *file, int line );
void *ShareMeCopy( void *ptr, const char *file, int line );
void ShareMeFree( void *ptr, const char *file, int line );
#define ShareMalloc( nbytes )			ShareMeAlloc((nbytes), __FILE__, __LINE__)
#define ShareCalloc( count, nbytes )	ShareMeCalloc((count), (nbytes), __FILE__, __LINE__)
#define ShareCopy( ptr )				ShareMeCopy((ptr), __FILE__, __LINE__)
#define ShareFree( ptr )				((void)(ShareMeFree((ptr), __FILE__, __LINE__)))
void FreeMs( void *ptr, int bShareMem );

#endif // __MALLOCW_H__

