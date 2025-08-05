#ifndef ALPACASCRIPT_H
#define ALPACASCRIPT_H
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

// QCommon
#include "QCommonConsoleGlobal.h"
#include "DebugBoardType.h"
#include "PinID.h"
#include "ScriptVariable.h"
#include "TACCommand.h"

// Qt
#include <QByteArray>
#include <QList>
#include <QSharedPointer>
#include <QVariant>

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
	QByteArray					_action;
	QVariant					_arguement;
};

typedef QSharedPointer<_CommandEntry> CommandEntry;
typedef QList<CommandEntry> CommandEntries;

struct _ScriptCommand
{
	bool isEmpty()
	{
		if (_command.isEmpty())
			return true;

		if (_subCommands.count() == 0)
			return true;

		return false;
	}

	void clear()
	{
		_command.clear();
		_subCommands.clear();
	}

	QByteArray					_command;
	CommandEntries				_subCommands;
};

typedef QSharedPointer<_ScriptCommand> ScriptCommand;
typedef QMap<QString, ScriptCommand> ScriptCommands;

class QCOMMONCONSOLE_EXPORT AlpacaScript
{
public:
	AlpacaScript();

	quint32 count()
	{
		return _scriptCommands.count();
	}

	static QByteArray defaultScript(DebugBoardType boardType = eFTDI);

	bool parseScript(const QByteArray& alpacaScript, const ScriptVariables& scriptVariables, const TacCommands& tacCommands);
	CommandEntries replaceTokens(const ScriptVariables scriptVariables, const CommandEntries commandEntries);
	QStringList validateScript(const QByteArray& alpacaScript, const TacCommands& tacCommands);

	bool hasCommand(const QByteArray& command);
	QStringList availableCommands();

	ScriptCommand getCommand(const QByteArray& command);
	CommandEntries getCommandEntries(const QByteArray& command);

	void setVariableValue(const QString& variableName, int variableValue);
	void setVariableValue(const QString& variableName, float variableValue);

private:
	bool isScriptVariable(const ScriptVariables& scriptVariables, const QString &scriptVariable);
	QString createBaseCommandString(const QString& baseCommandString);
	bool processCommandEntries(ScriptCommand scriptCommand, CommandEntries& commandEntries, int level = 0);

	ScriptCommands				_scriptCommands;
};

#endif // ALPACASCRIPT_H
