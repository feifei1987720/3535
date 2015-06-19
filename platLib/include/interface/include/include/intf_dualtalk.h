#ifndef __DUALTALK_H__
#define __DUALTALK_H__

#include "RSFrame.h"
#include "exception.h"

class IDualTalk
{
	public:
		IDualTalk(){}
		virtual ~IDualTalk(){}
	public:	
 	     /**
		 *@remark  

		 *@return  0-1
		 
		*/                                                         
		virtual S32 OpenDualTalkDec(S32 s32DualTalkChn)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/**
		 *@remark   

		 *@return  0-1
		 
		*/
		virtual S32 CloseDualTalkDec(S32 s32DualTalkChn)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/**
		 *@remark  , 
		 *@bEnalbe [in] trueflase,
		 *@return  0-1
		 
		*/
		virtual S32 EnableStorage(BOOL bEnable)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/**
		 *@remark  :RSFrameHead
		                                

		 *@pRSFrame [in] ()
		 *@return  0-1
		 
		*/
		virtual S32 StoreFrame(RSFrameHead* pRSFrame)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/**
		 *@remark  :RSFrameHead

		 *@pRSFrame [in/out] ()
		 *@return  0 - 
		          -1  
		          1   
		 
		*/
		virtual S32 ConsumeFrame(RSFrameHead* pRSFrame)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		/**
		 *@remark  

		 *@pRSFrame       [in] ()
		 *@s32DualTalkChn [in] 
		 *@return 0-1
		*/
		virtual S32 Play(RSFrameHead* pRSFrame, S32 s32DualTalkChn)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/*virtual S32 EnableDualTalk(BOOL bEnable)
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}*/

		/**
		 *@remark  

		 *@return 0-1
		*/
		virtual S32 DisplayDualTalking()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/**
		 *@remark  

		 *@return 0-1
		*/		
		virtual S32 RemoteCloseDualTalk()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/**
		 *@remark  

		 *@return 10
		*/	
		virtual S32 CanDualtalkRelease()
		{
			return 1;  //
			
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);

		}
		
		/**
		 *@remark 

		 *@return 10
		*/	
		virtual S32 GetDualtalkChn()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
};



#endif
