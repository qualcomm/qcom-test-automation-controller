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

TACPIC32CXProtocol::TACPIC32CXProtocol() :
	_tacDriveTrain(Q_NULLPTR),
	_tickCount(0)
{
	setFrameCoder(new TACPIC32CXCoder);
}

TACPIC32CXProtocol::~TACPIC32CXProtocol()
{
	if (_frameCoder != Q_NULLPTR)
	{
		delete _frameCoder;
		_frameCoder = Q_NULLPTR;
	}
}

void TACPIC32CXProtocol::setTACDriveTrain
(
	TACDriveThread* tacDriveTrain
)
{
	Q_ASSERT(tacDriveTrain != Q_NULLPTR);

	_tacDriveTrain = tacDriveTrain;
}

quint32 TACPIC32CXProtocol::sendCommand
(
	const QByteArray& command,
	const Arguments& arguments,
	bool console,
	ReceiveInterface* receiveInterface,
	bool shouldStore
)
{
	quint32 result(kBadQueueValue);

	if (command.isEmpty() == false)
	{
		result = getNextSendID();

		FramePackage framePackage = FramePackage(new _FramePackage);

		framePackage->_packetID = result;
		framePackage->_request = command;
		framePackage->_arguments = arguments;
		framePackage->_requestHash = CommandStringToHash(command);
		framePackage->_console = console;
		framePackage->_shouldStore = shouldStore;
		framePackage->_recieveInterface = receiveInterface;
		if (_frameCoder == Q_NULLPTR)
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
	const QByteArray& completedFrame
)
{
	static QList<QByteArray> frames;

	if (completedFrame != QByteArray())
	{
		frames.push_back(completedFrame);
	}
	else
	{
		FramePackage& framePackage = pendingFramePackage();
		if (framePackage.isNull() == false)
		{
			framePackage->_responses = frames;

			// Consider all pic32cx frames to be valid
			framePackage->_valid = true;

			if (framePackage->_recieveInterface != Q_NULLPTR)
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
	const QByteArray& completedFrame
)
{
	Q_UNUSED(completedFrame);
}

void TACPIC32CXProtocol::triggerElapsed()
{
	_tickCount = 0;
}

