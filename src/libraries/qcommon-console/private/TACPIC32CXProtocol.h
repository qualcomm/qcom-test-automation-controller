// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef TACPIC32CXPROTOCOL_H
#define TACPIC32CXPROTOCOL_H

#include "QCommonConsoleGlobal.h"

class TACDriveThread;

// QCommon
#include "ProtocolInterface.h"
#include "ReceiveInterface.h"

#include <cstdint>
#include <string>

class QCOMMONCONSOLE_EXPORT TACPIC32CXProtocol :
  public QObject,
  public ProtocolInterface,
  public ReceiveInterface
{
	Q_OBJECT

	Q_INTERFACES(ProtocolInterface)

public:
	TACPIC32CXProtocol();
	virtual ~TACPIC32CXProtocol();

	void setTACDriveTrain(TACDriveThread* tacDriveTrain);

	uint32_t sendCommand(const std::string& command, const Arguments& arguments, bool console = false,
						ReceiveInterface* receiveInterface = nullptr, bool shouldStore = true);
	void endTransaction(ReceiveInterface* receiveInterface = nullptr);
	void sendHelpCommand();

	// Receive Interface
	virtual void receive(FramePackage& framePackage) override;

	// timing
	virtual void idle() override;

protected:
	virtual void frameComplete(const std::string& completedFrame) override;
	virtual void badFrame(const std::string& completedFrame) override;

private:
	TACDriveThread*				_tacDriveTrain;
	uint64_t					_tickCount;
	std::string					_currentCommand;

	void triggerElapsed();

	void queueCommand(const std::string& command);
};

#endif // TACPIC32CXPROTOCOL_H
