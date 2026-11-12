#ifndef TACCOMMAND_H
#define TACCOMMAND_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include "PinID.h"

#include <map>
#include <string>
#include <vector>

class TACCommand;

typedef std::vector<TACCommand> TACCommands;
typedef std::map<std::string, TACCommand> TACCommandMap;

class QCOMMONCONSOLE_EXPORT TACCommand
{
public:
	TACCommand() = default;
	TACCommand(const TACCommand&) = default;
	TACCommand(TACCommand&&) = default;

	~TACCommand() = default;

	TACCommand& operator=(const TACCommand&) = default;
	TACCommand& operator=(TACCommand&&) = default;

	std::string command();

	static bool contains(const std::string& command, const TACCommands& tacCommands);
	static TACCommand find(const std::string& command, const TACCommands& tacCommands);
	static TACCommand find(PinID pin, const TACCommands& tacCommands);

	PinID						_pin{static_cast<PinID>(-1)};
	std::string					_command;
	std::string					_helpText;
	bool						_currentState{false};
	bool						_isInverted{false};
	std::string					_tabName;
	std::string					_groupName;
	std::string					_cellLocation;
};

#endif // TACCOMMAND_H
