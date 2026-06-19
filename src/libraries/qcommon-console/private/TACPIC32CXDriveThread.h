// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef TACPIC32CXDRIVETHREAD_H
#define TACPIC32CXDRIVETHREAD_H


#include "QCommonConsoleGlobal.h"

#include "TACDriveThread.h"
#include "TACPIC32CXProtocol.h"

#include "SerialPort.h"
#include "SerialPortInfo.h"

// Qt
#include <QFile>

class QCOMMONCONSOLE_EXPORT TACPIC32CXDriveThread :
	public TACDriveThread
{
	Q_OBJECT

public:
	TACPIC32CXDriveThread(uint hash);
	~TACPIC32CXDriveThread();

	QString locked()
	{
		return QString();
	}

	void run();

	void sendCommand(const QByteArray& command, bool console = false,
							 ReceiveInterface* receiveInterface = Q_NULLPTR, bool shouldStore = true);

	void setPinState(quint16 pin, bool state);
	void sendCommandSequence(CommandEntries& commandEntries);

	int getResetCount();
	void clearResetCount();

	// properties
	void setName(const QByteArray& newName);

	// Send Interface
	quint32 send(const QByteArray& sendMe, const Arguments& arguments, bool console, ReceiveInterface* recieveInterface, bool store = true);
	bool ready();

	// Receive Interface
	void receive(FramePackage& framePackage);

	void i2CReadRegister(quint32 addr, quint32 reg);
	void i2CWriteRegister(quint32 addr, quint32 reg, quint32 data);

	void endTransaction(ReceiveInterface *receiveInterface);
protected:
	bool openSerialDevice();

protected slots:
	void handleError(QSerialPort::SerialPortError error);
	void on_readyRead();

private:

	QString serialPortError();
	bool readSerialData();

	static bool						_initialized;
	bool							_connected{false};
	TACPIC32CXProtocol				_tacProtocol;
	SerialPortInfo					_tacPortInfo;
	SerialPort*						_serialPort{Q_NULLPTR};
	bool							_readyRead{false};

	QByteArray						_serialBuffer;

	void handleSetPin(FramePackage& framePackage);
	void handleVersionResponse(FramePackage& framePackage);
	void handleClearBuffer(FramePackage& framePackage);

	void setupConnected();
	void setupDiscovery();

	void log(FramePackage& framePackage);
};

#endif // TACPIC32CXDRIVETHREAD_H
