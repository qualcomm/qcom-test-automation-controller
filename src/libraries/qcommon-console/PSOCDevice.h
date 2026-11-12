// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef PSOCDEVICE_H
#define PSOCDEVICE_H

#include "QCommonConsoleGlobal.h"

#include "AlpacaDevice.h"

// QCommon
class _PSOCPlatformConfiguration;

class QCOMMONCONSOLE_EXPORT PSOCDevice :
	public _AlpacaDevice
{
public:
	PSOCDevice() = default;
	virtual ~PSOCDevice();

	static uint32_t updateAlpacaDevices();

	virtual bool open() override;

	virtual void buildMapping() override;

private:
	_PSOCPlatformConfiguration*	_psocPlatformConfiguration{nullptr};
};

#endif // TACDEVICE_H
