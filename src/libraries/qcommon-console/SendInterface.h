#ifndef SENDINTERFACE_H
#define SENDINTERFACE_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "QCommonConsoleGlobal.h"
#include "ReceiveInterface.h"

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

typedef std::variant<std::monostate, bool, uint32_t, std::string> Argument;
typedef std::vector<Argument> Arguments;

class QCOMMONCONSOLE_EXPORT SendInterface
{
public:
	SendInterface() {}
	virtual ~SendInterface() {}

	virtual bool ready() = 0;
	virtual uint32_t send(const std::string& sendMe, const Arguments& argument, bool command, ReceiveInterface* recieveInterface, bool store = true) = 0;
	virtual void addDelay(uint32_t delayInMilliSeconds, ReceiveInterface* recieveInterface) = 0;
	virtual void addLogComment(const std::string& comment) = 0;
	virtual void addEndTransaction(ReceiveInterface* recieveInterface) = 0;
};

#endif // SENDINTERFACE_H

