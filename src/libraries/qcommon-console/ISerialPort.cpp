// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ISerialPort.h"
#include "SerialPortWin32.h"
#include "SerialPortLinux.h"

std::shared_ptr<ISerialPort> ISerialPort::create()
{
#if defined(_WIN32) || defined(Q_OS_WIN)
	return std::make_shared<SerialPortWin32>();
#else
	return std::make_shared<SerialPortLinux>();
#endif
}

std::shared_ptr<ISerialPort> ISerialPort::create(const std::string& portName)
{
	auto port = create();
	port->setPortName(portName);
	return port;
}
