#ifndef TACSTM32DRIVETHREAD_H
#define TACSTM32DRIVETHREAD_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include "TACDriveThread.h"
#include "TACSTM32Protocol.h"

// QCommon
#include "ReceiveInterface.h"

// Qt
#include <QFile>

class STM32Device;

class QCOMMONCONSOLE_EXPORT TACSTM32DriveThread :
	public TACDriveThread
{
Q_OBJECT

public:
	TACSTM32DriveThread(uint hash);
	~TACSTM32DriveThread();

	virtual void run();

	virtual void sendCommand(const QByteArray& command, bool console = false,
		ReceiveInterface* receiveInterface = Q_NULLPTR, bool shouldStore = true);

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
	bool openSTM32Device();

private:
	bool						_connected{false};
	STM32Device*				_stm32Device{Q_NULLPTR};

	TACSTM32Protocol			_tacProtocol;

	virtual void setupConnected();
	virtual void setupDiscovery();
};

#endif // TACSTM32DRIVETHREAD_H
