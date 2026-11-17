// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "AlpacaScript.h"

// QCommonConsole
#include "AppCore.h"
#include "ConsoleApplicationEnhancements.h"
#include "ScriptVariable.h"
#include "StringUtilities.h"

// c++
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <utility>


// QTAC Script
AlpacaScript::AlpacaScript()
{

}

std::string AlpacaScript::defaultScript(DebugBoardType boardType)
{
	std::string result;

	(void)boardType;

	std::string scriptPath = tacConfigRoot() + "DefaultScript.txt";

	std::ifstream file(scriptPath, std::ios::binary);
	if (file.is_open() == true)
	{
		std::ostringstream fileContents;
		fileContents << file.rdbuf();
		result = fileContents.str();
	}

	return result;
}

bool AlpacaScript::parseScript(const std::string& alpacaScript, const ScriptVariables& scriptVariables, const TACCommands& tacCommands)
{
	bool result{true};

	std::istringstream alpacaScriptStream(alpacaScript);

	ScriptCommand scriptCommand;

	std::string lineText;
	while (std::getline(alpacaScriptStream, lineText))
	{
		std::string line = trim(lineText);
		if (line.empty() || startsWith(line, "//"))
			continue;

		std::vector<std::string> words = splitWords(line);
		if (words.size() >= 2)
		{
			if (toLower(words.at(0)) == "def")
			{
				scriptCommand = ScriptCommand(new _ScriptCommand);

				scriptCommand->_command = createBaseCommandString(words.at(1));

				_scriptCommands[scriptCommand->_command] = scriptCommand;
			}
			else if (TACCommand::contains(words.at(0), tacCommands))
			{
				std::string command{words.at(0)};

				TACCommand tacCommand = TACCommand::find(command, tacCommands);

				CommandEntry commandEntry = CommandEntry(new _CommandEntry);

				commandEntry->_commandAction = _CommandEntry::eSetPin;
				commandEntry->_pinID = tacCommand._pin;
				commandEntry->_action = command;
				commandEntry->_arguement = (words.at(1) == "1") ? true : false;

				scriptCommand->_subCommands.push_back(commandEntry);
			}
			else if (equalsIgnoreCase(words.at(0), "logcomment"))
			{
				CommandEntry commandEntry = CommandEntry(new _CommandEntry);

				commandEntry->_commandAction = _CommandEntry::eLog;
				commandEntry->_action = words.at(0);

				std::string comment = line;
				size_t commentPos = toLower(comment).find("logcomment ");
				if (commentPos != std::string::npos)
					comment.erase(commentPos, std::string("logcomment ").size());

				commandEntry->_arguement = comment;

				scriptCommand->_subCommands.push_back(commandEntry);

			}
			else if (words.at(0) == "delay")
			{
				std::string delayValueString = words.at(1);
				uint16_t delayValue{0};
				bool isNumber = toUInt16(delayValueString, delayValue);
				if (isNumber)
				{
					if (delayValue > 0 && delayValue < 15000)
					{
						CommandEntry commandEntry = CommandEntry(new _CommandEntry);

						commandEntry->_commandAction = _CommandEntry::eDelay;
						commandEntry->_action = words.at(0);
						commandEntry->_arguement = static_cast<uint32_t>(delayValue);

						scriptCommand->_subCommands.push_back(commandEntry);
					}
					else
					{
						AppCore::writeToApplicationLogLine("Delay argument '" + words.at(1) + "' is invalid");

						result = false;
					}
				}
				else if (isScriptVariable(scriptVariables, delayValueString))
				{
					// If we encounter a variable, do not substitute it.
					// We will replace it right before sendCommandSequence() in a copy of commandEntries
					CommandEntry commandEntry = CommandEntry(new _CommandEntry);

					commandEntry->_commandAction = _CommandEntry::eDelay;
					commandEntry->_action = words.at(0);
					commandEntry->_arguement = delayValueString;

					scriptCommand->_subCommands.push_back(commandEntry);
				}
				else
				{
					AppCore::writeToApplicationLogLine("Delay argument '" + words.at(1) + "' is invalid");

					result = false;
				}
			}
			else
			{
				AppCore::writeToApplicationLogLine("Unhandled Command (skipped): " + words.at(0));
				// Do not set result = false here so that configurations with
				// optional pins (e.g. usb1 or sedl not present on this board) still function.
			}
		}
		else if (words.size() == 1)
		{
			if (scriptCommand != nullptr && !startsWith(words.at(0), "//"))
			{
				CommandEntry commandEntry = CommandEntry(new _CommandEntry);

				commandEntry->_commandAction = _CommandEntry::eBaseCommand;
				commandEntry->_action = words.at(0);

				scriptCommand->_subCommands.push_back(commandEntry);
			}
		}
	}

	return result;
}

