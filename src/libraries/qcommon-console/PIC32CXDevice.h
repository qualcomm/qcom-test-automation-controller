#ifndef PIC32CXDEVICE_H
#define PIC32CXDEVICE_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "AlpacaDevice.h"
#include "PIC32CXPlatformConfiguration.h"

class QCOMMONCONSOLE_EXPORT PIC32CXDevice :
	public _AlpacaDevice
{
public:
	PIC32CXDevice() = default;
	virtual ~PIC32CXDevice();

	static bool programDevice(AlpacaDevice alpacaDevice, PlatformID platformID, QByteArray& errorMessage);

	static quint32 updateAlpacaDevices();

	virtual bool open();

	void buildCommandList();
	virtual void buildMapping();

private:
	_PIC32CXPlatformConfiguration* _pic32cxPlatformConfiguration{Q_NULLPTR};
};

#endif // PIC32CXDEVICE_H
