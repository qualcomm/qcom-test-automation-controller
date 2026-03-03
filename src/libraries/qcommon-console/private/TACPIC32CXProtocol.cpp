#include "TACPIC32CXProtocol.h"

// Confidential and Proprietary – Qualcomm Technologies, Inc.

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
// Copyright ©2018 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

// Author: msimpson

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

