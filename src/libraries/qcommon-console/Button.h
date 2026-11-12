#ifndef BUTTON_H
#define BUTTON_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

// QCommon
#include "CommandGroup.h"
#include "StringUtilities.h"

#include <map>
#include <string>
#include <utility>
#include <vector>

struct Button;

typedef std::vector<Button> ButtonList;
typedef std::map<HashType, Button> Buttons;

struct QCOMMONCONSOLE_EXPORT Button
{
public:
	Button() = default;
	~Button() = default;
	Button(const Button& copyMe) = default;

	static HashType makeHash(const Button& button);

	HashType					_hash;
	std::string					_label;
	std::string					_toolTip;
	std::string					_command;
	CommandGroups				_commandGroup{eUnknownCommandGroup};
	std::string					_tab;
	std::pair<int, int>			_cellLocation{-1, -1};
};

#endif // BUTTON_H
