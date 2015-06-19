/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Zhong caiwang <caiwang213@gmail.com>
 * Apr 2011-04-16
 *
 * Remote playback relevant interface defines.
 */

#ifndef __INTF_PLAYBACK_H__
#define __INTF_PLAYBACK_H__

//
#define Manual_FileType 0x01
#define Time_FileType  	0x02
#define Alarm_FileType  0x04

typedef struct tag_RECFILE_INFO_S
{
	S32      s32ChnNO;
	U32      u32Type;
	U32      u32Size;
	DateTime stStart;
	DateTime stEnd;
	S8       s8FileName[128];
} RECFILE_INFO_S;

typedef struct tag_REC_CALENDAR_S
{
	BOOL bHasRec[31];    /*  */
	U32  u32Type[31];    /*  */
} REC_CALENDAR_S;

typedef struct tag_REC_DAY_HALFHOUR_S
{
	BOOL bHasRec[48];   /*  */
	U32  u32Type[48];   /*  */
} REC_DAY_HALFHOUR_S;

typedef struct tag_LOG
{
	S32 s32ChnNO;
	U32 u32Type;
	DateTime stPoint;
} LOG_S;

#define RECORDTIME_TO_DATETIME( stDateTime, stRecordTime ) \
	do \
	{ \
		stDateTime.Year   = stRecordTime.year + 2000;  \
		stDateTime.Month  = stRecordTime.month; \
		stDateTime.Day    = stRecordTime.day;   \
		stDateTime.Hour   = stRecordTime.hour;  \
		stDateTime.Minute = stRecordTime.minute;\
		stDateTime.Second = stRecordTime.second;\
}while(0)

#define DATETIME_TO_RECORDTIME( stRecordTime, stDateTime ) \
	do \
	{ \
	 	stRecordTime.year   = stDateTime.Year - 2000;  \
	 	stRecordTime.month  = stDateTime.Month; \
	 	stRecordTime.day    = stDateTime.Day;   \
	 	stRecordTime.hour   = stDateTime.Hour;  \
	 	stRecordTime.minute = stDateTime.Minute;\
	 	stRecordTime.second = stDateTime.Second;\
	}while(0)
class IPlayback
{
	public:
		IPlayback() { }
		virtual ~IPlayback() { }

	public: 
		/**              
		 * @remarks    
		 * @return     true,false
		 */
		virtual BOOL ValidateHdd()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/**              
		 * @remarks     ,, 
		 * @s32FsmFd    [Out]FileManager,
		 * @return      , -1
		 */
		virtual S32 GetRecFileNum( U32 u32Channel, U32 u32Type, DateTime *pStart, DateTime *pEnd, S32 *s32FsmFd )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/**              
		 * @remarks     
		 * @s32FsmFd   [in] 
		 * @pstRecList [out] 
		 * @return     0,-1
		 */
		virtual S32 GetRecFileList( S32 s32FsmFd, RECFILE_INFO_S *pstRecList )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks     ,
		 * @u32Channel  
		 * @return     0,-1
		 */
		virtual S32 GetRecFileCalendar( U32 u32Channel, DateTime *pDate, U32 u32Type, REC_CALENDAR_S *pstCalendar )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks     ,
		 * @return      ,-1
		 */
	    virtual S32 DevFsOpen( S32 s32ChnNO )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks     
		 * @return      0,-1
		 */
	    virtual S32 DevFsClose( S32 s32FsmFd )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    pStarts32Offset, pStart
		 * 			   pStartpEnd
		 * @s32FsFd    [in]
		 * @pStart     [in]
		 * @pEnd       [in]
		 * @s32Offset  [in]pStart
		 * @pstOutPut  [out]
		 * @return     0,-1
		 */
	    virtual S32 DevFsSeek( S32 s32FsFd, DateTime *pStart, DateTime *pEnd, S32 s32Offset, RECFILE_INFO_S *pstOutPut )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks   
		 * @return    ,-1,0
		 */
	    virtual S32 DevFsReadData( S32 s32FsFd, S8 *ps8ReadBuff, S32 s32ReadLen , S32 *s32Offset = NULL)
//	    virtual S32 DevFsReadData( S32 s32FsFd, S8 *ps8ReadBuff, S32 s32ReadLen )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    
		 * @pstOutPut  [out]
		 * @return     0,-1
		 */
	    virtual S32 DevFsGetNextFile( S32 s32FsFd, REC_OUTPUT_DAY_S *pstRecOutPut )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks      
		 * @pstRecOutPut [out]s32ChnNOpStartpEnd
		 * @return       0,-1
		 */
	    virtual S32 DevFsFindStart( S32 s32FsFd, U32 u32RecType, DateTime *pStart, DateTime *pEnd,  REC_OUTPUT_DAY_S *pstRecOutPut )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    pTimePoints32Offset,pTimePointpstRecOutPut
		 * @s32FsFd    [in]
		 * @pTimePoint [in]
		 * @s32Offset  [in]pTimePoint
		 * @pstOutPut  [out]
		 * @return     0,-1
		 */
	    virtual S32 DevFsSeek( S32 s32FsFd, REC_TIME_S *pTimePoint, S32 s32Offset, REC_OUTPUT_DAY_S *pstRecOutPut )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    
		 * @s32FsFd    [in]
		 */
	    virtual S32 DevFsFindEnd( S32 s32FsFd )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks     ,
		 * @u32Channel  
		 * @return     0,-1
		 */
		virtual S32 GetRecFileDayHalfHour( U32 u32Channel, DateTime *pDate, U32 u32Type, REC_DAY_HALFHOUR_S *pstDayHalfHour )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    
		 * @s32FsFd    [in]
		 */
		virtual S32 StartSearchRecord(U32 u32Chn, U32 u32FileType, DateTime StartTime, DateTime EndTime, S32 *ps32FileCount)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    , 
		 * @s32FsFd    [in]
		 */
		virtual S32 GetOneSearchRecordResult(S32 s32SearchFd, S32 s32FileListNum, RECFILE_INFO_S *pFileListInfo)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    
		 * @s32FsFd    [in]
		 */
		virtual S32 EndSearchRecord(S32 s32SearchFd)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks     
		 * @return     0,-1
		 */
#define MAX_LOG_PAGE_NUM       6
		virtual S32 GetLog( U32 u32Type, DateTime *pStart, DateTime *pEnd, U32 u32Opera, U32 *u32ExpNum, LOG_S *pLogList )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks     
		 * @return     1 0 
		 */
		virtual bool GetDevicePlaybackState()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
};

#endif

