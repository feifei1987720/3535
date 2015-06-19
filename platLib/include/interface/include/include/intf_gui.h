/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Zhong caiwang <caiwang213@gmail.com>
 * Apr 2011-06-04
 *
 * Paramenter config relevant interface defines.
 */

#ifndef __INTF_GUI_H__
#define __INTF_GUI_H__

class IGui
{
	public:
		IGui() {}
		virtual ~IGui() {}
	
	public:
		/** 
		 * @remarks    OSD 
		 * @return     0, -1.
		 */
		virtual S32 UpdateOsd()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    OSD 
		 * @return     0, -1.
		 */
		virtual S32 DrawIntRules()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    
		 * @return     0, -1.
		 */
		virtual S32 AiVideoUnbinding()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks    gui
		 * @return     0, -1.
		 */
		virtual S32 WaitForGuiStart()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
		
		/** 
		 * @remarks     
		 * @return     0, -1.
		 */
		virtual S32 UpdateDisplayMode()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
};

#endif
