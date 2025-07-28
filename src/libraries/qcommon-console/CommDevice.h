#ifndef COMMDEVICE_H
#define COMMDEVICE_H
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
// Copyright 2013-2018 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// Qt
#include <QFile>
#include <QObject>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>

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
	}

	virtual bool Send(const QByteArray& sendData);
	quint64 Read(QByteArray& incomingBytes);

public slots:
	bool IsOpen();
	
	void on_ReadyRead();

	void on_parityChanged(QSerialPort::Parity parity);
    void on_baudRateChanged(qint32 baudRate, QSerialPort::Directions directions);
    void on_dataBitsChanged(QSerialPort::DataBits dataBits);
	void on_stopBitsChanged(QSerialPort::StopBits stopBits);
	void on_error(QSerialPort::SerialPortError serialPortError);
	void on_dataTerminalReadyChanged(bool set);
	void on_flowControlChanged(QSerialPort::FlowControl flow);
	void on_requestToSendChanged(bool set);

protected:
	QSerialPortInfo				_serialPortInfo;
	int							_comPort;
	QSerialPort*				_serialPort;
	bool						_frameCoderOwned;
	
	bool						_logSerialData;
	QString						_logSerialPath;
	QFile						_logFile;
};

#endif // COMMDEVICE_H
