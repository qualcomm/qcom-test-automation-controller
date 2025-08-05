// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "SerialPort.h"

SerialPort::SerialPort
(
	const QSerialPortInfo& serialPortInfo
)
{
	setPort(serialPortInfo);
	_serialPortInfo = serialPortInfo;
}

bool SerialPort::open(OpenMode mode)
{
	bool result(false);

	setBaudRate(_serialPortSettings._baudRate, QSerialPort::AllDirections);
	setDataBits(_serialPortSettings._dataBits);
	setParity(_serialPortSettings._parity);
	setStopBits(_serialPortSettings._stopBits);
	setFlowControl(_serialPortSettings._flowControl);

	result = QSerialPort::open(mode);
	if (result)
		emit PortOpened(_serialPortInfo);

	return result;
}

void SerialPort::close()
{
	QSerialPort::close();
	emit PortClosed(_serialPortInfo);
}

QByteArray SerialPort::readAll()
{
	waitForReadyRead(_serialPortSettings._timeout);

	return QSerialPort::readAll();
}
