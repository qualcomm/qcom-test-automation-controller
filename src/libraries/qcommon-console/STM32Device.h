#ifndef STM32DEVICE_H
#define STM32DEVICE_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"
#include "AlpacaDevice.h"

#include <string>
#include <vector>

// QCommonConsole
class _STM32PlatformConfiguration;

struct STM32HIDDeviceInfo
{
	HashType		_hash{0};
	std::string		_devicePath;
	std::string		_serialNumber;
};

typedef std::vector<STM32HIDDeviceInfo> STM32HIDDeviceInfoList;

class QCOMMONCONSOLE_EXPORT STM32Device :
	public _AlpacaDevice
{
public:
	STM32Device() = default;
	~STM32Device();

	static bool programDevice(AlpacaDevice alpacaDevice, PlatformID platformID, std::string& errorMessage);

	static uint32_t updateAlpacaDevices();

	static STM32HIDDeviceInfoList enumerateHIDDevices();
	static std::string vtpPortName(const std::string& serialNumber);

	virtual bool open();

	void buildCommandList();
	virtual void buildMapping();

	// HID transport - the only place in the codebase permitted to call hidapi functions
	bool openTransport();
	bool write(const std::string& report);
	void close();

private:
	_STM32PlatformConfiguration*					_stm32PlatformConfiguration{nullptr};

	std::string										_devicePath;
	void*											_hidHandle{nullptr};
};

#endif // STM32DEVICE_H
