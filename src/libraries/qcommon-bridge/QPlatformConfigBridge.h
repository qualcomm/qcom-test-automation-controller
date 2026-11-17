#ifndef QPLATFORMCONFIGBRIDGE_H
#define QPLATFORMCONFIGBRIDGE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonBridgeGlobal.h"
#include "QCommonTypes.h"

// QCommonConsole
#include "Button.h"
#include "PlatformConfiguration.h"
#include "PlatformConfigurationException.h"

// Qt
#include <QPoint>
#include <QString>

namespace QPlatformConfigBridge
{
	inline QString getExceptionMessage(const PlatformConfigurationException& e)
	{
		return toQString(e.getMessage());
	}

	inline QString buttonLabel(const Button& b)
	{
		return toQString(b._label);
	}

	inline QString buttonTooltip(const Button& b)
	{
		return toQString(b._toolTip);
	}

	inline QString buttonCommand(const Button& b)
	{
		return toQString(b._command);
	}

	inline QString buttonTab(const Button& b)
	{
		return toQString(b._tab);
	}

	inline QPoint buttonCellLocation(const Button& b)
	{
		return toQPoint(b._cellLocation);
	}

	inline bool setButtonLabel(PlatformConfiguration& cfg, HashType hash, const QString& label)
	{
		return cfg ? cfg->setButtonLabel(hash, toStdString(label)) : false;
	}

	inline HashType addButtonLabel(PlatformConfiguration& cfg, const QString& label, const QString& tab)
	{
		return cfg ? cfg->addButtonLabel(toStdString(label), toStdString(tab)) : 0;
	}

	inline bool setButtonTooltip(PlatformConfiguration& cfg, HashType hash, const QString& tip)
	{
		return cfg ? cfg->setButtonTooltip(hash, toStdString(tip)) : false;
	}

	inline HashType addButtonTooltip(PlatformConfiguration& cfg, const QString& label, const QString& tab, const QString& tip)
	{
		return cfg ? cfg->addButtonTooltip(toStdString(label), toStdString(tab), toStdString(tip)) : 0;
	}

	inline bool setButtonCommand(PlatformConfiguration& cfg, HashType hash, const QString& cmd)
	{
		return cfg ? cfg->setButtonCommand(hash, toStdString(cmd)) : false;
	}

	inline HashType addButtonCommand(PlatformConfiguration& cfg, const QString& label, const QString& tab, const QString& cmd)
	{
		return cfg ? cfg->addButtonCommand(toStdString(label), toStdString(tab), toStdString(cmd)) : 0;
	}

	inline bool setButtonTab(PlatformConfiguration& cfg, HashType hash, const QString& tab)
	{
		return cfg ? cfg->setButtonTab(hash, toStdString(tab)) : false;
	}

	inline HashType addButtonTab(PlatformConfiguration& cfg, const QString& label, const QString& tab)
	{
		return cfg ? cfg->addButtonTab(toStdString(label), toStdString(tab)) : 0;
	}

	inline bool setButtonCellLocation(PlatformConfiguration& cfg, HashType hash, const QPoint& loc)
	{
		return cfg ? cfg->setButtonCellLocation(hash, toStdPair(loc)) : false;
	}

	inline HashType addButtonCellLocation(PlatformConfiguration& cfg, const QString& label, const QString& tab, const QPoint& loc)
	{
		return cfg ? cfg->addButtonCellLocation(toStdString(label), toStdString(tab), toStdPair(loc)) : 0;
	}

	inline bool setButtonCommandGroup(PlatformConfiguration& cfg, HashType hash, CommandGroups grp)
	{
		return cfg ? cfg->setButtonCommandGroup(hash, grp) : false;
	}

	inline HashType addButtonCommandGroup(PlatformConfiguration& cfg, const QString& label, const QString& tab, CommandGroups grp)
	{
		return cfg ? cfg->addButtonCommandGroup(toStdString(label), toStdString(tab), grp) : 0;
	}
}

#endif // QPLATFORMCONFIGBRIDGE_H
