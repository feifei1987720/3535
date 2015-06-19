/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Zhong caiwang <caiwang213@gmail.com>
 * Apr 2011-04-16
 *
 * Ptz control relevant interface defines.
 */

#ifndef __INTF_PTZCONTROL_H__
#define __INTF_PTZCONTROL_H__

#define PTZ_CTRL_START   0 
#define PTZ_CTRL_STOP    1 
typedef enum 
{
	PTZ_CMD_UP = 1,
	PTZ_CMD_DOWN,
	PTZ_CMD_LEFT,
	PTZ_CMD_RIGHT,
	PTZ_CMD_UPLEFT,
	PTZ_CMD_UPRIGHT,
	PTZ_CMD_DOWNLEFT,
	PTZ_CMD_DOWNRIGHT,
	PTZ_CMD_ZOOMIN,	
	PTZ_CMD_ZOOMOUT,
	PTZ_CMD_FOCUSNEAR,
	PTZ_CMD_FOCUSFAR,
	PTZ_CMD_IRISOPEN,
	PTZ_CMD_IRISCLOSE,	
	PTZ_CMD_AUTOSCAN,		
	PTZ_CMD_CRUISE,

	PTZ_CMD_SETPRESET,	//17
	PTZ_CMD_CLEARPRESET,
	PTZ_CMD_CALLPRESET,
	
	PTZ_CMD_AUXILIART_OPEN,	
	PTZ_CMD_AUXILIART_CLOSE,
}PTZ_CTRL_CMD_E;

class IPTZControl
{
	public:
		IPTZControl() { }
		virtual ~IPTZControl() { }

	public: 
		/** 
		 * @remarks     
		 * @return    0, -1.
		 */
	    virtual S32 PtzCtrl( S32 s32Cmd, S32 s32ChnNO, S32 s32Param1, S32 s32Param2 )
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks     
		 * @return    true, false.
		 */
	    virtual BOOL IsGuiPtzAutoScan()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}

		/** 
		 * @remarks   
		 * @return    0, -1.
		 */
	    virtual S32 PtzBindProtocol()
		{
			SHOW_NOT_IMPLE_FUNC;
			throw Exception(__FUNCTION__);
		}
};

#endif
