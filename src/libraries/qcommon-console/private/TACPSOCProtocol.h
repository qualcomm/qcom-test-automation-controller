// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef TACPSOCPROTOCOL_H
#define TACPSOCPROTOCOL_H

#include "QCommonConsoleGlobal.h"

class TACDriveThread;

// QCommon
#include "ProtocolInterface.h"
#include "ReceiveInterface.h"

class QCOMMONCONSOLE_EXPORT TACPSOCProtocol :
	public QObject,
	public ProtocolInterface,
	public ReceiveInterface
{
Q_OBJECT

Q_INTERFACES(ProtocolInterface)

public:
	TACPSOCProtocol();
	virtual ~TACPSOCProtocol();

	void setTACDriveTrain(TACDriveThread* tacDriveTrain);

	quint32 sendCommand(const QByteArray& command, const Arguments& arguments, bool console = false,
		ReceiveInterface* receiveInterface = Q_NULLPTR, bool shouldStore = true);
	void endTransaction(ReceiveInterface* receiveInterface = Q_NULLPTR);
	void sendHelpCommand();

	// Receive Interface
	virtual void receive(FramePackage& framePackage);

	// timing
	virtual void idle();

protected:
	virtual void frameComplete(const QByteArray& completedFrame);
	virtual void badFrame(const QByteArray& completedFrame);

private:
	TACDriveThread*				_tacDriveTrain;
	quint64						_tickCount;
	QByteArray					_currentCommand;

	void triggerElapsed();

	void queueCommand(const QByteArray& command);
};

#endif // TACPSOCPROTOCOL_H
