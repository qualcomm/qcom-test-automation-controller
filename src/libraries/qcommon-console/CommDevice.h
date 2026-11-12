#ifndef COMMDEVICE_H
#define COMMDEVICE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ISerialPort.h"

#include <cstdint>
#include <fstream>
#include <memory>
#include <string>

// Qt
#include <QObject>
#include <QtSerialPort/QSerialPortInfo>

class CommDevice :
	public QObject
{
	Q_OBJECT

public:
	CommDevice();
	~CommDevice();
	
	bool Open();
	void Close();

	void SetSerialPortInfo(const QSerialPortInfo& serialPortInfo)
	{
		_serialPortInfo = serialPortInfo;
		_comPort = _serialPortInfo.portName().remove("com", Qt::CaseInsensitive).toInt();
		_portName = _serialPortInfo.portName().toStdString();
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

public slots:
	bool IsOpen();
	void on_ReadyRead();

protected:
	QSerialPortInfo				_serialPortInfo;
	int							_comPort{0};
	std::string					_portName;
	SerialPortPtr				_serialPort{nullptr};
	bool						_frameCoderOwned{false};
	
	bool						_logSerialData{false};
	std::string					_logSerialPath;
	std::ofstream				_logFile;
};

#endif // COMMDEVICE_H
