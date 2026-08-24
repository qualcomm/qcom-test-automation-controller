#ifndef TACSTM32PROTOCOL_H
#define TACSTM32PROTOCOL_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

class TACDriveThread;

// QCommon
#include "ProtocolInterface.h"
#include "ReceiveInterface.h"

class QCOMMONCONSOLE_EXPORT TACSTM32Protocol :
	public QObject,
	public ProtocolInterface,
	public ReceiveInterface
{
Q_OBJECT

Q_INTERFACES(ProtocolInterface)

public:
	TACSTM32Protocol();
	virtual ~TACSTM32Protocol();

	void setTACDriveTrain(TACDriveThread* tacDriveTrain);

	quint32 sendCommand(const QByteArray& command, const Arguments& arguments, bool console = false,
		ReceiveInterface* receiveInterface = Q_NULLPTR, bool shouldStore = true);
	void endTransaction(ReceiveInterface* receiveInterface = Q_NULLPTR);

	// Receive Interface
	virtual void receive(FramePackage& framePackage);

	// timing
	virtual void idle();

protected:
	virtual void frameComplete(const QByteArray& completedFrame);
	virtual void badFrame(const QByteArray& completedFrame);

private:
	TACDriveThread*				_tacDriveTrain;
};

#endif // TACSTM32PROTOCOL_H
