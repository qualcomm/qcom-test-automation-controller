#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
