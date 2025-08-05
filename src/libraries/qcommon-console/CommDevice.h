#ifndef COMMDEVICE_H
#define COMMDEVICE_H
/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
