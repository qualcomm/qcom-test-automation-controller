#ifndef TABS_H
#define TABS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
			Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

// QCommon
#include "StringUtilities.h"

// Qt
#include <QList>
#include <QString>

struct Tab;

typedef QList<Tab> Tabs;

struct QCOMMONCONSOLE_EXPORT Tab
{
	Tab() = default;
	Tab(const Tab& copyMe) = default;
	~Tab() = default;

	static HashType makeHash(const Tab& tab);

	static Tab find(HashType hash, Tabs& tabs);
	static void addTab(Tab& tab, Tabs& tabs);
	static void removeTab(HashType hash, Tabs& tabs);
	static void updateTabName(HashType hash, QString& name, Tabs& tabs);
	static void setTabVisible(HashType hash, bool visible, Tabs& tabs);

	static QStringList toStringList(Tabs& tabs, bool configurableTabs = true);

	QString						_name;
	bool						_userTab{false};
	bool						_moveable{false};
	bool						_configurable{false};
	bool						_visible{true};
	int							_ordinal{0};

	// this for editing data
	HashType					_hash{0};
	bool						_deleted{false};
	bool						_added{false};
	QString						_newText;
};

#endif // TABS_H
