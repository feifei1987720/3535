/*
 * =====================================================================================
 *
 *       Filename:  intf_mdvr.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Thursday, October 11, 2012 09:59:57 HKT
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __INTF_MDVR_H__
#define __INTF_MDVR_H__

typedef enum{
	INTF_ELOCK_CTRL_TYPE_LOCK,
	INTF_ELOCK_CTRL_TYPE_UNLOCK,
	INTF_ELOCK_CTRL_TYPE_GETLOCKSTATUS,
	INTF_ELOCK_CTRL_TYPE_ICCARD_RES, //ICCard 
}INTF_ELOCK_CTRL_TYPE_E;

//LockStatus,HostLockStatus,LockStatusControl
#define INTF_ELLSTATUS_LOCKSTATUS_NULL		0
#define INTF_ELLSTATUS_LOCKSTATUS_ONLOCK	1
#define INTF_ELLSTATUS_LOCKSTATUS_UNLOCK 	2
#define INTF_ELLSTATUS_LOCKSTATUS_OVERTIME	3
#define INTF_ELLSTATUS_LOCKSTATUS_REMOVE	4

//AlarmType
#define INTF_ELLSTATUS_ALARMTYPE_NULL		0
#define INTF_ELLSTATUS_ALARMTYPE_REQUNLOCK	1
#define INTF_ELLSTATUS_ALARMTYPE_LOCKLOST	2

typedef struct{
	unsigned char HostLockStatus:4; //ONLOCK,UNLOCK
	unsigned char Reserved1:4;
	unsigned char Reserved2[3];
}INTF_ELHOST_STATUS_S;

typedef struct{
	unsigned char LockStatus:4;		   /* :,,() */
	unsigned char AlarmType:4;
	unsigned char LockStatusControl:4; /* :  ONLOCK, UNLOCK */
	unsigned char reserved1:4;
	unsigned char Reserved2[2];
}INTF_ELLOCK_STATUS_S;

typedef struct{
	char HostIMEI[16];
	INTF_ELHOST_STATUS_S ELHostStatus;
	INTF_ELLOCK_STATUS_S ELLockStatus[12];
}INTF_ELOCK_STATUS_S;

typedef enum{
	INTF_ELOCK_ICCARD_RESULT_ERR,				//
	INTF_ELOCK_ICCARD_RESULT_SUCCESS,			//
	INTF_ELOCK_ICCARD_RESULT_ICCARDID_ERR,		//
	INTF_ELOCK_ICCARD_RESULT_NO_AREASWITCH,		//
}INTF_ELOCK_ICCARD_RESULT_E;

typedef struct{
	union{
		struct{
			U32 u32ELockMask;
		}ELockCtrl_Request_Param_In;
		
		struct{
			U32 u32ICCardReqSeq;							//
			INTF_ELOCK_ICCARD_RESULT_E enICCardReqResult;	//
		}ICCard_Result_Param_In;
	};
}INTF_ELOCK_CTRL_PARAM_IN_S;

typedef struct{
	union{
		//INTF_ELECLOCK_HANDLE_TYPE_LOCK,
		//INTF_ELECLOCK_HANDLE_TYPE_UNLOCK,
		struct{
			U32 u32ELockMaskRes; //10
		}ELockReturn;

		//INTF_ELECLOCK_HANDLE_TYPE_GETLOCKSTATUS
		struct{
			U8 u8Result;						//10
			INTF_ELOCK_STATUS_S ELockStatusRes; //
		}ELockStatus;
	};
}INTF_ELOCK_CTRL_PARAM_OUT_S;


#define INTF_ELOCK_MAX_CARDID_LEN 12

typedef enum{
	INTF_ELOCK_ICCARD_TYPE_ONLOCK_E = 1,
	INTF_ELOCK_ICCARD_TYPE_UNLOCK_E = 2,
}INTF_ELOCK_ICCARD_TYPE_E;

typedef struct{
	INTF_ELOCK_STATUS_S			ELockStatusRes; //

	S8 s8ICCardID[INTF_ELOCK_MAX_CARDID_LEN];
	U8 u8ICCardType;				//INTF_ELOCK_ICCARD_TYPE_E  
	U8 Reserved[3];
	
	//DOUBLE dLongitude; 			//  0-180
	//DOUBLE dLatitude; 			//  0-180
	//S8  s8LatitudeDirect;		//  'N' or 'S'
	//S8  s8LongitudeDirect;   	//  'E' or 'W'
	//S8  s8State;				// 'A',  'V'

	U32 u32ICCardReqSeq;		//
}INTF_ELOCK_ICCARD_RESINFO_S;

//Request
typedef enum{
	INTF_ELOCK_REQUEST_LOCK,
	INTF_ELOCK_REQUEST_UNLOCK,
	INTF_ELOCK_STATUSREPORT,			// 
	
	INTF_ELOCK_CONNECTFAILED_ALARM,		// 
	INTF_ELOCK_BINDFAILED_ALARM,		// 
	INTF_ELOCK_TIMEOUT_ALARM,			// 
	INTF_ELOCK_REMOVE_ALARM,			// 
	INTF_ELOCK_ONLOCK_ALARM,			// 	
    INTF_ELOCK_UNLOCK_ALARM,	 		// 	
	
	INTF_ELOCK_ONLOCK_STATUS_ERR_ALARM,	//
	INTF_ELOCK_UNLOCK_STATUS_ERR_ALARM,	//

	INTF_ELOCK_ICCARD_RES_ALARM,		//
}INTF_ELOCK_REQUEST_E;

typedef struct{
	INTF_ELOCK_REQUEST_E RequestType;

	union{
		//INTF_ELOCK_REQUEST_STATUSREPORT
		INTF_ELOCK_STATUS_S			ELockStatusRes; //
		
		//INTF_ELOCK_ICCARD_REQ_ALARM
		INTF_ELOCK_ICCARD_RESINFO_S ICCardRes;		//ICCard, Seq ,SeqELockCtrl
	};
}INTF_ELOCK_REQUEST_CALLBACK_PARAM_S;

typedef void INTF_ELOCK_USERH;
typedef int (* ELockRequestCallBack)(INTF_ELOCK_REQUEST_CALLBACK_PARAM_S *pParam, INTF_ELOCK_USERH *pELUserH);

class IMdvr
{
	public:
		IMdvr() {}
		virtual ~IMdvr() {}
	
	public:
		
		/** 
		 * @remarks    
		 * @return     0, -1.
		 */
		virtual S32 RegisterELockRequestCallBack(ELockRequestCallBack ELockRequestFuncs, INTF_ELOCK_USERH *pELUserH)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    
		 * @return     0, -1.
		 */
		virtual S32 ELockCtrl(INTF_ELOCK_CTRL_TYPE_E ELockCtrlType, INTF_ELOCK_CTRL_PARAM_IN_S *pParamIn, INTF_ELOCK_CTRL_PARAM_OUT_S *pParamOut)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    
		 * @return     0, -1.
		 */
		virtual S32 ELockReportFrequency()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
 };

#endif
