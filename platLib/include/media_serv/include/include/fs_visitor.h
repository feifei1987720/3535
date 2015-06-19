/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Zhong caiwang <caiwang213@gmail.com>
 * Apr 2011-06-08
 *
 *  The FsVisitor is used to visit dvr file system which we can get media frames 
 */

#ifndef __FS_VISITOR_H__
#define __FS_VISITOR_H__
#include "comm_def.h"

#define MDEC_ALIGN_LEN  (320 * 1024)          /**/
#define MDEC_BUF_LEN    ((MDEC_ALIGN_LEN)* 2) /**/

#define	ERR_GETFILE_OK   (0)
#define	ERR_GETFILE_ERR  (-1)
#define	ERR_GETFILE_EOF  (1)
#define	ERR_GETFILE_OET  (2)    /* out of end time */

#define	REC_TYPE_NORMAL (0x1)
#define	REC_TYPE_ALARM  (0x2)
#define	REC_TYPE_ALL    (0xFFFFFFF)

#define MAX_VISITOR_NUM  (10)

class DevInterface;
class FsVisitor 
{
	public:
		FsVisitor( DevInterface *pIntf );
		~FsVisitor();
		S32 Init( S32 s32ChnNO, U32 u32RecType, DateTime *pStart, DateTime *pEnd , S32 s32Exact = 0);
		S32 StartVisit();
		S32 EndVisit();
		S32 ReStartVisit();

		/** 
		 * @remarks    
		 * @return     0, -1.1
		 */
		S32 GetFileFrame( S8 **ps8FrameBuff, S32 *s32FrameLen, U64 *u64CurFrmTime );
		S32 JumpToNextFile();
		S32 SetRePosTime( DateTime *pStart, DateTime *pEnd );

		S32 GetVisitorNum();

	public:
		U64 m_u64CurFrmTime;           /*  */
	private:
		/*  */
		S32      m_s32ChnNO;
		DateTime m_stStart;            /*  */
		DateTime m_stEnd;              /*  */
		S32      m_s32Offset;          /*  */
		U32      m_u32RecType;         /* */

		U8  *m_pu8RdvrfsAlign;         /* DVRFSBUF*/
		U8  *m_pu8SdBuf;               /* */
		U8  *m_pu8SdBufLf;             /* */
		U8  *m_pu8SdFrmHd;              /* */
		U32 m_u32SdBufLen;             /* */
		U32 m_u32SdFrmLen;             /* */
		U32 m_u32SdBufLfLen;           /* */
		FRAMETYPE_E m_enCurFrmType;    /* */

		/*  */
		S32  m_s32FsFd;                   /* */
		REC_OUTPUT_DAY_S m_stRecOutPut;   /*  */
		BOOL m_bFirstRead;                /*  */

		DevInterface  *m_pIntf;

		static S32 m_s32VisitorNum;

};
#endif
