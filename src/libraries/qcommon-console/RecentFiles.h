#ifndef RECENTFILES_H
#define RECENTFILES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

// QT
#include <QList>
#include <QString>

class QCOMMONCONSOLE_EXPORT RecentFiles
{
public:
	RecentFiles(const QString& appName, const QString& settingsGroup);

	void clear();
	void hideExtensions(bool state = false);

	int fileCount();
	void setMaxFileCount(int maxFiles);

	void addFile(const QString& filePath);
	void removeFile(const QString& filePath);
	void removeFile(int fileIndex);
	QString getFilePath(int fileIndex);
	QString getFileName(int fileIndex);

private:
	void load();
	void save();

	QString						_appName;
	bool						_hideExtensions{false};
	int							_maxFileCount{10};
	QString						_settingsGroup;
	QStringList					_recentFiles;
};

#endif // RECENTFILES_H
