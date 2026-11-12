// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACCommands.h"
#include "TACPSOCCommand.h"

// QCommon
#include "ReceiveInterface.h"
#include "SendInterface.h"

// Qt
#include <QtGlobal>

#include <cstdio>
#include <stdexcept>
#include <string>

const uint16_t kMaxI2CPins(8);

static std::string hexByteString(uint32_t val)
{
	char buf[16];
	std::snprintf(buf, sizeof(buf), "0x%02x", val & 0xff);
	return std::string(buf);
}

TACPSOCCommand::TACPSOCCommand
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

TACPSOCCommand::~TACPSOCCommand()
{
	addEndTransaction();
}

void TACPSOCCommand::version()
{
	send(kVersionCommand, Arguments(), false, false);
}

void TACPSOCCommand::name()
{
	send(kGetNameCommand, Arguments(), false, false);
}

void TACPSOCCommand::uuid()
{
	send(kGetUUIDCommand, Arguments(), false, false);
}

void TACPSOCCommand::setPinState(uint16_t pin, bool state)
{
	Arguments args;

	args.push_back(state);
	args.push_back(pin);

	send(kSetPinCommand, args, false, false);
}

void TACPSOCCommand::setName
(
	const std::string& newName
)
{
	Arguments args;

	args.push_back(newName);

	send(kSetNameCommand, args, false, false);
}

void TACPSOCCommand::getResetCount()
{
	send(kGetResetCountCommand, Arguments(), false, false);
}

void TACPSOCCommand::clearResetCount()
{
	send(kClearResetCountCommand, Arguments(), false, false);
}

void TACPSOCCommand::i2CReadRegister
(
	uint32_t addr,
	uint32_t reg
)
{
	if (addr > 0xff)
		throw std::out_of_range("Error: invalid address");

	if (reg > 0xff)
		throw std::out_of_range("Error: invalid register");

	std::string addrString = hexByteString(addr);
	std::string regString = hexByteString(reg);

	Arguments args;

	args.push_back(addrString + " " + regString);
	send(kI2CReadRegisterCommand, args, false, false);

	addDelay(500);

	args.clear();
	send(kI2CReadRegisterValueCommand, args, false, false);
}

void TACPSOCCommand::i2CWriteRegister
(
	uint32_t addr,
	uint32_t reg,
	uint32_t data
)
{
	if (addr > 0xff)
		throw std::out_of_range("Error: invalid address");

	if (reg > 0xff)
		throw std::out_of_range("Error: invalid register");

	if (data > 0xff)
		throw std::out_of_range("Error: invalid data");

	std::string addrString = hexByteString(addr);
	std::string regString = hexByteString(reg);
	std::string dataString = hexByteString(data);

	Arguments args;

	args.push_back(addrString + " " + regString + " " + dataString);
	send(kI2CWriteRegisterCommand, args, false, false);
}

void TACPSOCCommand::setAddressPinState
(
	const std::string& i2cAddress,
	uint16_t pin,
	bool state
)
{
	Arguments args;
	args.push_back(i2cAddress);
	args.push_back(static_cast<uint32_t>(pin % kMaxI2CPins));
	args.push_back(state);

	send(kI2CWriteRegisterCommand, args, false, false);
}

void TACPSOCCommand::send
(
	const std::string& command,
	const Arguments& arguments,
	bool console,
	bool store
)
{
	_sender->send(command, arguments, console, _receiver, store);
}

void TACPSOCCommand::addDelay(uint32_t delayInMilliSeconds)
{
	_sender->addDelay(delayInMilliSeconds, _receiver);
}

void TACPSOCCommand::addLogComment
(
	const std::string& comment
)
{
	_sender->addLogComment(comment);
}

void TACPSOCCommand::addEndTransaction()
{
	_sender->addEndTransaction(_receiver);
}

void TACPSOCCommand::platformID()
{
	send(kGetPlatformIDCommand, Arguments(), false, false);
}
