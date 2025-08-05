#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
