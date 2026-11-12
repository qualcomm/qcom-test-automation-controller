#ifndef TACSTM32PROTOCOL_H
#define TACSTM32PROTOCOL_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

class TACDriveThread;

// QCommon
#include "ProtocolInterface.h"
#include "ReceiveInterface.h"

#include <cstdint>
#include <string>

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

	uint32_t sendCommand(const std::string& command, const Arguments& arguments, bool console = false,
		ReceiveInterface* receiveInterface = nullptr, bool shouldStore = true);
	void endTransaction(ReceiveInterface* receiveInterface = nullptr);

	// Receive Interface
	virtual void receive(FramePackage& framePackage) override;

	// timing
	virtual void idle() override;

protected:
	virtual void frameComplete(const std::string& completedFrame) override;
	virtual void badFrame(const std::string& completedFrame) override;

private:
	TACDriveThread*				_tacDriveTrain;
};

#endif // TACSTM32PROTOCOL_H
