#ifndef STM32DEVICE_H
#define STM32DEVICE_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"
#include "AlpacaDevice.h"

// QCommonConsole
class _STM32PlatformConfiguration;

struct STM32HIDDeviceInfo
{
	HashType		_hash{0};
	QByteArray		_devicePath;
	QByteArray		_serialNumber;
};

typedef QList<STM32HIDDeviceInfo> STM32HIDDeviceInfoList;

class QCOMMONCONSOLE_EXPORT STM32Device :
	public _AlpacaDevice
{
public:
	STM32Device() = default;
	~STM32Device();

	static bool programDevice(AlpacaDevice alpacaDevice, PlatformID platformID, QByteArray& errorMessage);

	static quint32 updateAlpacaDevices();

	static STM32HIDDeviceInfoList enumerateHIDDevices();
	static QByteArray vtpPortName(const QByteArray& serialNumber);

	virtual bool open();

	void buildCommandList();
	virtual void buildMapping();

	// HID transport - the only place in the codebase permitted to call hidapi functions
	bool openTransport();
	bool write(const QByteArray& report);
	void close();

private:
	_STM32PlatformConfiguration*					_stm32PlatformConfiguration{Q_NULLPTR};

	QByteArray										_devicePath;
	void*											_hidHandle{Q_NULLPTR};
	quint8											_invertMask{0};

	quint8 buildInvertMask() const;
};

#endif // STM32DEVICE_H
