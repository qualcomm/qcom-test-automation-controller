#ifndef BUTTON_H
#define BUTTON_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

// QCommon
#include "CommandGroup.h"
#include "StringUtilities.h"

// Qt
#include <QList>
#include <QMap>
#include <QString>

struct Button;

typedef QList<Button> ButtonList;
typedef QMap<HashType, Button> Buttons;

struct QCOMMONCONSOLE_EXPORT Button
{
public:
	Button() = default;
	~Button() = default;
	Button(const Button& copyMe) = default;

	static HashType makeHash(const Button& button);

	HashType					_hash;
	QString						_label;
	QString						_toolTip;
	QString						_command;
	CommandGroups				_commandGroup{eUnknownCommandGroup};
	QString						_tab;
	QPoint						_cellLocation{QPoint(-1,-1)};
};

#endif // BUTTON_H
