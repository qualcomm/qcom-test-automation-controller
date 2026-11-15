// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QAppPaths.h"
#include "QCommonTypes.h"

// QCommonConsole
#include "AppCore.h"

// Forward declaration from qcommon-console
std::string documentsDataPath(const std::string& append = "");

namespace QTAC
{
namespace Paths
{
	QString applicationBinPath()
	{
		return toQString(AppCore::applicationBinPath());
	}

	QString applicationDataPath()
	{
		return toQString(AppCore::applicationDataPath());
	}

	QString tacConfigRoot()
	{
		return toQString(AppCore::configurationsPath());
	}

	QString configurationsPath()
	{
		return toQString(AppCore::configurationsPath());
	}

	QString userConfigurationsPath()
	{
		return toQString(AppCore::userConfigurationsPath());
	}

	QString configurationSavePath(const QString& currentPath)
	{
		return toQString(AppCore::configurationSavePath(toStdString(currentPath)));
	}

	QString documentsDataPath(const QString& append)
	{
		return toQString(::documentsDataPath(toStdString(append)));
	}
}

namespace Logging
{
	void writeToApplicationLog(const QString& line)
	{
		AppCore::writeToApplicationLog(toStdString(line));
	}

	void writeToApplicationLogLine(const QString& line)
	{
		AppCore::writeToApplicationLogLine(toStdString(line));
	}

	void writeToRuntimeLog(const QString& line)
	{
		AppCore::writeToRuntimeLog(toStdString(line));
	}
}
}
