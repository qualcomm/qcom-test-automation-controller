#ifndef QRECENTFILES_H
#define QRECENTFILES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonBridgeGlobal.h"
#include "RecentFiles.h"

// Qt
#include <QString>

class QCOMMONBRIDGE_EXPORT QRecentFiles : public RecentFiles
{
public:
	QRecentFiles(const QString& appName, const QString& settingsGroup) :
		RecentFiles(appName.toStdString(), settingsGroup.toStdString())
	{
	}

	QString filePath(int fileIndex)
	{
		return QString::fromStdString(getFilePath(fileIndex));
	}

	QString fileName(int fileIndex)
	{
		return QString::fromStdString(getFileName(fileIndex));
	}

	using RecentFiles::addFile;
	void addFile(const QString& path)
	{
		RecentFiles::addFile(path.toStdString());
	}

	using RecentFiles::removeFile;
	void removeFile(const QString& path)
	{
		RecentFiles::removeFile(path.toStdString());
	}
};

#endif // QRECENTFILES_H
