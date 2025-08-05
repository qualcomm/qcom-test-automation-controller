#ifndef SERIALPORT_H
#define SERIALPORT_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

// Qt
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

struct QCOMMONCONSOLE_EXPORT SerialPortSettings
{
	SerialPortSettings()
	{
		_baudRate = QSerialPort::Baud115200;
		_timeout = 0;
		_dataBits = QSerialPort::Data8;
		_parity = QSerialPort::NoParity;
		_stopBits = QSerialPort::OneStop;
		_flowControl = QSerialPort::NoFlowControl;
	}

	SerialPortSettings(const SerialPortSettings& copyMe) = default;

	qint32						_baudRate;
	quint32                     _timeout;
	QSerialPort::DataBits		_dataBits;
	QSerialPort::Parity			_parity;
	QSerialPort::StopBits		_stopBits;
	QSerialPort::FlowControl	_flowControl;
};


class QCOMMONCONSOLE_EXPORT SerialPort :
	public QSerialPort
{
	Q_OBJECT

public:
	SerialPort(const QSerialPortInfo& serialPortInfo);

	SerialPortSettings getSerialPortSettings()
	{
		return _serialPortSettings;
	}

	void setSerialPortSettings(const SerialPortSettings& setMe)
	{
		_serialPortSettings = setMe;
	}

	virtual bool open(OpenMode mode);
	virtual void close();

	QByteArray readAll();

signals:
	void PortOpened(const QSerialPortInfo& serialPortInfo);
	void PortClosed(const QSerialPortInfo& serialPortInfo);

protected:
	QSerialPortInfo				_serialPortInfo;
	SerialPortSettings			_serialPortSettings;
};

#endif // SERIALPORT_H
