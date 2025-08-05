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

#include "TACLiteCommand.h"
#include "TACCommands.h"

// QCommon
#include "ReceiveInterface.h"
#include "SendInterface.h"

// Qt
#include <QtGlobal>

// C++
#include <stdexcept>

TacLiteCommand::TacLiteCommand
(
	SendInterface* sender,
	ReceiveInterface* receiver
) :
	_receiver(receiver),
	_sender(sender),
	_ready(false)
{
	Q_ASSERT(_receiver != Q_NULLPTR);
	Q_ASSERT(_sender != Q_NULLPTR);

	_ready = sender->ready();
}

TacLiteCommand::~TacLiteCommand()
{
	addEndTransaction();
}

void TacLiteCommand::version()
{
	send(kVersionCommand, Arguments(), false, _receiver);
}

void TacLiteCommand::name()
{
	send(kGetNameCommand, Arguments(), false, _receiver);
}

void TacLiteCommand::uuid()
{
	send(kGetUUIDCommand, Arguments(), false, false);
}

void TacLiteCommand::setPinState
(
	quint16 pin,
	bool state
)
{
	Arguments args;

	args.push_back(state);
	args.push_back(pin);

	send(kSetPinCommand, args, false, false);
}

void TacLiteCommand::battery
(
	bool state
)
{
	Arguments args;

	args.push_back(state);

	send(kSetBatteryCommand, args, false, false);
}

void TacLiteCommand::usb0
(
	bool state
)
{
	Arguments args;

	args.push_back(state);

	send(kSetUSB0Command, args, false, _receiver);
}

void TacLiteCommand::usb1
(
	bool state
)
{
	Arguments args;

	args.push_back(state);

	send(kSetUSB1Command, args, false, _receiver);
}

void TacLiteCommand::externalPowerControl(bool state)
{
	Arguments args;

	args.push_back(state);

	send(kSetExternalPowerControlCommand, args, false, _receiver);
}

void TacLiteCommand::powerKey
(
	bool state
)
{
	Arguments args;

	args.push_back(state);

	send(kSetPowerKeyCommand, args, false, _receiver);
}

void TacLiteCommand::volumeUp
(
	bool state
)
{
	Arguments args;

	args.push_back(state);

	send(kSetVolumeUpCommand, args, false, _receiver);
}

void TacLiteCommand::volumeDown
(
	bool state
)
{
	Arguments args;

	args.push_back(state);

	send(kSetVolumeDownCommand, args, false, _receiver);
}

void TacLiteCommand::setName
(
	const QByteArray& newName
)
{
	Arguments args;

	args.push_back(newName);

	send(kSetNameCommand, args, false, _receiver);
}

void TacLiteCommand::getResetCount()
{
	send(kGetResetCountCommand, Arguments(), false, _receiver);
}

void TacLiteCommand::clearResetCount()
{
	send(kClearResetCountCommand, Arguments(), false, _receiver);
}

void TacLiteCommand::send
(
	const QByteArray& command,
	const Arguments& arguments,
	bool console,
	bool store
)
{
	_sender->send(command, arguments, console, _receiver, store);
}

void TacLiteCommand::addDelay(quint32 delayInMilliSeconds)
{
	_sender->addDelay(delayInMilliSeconds, _receiver);
}

void TacLiteCommand::addLogComment
(
	const QByteArray& comment
)
{
	_sender->addLogComment(comment);
}

void TacLiteCommand::addEndTransaction()
{
	_sender->addEndTransaction(_receiver);
}

void TacLiteCommand::disconnectUIM1Button
(
	bool state
)
{
	Arguments args;

	args.push_back(state);

	send(kSetDisconnectUIM1Command, args, false, _receiver);
}

void TacLiteCommand::disconnectUIM2Button
(
	bool state
)
{	
	Arguments args;

	args.push_back(state);

	send(kSetDisconnectUIM2Command, args, false, _receiver);
}

void TacLiteCommand::forcePSHoldHigh(bool state)
{
	Arguments args;

	args.push_back(state);

	send(kSetForcePSHoldHighCommand, args, false, _receiver);
}

void TacLiteCommand::disconnectSDCard(bool state)
{
	Arguments args;

	args.push_back(state);

	send(kSetDisconnectSDCardCommand, args, false, _receiver);
}

void TacLiteCommand::primaryEDL
(
	bool state
)
{
	Arguments args;

	args.push_back(state);

	send(kSetPrimaryEDLCommand, args, false, _receiver);
}

void TacLiteCommand::secondaryEDL
(
	bool state
)
{
	Arguments args;

	args.push_back(state);

	send(kSetSecondaryEDLCommand, args, false, _receiver);
}

void TacLiteCommand::secondaryPMResinN(bool state)
{
	Arguments args;

	args.push_back(state);

	send(kSecondaryPM_RESIN_NCommand, args, false, _receiver);
}

void TacLiteCommand::eud(bool state)
{
	Arguments args;

	args.push_back(state);

	send(kSetEUDCommand, args, false, _receiver);
}

void TacLiteCommand::platformID()
{
	send(kGetPlatformIDCommand, Arguments(), false, _receiver);
}

void TacLiteCommand::headsetDisconnect(bool state)
{

	Arguments args;

	args.push_back(state);

	send(kSetHeadsetDisconnectCommand, args, false, _receiver);
}

