#ifndef TABS_H
#define TABS_H
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
