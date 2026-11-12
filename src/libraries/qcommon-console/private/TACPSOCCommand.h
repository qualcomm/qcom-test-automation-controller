// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef TACPSOCCOMMAND_H
#define TACPSOCCOMMAND_H

#include "QCommonConsoleGlobal.h"

// QCommonConsole
class ReceiveInterface;
#include "SendInterface.h"

// Qt
#include <QByteArray>

#include <cstdint>
#include <string>

class QCOMMONCONSOLE_EXPORT TACPSOCCommand
{
public:
	TACPSOCCommand(SendInterface* sender, ReceiveInterface* receiver);
	~TACPSOCCommand();

	void version();
	void name();
	void uuid();

	virtual void setPinState(uint16_t pin, bool state);
	virtual void setAddressPinState(const std::string& i2cAddress, uint16_t pin, bool state);

	void setName(const QByteArray& newName);

	void getResetCount();
	void clearResetCount();

	void i2CReadRegister(uint32_t addr, uint32_t reg);
	void i2CWriteRegister(uint32_t addr, uint32_t reg, uint32_t data);

	void send(const QByteArray& command, const Arguments& arguments, bool console, bool store = true);
	void addDelay(uint32_t delayInMilliSeconds);
	void addLogComment(const QByteArray& comment);
	void addEndTransaction();

	void platformID();
private:
	ReceiveInterface*			_receiver;
	SendInterface*				_sender;
	bool						_ready;
};

#endif // TACPSOCCOMMAND_H
