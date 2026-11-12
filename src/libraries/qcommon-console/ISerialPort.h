#ifndef ISERIALPORT_H
#define ISERIALPORT_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"
#include "SerialPortSettings.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

/**
 * @brief Unified standard C++ interface for serial port communication.
 * Provides a platform-independent API implemented natively on Windows (Win32 COM API)
 * and Linux (POSIX termios) without using or mimicking Qt libraries.
 */
class QCOMMONCONSOLE_EXPORT ISerialPort
{
public:
	virtual ~ISerialPort() = default;

	// Port lifecycle
	virtual bool open() = 0;
	virtual bool open(const std::string& portName) = 0;
	virtual void close() = 0;
	virtual bool isOpen() const = 0;

	// Port configuration
	virtual std::string portName() const = 0;
	virtual void setPortName(const std::string& name) = 0;

	virtual SerialPortSettings getSerialPortSettings() const = 0;
	virtual bool setSerialPortSettings(const SerialPortSettings& settings) = 0;

	virtual bool setBaudRate(int32_t baudRate) = 0;
	virtual int32_t baudRate() const = 0;

	virtual bool setDataBits(SerialDataBits dataBits) = 0;
	virtual SerialDataBits dataBits() const = 0;

	virtual bool setParity(SerialParity parity) = 0;
	virtual SerialParity parity() const = 0;

	virtual bool setStopBits(SerialStopBits stopBits) = 0;
	virtual SerialStopBits stopBits() const = 0;

	virtual bool setFlowControl(SerialFlowControl flowControl) = 0;
	virtual SerialFlowControl flowControl() const = 0;

	// Data I/O
	virtual int64_t write(const char* data, size_t size) = 0;
	virtual int64_t write(const std::string& data) = 0;
	virtual int64_t read(char* buffer, size_t maxSize) = 0;
	virtual std::string read(size_t maxSize) = 0;
	virtual std::string readAll() = 0;
	virtual int64_t bytesAvailable() const = 0;
	virtual bool flush() = 0;
	virtual void clearBuffers() = 0;
	virtual bool waitForReadyRead(int timeoutMs) = 0;

	// Error handling
	virtual SerialPortError error() const = 0;
	virtual std::string errorString() const = 0;

	// Factory methods: creates the platform-specific native implementation
	static std::shared_ptr<ISerialPort> create();
	static std::shared_ptr<ISerialPort> create(const std::string& portName);
};

using SerialPortPtr = std::shared_ptr<ISerialPort>;

#endif // ISERIALPORT_H
