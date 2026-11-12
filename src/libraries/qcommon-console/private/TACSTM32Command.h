#ifndef TACSTM32COMMAND_H
#define TACSTM32COMMAND_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

// QCommon
class ReceiveInterface;
#include "SendInterface.h"

#include <cstdint>
#include <string>

class QCOMMONCONSOLE_EXPORT TACSTM32Command
{
public:
	TACSTM32Command(SendInterface* sender, ReceiveInterface* receiver);
	~TACSTM32Command();

	virtual void setPinState(uint16_t pin, bool state);

	void send(const std::string& command, const Arguments& arguments, bool console, bool store = true);
	void addDelay(uint32_t delayInMilliSeconds);
	void addLogComment(const std::string& comment);
	void addEndTransaction();

private:
	ReceiveInterface*			_receiver;
	SendInterface*				_sender;
	bool						_ready;
};

#endif // TACSTM32COMMAND_H
