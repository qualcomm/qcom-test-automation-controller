// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "RecentFiles.h"

// QCommon
#include "AlpacaSettings.h"
#include "Range.h"

// QT
#include <QFileInfo>
#include <QSettings>

static int indexOfCaseInsensitive(const std::vector<std::string>& files, const std::string& filePath)
{
	QString targetFilePath = QString::fromStdString(filePath);

	for (size_t index = 0; index < files.size(); index++)
	{
		if (QString::fromStdString(files[index]).compare(targetFilePath, Qt::CaseInsensitive) == 0)
			return static_cast<int>(index);
	}

	return -1;
}

RecentFiles::RecentFiles
(
	const std::string& appName,
	const std::string& settingsGroup
) :
	_appName(appName),
	_settingsGroup(settingsGroup)
{
	load();
}

void RecentFiles::clear()
{
	_recentFiles.clear();

	save();
}

void RecentFiles::hideExtensions
(
	bool state
)
{
	_hideExtensions = state;
}

int RecentFiles::fileCount()
{
	return static_cast<int>(_recentFiles.size());
}

void RecentFiles::setMaxFileCount
(
	int maxFiles
)
{
	if (maxFiles != _maxFileCount)
	{
		if (maxFiles < 0)
			maxFiles = 0;

		if (maxFiles > 20)
			maxFiles = 20;

		_maxFileCount = maxFiles;
		while (static_cast<int>(_recentFiles.size()) > _maxFileCount)
		{
			_recentFiles.pop_back();
		}

		save();
	}
}

void RecentFiles::addFile
(
	const std::string& filePath
)
{
	int index = indexOfCaseInsensitive(_recentFiles, filePath);

	if (index == -1)
	{
		if (static_cast<int>(_recentFiles.size()) == _maxFileCount)
			_recentFiles.pop_back();

		_recentFiles.insert(_recentFiles.begin(), filePath);
	}
	else
	{
		if (index != 0)
		{
			_recentFiles.erase(_recentFiles.begin() + index);
			_recentFiles.insert(_recentFiles.begin(), filePath);
		}
	}

	save();
}

void RecentFiles::removeFile
(
	const std::string& filePath
)
{
	int fileIndex(0);

	for (const auto& savedFileName: _recentFiles)
	{
		if (filePath == savedFileName)
		{
			removeFile(fileIndex);
			save();
			return;
		}

		fileIndex++;
	}
}

void RecentFiles::removeFile
(
	int fileIndex
)
{
	if (fileIndex >= 0 && fileIndex < static_cast<int>(_recentFiles.size()))
		_recentFiles.erase(_recentFiles.begin() + fileIndex);
}

std::string RecentFiles::getFilePath
(
	int fileIndex
)
{
	std::string result;

	if (fileIndex >= 0 && fileIndex < static_cast<int>(_recentFiles.size()))
		result = _recentFiles.at(fileIndex);

	return result;
}

std::string RecentFiles::getFileName
(
	int fileIndex
)
{
	std::string result(getFilePath(fileIndex));

	if (result.empty() == false)
	{
		QFileInfo fileInfo(QString::fromStdString(result));

		if (_hideExtensions)
			result = fileInfo.baseName().toStdString();
		else
			result = fileInfo.fileName().toStdString();
	}

	return result;
}

void RecentFiles::load()
{
	AlpacaSettings settings(_appName);

	if (_settingsGroup.empty() == false)
		settings.beginGroup(QString::fromStdString(_settingsGroup));

	_maxFileCount = settings.value("maxFileCount", _maxFileCount).toInt();

	for (auto i: range(settings.beginReadArray("recentFiles")))
	{
		settings.setArrayIndex(i);

		QString recentFile = settings.value("recentFile").toString();
		if (recentFile.isEmpty() == false)
		{
			QFileInfo fileInfo(recentFile);
			if (fileInfo.exists())
				_recentFiles.push_back(recentFile.toStdString());
		}
	}

	settings.endArray();

	if (_settingsGroup.empty() == false)
		settings.endGroup();
}

void RecentFiles::save()
{
	AlpacaSettings settings(_appName);

	if (_settingsGroup.empty() == false)
		settings.beginGroup(QString::fromStdString(_settingsGroup));

	settings.remove("recentFiles");

	settings.setValue("maxFileCount", _maxFileCount);

	settings.beginWriteArray("recentFiles");

	for (auto i: range(_recentFiles.size()))
	{
		settings.setArrayIndex(i);
		settings.setValue("recentFile", QString::fromStdString(_recentFiles.at(i)));
	}

	settings.endArray();

	if (_settingsGroup.empty() == false)
		settings.endGroup();
}
