// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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

