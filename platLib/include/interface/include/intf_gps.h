
#ifndef __INTF_GPS_H__
#define __INTF_GPS_H__

//#include "comm_def.h"
#include "RSGpsInfo.h"

#define GPSINFO_FILE "/log/0/gpsinfo/gpsinfofile"
//#define GPSINFO_FILE "./gpsinfo/gpsinfofile"

#define DATETIME_TO_MDVRTIME(stDateTime, stMDVRTime) \
	do \
	{ \
		stMDVRTime.year  	= stDateTime.Year - 2000;  \
		stMDVRTime.month 	= stDateTime.Month; \
		stMDVRTime.day   	= stDateTime.Day;  	\
		stMDVRTime.hour  	= stDateTime.Hour;  \
		stMDVRTime.minute 	= stDateTime.Minute;\
		stMDVRTime.second 	= stDateTime.Second;\
}while(0)

#define MDVRTIME_TO_DATETIME(stMDVRTime, stDateTime) \
	do \
	{ \
		stDateTime.Year 	= stMDVRTime.year + 2000;  \
		stDateTime.Month	= stMDVRTime.month; \
		stDateTime.Day  	= stMDVRTime.day;   \
		stDateTime.Hour 	= stMDVRTime.hour;  \
		stDateTime.Minute 	= stMDVRTime.minute;\
		stDateTime.Second 	= stMDVRTime.second;\
	}while(0)

//double 
#define SWAP_DOUBLE(dbValue) \
	do \
{ \
	unsigned long long ullTmpValue = 0;\
	ullTmpValue = ((*((unsigned long long *)&dbValue)<< 32) + (*((unsigned long long *)&dbValue) >> 32));   \
	dbValue = *((double *)&ullTmpValue); \
}while(0)


typedef struct tag_GPS_SEARCH_DAY_RETURN_S{
	DateTime StartTime;
	DateTime EndTime;
}GPS_SEARCH_DAY_RETURN_S;

typedef enum{
	READ_GPS_PACKET_RETURN_ERROR 	= -1,
	READ_GPS_PACKET_RETURN_SUCCESS 	= 0,
	READ_GPS_PACKET_RETURN_FILEOVER = 1,
	READ_GPS_PACKET_RETURN_TIMEEND 	= 2,
}READ_GPS_PACKET_RETURN_E;

class IGps
{
	public:
		IGps(){}
		virtual ~IGps(){}
	public:

		virtual S32 StartGpsReplay(DateTime StartTime, DateTime EndTime, VOID **pReturnHandle)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 EndGpsReplay(VOID **pHandle)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 ReStartGpsReplay(DateTime StartTime, DateTime EndTime, VOID *pHandle)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual READ_GPS_PACKET_RETURN_E ReadGpsReplayPacket(RSGpsInfoPacket_t *pGpsInfoPacket, VOID *pHandle)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 JumpNextGpsFile(VOID *pHandle)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    GPS
		 */
		virtual S32 SwitchGpsInfoStruct(SYSSTATEDATA_MDVR *pGpsInfo, RSGpsInfoPacket_t *pGpsInfoPacket)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    	gps
		 */
		virtual S32 GetCurGpsInfo(RSGpsInfoPacket_t *pGpsInfoPacket)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}


		/** 
		 * @remarks    	GPS
		 * @return     	.
		 */
		virtual S32 GetGpsReportFrequence()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    	GPS
		 * @return     	GPS1 0.
		 */
		virtual S32 GetGpsEnable()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    			GPS, EndSearchGpsFileByDay
		 * @pSearchDate  		
		 * @pGpsSearchReturn 	GPS,
		 * @return     			0 -1.
		 */
		virtual S32 StartSearchGpsFileByDay(DateTime *pSearchDate, GPS_SEARCH_DAY_RETURN_S **pGpsSearchReturn, U32 *pu32FileNum)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    			GPSStartSearchGpsFileByDay
		 * @pGpsSearchReturn 	GPS
		 * @return     			0 -1.
		 */
		virtual S32 EndSearchGpsFileByDay(GPS_SEARCH_DAY_RETURN_S *pGpsSearchReturn)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    	 GPS
		 * @pSearchDate  
		 * @pu32Calendar GPS1
		 * @return     	 0 -1.
		 */
		virtual S32 StartSearchGpsFileByMonth(DateTime *pSearchDate, U32 *pu32Calendar)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
};

#endif
