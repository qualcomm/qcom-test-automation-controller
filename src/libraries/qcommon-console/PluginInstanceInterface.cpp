// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: msimpson
*/

#include "PluginInstanceInterface.h"


PluginInstanceInterface::PluginInstanceInterface()
{
}

PluginInstanceInterface::~PluginInstanceInterface()
{	
	// We don't own these
	_sendInterface = Q_NULLPTR;
	_dataSink = Q_NULLPTR;
}
	
void PluginInstanceInterface::SetSendInterface
(
	SendInterface* sendInterface
)
{
	_sendInterface = sendInterface;
}

void PluginInstanceInterface::SetDataSink
(
	DataSinkInterface* dataSink
)
{
	_dataSink = dataSink;
}
