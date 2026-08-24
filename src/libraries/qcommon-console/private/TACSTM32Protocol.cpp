// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACSTM32Protocol.h"

#include "TACCommands.h"
#include "TACDriveThread.h"
#include "TACSTM32Coder.h"

// QCommon
#include "TickCount.h"

TACSTM32Protocol::TACSTM32Protocol() :
	_tacDriveTrain(Q_NULLPTR)
{
	setFrameCoder(new TACSTM32Coder);
}

TACSTM32Protocol::~TACSTM32Protocol()
{
	if (_frameCoder != Q_NULLPTR)
	{
		delete _frameCoder;
		_frameCoder = Q_NULLPTR;
	}
}

void TACSTM32Protocol::setTACDriveTrain
(
	TACDriveThread* tacDriveTrain
)
{
	Q_ASSERT(tacDriveTrain != Q_NULLPTR);

	_tacDriveTrain = tacDriveTrain;
}

quint32 TACSTM32Protocol::sendCommand
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
	const QByteArray& completedFrame
)
{
	Q_UNUSED(completedFrame)

	clearPendingFrame();
}

void TACSTM32Protocol::badFrame
(
	const QByteArray& completedFrame
)
{
	Q_UNUSED(completedFrame);
}
