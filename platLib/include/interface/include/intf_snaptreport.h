#ifndef __SNAPTREPORT_H__
#define __SNAPTREPORT_H__

/* xrh 2011-11-30 */

#define	SNPR_HEAD  0X1    //
#define SNPR_DATA  0X2    //
#define SNPR_ERR   0x3    //

enum SNPR_E{
	SNPR_MOTION = 0   //motion
};
struct SNPR_INFO {
	BYTE      type;          //(SNPR_HEAD|DATA|ERR)
	union {
		struct {
			SNPR_E   snprType;  //
			DateTime dateTime;  //
			UINT     picSize;   //
			WORD     ch;        // 0 ~ n
			WORD     reserved1;
			WORD     reserved2;
		}Head;
		struct {
			VOID  *  datas;    //
			UINT     dataLen;  //
		}Data;
		struct {
			S32      err;      // 0  -1 disable -2
		}Error;
	};
};

typedef S32 (*pfnSNPRCallback)(SNPR_INFO *snprInfo, VOID *userData); //
struct SNPRegisterInfo {
	pfnSNPRCallback cb_snpr;
	VOID         *  userData; //
};
/*:
 * A: SNPRegister.(SNPRUnRegister )
 *    (cb_snpr)
 *
 * B: SNPREnable 
 *
 * C: SNPREnable / 
 *    uKeyEnable,;disable
 *
 * D: Enable
 *    , HEAD + n*Data + EOF/Err ,
 *    nEOF( 
 *    DisableuKeySNPREnableFALSE,
 *    SNPR_INFO 
 */

class ISNPReport
{
	public:
		ISNPReport(){}
		virtual ~ISNPReport(){}
	public:
		
		/** 
		 * @remarks        
		 * @uKey         [out]  
		 * @registerData [in]   ,0 (SNPRRegisterInfo)
		 * @return       0 , -1
		 */

		virtual S32 SNPRegister(UINT &uKey,VOID *registerData)
		{
			return -1;

			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);

		}
		
		/** 
		 * @remarks        
		 * @uKey         [in]  
		 * @return       0 , -1
		 */

		virtual S32 SNPRUnRegister(UINT uKey)
		{
			return -1;

			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks      
		 * @uKey         [in]  
		 * @bEnable      [in]  TRUE- FALSE-
		 * @return       0 , -1
		 */

		virtual S32 SNPREnable(UINT uKey,BOOL bEnable)
		{
			return -1;
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
};


#endif
