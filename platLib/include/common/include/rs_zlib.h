/*
 * =====================================================================================
 *
 *       Filename:  rs_zlib.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Thursday, December 06, 2012 10:06:33 HKT
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef _RS_ZLIB_H_
#define _RS_ZLIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#define RS_Z_WINDOWS_BIT -15
	
//Return Code
#define RS_Z_OK            0
#define RS_Z_STREAM_END    1
#define RS_Z_NEED_DICT     2
#define RS_Z_ERRNO        (-1)
#define RS_Z_STREAM_ERROR (-2)
#define RS_Z_DATA_ERROR   (-3)
#define RS_Z_MEM_ERROR    (-4)
#define RS_Z_BUF_ERROR    (-5)
#define RS_Z_VERSION_ERROR (-6)


	typedef enum{
		RS_Z_COMPRESS_LEVEL_DEFAULT = 0,
		RS_Z_COMPRESS_LEVEL_NOCOMPRESSION,
		RS_Z_COMPRESS_LEVEL_BEST_SPEED,
		RS_Z_COMPRESS_LEVEL_BEST_COMPRESSION,
	}RS_Z_COMPRESS_LEVEL_E;

	/*  
	 * @remarks     ZLIB
	 * @pDestBuf	[out] 
	 * @pDestLen		[in/out] in:MAXout:
	 * @pSourceBuf	[in] 
	 * @SourceLen	[in] 
	 * @enLevel		[in] 
	 * @return       RS_Z_OK, .
	*/
	int RS_Z_Compress(char *pDestBuf, unsigned long *pDestLen,
			const char *pSourceBuf, unsigned long SourceLen,
			RS_Z_COMPRESS_LEVEL_E enLevel = RS_Z_COMPRESS_LEVEL_DEFAULT);

	/*  
	 * @remarks     ZLIB
	 * @pDestBuf	[out] 
	 * @pDestLen		[in/out] in:MAXout:
	 * @pSourceBuf	[in] 
	 * @SourceLen	[in] 
	 * @return      RS_Z_OK, .
	*/
	int RS_Z_UnCompress(char *pDestBuf, unsigned long *pDestLen,
			const char *pSourceBuf, unsigned long SourceLen);

	
#ifdef __cplusplus
};
#endif
#endif
