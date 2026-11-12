#ifndef RECENTFILES_H
#define RECENTFILES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include <string>
#include <vector>

class QCOMMONCONSOLE_EXPORT RecentFiles
{
public:
	RecentFiles(const std::string& appName, const std::string& settingsGroup);

	void clear();
	void hideExtensions(bool state = false);

	int fileCount();
	void setMaxFileCount(int maxFiles);

	void addFile(const std::string& filePath);
	void removeFile(const std::string& filePath);
	void removeFile(int fileIndex);
	std::string getFilePath(int fileIndex);
	std::string getFileName(int fileIndex);

private:
	void load();
	void save();

	std::string					_appName;
	bool						_hideExtensions{false};
	int							_maxFileCount{10};
	std::string					_settingsGroup;
	std::vector<std::string>	_recentFiles;
};

#endif // RECENTFILES_H
