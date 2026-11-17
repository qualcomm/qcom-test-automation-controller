#ifndef SERIALPORTLINUX_H
#define SERIALPORTLINUX_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ISerialPort.h"

/**
 * @brief Native Linux/POSIX serial port implementation using termios.
 * Uses open, tcgetattr, tcsetattr, read, write, ioctl(FIONREAD), tcflush, poll.
 * Completely free of Qt libraries.
 */
class QCOMMONCONSOLE_EXPORT SerialPortLinux : public ISerialPort
{
public:
	SerialPortLinux();
	explicit SerialPortLinux(const std::string& portName);
	~SerialPortLinux() override;

	bool open() override;
	bool open(const std::string& portName) override;
	void close() override;
	bool isOpen() const override;

	std::string portName() const override;
	void setPortName(const std::string& name) override;

	SerialPortSettings getSerialPortSettings() const override;
	bool setSerialPortSettings(const SerialPortSettings& settings) override;

	bool setBaudRate(int32_t baudRate) override;
	int32_t baudRate() const override;

	bool setDataBits(SerialDataBits dataBits) override;
	SerialDataBits dataBits() const override;

	bool setParity(SerialParity parity) override;
	SerialParity parity() const override;

	bool setStopBits(SerialStopBits stopBits) override;
	SerialStopBits stopBits() const override;

	bool setFlowControl(SerialFlowControl flowControl) override;
	SerialFlowControl flowControl() const override;

	int64_t write(const char* data, size_t size) override;
	int64_t write(const std::string& data) override;
	int64_t read(char* buffer, size_t maxSize) override;
	std::string read(size_t maxSize) override;
	std::string readAll() override;
	int64_t bytesAvailable() const override;
	bool flush() override;
	void clearBuffers() override;
	bool waitForReadyRead(int timeoutMs) override;

	SerialPortError error() const override;
	std::string errorString() const override;

private:
	bool configureTermios();

	std::string        _portName;
	SerialPortSettings _settings;
	SerialPortError    _lastError{SerialPortError::NoError};
	std::string        _lastErrorString;
	int                _fd{-1};
};

#endif // SERIALPORTLINUX_H
