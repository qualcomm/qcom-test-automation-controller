// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "SerialPort.h"

SerialPort::SerialPort() :
	_impl(ISerialPort::create())
{
}

SerialPort::SerialPort(const std::string& portName) :
	_impl(ISerialPort::create(portName))
{
}

bool SerialPort::open()
{
	return _impl ? _impl->open() : false;
}

bool SerialPort::open(const std::string& portName)
{
	return _impl ? _impl->open(portName) : false;
}

void SerialPort::close()
{
	if (_impl)
		_impl->close();
}

bool SerialPort::isOpen() const
{
	return _impl ? _impl->isOpen() : false;
}

std::string SerialPort::portName() const
{
	return _impl ? _impl->portName() : std::string();
}

void SerialPort::setPortName(const std::string& name)
{
	if (_impl)
		_impl->setPortName(name);
}

SerialPortSettings SerialPort::getSerialPortSettings() const
{
	return _impl ? _impl->getSerialPortSettings() : SerialPortSettings();
}

bool SerialPort::setSerialPortSettings(const SerialPortSettings& settings)
{
	return _impl ? _impl->setSerialPortSettings(settings) : false;
}

bool SerialPort::setBaudRate(int32_t baudRate)
{
	return _impl ? _impl->setBaudRate(baudRate) : false;
}

int32_t SerialPort::baudRate() const
{
	return _impl ? _impl->baudRate() : 0;
}

bool SerialPort::setDataBits(SerialDataBits dataBits)
{
	return _impl ? _impl->setDataBits(dataBits) : false;
}

SerialDataBits SerialPort::dataBits() const
{
	return _impl ? _impl->dataBits() : SerialDataBits::Data8;
}

bool SerialPort::setParity(SerialParity parity)
{
	return _impl ? _impl->setParity(parity) : false;
}

SerialParity SerialPort::parity() const
{
	return _impl ? _impl->parity() : SerialParity::NoParity;
}

bool SerialPort::setStopBits(SerialStopBits stopBits)
{
	return _impl ? _impl->setStopBits(stopBits) : false;
}

SerialStopBits SerialPort::stopBits() const
{
	return _impl ? _impl->stopBits() : SerialStopBits::OneStop;
}

bool SerialPort::setFlowControl(SerialFlowControl flowControl)
{
	return _impl ? _impl->setFlowControl(flowControl) : false;
}

SerialFlowControl SerialPort::flowControl() const
{
	return _impl ? _impl->flowControl() : SerialFlowControl::NoFlowControl;
}

int64_t SerialPort::write(const char* data, size_t size)
{
	return _impl ? _impl->write(data, size) : -1;
}

int64_t SerialPort::write(const std::string& data)
{
	return _impl ? _impl->write(data) : -1;
}

int64_t SerialPort::read(char* buffer, size_t maxSize)
{
	return _impl ? _impl->read(buffer, maxSize) : -1;
}

std::string SerialPort::read(size_t maxSize)
{
	return _impl ? _impl->read(maxSize) : std::string();
}

std::string SerialPort::readAll()
{
	return _impl ? _impl->readAll() : std::string();
}

int64_t SerialPort::bytesAvailable() const
{
	return _impl ? _impl->bytesAvailable() : 0;
}

bool SerialPort::flush()
{
	return _impl ? _impl->flush() : false;
}

void SerialPort::clearBuffers()
{
	if (_impl)
		_impl->clearBuffers();
}

bool SerialPort::waitForReadyRead(int timeoutMs)
{
	return _impl ? _impl->waitForReadyRead(timeoutMs) : false;
}

SerialPortError SerialPort::error() const
{
	return _impl ? _impl->error() : SerialPortError::NotOpenError;
}

std::string SerialPort::errorString() const
{
	return _impl ? _impl->errorString() : "Serial port not initialized";
}
