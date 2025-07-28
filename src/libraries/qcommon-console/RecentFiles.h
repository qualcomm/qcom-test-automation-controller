#ifndef RECENTFILES_H
#define RECENTFILES_H
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
// Copyright 2019 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
