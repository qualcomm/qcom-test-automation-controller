/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "AlpacaScript.h"

// QCommonConsole
#include "AppCore.h"
#include "ConsoleApplicationEnhancements.h"
#include "ScriptVariable.h"

// Qt
#include <QSharedPointer>
#include <QTextStream>

// c++
#include <utility>

// QTAC Script
AlpacaScript::AlpacaScript()
{

}

QByteArray AlpacaScript::defaultScript(DebugBoardType boardType)
{
	QByteArray result;
	QString scriptPath;

	Q_UNUSED(boardType);

// TODO: Handle everything on tacConfigRoot()
#ifdef DEBUG
#ifdef Q_OS_WINDOWS
	scriptPath = "C:\\github\\open-source\\qcom-test-automation-controller\\configurations\\DefaultScript.txt";
#endif
#ifdef Q_OS_LINUX
	scriptPath = "/local/mnt/workspace/github/AlpacaRepos/AlpacaConfigs/tac_configs/DefaultScript.txt";
#endif
#else
	scriptPath = tacConfigRoot() + "DefaultScript.txt";
#endif

	QFile file(scriptPath);
	if (file.open(QIODevice::ReadOnly) == true)
	{
		result = file.readAll();
		file.close();
	}

	return result;
}

bool AlpacaScript::parseScript(const QByteArray& alpacaScript, const ScriptVariables& scriptVariables, const TacCommands& tacCommands)
{
	bool result{true};

	QTextStream alpacaScriptStream(alpacaScript);

	ScriptCommand scriptCommand;

	while (alpacaScriptStream.atEnd() == false)
	{
		QString line = alpacaScriptStream.readLine().trimmed();
		QStringList words = line.split(' ', Qt::SkipEmptyParts);
		if (words.count() >= 2)
		{
			if (words.at(0).toLower() == "def")
			{
				scriptCommand = ScriptCommand(new _ScriptCommand);

				scriptCommand->_command = createBaseCommandString(words.at(1)).toLatin1();

				_scriptCommands[scriptCommand->_command] = scriptCommand;
			}
			else if (TacCommand::contains(words.at(0), tacCommands))
			{
				QString command{words.at(0)};

				TacCommand tacCommand = TacCommand::find(command, tacCommands);

				CommandEntry commandEntry = CommandEntry(new _CommandEntry);

				commandEntry->_commandAction = _CommandEntry::eSetPin;
				commandEntry->_pinID = tacCommand._pin;
				commandEntry->_action = command.toLatin1();
				commandEntry->_arguement = (words.at(1) == "1") ? true : false;

				scriptCommand->_subCommands.push_back(commandEntry);
			}
			else if (words.at(0).compare("logcomment", Qt::CaseInsensitive) == 0)
			{
				CommandEntry commandEntry = CommandEntry(new _CommandEntry);

				commandEntry->_commandAction = _CommandEntry::eLog;
				commandEntry->_action = words.at(0).toLatin1();
				commandEntry->_arguement = line.replace("logcomment ", "", Qt::CaseInsensitive).toLatin1();

				scriptCommand->_subCommands.push_back(commandEntry);

			}
			else if (words.at(0) == "delay")
			{
				bool isNumber{false};

				QString delayValueString = words.at(1);
				ushort delayValue =  delayValueString.toLatin1().toUShort(&isNumber);
				if (isNumber)
				{
					if (delayValue > 0 && delayValue < 15000)
					{
						CommandEntry commandEntry = CommandEntry(new _CommandEntry);

						commandEntry->_commandAction = _CommandEntry::eDelay;
						commandEntry->_action = words.at(0).toLatin1();
						commandEntry->_arguement = delayValue;

						scriptCommand->_subCommands.push_back(commandEntry);
					}
					else
					{
						QString message;

						message = QString("Delay argument '%1' is invalid").arg(words.at(1));
						AppCore::writeToApplicationLogLine(message);

						result = false;
					}
				}
				else if (isScriptVariable(scriptVariables, delayValueString))
				{
					// If we encounter a variable, do not substitute it.
					// We will replace it right before sendCommandSequence() in a copy of commandEntries
					CommandEntry commandEntry = CommandEntry(new _CommandEntry);

					commandEntry->_commandAction = _CommandEntry::eDelay;
					commandEntry->_action = words.at(0).toLatin1();
					commandEntry->_arguement = delayValueString;

					scriptCommand->_subCommands.push_back(commandEntry);
				}
				else
				{
					QString message;

					message = QString("Delay argument '%1' is invalid").arg(words.at(1));
					AppCore::writeToApplicationLogLine(message);

					result = false;
				}
			}
			else
			{
				if (words.at(0).startsWith("//") == false) // ignore comments
				{
					QString message;

					message = QString("Unhandled Command: %1").arg(words.at(0));
					AppCore::writeToApplicationLogLine(message);

					result = false;
				}
			}
		}
		else if (words.count() == 1)
		{
			if (scriptCommand.isNull() == false)
			{
				CommandEntry commandEntry = CommandEntry(new _CommandEntry);

				commandEntry->_commandAction = _CommandEntry::eBaseCommand;
				commandEntry->_action = words.at(0).toLatin1();

				scriptCommand->_subCommands.push_back(commandEntry);
			}
		}
	}

	return result;
}

