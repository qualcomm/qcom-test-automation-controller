#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

// Confidential and Proprietary – Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright © 2013-2016 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
   $Id: //depot/QDL/QCommon/PluginInterface.h#5 $
   $Header: //depot/QDL/QCommon/PluginInterface.h#5 $
   $Date: 2016/10/25 $
   $DateTime: 2016/10/25 14:38:30 $
   $Change: 925615 $
   $File: //depot/QDL/QCommon/PluginInterface.h $
   $Revision: #5 $
   %Author: msimpson $
*/

// QCommon
#include "PluginInstanceInterface.h"
#include "PropertyPageInterface.h"

// QT
#include <QtPlugin>

class SendInterface;

class PluginInterface
{
public:
	virtual QString Name(void) = 0;
	virtual bool Init(const QString& pluginPath, bool resetSettings) = 0;
	
	virtual PluginInstanceInterface* Create() = 0;
	virtual void Destroy(QObject* destroyMe) = 0;

	virtual PropertyPageInterface* GetPropertyPageInterface() = 0;
	virtual void ReleasePropertyPageInterface(PropertyPageInterface* ppI) = 0;
};

#define PluginInterface_iid "com.qualcomm.simpson.PluginInterface"

Q_DECLARE_INTERFACE(PluginInterface, PluginInterface_iid)

#endif // PLUGININTERFACE_H
