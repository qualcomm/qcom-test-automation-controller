// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACPIC32CXProtocol.h"

// Alpaca Server
#include "TACCommands.h"
#include "TACDriveThread.h"
#include "TACPIC32CXCoder.h"

// QCommon
#include "TACCommandHashes.h"
#include "TickCount.h"

#include <string>
#include <vector>

TACPIC32CXProtocol::TACPIC32CXProtocol() :
	_tacDriveTrain(nullptr),
	_tickCount(0)
{
	setFrameCoder(new TACPIC32CXCoder);
}

TACPIC32CXProtocol::~TACPIC32CXProtocol()
{
	if (_frameCoder != nullptr)
	{
		delete _frameCoder;
		_frameCoder = nullptr;
	}
}

void TACPIC32CXProtocol::setTACDriveTrain
(
	TACDriveThread* tacDriveTrain
)
{
	Q_ASSERT(tacDriveTrain != nullptr);

	_tacDriveTrain = tacDriveTrain;
}

uint32_t TACPIC32CXProtocol::sendCommand
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
		framePackage->_recieveInterface = receiveInterface;
		if (_frameCoder == nullptr)
			framePackage->_codedRequest = command;
		else
			framePackage->_codedRequest = _frameCoder->encode(command, arguments);

		framePackage->_tickcount = tickCount();

		ProtocolInterface::queueCommand(framePackage);
	}

	return result;
}

void TACPIC32CXProtocol::endTransaction(ReceiveInterface* receiveInterface)
{
	ProtocolInterface::queueEndTransaction(receiveInterface);
}

void TACPIC32CXProtocol::receive
(
	FramePackage& framePackage
)
{
	_tacDriveTrain->receive(framePackage);
}

void TACPIC32CXProtocol::idle()
{
}

void TACPIC32CXProtocol::frameComplete
(
	const std::string& completedFrame
)
{
	static std::vector<std::string> frames;

	if (!completedFrame.empty())
	{
		frames.push_back(completedFrame);
	}
	else
	{
		FramePackage& framePackage = pendingFramePackage();
		if (framePackage != nullptr)
		{
			framePackage->_responses = frames;

			// Consider all pic32cx frames to be valid
			framePackage->_valid = true;

			if (framePackage->_recieveInterface != nullptr)
			{
				framePackage->_recieveInterface->receive(framePackage);
			}
			else
			{
				_tacDriveTrain->receive(framePackage);
			}
		}

		clearPendingFrame();

		frames.clear();
	}
}

void TACPIC32CXProtocol::badFrame
(
	const std::string& completedFrame
)
{
	(void)completedFrame;
}

void TACPIC32CXProtocol::triggerElapsed()
{
	_tickCount = 0;
}
