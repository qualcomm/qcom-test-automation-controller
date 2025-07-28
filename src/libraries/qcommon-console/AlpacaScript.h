#ifndef ALPACASCRIPT_H
#define ALPACASCRIPT_H
// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
