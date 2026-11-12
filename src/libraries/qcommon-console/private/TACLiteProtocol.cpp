// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACLiteProtocol.h"

#include "TACCommands.h"
#include "TACDriveThread.h"
#include "TACLiteCoder.h"

// QCommon
#include "TickCount.h"

#include <string>

TACLiteProtocol::TACLiteProtocol() :
	_tacDriveTrain(nullptr),
	_tickCount(0)
{
	setFrameCoder(new TACLiteCoder);
}

TACLiteProtocol::~TACLiteProtocol()
{
	if (_frameCoder != nullptr)
	{
		delete _frameCoder;
		_frameCoder = nullptr;
	}
}

void TACLiteProtocol::setTACDriveTrain
(
	TACDriveThread* tacDriveTrain
)
{
	Q_ASSERT(tacDriveTrain != nullptr);

	_tacDriveTrain = tacDriveTrain;
}

uint32_t TACLiteProtocol::sendCommand
(
	const std::string& command,
	const Arguments& arguments,
	bool console,
	ReceiveInterface* receiveInterface,
	bool shouldStore
)
{
	uint32_t result(kBadQueueValue);

	if (!command.empty())
	{
		result = getNextSendID();

		FramePackage framePackage = std::make_shared<_FramePackage>();

		framePackage->_packetID = result;
		framePackage->_request = command;
		framePackage->_arguments = arguments;
		framePackage->_requestHash = CommandStringToHash(command);
		framePackage->_console = console;
		framePackage->_shouldStore = shouldStore;
		framePackage->_tickcount = tickCount();

		framePackage->_recieveInterface = receiveInterface;
		if (_frameCoder == nullptr)
			framePackage->_codedRequest = command;
		else
			framePackage->_codedRequest = _frameCoder->encode(command, arguments);

		ProtocolInterface::queueCommand(framePackage);
	}

	return result;
}

void TACLiteProtocol::endTransaction(ReceiveInterface* receiveInterface)
{
	ProtocolInterface::queueEndTransaction(receiveInterface);
}

void TACLiteProtocol::receive
(
	FramePackage& framePackage
)
{
	_tacDriveTrain->receive(framePackage);
}

void TACLiteProtocol::idle()
{
}

void TACLiteProtocol::frameComplete
(
	const std::string& completedFrame
)
{
	(void)completedFrame;

	clearPendingFrame();
}

void TACLiteProtocol::badFrame
(
	const std::string& completedFrame
)
{
	(void)completedFrame;
}

void TACLiteProtocol::triggerElapsed()
{
	_tickCount = 0;
}
