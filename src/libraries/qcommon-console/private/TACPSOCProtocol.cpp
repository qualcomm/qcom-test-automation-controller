// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACPSOCProtocol.h"

// Alpaca Server
#include "TACCommands.h"
#include "TACPSOCCommand.h"
#include "TACDriveThread.h"
#include "TACPSOCCoder.h"

// QCommon
#include "TickCount.h"

#include <cctype>
#include <string>
#include <vector>

TACPSOCProtocol::TACPSOCProtocol() :
	_tacDriveTrain(nullptr),
	_tickCount(0)
{
	setFrameCoder(new TACPSOCCoder);
}

TACPSOCProtocol::~TACPSOCProtocol()
{
	if (_frameCoder != nullptr)
	{
		delete _frameCoder;
		_frameCoder = nullptr;
	}
}

void TACPSOCProtocol::setTACDriveTrain
(
	TACDriveThread* tacDriveTrain
)
{
	Q_ASSERT(tacDriveTrain != nullptr);

	_tacDriveTrain = tacDriveTrain;
}

uint32_t TACPSOCProtocol::sendCommand
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

void TACPSOCProtocol::endTransaction(ReceiveInterface* receiveInterface)
{
	ProtocolInterface::queueEndTransaction(receiveInterface);
}

void TACPSOCProtocol::receive
(
	FramePackage& framePackage
)
{
	_tacDriveTrain->receive(framePackage);
}

void TACPSOCProtocol::idle()
{
}

static std::string trimString(const std::string& s)
{
	size_t first = s.find_first_not_of(" \t\r\n");
	if (first == std::string::npos) return "";
	size_t last = s.find_last_not_of(" \t\r\n");
	return s.substr(first, last - first + 1);
}

static bool equalsIgnoreCase(const std::string& a, const std::string& b)
{
	if (a.size() != b.size()) return false;
	for (size_t i = 0; i < a.size(); ++i)
		if (std::tolower(static_cast<unsigned char>(a[i])) != std::tolower(static_cast<unsigned char>(b[i])))
			return false;
	return true;
}

void TACPSOCProtocol::frameComplete
(
	const std::string& completedFrame
)
{
	static std::vector<std::string> frames;

	if (!completedFrame.empty())
	{
		if (completedFrame.find(kCommand) == std::string::npos)
		{
			frames.push_back(completedFrame);
		}
	}
	else
	{
		FramePackage& framePackage = pendingFramePackage();
		if (framePackage != nullptr)
		{
			framePackage->_responses = frames;
			framePackage->_valid = !frames.empty() && equalsIgnoreCase(trimString(frames.back()), "ok");

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

void TACPSOCProtocol::badFrame
(
	const std::string& completedFrame
)
{
	(void)completedFrame;
}

void TACPSOCProtocol::triggerElapsed()
{
	_tickCount = 0;
}
