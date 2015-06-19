#ifndef __INTF_ALARMREPORT_H__
#define __INTF_ALARMREPORT_H__

//status
#define INTF_ALARM_START 1
#define INTF_ALARM_STOP  0

typedef struct{
	S32 s32Channel;
	S32 s32Status; //0:, 1:
	S8  s8RecState; // 
	S8  s8RecSchedule; //
	S8  s8MDRecEnable;//MD
	S8  s8Reserve[89];
}INTF_GENERAL_ALARM_S;

typedef enum{
	INTF_IO_ALARM_E= 0,        	//I/O
	INTF_MOTION_ALARM_E,       	//
	INTF_AI_ALARM_E,  			//
	INTF_AVD_ALARM_E,  		 	//
	INTF_AI_RULE_E,  	 	//
	INTF_VIDEOLOSS_ALARM_E, 		//
	INTF_HDD_ALARM_E,          	//
	INTF_DEVSTATUS_REPORT_E = 8,    //
    INTF_TELMODULE_ALARM_E,         //
	INTF_MDVR_ALARM_E,     		//
	INTF_ICCARD_ALARM_E,     	//IC
	INTF_RECORDSTATUS_ALARM_E, //
	INTF_HDDSTATUS_ALARM_E,    //
	INTF_FTPUPGRADE_ALARM_E,   //Ftp
}INTF_ALARM_TYPE_E;

//
//
typedef struct{
	S8 s8Reserve[100];
}INTF_HDD_ALARM_S;

//
typedef struct{
    U8 u8Channel;  //
    U8 u8RuleNo;   //
    U8 u8RuleEnable;//
    U8 u8RuleType;//01
    U8 u8Direction;//
    U16 u16aiPoints[16];//xy
    S8 u8s8Reserve[63];
}INTF_AI_RULE_S;

//
typedef enum{
	INTF_TRIWIRE_ALARM = 1, //
	INTF_PERIMETER_ALARM = 2, //
}INTF_AIALARMTYPE_E;

typedef struct{
	INTF_AIALARMTYPE_E AIAlarmType;
	S32 s32Channel; //
	S32 s32AIRuleNo; //AI
	S32 s32AlarmStatus; // INTF_ALARM_START INTF_ALARM_STOP
	S8  s8Reserve[88];
}INTF_AI_ALARM_S;

typedef enum{
	INTF_EXIGENCE_ALARM = 0,		//
	INTF_SPEED_ALARM,				//
	INTF_TIRE_DRIVE_ALARM,			//
	INTF_BEFOREHAND_ALARM,			//
	INTF_GNSS_TROUBLE_ALARM,		//GNSS
	INTF_GNSS_CONNECT_ALARM,		//GNSS
	INTF_GNSS_SHORT_CIRCUIT_ALARM,	//GNSS 
	INTF_ELETRICITY_LACK_ALARM,		//
	INTF_NO_ELETRICITY_ALARM,		//
	INTF_DISPLAY_TROUBLE_ALARM,		//LCD
	INTF_TTS_TROUBLE_ALARM,			//TTS
	INTF_VIDICON_TROUBLE_ALARM,		//

	INTF_TOTAL_DRIVE_ALARM,			//
	INTF_OVERTIME_PARKING_ALARM,	//
	INTF_INANDOUT_AREA_ALARM,		//
	INTF_INANDOUT_CHANNEL_ALARM,	//
	INTF_DRIVE_TIME_ALARM,			//
	INTF_DEPARTURE_CHANNEL_ALARM,	//
	INTF_VSS_ALARM,					//VSS
	INTF_CAR_OIL_ALARM,				//
	INTF_CAR_STEAL_ALARM,			//
	INTF_CAR_IGNITION_ALARM,		//
	INTF_CAR_DISPLACEMENT_ALARM,	//
	INTF_CAR_ACCIDENT_ALARM,		//
	INTF_SCRAM_ALARM,               //
	
	INTF_GPS_ALARM,                 // Gps
}INTF_MDVRAlarmType_E;

/*EyeHome-Host,*/  

typedef enum{
	INTF_LOWPOWER_ALARM = 0,                //
	INTF_MODULE_UNNORMAL_ALARM,             //,
	INTF_SENSOR_LOWPOWER_ALARM,             //
	INTF_LINE_DISCONNECT_ALARM,             //
	INTF_RINGIN_ALARM,                      //
	INTF_DIAL_ALARM,                        //
	INTF_VOICE_RECORD_ALARM,                //
	INTF_LEARN_CODE_ALARM,                  //
	INTF_ALARM_LINK_ALARM,                  //
	INTF_SNESOR_ALARM_ALARM,                //
	INTF_WORKMODE_CHANGE_ALARM,             //
}INTF_TELMODULEAlarmType_E;

