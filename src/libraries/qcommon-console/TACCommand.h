#ifndef TACCOMMAND_H
#define TACCOMMAND_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

#include "PinID.h"

// Qt
#include <QList>

class TacCommand;

typedef QList<TacCommand> TacCommands;
typedef QMap<QString, TacCommand> TacCommandMap;

class QCOMMONCONSOLE_EXPORT TacCommand
{
public:
	TacCommand() = default;
	TacCommand(const TacCommand&) = default;
	TacCommand(TacCommand&&) = default;

	~TacCommand() = default;

	TacCommand& operator=(const TacCommand&) = default;
	TacCommand& operator=(TacCommand&&) = default;

	QString command();

	static bool contains(const QString& command, const TacCommands& tacCommands);
	static TacCommand find(const QString& command, const TacCommands& tacCommands);
	static TacCommand find(PinID pin, const TacCommands& tacCommands);

	PinID						_pin{static_cast<PinID>(-1)};
	QString						_command;
	QString						_helpText;
	bool						_currentState{false};
	bool						_isInverted{false};
	QByteArray					_tabName;
	QByteArray					_groupName;
	QByteArray					_cellLocation;
};

#endif // TACCOMMAND_H
