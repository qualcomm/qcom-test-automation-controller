#ifndef COMMANDGROUP_H
#define COMMANDGROUP_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include <cctype>
#include <string>
#include <vector>

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
	static std::string toString(CommandGroups commandGroup)
	{
		switch (commandGroup)
		{
		case eConnectionGroup: return "Connections";
		case eButtonGroup: return "Buttons";
		case eSwitchGroup: return "Switches";
		case eQuickSettingsGroup: return "Quick Settings";
		default:;
		}

		return std::string();
	}

	static std::vector<std::string> toStringList()
	{
		std::vector<std::string> result;

		result.push_back("<Select Pin Group>");
		result.push_back(toString(eConnectionGroup));
		result.push_back(toString(eButtonGroup));
		result.push_back(toString(eSwitchGroup));
		result.push_back(toString(eQuickSettingsGroup));

		return result;
	}

	static CommandGroups fromString(const std::string commandGroupStr)
	{
		std::string temp{commandGroupStr};
		for (char& c: temp)
			c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

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
