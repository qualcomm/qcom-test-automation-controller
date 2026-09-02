// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACSTM32Command.h"
#include "TACCommands.h"

// QCommon
#include "ReceiveInterface.h"
#include "SendInterface.h"

// Qt
#include <QtGlobal>

TACSTM32Command::TACSTM32Command
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

TACSTM32Command::~TACSTM32Command()
{
	addEndTransaction();
}

void TACSTM32Command::setPinState
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

void TACSTM32Command::send
(
	const QByteArray& command,
	const Arguments& arguments,
	bool console,
	bool store
)
{
	_sender->send(command, arguments, console, _receiver, store);
}

void TACSTM32Command::addDelay(quint32 delayInMilliSeconds)
{
	_sender->addDelay(delayInMilliSeconds, _receiver);
}

void TACSTM32Command::addLogComment
(
	const QByteArray& comment
)
{
	_sender->addLogComment(comment);
}

void TACSTM32Command::addEndTransaction()
{
	_sender->addEndTransaction(_receiver);
}
