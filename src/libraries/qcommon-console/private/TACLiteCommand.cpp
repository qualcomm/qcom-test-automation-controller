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
#include <string>

TACLiteCommand::TACLiteCommand
(
	SendInterface* sender,
	ReceiveInterface* receiver
) :
	_receiver(receiver),
	_sender(sender),
	_ready(false)
{
	Q_ASSERT(_receiver != nullptr);
	Q_ASSERT(_sender != nullptr);

	_ready = sender->ready();
}

TACLiteCommand::~TACLiteCommand()
{
	addEndTransaction();
}

void TACLiteCommand::version()
{
	send(kVersionCommand, Arguments(), false, true);
}

void TACLiteCommand::name()
{
	send(kGetNameCommand, Arguments(), false, true);
}

void TACLiteCommand::uuid()
{
	send(kGetUUIDCommand, Arguments(), false, false);
}

void TACLiteCommand::setPinState
(
	uint16_t pin,
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

	send(kSetUSB0Command, args, false, true);
}

void TACLiteCommand::usb1
(
	bool state
)
{
	Arguments args;

	args.push_back(state);

	send(kSetUSB1Command, args, false, true);
}

void TACLiteCommand::externalPowerControl(bool state)
{
	Arguments args;

	args.push_back(state);

	send(kSetExternalPowerControlCommand, args, false, true);
}

void TACLiteCommand::powerKey
(
	bool state
)
{
	Arguments args;

	args.push_back(state);

	send(kSetPowerKeyCommand, args, false, true);
}

void TACLiteCommand::volumeUp
(
	bool state
)
{
	Arguments args;

	args.push_back(state);

	send(kSetVolumeUpCommand, args, false, true);
}

void TACLiteCommand::volumeDown
(
	bool state
)
{
	Arguments args;

	args.push_back(state);

	send(kSetVolumeDownCommand, args, false, true);
}

void TACLiteCommand::setName
(
	const std::string& newName
)
{
	Arguments args;

	args.push_back(newName);

	send(kSetNameCommand, args, false, true);
}

void TACLiteCommand::getResetCount()
{
	send(kGetResetCountCommand, Arguments(), false, true);
}

void TACLiteCommand::clearResetCount()
{
	send(kClearResetCountCommand, Arguments(), false, true);
}

void TACLiteCommand::send
(
	const std::string& command,
	const Arguments& arguments,
	bool console,
	bool store
)
{
	_sender->send(command, arguments, console, _receiver, store);
}

void TACLiteCommand::addDelay(uint32_t delayInMilliSeconds)
{
	_sender->addDelay(delayInMilliSeconds, _receiver);
}

void TACLiteCommand::addLogComment
(
	const std::string& comment
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

	send(kSetDisconnectUIM1Command, args, false, true);
}

void TACLiteCommand::disconnectUIM2Button
(
	bool state
)
{	
	Arguments args;

	args.push_back(state);

	send(kSetDisconnectUIM2Command, args, false, true);
}

void TACLiteCommand::forcePSHoldHigh(bool state)
{
	Arguments args;

	args.push_back(state);

	send(kSetForcePSHoldHighCommand, args, false, true);
}

void TACLiteCommand::disconnectSDCard(bool state)
{
	Arguments args;

	args.push_back(state);

	send(kSetDisconnectSDCardCommand, args, false, true);
}

void TACLiteCommand::primaryEDL
(
	bool state
)
{
	Arguments args;

	args.push_back(state);

	send(kSetPrimaryEDLCommand, args, false, true);
}

void TACLiteCommand::secondaryEDL
(
	bool state
)
{
	Arguments args;

	args.push_back(state);

	send(kSetSecondaryEDLCommand, args, false, true);
}

void TACLiteCommand::secondaryPMResinN(bool state)
{
	Arguments args;

	args.push_back(state);

	send(kSecondaryPM_RESIN_NCommand, args, false, true);
}

void TACLiteCommand::eud(bool state)
{
	Arguments args;

	args.push_back(state);

	send(kSetEUDCommand, args, false, true);
}

void TACLiteCommand::platformID()
{
	send(kGetPlatformIDCommand, Arguments(), false, true);
}

void TACLiteCommand::headsetDisconnect(bool state)
{

	Arguments args;

	args.push_back(state);

	send(kSetHeadsetDisconnectCommand, args, false, true);
}
