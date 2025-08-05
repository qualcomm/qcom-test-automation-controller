// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "RecentFiles.h"

// QCommon
#include "AlpacaSettings.h"
#include "Range.h"

// QT
#include <QFileInfo>
#include <QSettings>

RecentFiles::RecentFiles
(
	const QString& appName,
	const QString& settingsGroup
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
	return _recentFiles.count();
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
		while (_recentFiles.count() > _maxFileCount)
		{
			_recentFiles.pop_back();
		}

		save();
	}
}

void RecentFiles::addFile
(
	const QString& filePath
)
{
	if (_recentFiles.contains(filePath, Qt::CaseInsensitive) == false)
	{
		if (_recentFiles.count() == _maxFileCount)
			_recentFiles.pop_back();

		_recentFiles.push_front(filePath);
	}
	else
	{
		int index = _recentFiles.indexOf(filePath);
		if (index != -1 && index != 0)
		{
			_recentFiles.removeAt(index);
			_recentFiles.push_front(filePath);
		}
	}

	save();
}

void RecentFiles::removeFile
(
	const QString& filePath
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
	if (fileIndex >= 0 && fileIndex < _recentFiles.count())
		_recentFiles.erase(_recentFiles.begin() + fileIndex);
}

QString RecentFiles::getFilePath
(
	int fileIndex
)
{
	QString result;

	if (fileIndex >= 0 && fileIndex < _recentFiles.count())
		result = _recentFiles.at(fileIndex);

	return result;
}

QString RecentFiles::getFileName
(
	int fileIndex
)
{
	QString result(getFilePath(fileIndex));

	if (result.isEmpty() == false)
	{
		QFileInfo fileInfo(result);

		if (_hideExtensions)
			result = fileInfo.baseName();
		else
			result = fileInfo.fileName();
	}

	return result;
}

void RecentFiles::load()
{
	AlpacaSettings settings(_appName);

	if (_settingsGroup.isEmpty() == false)
		settings.beginGroup(_settingsGroup);

	_maxFileCount = settings.value("maxFileCount", _maxFileCount).toInt();

	for (auto i: range(settings.beginReadArray("recentFiles")))
	{
		settings.setArrayIndex(i);

		QString recentFile = settings.value("recentFile").toString();
		if (recentFile.isEmpty() == false)
		{
			QFileInfo fileInfo(recentFile);
			if (fileInfo.exists())
				_recentFiles.push_back(recentFile);
		}
	}

	settings.endArray();

	if (_settingsGroup.isEmpty() == false)
		settings.endGroup();
}

void RecentFiles::save()
{
	AlpacaSettings settings(_appName);

	if (_settingsGroup.isEmpty() == false)
		settings.beginGroup(_settingsGroup);

	settings.remove("recentFiles");

	settings.setValue("maxFileCount", _maxFileCount);

	settings.beginWriteArray("recentFiles");

	for (auto i: range(_recentFiles.size()))
	{
		settings.setArrayIndex(i);
		settings.setValue("recentFile", _recentFiles.at(i));
	}

	settings.endArray();

	if (_settingsGroup.isEmpty() == false)
		settings.endGroup();
}
