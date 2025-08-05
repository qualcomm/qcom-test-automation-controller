#ifndef FTDIDEVICE_H
#define FTDIDEVICE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

// libTAC
#include "AlpacaDevice.h"

// QCommon
class _FTDIPlatformConfiguration;

class QCOMMONCONSOLE_EXPORT FTDIDevice :
	public _AlpacaDevice
{
public:
	FTDIDevice() = default;
	virtual ~FTDIDevice();

	static bool programDevice(AlpacaDevice alpacaDevice, PlatformID platformID, QByteArray& errorMessage);

	static quint32 updateAlpacaDevices();

	virtual bool open();

	void buildCommandList();
	virtual void buildMapping();

private:
	_FTDIPlatformConfiguration* _ftdiPlatformConfiguration{Q_NULLPTR};
};

#endif // FTDIDEVICE_H