CommandEntries AlpacaScript::replaceTokens(const ScriptVariables scriptVariables, const CommandEntries commandEntries)
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
			QString variableString = cmdEntry->_arguement.toString();
			if (isScriptVariable(scriptVariables, variableString))
			{
				variableString = variableString.replace("$", "");
				if (scriptVariables.contains(variableString))
				{
					cmdEntry->_arguement = scriptVariables[variableString]._defaultValue;
				}
			}
		}
		else if (cmdEntry->_commandAction == _CommandEntry::eLog)
		{
			QString formattedLogMessage{""};
			QString commentString = cmdEntry->_arguement.toString();
			QStringList commentTokens = commentString.split(" ", Qt::SkipEmptyParts);

			for (auto& cmdToken : commentTokens)
			{
				if (isScriptVariable(scriptVariables, cmdToken))
				{
					commentString = cmdToken.replace("$", "");
					QString variableValue = QString::number(scriptVariables[cmdToken]._defaultValue.toUInt());
					formattedLogMessage.push_back(QString("%1 ").arg(variableValue));
				}
				else
					formattedLogMessage.push_back(QString("%1 ").arg(cmdToken));
			}
			cmdEntry->_arguement = formattedLogMessage;
		}
	}

	return result;
}

QStringList AlpacaScript::validateScript
(
	const QByteArray &alpacaScript,
	const TacCommands& tacCommands
)
{
	QStringList result;

	QTextStream alpacaScriptStream(alpacaScript);

	QStringList scriptCommands;
	int lineNumber{0};

	while (alpacaScriptStream.atEnd() == false)
	{
		QString line = alpacaScriptStream.readLine().trimmed();
		QStringList words = line.split(' ', Qt::SkipEmptyParts);
		if (words.count() >= 2)
		{
			if (words.at(0).toLower() == "def")
			{
				scriptCommands.push_back(createBaseCommandString(words.at(1)));
			}
		}
	}

	alpacaScriptStream.reset();


	while (alpacaScriptStream.atEnd() == false)
	{
		lineNumber++;

		QString line = alpacaScriptStream.readLine().trimmed();
		QStringList words = line.split(' ', Qt::SkipEmptyParts);
		if (words.count() == 2)
		{
			QString firstParameter{words.at(0)};
			QString secondParameter{words.at(1)};

			if (firstParameter == "def")
			{
				// nop, already handle in the loop above
			}
			else if (TacCommand::contains(firstParameter, tacCommands) == false)
			{
				QString message;

				message = QString("line number: %1 Unrecognized command %2").arg(lineNumber).arg(firstParameter);

				result.push_back(message);
			}
			else if (words.at(0).compare("logcomment", Qt::CaseInsensitive) == 0)
			{
			}
			else if (words.at(0) == "delay")
			{
				bool isNumber{false};

				ushort delayValue =  secondParameter.toLatin1().toUShort(&isNumber);
				if (isNumber == false || delayValue > 15000)
				{
					QString message;

					message = QString("Line number: %1 Delay argument %2 is invalid").arg(lineNumber).arg(secondParameter);

					result.push_back(message);
				}
			}
		}
		else
		{
			if (words.at(0).startsWith("//") == false) // ignore comments
			{
				if (scriptCommands.contains(words.at(0)) == false)
				{
					QString message;

					message = QString("Line number: %1 Unhandled Command: %2").arg(lineNumber).arg(words.at(0));

					result.push_back(message);
				}
			}
		}
	}

	return result;
}

bool AlpacaScript::hasCommand
(
	const QByteArray& command
)
{
	bool result{false};

	for (const auto& scriptCommand: std::as_const(_scriptCommands))
	{
		if (scriptCommand->_command == command)
		{
			result = true;
			break;
		}
	}

	return result;
}

QStringList AlpacaScript::availableCommands()
{
	QStringList result;

	for (const auto& scriptCommand: std::as_const(_scriptCommands))
		result.push_back(scriptCommand->_command);

	auto sortLambda = [] (const QString& s1, const QString& s2) -> bool
	{
		return s1 < s2;
	};


	std::stable_sort(result.begin(), result.end(), sortLambda);

	return result;
}

ScriptCommand AlpacaScript::getCommand(const QByteArray &command)
{
	ScriptCommand result;

	for (const auto& scriptCommand: std::as_const(_scriptCommands))
	{
		if (scriptCommand->_command == command)
		{
			result = scriptCommand;
			break;
		}
	}

	return result;
}

CommandEntries AlpacaScript::getCommandEntries(const QByteArray &command)
{
	CommandEntries result;

	if (_scriptCommands.find(command) != _scriptCommands.end())
	{
		ScriptCommand scriptCommand = _scriptCommands[command];

		processCommandEntries(scriptCommand, result, 1);
	}

	return result;
}

bool AlpacaScript::isScriptVariable(const ScriptVariables &scriptVariables, const QString &scriptVariable)
{
	bool result{false};

	if (scriptVariable.isEmpty() == false && scriptVariable.startsWith("$") == true)
	{
		int scriptVariableLen = scriptVariable.size();
		if (scriptVariableLen >= 2)
		{
			QString token = scriptVariable.sliced(1, scriptVariableLen-1);

			if (scriptVariables.contains(token))
				result = true;
			else
			{
				QString message = QString("Script variable '%1' is not defined. Check script variables table for definition").arg(scriptVariable);
				AppCore::writeToApplicationLogLine(message);
			}
		}
	}
	return result;
}

QString AlpacaScript::createBaseCommandString
(
	const QString& baseCommandString
)
{
	QString result{baseCommandString};

	result = result.replace("(","");
	result = result.replace(")","");
	result = result.trimmed();

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

	for (const auto& commandEntry: std::as_const(scriptCommand->_subCommands))
	{
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
					processCommandEntries(childScript, commandEntries, level + 1);
				}
				else
				{
					QString message;

					message = QString("Recursive Level == 0").arg(commandEntry->_action.data());
					AppCore::writeToApplicationLogLine(message);

					result = false;
				}
				break;
			}
		}
	}

	return result;
}