typedef struct{
	S32 s32AreaNum;
	S32 s32SensorNum;
	S32 s32Status;
	INTF_TELMODULEAlarmType_E TelModuleAlarmType;
	S8  s8Reserve[90];
}INTF_TELMODULE_ALARM_S;

typedef struct{
	INTF_MDVRAlarmType_E MDVRAlarmType;

	union
	{   //

		//INTF_SPEED_ALARM
		struct{
			FLOAT fSpeed; //
			FLOAT fOverSpeed; //
			S32 s32AlarmDuretion; //
			S8 s8Reserve[84];
		}SpeedAlarm; //

		//INTF_OVERTIME_PARKING_ALARM
		struct{
			U32 u32ParkTime; //''
			
			DOUBLE dLongitude; 			//  0-180
			DOUBLE dLatitude; 			//  0-180
			S8  s8LatitudeDirect;		//  'N' or 'S'
			S8  s8LongitudeDirect;   	//  'E' or 'W'
			S8  s8State;				// 'A',  'V'
		}OvertimeParkAlarm; //

		//INTF_SCRAM_ALARM
		struct{
			FLOAT fCurAcc; //  
			S8 reserve[92];
		}ScramAlarm;
		
		//INTF_TIRE_DRIVE_ALARM
		struct{
			S32 TireDriveTime; // 
			S8 reserve[92];
		}TiredriveAlarm;

		struct{
			U8 sens_num_fun[8][2];
			U8 sens_num_stat[8][2];
			S8 reserve[64];
		}IoSensorSetting;            //iosensor

		struct{
			PuInfoHead_t Head;
			GpsInfo_t Body;
			S8 reserve[16];
		}RSGpsInfoPacket_t;         //GPS
	};
}INTF_MDVR_ALARM_S;

typedef enum{
	INTF_SLOTCARD_SUCESS_ICALARM,		//
	INTF_SLOTCARD_FAILED_ICALARM,		//
	INTF_SLOTCARD_EXCEPTION_ICALARM,	//
	INTF_LOCK_ICALARM,					//
	INTF_UNLOCK_ICALARM,				//
}INTF_ICCARD_AlarmType_E;

typedef struct{
	INTF_ICCARD_AlarmType_E ICCardAlarmType;
	S8 s8ICID[32];
}INTF_ICCARD_ALARM_S;

typedef struct{
	UINT RecordAlarmType;  //
	UINT ChannelNum;     // 0xffffffff
	BYTE RecordStatus; //
	BYTE Resered[3];
    UINT  AllChanRecState;    //	
}INTF_RECORDSTATUS_ALARM_S;

typedef struct{
	BYTE HddStatus; //
	BYTE HddSN[32]; //
}INTF_HDDSTATUS_ALARM_S;

typedef struct {
	BYTE UpgradeProcess;  //  100 
	BYTE UpgradeStatus;    // 
	BYTE reserved[2];
}INTF_FTPUPGRADE_ALARM_S;

typedef struct{
	INTF_ALARM_TYPE_E AlarmType;
	DateTime AlarmTime;

	union
	{
		INTF_GENERAL_ALARM_S GeneralAlarm;
		INTF_HDD_ALARM_S HddAlarm;
		INTF_AI_RULE_S  AIRule;
		INTF_MDVR_ALARM_S MDVRAlarm;
		INTF_ICCARD_ALARM_S ICCardAlarm;
		INTF_AI_ALARM_S AIAlarm;
		INTF_TELMODULE_ALARM_S TelModuleAlarm;
		INTF_RECORDSTATUS_ALARM_S RecordStatusAlarm;
		INTF_HDDSTATUS_ALARM_S  HddStatusAlarm;
		INTF_FTPUPGRADE_ALARM_S  FtpUpgradeAlarm;
	};
}INTF_ALARM_INFO_S;

typedef struct tag_INTF_ICCARD_INFO_S{
	S8 s8ICID[32];
	tag_INTF_ICCARD_INFO_S *pNext;
}INTF_ICCARD_INFO_S;

//SnapIntoBuffer Return Value

typedef enum{
	DEVALLSTATUS    =0,
	REMOTEDEVSTATUS = 1,
}INTF_REQSUBMSG_E;

