#ifndef COMMANDGROUP_H
#define COMMANDGROUP_H
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
// Copyright 2022 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
