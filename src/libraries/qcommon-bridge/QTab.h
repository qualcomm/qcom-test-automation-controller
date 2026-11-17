#ifndef QTAB_H
#define QTAB_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonBridgeGlobal.h"
#include "QCommonTypes.h"
#include "Tabs.h"

// Qt
#include <QString>
#include <QStringList>

namespace QTabs
{
	inline QStringList toStringList(const Tabs& tabs, bool configurableTabs = true)
	{
		return toQStringList(Tab::toStringList(tabs, configurableTabs));
	}

	inline QStringList toStringList(const QStringList& tabs)
	{
		return tabs;
	}
}

#endif // QTAB_H