CommandEntries AlpacaScript::replaceTokens(const ScriptVariables& scriptVariables, const CommandEntries& commandEntries)
{
	CommandEntries result;

	for (const auto &cmdEntry : commandEntries)
	{
		CommandEntry commandEntry = CommandEntry(new _CommandEntry);
		commandEntry->_commandAction = cmdEntry->_commandAction;
		commandEntry->_action = cmdEntry->_action;
		commandEntry->_arguement = cmdEntry->_arguement;
		commandEntry->_pinID = cmdEntry->_pinID;

		result.push_back(commandEntry);
	}

	for (auto &cmdEntry : result)
	{
		if (cmdEntry->_commandAction == _CommandEntry::eDelay)
		{
			if (std::holds_alternative<std::string>(cmdEntry->_arguement))
			{
				std::string variableString = std::get<std::string>(cmdEntry->_arguement);
				if (isScriptVariable(scriptVariables, variableString))
				{
					variableString.erase(std::remove(variableString.begin(), variableString.end(), '$'), variableString.end());
					auto variableIterator = scriptVariables.find(variableString);
					if (variableIterator != scriptVariables.end())
					{
						uint32_t delayMs = 0;
						const auto& val = variableIterator->second._defaultValue;
						if (std::holds_alternative<uint32_t>(val))
						{
							delayMs = std::get<uint32_t>(val);
						}
						else if (std::holds_alternative<std::string>(val))
						{
							toUInt32(std::get<std::string>(val), delayMs);
						}
						else if (std::holds_alternative<bool>(val))
						{
							delayMs = std::get<bool>(val) ? 1 : 0;
						}
						cmdEntry->_arguement = delayMs;
					}
				}
			}
		}
		else if (cmdEntry->_commandAction == _CommandEntry::eLog)
		{
			std::string commentString;
			if (std::holds_alternative<std::string>(cmdEntry->_arguement))
				commentString = std::get<std::string>(cmdEntry->_arguement);

			std::vector<std::string> commentTokens = splitWords(commentString);

			std::string formattedLogMessage;
			for (auto& cmdToken : commentTokens)
			{
				if (isScriptVariable(scriptVariables, cmdToken))
				{
					std::string variableName = cmdToken;
					variableName.erase(std::remove(variableName.begin(), variableName.end(), '$'), variableName.end());

					auto variableIterator = scriptVariables.find(variableName);
					if (variableIterator != scriptVariables.end())
					{
						std::string valStr;
						const auto& val = variableIterator->second._defaultValue;
						if (std::holds_alternative<uint32_t>(val))
							valStr = std::to_string(std::get<uint32_t>(val));
						else if (std::holds_alternative<std::string>(val))
							valStr = std::get<std::string>(val);
						else if (std::holds_alternative<bool>(val))
							valStr = std::get<bool>(val) ? "true" : "false";

						formattedLogMessage += valStr + " ";
					}
				}
				else
					formattedLogMessage += cmdToken + " ";
			}
			cmdEntry->_arguement = formattedLogMessage;
		}
	}

	return result;
}

std::vector<std::string> AlpacaScript::validateScript
(
	const std::string &alpacaScript,
	const TACCommands& tacCommands
)
{
	std::vector<std::string> result;

	std::istringstream alpacaScriptStream(alpacaScript);

	std::vector<std::string> scriptCommands;
	int lineNumber{0};

	std::string lineText;
	while (std::getline(alpacaScriptStream, lineText))
	{
		std::string line = trim(lineText);
		std::vector<std::string> words = splitWords(line);
		if (words.size() >= 2)
		{
			if (toLower(words.at(0)) == "def")
			{
				scriptCommands.push_back(createBaseCommandString(words.at(1)));
			}
		}
	}

	alpacaScriptStream.clear();
	alpacaScriptStream.seekg(0);

	while (std::getline(alpacaScriptStream, lineText))
	{
		lineNumber++;

		std::string line = trim(lineText);
		std::vector<std::string> words = splitWords(line);
		if (words.size() == 2)
		{
			std::string firstParameter{words.at(0)};
			std::string secondParameter{words.at(1)};

			if (firstParameter == "def")
			{
				// nop, already handle in the loop above
			}
			else if (TACCommand::contains(firstParameter, tacCommands) == false)
			{
				result.push_back("line number: " + std::to_string(lineNumber) + " Unrecognized command " + firstParameter);
			}
			else if (equalsIgnoreCase(words.at(0), "logcomment"))
			{
			}
			else if (words.at(0) == "delay")
			{
				uint16_t delayValue{0};
				bool isNumber = toUInt16(secondParameter, delayValue);
				if (isNumber == false || delayValue > 15000)
				{
					result.push_back("Line number: " + std::to_string(lineNumber) + " Delay argument " + secondParameter + " is invalid");
				}
			}
		}
		else
		{
			if (startsWith(words.at(0), "//") == false) // ignore comments
			{
				if (std::find(scriptCommands.begin(), scriptCommands.end(), words.at(0)) == scriptCommands.end())
				{
					result.push_back("Line number: " + std::to_string(lineNumber) + " Unhandled Command: " + words.at(0));
				}
			}
		}
	}

	return result;
}

