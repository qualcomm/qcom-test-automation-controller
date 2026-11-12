// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACCommands.h"
#include "TACPIC32CXCommand.h"

// QCommon
#include "ReceiveInterface.h"
#include "SendInterface.h"

// Qt
#include <QtGlobal>

#include <string>

TACPIC32CXCommand::TACPIC32CXCommand(SendInterface* sender, ReceiveInterface* receiver) :
	_receiver(receiver),
	_sender(sender),
	_ready(false)
{
	Q_ASSERT(_receiver != nullptr);
	Q_ASSERT(_sender != nullptr);

	_ready = sender->ready();
}

TACPIC32CXCommand::~TACPIC32CXCommand()
{
	addEndTransaction();
}

void TACPIC32CXCommand::version()
{
	send(kVersionCommand, Arguments(), false, false);
}

void TACPIC32CXCommand::name()
{
	send(kGetNameCommand, Arguments(), false, false);
}

void TACPIC32CXCommand::uuid()
{
	send(kGetUUIDCommand, Arguments(), false, false);
}

void TACPIC32CXCommand::setPinState(uint16_t pin, bool state)
{
	Arguments args;

	args.push_back(state);
	args.push_back(pin);

	send(kPIC32CXSetPinCommand, args, false, false);
}

void TACPIC32CXCommand::setName(const std::string& newName)
{
	Arguments args;

	args.push_back(newName);

	send(kSetNameCommand, args, false, false);
}

void TACPIC32CXCommand::send(const std::string& command, const Arguments& arguments, bool console, bool store)
{
	_sender->send(command, arguments, console, _receiver, store);
}

void TACPIC32CXCommand::addDelay(uint32_t delayInMilliSeconds)
{
	_sender->addDelay(delayInMilliSeconds, _receiver);
}

void TACPIC32CXCommand::addLogComment(const std::string& comment)
{
	_sender->addLogComment(comment);
}

void TACPIC32CXCommand::addEndTransaction()
{
	_sender->addEndTransaction(_receiver);
}

void TACPIC32CXCommand::platformID()
{
	send(kPIC32CXPlatformIDCommand, Arguments(), false, false);
}

void TACPIC32CXCommand::clearBuffer()
{
	send(kPIC32CXClearBufferCommand, Arguments(), false, false);
}
