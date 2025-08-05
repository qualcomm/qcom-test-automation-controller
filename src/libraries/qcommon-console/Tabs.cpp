// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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


