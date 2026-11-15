#ifndef QAPPPATHS_H
#define QAPPPATHS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonBridgeGlobal.h"

// Qt
#include <QString>

namespace QTAC
{
namespace Paths
{
	QCOMMONBRIDGE_EXPORT QString applicationBinPath();
	QCOMMONBRIDGE_EXPORT QString applicationDataPath();
	QCOMMONBRIDGE_EXPORT QString tacConfigRoot();
	QCOMMONBRIDGE_EXPORT QString configurationsPath();
	QCOMMONBRIDGE_EXPORT QString userConfigurationsPath();
	QCOMMONBRIDGE_EXPORT QString configurationSavePath(const QString& currentPath = QString());
	QCOMMONBRIDGE_EXPORT QString documentsDataPath(const QString& append = QString());
}

namespace Logging
{
	QCOMMONBRIDGE_EXPORT void writeToApplicationLog(const QString& line);
	QCOMMONBRIDGE_EXPORT void writeToApplicationLogLine(const QString& line);
	QCOMMONBRIDGE_EXPORT void writeToRuntimeLog(const QString& line);
}
}

using QTAC::Paths::applicationBinPath;
using QTAC::Paths::applicationDataPath;
using QTAC::Paths::tacConfigRoot;
using QTAC::Paths::configurationsPath;
using QTAC::Paths::userConfigurationsPath;
using QTAC::Paths::configurationSavePath;
using QTAC::Paths::documentsDataPath;
using QTAC::Logging::writeToApplicationLog;
using QTAC::Logging::writeToApplicationLogLine;
using QTAC::Logging::writeToRuntimeLog;

#endif // QAPPPATHS_H