bool AlpacaScript::hasCommand
(
	const std::string& command
)
{
	if (_scriptCommands.find(command) != _scriptCommands.end())
		return true;
	for (const auto& [cmd, sc] : _scriptCommands)
	{
		if (equalsIgnoreCase(cmd, command))
			return true;
	}
	return false;
}

std::vector<std::string> AlpacaScript::availableCommands()
{
	std::vector<std::string> result;

	for (const auto& entry: _scriptCommands)
		result.push_back(entry.first);

	std::sort(result.begin(), result.end());

	return result;
}

ScriptCommand AlpacaScript::getCommand(const std::string &command)
{
	auto scriptCommandIterator = _scriptCommands.find(command);
	if (scriptCommandIterator != _scriptCommands.end())
		return scriptCommandIterator->second;

	for (const auto& [cmd, sc] : _scriptCommands)
	{
		if (equalsIgnoreCase(cmd, command))
			return sc;
	}

	return ScriptCommand();
}

CommandEntries AlpacaScript::getCommandEntries(const std::string &command)
{
	CommandEntries result;

	auto scriptCommandIterator = _scriptCommands.find(command);
	if (scriptCommandIterator != _scriptCommands.end())
	{
		processCommandEntries(scriptCommandIterator->second, result, 1);
	}
	else
	{
		for (const auto& [cmd, sc] : _scriptCommands)
		{
			if (equalsIgnoreCase(cmd, command))
			{
				processCommandEntries(sc, result, 1);
				break;
			}
		}
	}

	return result;
}

bool AlpacaScript::isScriptVariable(const ScriptVariables &scriptVariables, const std::string &scriptVariable)
{
	bool result{false};

	if (scriptVariable.empty() == false && startsWith(scriptVariable, "$") == true)
	{
		size_t scriptVariableLen = scriptVariable.size();
		if (scriptVariableLen >= 2)
		{
			std::string token = scriptVariable.substr(1, scriptVariableLen - 1);

			if (scriptVariables.find(token) != scriptVariables.end())
				result = true;
			else
			{
				AppCore::writeToApplicationLogLine("Script variable '" + scriptVariable + "' is not defined. Check script variables table for definition");
			}
		}
	}
	return result;
}

std::string AlpacaScript::createBaseCommandString
(
	const std::string& baseCommandString
)
{
	std::string result{baseCommandString};

	result.erase(std::remove(result.begin(), result.end(), '('), result.end());
	result.erase(std::remove(result.begin(), result.end(), ')'), result.end());
	result = trim(result);

	return result;
}

bool AlpacaScript::processCommandEntries
(
	ScriptCommand scriptCommand,
	CommandEntries &commandEntries,
	int level
)
{
	bool result{true};

	if (scriptCommand == nullptr)
		return false;

	for (const auto& commandEntry: scriptCommand->_subCommands)
	{
		if (commandEntry == nullptr)
			continue;

		switch (commandEntry->_commandAction)
		{
			case _CommandEntry::eNotSet:
				break;

			case _CommandEntry::eSetPin:
			case _CommandEntry::eLog:
			case _CommandEntry::eDelay:
				commandEntries.push_back(commandEntry);
				break;
			case _CommandEntry::eBaseCommand:
			{
				if (level < 7)
				{
					ScriptCommand childScript = getCommand(commandEntry->_action);
					if (childScript != nullptr)
					{
						processCommandEntries(childScript, commandEntries, level + 1);
					}
					else
					{
						AppCore::writeToApplicationLogLine("Command " + commandEntry->_action + " not found in Alpaca script");
					}
				}
				else
				{
					AppCore::writeToApplicationLogLine("Recursive Level == 0");

					result = false;
				}
				break;
			}
		}
	}

	return result;
}
