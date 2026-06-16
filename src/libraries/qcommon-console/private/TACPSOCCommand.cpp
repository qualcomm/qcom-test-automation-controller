// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACCommands.h"
#include "TACPSOCCommand.h"

// QCommon
#include "ReceiveInterface.h"
#include "SendInterface.h"

// Qt
#include <QtGlobal>

// C++
#include <stdexcept>

TACPSOCCommand::TACPSOCCommand
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

void TACPSOCCommand::setPinState(quint16 pin, bool state)
{
	Arguments args;

	args.push_back(state);
	args.push_back(pin);

	send(kSetPinCommand, args, false, false);
}

void TACPSOCCommand::setName
(
	const QByteArray& newName
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
	quint32 addr,
	quint32 reg
)
{
	QByteArray addrString;
	QByteArray regString;
	QByteArray cmd;

	if (addr > 0xff)
	   throw std::out_of_range("Error: invalid address");

	 if (reg > 0xff)
		throw std::out_of_range("Error: invalid register");

	addrString = QByteArray::number(addr, 16);
	if (addrString.size() == 1)
	{
		addrString = "0x0" + addrString;
	}
	else
	{
		addrString = "0x" + addrString;
	}

	regString = QByteArray::number(reg, 16);
	if (regString.size() == 1)
	{
		regString = "0x0" + regString;
	}
	else
	{
		regString = "0x" + regString;
	}

	Arguments args;

	args.push_back(addrString + " " + regString);
	send(kI2CReadRegisterCommand, args, false, false);

	addDelay(500);

	args.clear();
	send(kI2CReadRegisterValueCommand, args, false, false);
}

void TACPSOCCommand::i2CWriteRegister
(
	quint32 addr,
	quint32 reg,
	quint32 data
)
{
   QByteArray addrString;
   QByteArray regString;
   QByteArray dataString;
   QByteArray cmd;

   if (addr > 0xff)
	  throw std::out_of_range("Error: invalid address");

	if (reg > 0xff)
	   throw std::out_of_range("Error: invalid register");

   if (data > 0xff)
	   throw std::out_of_range("Error: invalid data");

   addrString = QByteArray::number(addr, 16);
   if (addrString.size() == 1)
   {
	   addrString = "0x0" + addrString;
   }
   else
   {
	   addrString = "0x" + addrString;
   }

   regString = QByteArray::number(reg, 16);
   if (regString.size() == 1)
   {
	   regString = "0x0" + regString;
   }
   else
   {
	   regString = "0x" + regString;
   }

   dataString = QByteArray::number(data, 16);
   if (dataString.size() == 1)
   {
	   dataString = "0x0" + dataString;
   }
   else
   {
	   dataString = "0x" + dataString;
   }

   Arguments args;

   args.push_back(addrString + " " + regString + " " + dataString);
   send(kI2CWriteRegisterCommand, args, false, false);
}

void TACPSOCCommand::send
(
	const QByteArray& command,
	const Arguments& arguments,
	bool console,
	bool store
)
{
	_sender->send(command, arguments, console, _receiver, store);
}

void TACPSOCCommand::addDelay(quint32 delayInMilliSeconds)
{
	_sender->addDelay(delayInMilliSeconds, _receiver);
}

void TACPSOCCommand::addLogComment
(
	const QByteArray& comment
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

