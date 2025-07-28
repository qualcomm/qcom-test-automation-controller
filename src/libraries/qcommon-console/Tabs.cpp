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
	Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "Tabs.h"


HashType Tab::makeHash(const Tab& tab)
{
	HashType hash{0};

	hash = strHash(tab._name + fromBool(tab._userTab) + fromBool(tab._moveable) + fromBool(tab._configurable));

	return hash;
}

Tab Tab::find(HashType hash, Tabs &tabs)
{
	for (const auto& tab: tabs)
	{
		if (tab._hash == hash)
		{
			return tab;
		}
	}

	return Tab();
}

void Tab::addTab(Tab& tab, Tabs& tabs)
{
	Q_UNUSED(tabs)

	// Todo, why is this commented out?
	tab._hash = Tab::makeHash(tab);
//	tabs.append(tab);
}

void Tab::removeTab(HashType hash, Tabs& tabs)
{
	for (const auto &tab: tabs)
	{
		if (tab._hash == hash)
		{
//			tabs.removeOne(tab);
			break;
		}
	}
}

void Tab::updateTabName(HashType hash, QString &name, Tabs& tabs)
{
	for (auto &tab: tabs)
	{
		if (tab._hash == hash)
		{
			tab._name = name;
			break;
		}
	}
}

void Tab::setTabVisible(HashType hash, bool visible, Tabs &tabs)
{
	for (auto &tab: tabs)
	{
		if (tab._hash == hash)
		{
			tab._visible = visible;
			break;
		}
	}
}

QStringList Tab::toStringList(Tabs &tabs, bool configurableTabs)
{
	QStringList result;

	if (configurableTabs)
		result.push_back("<select a group>");

	for (const auto& tab: tabs)
	{
		if (configurableTabs)
		{
			if (tab._configurable)
				result.push_back(tab._newText.isEmpty() ? tab._name : tab._newText);
		}
		else
		{
			result.push_back(tab._newText.isEmpty() ? tab._name : tab._newText);
		}
	}
	return result;
}


