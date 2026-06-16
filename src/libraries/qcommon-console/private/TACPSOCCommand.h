#ifndef TACPSOCCOMMAND_H
#define TACPSOCCOMMAND_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

// QCommonConsole
class ReceiveInterface;
#include "SendInterface.h"

// Qt
#include <QByteArray>
#include <QVariant>

class QCOMMONCONSOLE_EXPORT TACPSOCCommand
{
public:
	TACPSOCCommand(SendInterface* sender, ReceiveInterface* receiver);
	~TACPSOCCommand();

	void version();
	void name();
	void uuid();

	virtual void setPinState(quint16 pin, bool state);

	void setName(const QByteArray& newName);

	void getResetCount();
	void clearResetCount();

	void i2CReadRegister(quint32 addr, quint32 reg);
	void i2CWriteRegister(quint32 addr, quint32 reg, quint32 data);

	void send(const QByteArray& command, const Arguments& arguments, bool console, bool store = true);
	void addDelay(quint32 delayInMilliSeconds);
	void addLogComment(const QByteArray& comment);
	void addEndTransaction();

	void platformID();
private:
	ReceiveInterface*			_receiver;
	SendInterface*				_sender;
	bool						_ready;
};

#endif // TACPSOCCOMMAND_H
