#ifndef SERIALPORT_H
#define SERIALPORT_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ISerialPort.h"

/**
 * @brief Concrete cross-platform SerialPort class implementing ISerialPort.
 * Forwards to the native platform implementation (SerialPortWin32 on Windows,
 * SerialPortLinux on Linux).
 * Completely free of Qt serial port libraries.
 */
class QCOMMONCONSOLE_EXPORT SerialPort : public ISerialPort
{
public:
	SerialPort();
	explicit SerialPort(const std::string& portName);
	~SerialPort() override = default;

	bool open() override;
	bool open(const std::string& portName) override;
	bool open(int mode) { (void)mode; return open(); }
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
	void clear(int mode = 0) { (void)mode; clearBuffers(); }
	bool waitForReadyRead(int timeoutMs) override;

	SerialPortError error() const override;
	std::string errorString() const override;

	ISerialPort* impl() const { return _impl.get(); }

private:
	std::shared_ptr<ISerialPort> _impl;
};

#endif // SERIALPORT_H
