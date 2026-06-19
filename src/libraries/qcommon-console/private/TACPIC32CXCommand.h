// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef TACPIC32CXCOMMAND_H
#define TACPIC32CXCOMMAND_H


#include "QCommonConsoleGlobal.h"

// QCommonConsole
class ReceiveInterface;
#include "SendInterface.h"

// Qt
#include <QByteArray>
#include <QVariant>

class QCOMMONCONSOLE_EXPORT TACPIC32CXCommand
{
public:
	TACPIC32CXCommand(SendInterface* sender, ReceiveInterface* receiver);
	~TACPIC32CXCommand();

	void version();
	void name();
	void uuid();

	virtual void setPinState(quint16 pin, bool state);

	void setName(const QByteArray& newName);

	void send(const QByteArray& command, const Arguments& arguments, bool console, bool store = true);
	void addDelay(quint32 delayInMilliSeconds);
	void addLogComment(const QByteArray& comment);
	void addEndTransaction();

	void platformID();
	void clearBuffer();
private:
	ReceiveInterface*			_receiver;
	SendInterface*				_sender;
	bool						_ready;
};

#endif // TACPIC32CXCOMMAND_H
