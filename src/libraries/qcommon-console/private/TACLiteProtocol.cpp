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

#include "TACLiteProtocol.h"

// QTAC Server
#include "TACCommands.h"
#include "TACDriveThread.h"
#include "TACLiteCoder.h"

// QCommon
#include "TickCount.h"

TACLiteProtocol::TACLiteProtocol() :
	_tacDriveTrain(Q_NULLPTR),
	_tickCount(0)
{
	setFrameCoder(new TacLiteCoder);
}

TACLiteProtocol::~TACLiteProtocol()
{
	if (_frameCoder != Q_NULLPTR)
	{
		delete _frameCoder;
		_frameCoder = Q_NULLPTR;
	}
}

void TACLiteProtocol::setTacDriveTrain
(
	TACDriveThread* tacDriveTrain
)
{
	Q_ASSERT(tacDriveTrain != Q_NULLPTR);

	_tacDriveTrain = tacDriveTrain;
}

quint32 TACLiteProtocol::sendCommand
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
		framePackage->_tickcount = tickCount();

		framePackage->_recieveInterface = receiveInterface;
		if (_frameCoder == Q_NULLPTR)
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
	const QByteArray& completedFrame
)
{
	Q_UNUSED(completedFrame)

	clearPendingFrame();
}

void TACLiteProtocol::badFrame
(
	const QByteArray& completedFrame
)
{
	Q_UNUSED(completedFrame);
}

void TACLiteProtocol::triggerElapsed()
{
	_tickCount = 0;
}