typedef struct{
	union{
		struct{
			DateTime AlarmTime;
			UINT RecordEnable;  //, 0
			UINT RecordStatus;  //, 0
			UINT MotionEnable;  //, 0
			UINT MotionStatus;  //, 0
			UINT IOEnable;      //IO, 0
			UINT IOStatus;      //IO, 0
			UINT VLossStatus;   //, 0
			BYTE HddStatus[8];  //RS_HDDSTATUS_E
			BYTE HddSN[8][32];  //
			//32,  RS_RECORD_STATUS_E 
			BYTE RecordAllStatus[32];
			BYTE FtpNewVersFlag;    //:1 ;0 
			BYTE reserve[7];
		}IntfDevAllStatus;
		
		struct{
			int StructSize;
			int ChnEnable;
			unsigned long long DevType;  /*  */
			char CurChnName[20];         /*  */
			char CurChnState;            /*  */
			char RecState;               /*  */
			char CurRecType;             /*  */
			char MDState;                /* Motion */
			char IOState;                /* IO */
			char VLossState;             /*  */
			char HddState;               /*  */
			char MDSchedule;             /* */
			char AlarmSchedule;          /* */
			unsigned char ChannelNum;
			char Resered[2];
		}IntfRemoteChnStatus[16];
	};
}INTF_REQDEVSTATUS_S;

class IAlarmReport
{
	public:
		IAlarmReport(){}
		virtual ~IAlarmReport(){}
	public:
		/** 
		 * @remarks    ID
		 * @return     0, -1.
		 */
		virtual S32 RequestAlarmListReaderID(U32 *pu32ReturnReaderID)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    ID
		 * @return     0, -1.
		 */
		virtual S32 ReleaseAlarmListReaderID(U32 u32ReaderID)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    
		 * @    	   ,
		 * @return     0, -1.
		 */
		virtual S32 GetAlarmInfo(U32 u32ReaderID, INTF_ALARM_INFO_S *pReturnAlarmInfo)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks     AI	
		 * @return     	0, -1.
		 */
		virtual S32 WriteAiRules( U32 u32ChnMask, BOOL bFirstLogin )
		{
			return 0;
			
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks     AI
		 * @return     	0, -1.
		 */
		virtual S32 BoardSaveAiPara( BOOL bSaveAi )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    	
		 * @    	   	DestroySnap
		 * @    	   	s32Chn: 	
		 * @ 			ps8FileName:,, example: "2011_07_23_11_47_00","/2011_07_23_11_47_00.jpg"
		 * @return     	, -1.
		 */
		virtual LONG Snap(S32 s32Chn, S8 *ps8FileName)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    	buffer
		 * @    	   	Snap
		 * @ 			ps8FileName:
		 * @ 			ps8Buffer:
		 * @ 			lBufferSize:ps8Buffer
		 * @return     	0, -1.
		 */
		virtual S32 ReadSnapFile(S8 *ps8FileName, S8 *ps8Buffer, LONG lBufferSize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    	
		 * @    	   	Snap
		 * @ 			ps8FileName:Snap,
		 * @return     	0, -1.
		 */
		virtual S32 DestroySnap(S8 *ps8FileName)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    	
		 * @    	   	
		 * @ 			s32Chn:
		 * @ 			ps8FileName:128
		 * @ 			ps8Buffer:
		 * @ 			lBufferSize:
		 * @ 			plFileSize:
		 * @return     	:0 INTF_SNAP_RETURN_E 
		 */
		typedef enum{
			SNAP_SUCCESS				= 0,
			SNAP_PARAM_ERROR 			= -1,
			SNAP_FILECONTORL_ERROR		= -2,
			SNAP_FAIL					= -3,
			SNAP_BUFFER_NOT_ENOUGH 		= -4,
		}INTF_SNAP_RETURN_E;

		virtual S32 SnapIntoBuffer(S32 s32Chn, S8 *ps8FileName, S8 *ps8Buffer, LONG lBufferSize, LONG *plFileSize)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		virtual S32 UpdataICCardList(INTF_ICCARD_INFO_S *pICCardList)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		
		/** 
		 * @remarks    	 ICIC
		 * @return     	 true false.
		 */
		virtual S32 GetICCardListEnable()
		{
			return false;
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		
		/** 
		 * @remarks    	 
		 * @return     	 0 .
		 */
		virtual S32 RequestDevStatus(INTF_REQSUBMSG_E ReqSubMsgType ,INTF_REQDEVSTATUS_S *ReqDevStatusInfo)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
};

#endif
