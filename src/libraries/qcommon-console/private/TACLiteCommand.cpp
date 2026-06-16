// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACLiteCommand.h"
#include "TACCommands.h"

// QCommon
#include "ReceiveInterface.h"
#include "SendInterface.h"

// Qt
#include <QtGlobal>

// C++
#include <stdexcept>

TACLiteCommand::TACLiteCommand
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

TACLiteCommand::~TACLiteCommand()
{
	addEndTransaction();
}

void TACLiteCommand::version()
{
	send(kVersionCommand, Arguments(), false, _receiver);
}

void TACLiteCommand::name()
{
	send(kGetNameCommand, Arguments(), false, _receiver);
}

void TACLiteCommand::uuid()
{
	send(kGetUUIDCommand, Arguments(), false, false);
}

void TACLiteCommand::setPinState
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

void TACLiteCommand::battery
(
	bool state
)
{
	Arguments args;

	args.push_back(state);

	send(kSetBatteryCommand, args, false, false);
}

void TACLiteCommand::usb0
(
	bool state
)
{
	Arguments args;

	args.push_back(state);

	send(kSetUSB0Command, args, false, _receiver);
}

void TACLiteCommand::usb1
(
	bool state
)
{
	Arguments args;

	args.push_back(state);

	send(kSetUSB1Command, args, false, _receiver);
}

void TACLiteCommand::externalPowerControl(bool state)
{
	Arguments args;

	args.push_back(state);

	send(kSetExternalPowerControlCommand, args, false, _receiver);
}

void TACLiteCommand::powerKey
(
	bool state
)
{
	Arguments args;

	args.push_back(state);

	send(kSetPowerKeyCommand, args, false, _receiver);
}

void TACLiteCommand::volumeUp
(
	bool state
)
{
	Arguments args;

	args.push_back(state);

	send(kSetVolumeUpCommand, args, false, _receiver);
}

void TACLiteCommand::volumeDown
(
	bool state
)
{
	Arguments args;

	args.push_back(state);

	send(kSetVolumeDownCommand, args, false, _receiver);
}

void TACLiteCommand::setName
(
	const QByteArray& newName
)
{
	Arguments args;

	args.push_back(newName);

	send(kSetNameCommand, args, false, _receiver);
}

void TACLiteCommand::getResetCount()
{
	send(kGetResetCountCommand, Arguments(), false, _receiver);
}

void TACLiteCommand::clearResetCount()
{
	send(kClearResetCountCommand, Arguments(), false, _receiver);
}

void TACLiteCommand::send
(
	const QByteArray& command,
	const Arguments& arguments,
	bool console,
	bool store
)
{
	_sender->send(command, arguments, console, _receiver, store);
}

void TACLiteCommand::addDelay(quint32 delayInMilliSeconds)
{
	_sender->addDelay(delayInMilliSeconds, _receiver);
}

void TACLiteCommand::addLogComment
(
	const QByteArray& comment
)
{
	_sender->addLogComment(comment);
}

void TACLiteCommand::addEndTransaction()
{
	_sender->addEndTransaction(_receiver);
}

void TACLiteCommand::disconnectUIM1Button
(
	bool state
)
{
	Arguments args;

	args.push_back(state);

	send(kSetDisconnectUIM1Command, args, false, _receiver);
}

void TACLiteCommand::disconnectUIM2Button
(
	bool state
)
{	
	Arguments args;

	args.push_back(state);

	send(kSetDisconnectUIM2Command, args, false, _receiver);
}

void TACLiteCommand::forcePSHoldHigh(bool state)
{
	Arguments args;

	args.push_back(state);

	send(kSetForcePSHoldHighCommand, args, false, _receiver);
}

void TACLiteCommand::disconnectSDCard(bool state)
{
	Arguments args;

	args.push_back(state);

	send(kSetDisconnectSDCardCommand, args, false, _receiver);
}

void TACLiteCommand::primaryEDL
(
	bool state
)
{
	Arguments args;

	args.push_back(state);

	send(kSetPrimaryEDLCommand, args, false, _receiver);
}

void TACLiteCommand::secondaryEDL
(
	bool state
)
{
	Arguments args;

	args.push_back(state);

	send(kSetSecondaryEDLCommand, args, false, _receiver);
}

void TACLiteCommand::secondaryPMResinN(bool state)
{
	Arguments args;

	args.push_back(state);

	send(kSecondaryPM_RESIN_NCommand, args, false, _receiver);
}

void TACLiteCommand::eud(bool state)
{
	Arguments args;

	args.push_back(state);

	send(kSetEUDCommand, args, false, _receiver);
}

void TACLiteCommand::platformID()
{
	send(kGetPlatformIDCommand, Arguments(), false, _receiver);
}

void TACLiteCommand::headsetDisconnect(bool state)
{

	Arguments args;

	args.push_back(state);

	send(kSetHeadsetDisconnectCommand, args, false, _receiver);
}

