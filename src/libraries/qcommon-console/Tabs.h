#ifndef TABS_H
#define TABS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

// QCommon
#include "StringUtilities.h"

#include <string>
#include <vector>

struct Tab;

typedef std::vector<Tab> Tabs;

struct QCOMMONCONSOLE_EXPORT Tab
{
	Tab() = default;
	Tab(const Tab& copyMe) = default;
	~Tab() = default;

	static HashType makeHash(const Tab& tab);

	static Tab find(HashType hash, Tabs& tabs);
	static void addTab(Tab& tab, Tabs& tabs);
	static void removeTab(HashType hash, Tabs& tabs);
	static void updateTabName(HashType hash, std::string& name, Tabs& tabs);
	static void setTabVisible(HashType hash, bool visible, Tabs& tabs);

	static std::vector<std::string> toStringList(Tabs& tabs, bool configurableTabs = true);

	std::string					_name;
	bool						_userTab{false};
	bool						_moveable{false};
	bool						_configurable{false};
	bool						_visible{true};
	int							_ordinal{0};

	HashType					_hash{0};
	bool						_deleted{false};
	bool						_added{false};
	std::string					_newText;
};

#endif // TABS_H
