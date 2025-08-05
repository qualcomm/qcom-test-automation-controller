/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
