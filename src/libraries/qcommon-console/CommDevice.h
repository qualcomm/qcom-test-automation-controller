#ifndef COMMDEVICE_H
#define COMMDEVICE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
