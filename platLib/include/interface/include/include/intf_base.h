
/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Zhong caiwang <caiwang213@gmail.com>
 * Apr 2011-04-16
 *
 * Interfaces defines.
 */

#ifndef __INTF_BASE_H__
#define __INTF_BASE_H__

#include "comm_def.h"
#include "intf_preview.h"
#include "intf_playback.h"
#include "intf_ptzcontrol.h"
#include "intf_paramconfig.h"
#include "intf_gui.h"
#include "intf_dualtalk.h"
#include "intf_upgrade.h"
#include "intf_alarmreport.h"
#include "intf_misc.h"
#include "intf_platform.h"
#include "intf_gps.h"
#include "intf_snaptreport.h"
#include "intf_tvwall.h"
#include "intf_mdvr.h"
class DevInterface : public IPreview,
	public IPlayback, 
	public IPTZControl, 
	public IParamConfig,
	public IGui,
	public IDualTalk,
	public IUpgrade,
	public IAlarmReport,
	public IMisc,
	public IPlatform,
	public IGps,
	public ISNPReport,
	public ITvWall,
	public IMdvr
{   
	public:                        
		DevInterface() {}      
		virtual ~DevInterface() {}
	public:                        
};
#endif
