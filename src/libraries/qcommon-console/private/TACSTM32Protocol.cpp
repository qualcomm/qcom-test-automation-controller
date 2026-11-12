// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACSTM32Protocol.h"

#include "TACCommands.h"
#include "TACDriveThread.h"
#include "TACSTM32Coder.h"

// QCommon
#include "TickCount.h"

#include <string>

TACSTM32Protocol::TACSTM32Protocol() :
	_tacDriveTrain(nullptr)
{
	setFrameCoder(new TACSTM32Coder);
}

TACSTM32Protocol::~TACSTM32Protocol()
{
	if (_frameCoder != nullptr)
	{
		delete _frameCoder;
		_frameCoder = nullptr;
	}
}

void TACSTM32Protocol::setTACDriveTrain
(
	TACDriveThread* tacDriveTrain
)
{
	Q_ASSERT(tacDriveTrain != nullptr);

	_tacDriveTrain = tacDriveTrain;
}

uint32_t TACSTM32Protocol::sendCommand
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

void TACSTM32Protocol::endTransaction(ReceiveInterface* receiveInterface)
{
	ProtocolInterface::queueEndTransaction(receiveInterface);
}

void TACSTM32Protocol::receive
(
	FramePackage& framePackage
)
{
	_tacDriveTrain->receive(framePackage);
}

void TACSTM32Protocol::idle()
{
}

void TACSTM32Protocol::frameComplete
(
	const std::string& completedFrame
)
{
	(void)completedFrame;

	clearPendingFrame();
}

void TACSTM32Protocol::badFrame
(
	const std::string& completedFrame
)
{
	(void)completedFrame;
}
