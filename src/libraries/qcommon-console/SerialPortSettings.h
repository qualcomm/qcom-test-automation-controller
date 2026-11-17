#ifndef SERIALPORTSETTINGS_H
#define SERIALPORTSETTINGS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include <cstdint>
#include <string>

enum class SerialBaudRate : int32_t {
	Baud1200   = 1200,
	Baud2400   = 2400,
	Baud4800   = 4800,
	Baud9600   = 9600,
	Baud19200  = 19200,
	Baud38400  = 38400,
	Baud57600  = 57600,
	Baud115200 = 115200,
	Baud230400 = 230400,
	Baud460800 = 460800,
	Baud921600 = 921600,
	Unknown    = -1
};

enum class SerialDataBits {
	Data5 = 5,
	Data6 = 6,
	Data7 = 7,
	Data8 = 8,
	Unknown = -1
};

enum class SerialParity {
	NoParity = 0,
	EvenParity = 1,
	OddParity = 2,
	SpaceParity = 3,
	MarkParity = 4,
	Unknown = -1
};

enum class SerialStopBits {
	OneStop = 1,
	OneAndHalfStop = 2,
	TwoStop = 3,
	Unknown = -1
};

enum class SerialFlowControl {
	NoFlowControl = 0,
	HardwareControl = 1,
	SoftwareControl = 2,
	Unknown = -1
};

enum class SerialPortError {
	NoError = 0,
	DeviceNotFoundError,
	PermissionError,
	OpenError,
	WriteError,
	ReadError,
	ResourceError,
	UnsupportedOperationError,
	TimeoutError,
	NotOpenError,
	UnknownError
};

struct QCOMMONCONSOLE_EXPORT SerialPortSettings
{
	SerialPortSettings() = default;
	SerialPortSettings(const SerialPortSettings&) = default;
	SerialPortSettings& operator=(const SerialPortSettings&) = default;

	int32_t            _baudRate{static_cast<int32_t>(SerialBaudRate::Baud115200)};
	uint32_t           _timeout{0};
	SerialDataBits     _dataBits{SerialDataBits::Data8};
	SerialParity       _parity{SerialParity::NoParity};
	SerialStopBits     _stopBits{SerialStopBits::OneStop};
	SerialFlowControl  _flowControl{SerialFlowControl::NoFlowControl};
};

#endif // SERIALPORTSETTINGS_H
