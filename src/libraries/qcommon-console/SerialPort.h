#ifndef SERIALPORT_H
#define SERIALPORT_H
// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2013-2024 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
