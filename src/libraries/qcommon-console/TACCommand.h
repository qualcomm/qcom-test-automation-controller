#ifndef TACCOMMAND_H
#define TACCOMMAND_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include "PinID.h"

// Qt
#include <QList>

class TACCommand;

typedef QList<TACCommand> TACCommands;
typedef QMap<QString, TACCommand> TACCommandMap;

class QCOMMONCONSOLE_EXPORT TACCommand
{
public:
	TACCommand() = default;
	TACCommand(const TACCommand&) = default;
	TACCommand(TACCommand&&) = default;

	~TACCommand() = default;

	TACCommand& operator=(const TACCommand&) = default;
	TACCommand& operator=(TACCommand&&) = default;

	QString command();

	static bool contains(const QString& command, const TACCommands& tacCommands);
	static TACCommand find(const QString& command, const TACCommands& tacCommands);
	static TACCommand find(PinID pin, const TACCommands& tacCommands);

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
