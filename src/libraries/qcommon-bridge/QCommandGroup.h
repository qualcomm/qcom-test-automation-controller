#ifndef QCOMMANDGROUP_H
#define QCOMMANDGROUP_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonBridgeGlobal.h"
#include "QCommonTypes.h"
#include "CommandGroup.h"

// Qt
#include <QString>
#include <QStringList>

namespace QCommandGroup
{
	inline QString toString(CommandGroups commandGroup)
	{
		return toQString(CommandGroup::toString(commandGroup));
	}

	inline QStringList toStringList()
	{
		return toQStringList(CommandGroup::toStringList());
	}

	inline CommandGroups fromString(const QString& commandGroupStr)
	{
		return CommandGroup::fromString(toStdString(commandGroupStr));
	}
}

#endif // QCOMMANDGROUP_H
