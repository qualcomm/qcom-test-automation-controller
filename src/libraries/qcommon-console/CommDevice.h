#ifndef COMMDEVICE_H
#define COMMDEVICE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ISerialPort.h"
#include "SerialPortInfo.h"

#include <cstdint>
#include <fstream>
#include <memory>
#include <string>

class CommDevice
{
public:
	CommDevice();
	~CommDevice();
	
	bool Open();
	void Close();

	void SetSerialPortInfo(const SerialPortInfo& serialPortInfo)
	{
		_serialPortInfo = serialPortInfo;
		_portName = _serialPortInfo.portName();
		std::string name = _portName;
		if (name.rfind("COM", 0) == 0 || name.rfind("com", 0) == 0)
		{
			try {
				_comPort = std::stoi(name.substr(3));
			} catch (...) {
				_comPort = 0;
			}
		}
	}

	void SetPortName(const std::string& portName)
	{
		_portName = portName;
	}

	void SetSerialPort(SerialPortPtr serialPort)
	{
		_serialPort = serialPort;
	}

	SerialPortPtr GetSerialPort() const
	{
		return _serialPort;
	}

	virtual bool Send(const std::string& sendData);
	uint64_t Read(std::string& incomingBytes);

	bool IsOpen();

protected:
	SerialPortInfo				_serialPortInfo;
	int							_comPort{0};
	std::string					_portName;
	SerialPortPtr				_serialPort{nullptr};
	
	bool						_logSerialData{false};
	std::string					_logSerialPath;
	std::ofstream				_logFile;
};

#endif // COMMDEVICE_H
