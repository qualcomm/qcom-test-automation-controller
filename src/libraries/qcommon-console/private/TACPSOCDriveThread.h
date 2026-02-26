#ifndef TACPSOCDRIVETHREAD_H
#define TACPSOCDRIVETHREAD_H
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
// Copyright 2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"


#include "TACDriveThread.h"
#include "TACPSOCProtocol.h"

#include "SerialPort.h"
#include "SerialPortInfo.h"

// Qt
#include <QFile>

class QCOMMONCONSOLE_EXPORT TACPSOCDriveThread :
	public TACDriveThread
{
Q_OBJECT

public:
	TACPSOCDriveThread(uint hash);
	~TACPSOCDriveThread();

	virtual QString locked()
	{
		return QString();
	}

	virtual void run();

	virtual void sendCommand(const QByteArray& command, bool console = false,
		ReceiveInterface* receiveInterface = Q_NULLPTR, bool shouldStore = true);
	void endTransaction(ReceiveInterface* receiveInterface);

	virtual void setPinState(quint16 pin, bool state);
	virtual void sendCommandSequence(CommandEntries& commandEntries);

	virtual int getResetCount();
	virtual void clearResetCount();

	virtual void i2CReadRegister(quint32 addr, quint32 reg);
	virtual void i2CWriteRegister(quint32 addr, quint32 reg, quint32 data);

	// properties
	virtual void setName(const QByteArray& newName);

	// Send Interface
	virtual quint32 send(const QByteArray& sendMe, const Arguments& arguments, bool console, ReceiveInterface* recieveInterface, bool store = true);
	virtual bool ready();

	// Receive Interface
	virtual void receive(FramePackage& framePackage);

protected:
	bool openSerialDevice();

protected slots:
	void handleError(QSerialPort::SerialPortError error);
	void on_readyRead();

private:

	QString serialPortError();
	bool readSerialData();

	static bool					_initialized;
	bool						_connected{false};
	TACPSOCProtocol				_tacProtocol;
	SerialPortInfo				_tacPortInfo;
	SerialPort*					_serialPort{Q_NULLPTR};
	bool						_readyRead{false};

	void handleGetNameResponse(FramePackage& framePackage);
	void handleGetResetCount(FramePackage& framePackage);
	void handleI2CRead(FramePackage& framePackage);
	void handleI2CWrite(FramePackage& framePackage);
	void handleIdle(FramePackage& framePackage);
	void handleSetPin(FramePackage& framePackage);
	void handlePlatformID(FramePackage& framePackage);
	void handleSetName(FramePackage& framePackage);
	void handleUUIDResponse(FramePackage& framePackage);
	void handleVersionResponse(FramePackage& framePackage);

	virtual void setupConnected();
	virtual void setupDiscovery();

	void log(FramePackage& framePackage);
};

#endif // TACPSOCDRIVETHREAD_H
