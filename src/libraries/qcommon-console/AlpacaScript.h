#ifndef ALPACASCRIPT_H
#define ALPACASCRIPT_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "QCommonConsoleGlobal.h"
#include "DebugBoardType.h"
#include "PinID.h"
#include "ScriptVariable.h"
#include "TACCommand.h"

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

typedef std::variant<std::monostate, bool, uint32_t, std::string> CommandArgument;

struct _CommandEntry
{
	enum CommandAction
	{
		eNotSet,
		eSetPin,
		eLog,
		eDelay,
		eBaseCommand
	};

	CommandAction				_commandAction;
	PinID						_pinID{static_cast<PinID>(-1)};
	std::string					_action;
	CommandArgument				_arguement;
};

typedef std::shared_ptr<_CommandEntry> CommandEntry;
typedef std::vector<CommandEntry> CommandEntries;

struct _ScriptCommand
{
	bool isEmpty()
	{
		if (_command.empty())
			return true;

		if (_subCommands.size() == 0)
			return true;

		return false;
	}

	void clear()
	{
		_command.clear();
		_subCommands.clear();
	}

	std::string					_command;
	CommandEntries				_subCommands;
};

typedef std::shared_ptr<_ScriptCommand> ScriptCommand;
typedef std::map<std::string, ScriptCommand> ScriptCommands;

class QCOMMONCONSOLE_EXPORT AlpacaScript
{
public:
	AlpacaScript();

	uint32_t count()
	{
		return static_cast<uint32_t>(_scriptCommands.size());
	}

	static std::string defaultScript(DebugBoardType boardType = eFTDI);

	bool parseScript(const std::string& alpacaScript, const ScriptVariables& scriptVariables, const TACCommands& tacCommands);
	CommandEntries replaceTokens(const ScriptVariables scriptVariables, const CommandEntries commandEntries);
	std::vector<std::string> validateScript(const std::string& alpacaScript, const TACCommands& tacCommands);

	bool hasCommand(const std::string& command);
	std::vector<std::string> availableCommands();

	ScriptCommand getCommand(const std::string& command);
	CommandEntries getCommandEntries(const std::string& command);

	void setVariableValue(const std::string& variableName, int variableValue);
	void setVariableValue(const std::string& variableName, float variableValue);

private:
	bool isScriptVariable(const ScriptVariables& scriptVariables, const std::string& scriptVariable);
	std::string createBaseCommandString(const std::string& baseCommandString);
	bool processCommandEntries(ScriptCommand scriptCommand, CommandEntries& commandEntries, int level = 0);

	ScriptCommands				_scriptCommands;
};

#endif // ALPACASCRIPT_H
