#ifndef TACLITEPROTOCOL_H
#define TACLITEPROTOCOL_H
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
// Copyright 2022 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

// Author: msimpson

#include "QCommonConsoleGlobal.h"

// AlpacaServer
class TACDriveThread;

// QCommon
#include "ProtocolInterface.h"
#include "ReceiveInterface.h"
#include "SendInterface.h"

class QCOMMONCONSOLE_EXPORT TACLiteProtocol :
	public QObject,
	public ProtocolInterface,
	public ReceiveInterface
{
Q_OBJECT

Q_INTERFACES(ProtocolInterface)

public:
	TACLiteProtocol();
	virtual ~TACLiteProtocol();

	void setTacDriveTrain(TACDriveThread* tacDriveTrain);

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

#endif // TACLITEPROTOCOL_H
