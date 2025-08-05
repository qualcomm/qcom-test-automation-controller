#ifndef COMMANDGROUP_H
#define COMMANDGROUP_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

#include <QString>
#include <QStringList>

enum CommandGroups
{
	eUnknownCommandGroup = 0,
	eConnectionGroup,
	eButtonGroup,
	eSwitchGroup,
	eQuickSettingsGroup
};

class QCOMMONCONSOLE_EXPORT CommandGroup
{
public:
	static QString toString(CommandGroups commandGroup)
	{
		switch (commandGroup)
		{
		case eConnectionGroup: return "Connections";
		case eButtonGroup: return "Buttons";
		case eSwitchGroup: return "Switches";
		case eQuickSettingsGroup: return "Quick Settings";
		default:;
		}

		return QString();
	}

	static QStringList toStringList()
	{
		QStringList result;

		result.append("<Select Pin Group>");
		result.append(toString(eConnectionGroup));
		result.append(toString(eButtonGroup));
		result.append(toString(eSwitchGroup));
		result.append(toString(eQuickSettingsGroup));

		return result;
	}

	static CommandGroups fromString(const QString commandGroupStr)
	{
		QString temp{commandGroupStr.toLower()};

		if (temp == "connections")
			return eConnectionGroup;

		if (temp == "buttons")
			return eButtonGroup;

		if (temp == "switches")
			return eSwitchGroup;

		if (temp == "quick settings")
			return eQuickSettingsGroup;

		return eUnknownCommandGroup;
	}
};

#endif // COMMANDGROUP_H
