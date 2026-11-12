// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "RecentFiles.h"

// QCommon
#include "AlpacaSettings.h"

#include <cctype>
#include <filesystem>

static bool equalsCaseInsensitive(const std::string& a, const std::string& b)
{
	if (a.size() != b.size())
		return false;
	for (size_t i = 0; i < a.size(); ++i)
	{
		if (std::tolower(static_cast<unsigned char>(a[i])) != std::tolower(static_cast<unsigned char>(b[i])))
			return false;
	}
	return true;
}

static int indexOfCaseInsensitive(const std::vector<std::string>& files, const std::string& filePath)
{
	for (size_t index = 0; index < files.size(); index++)
	{
		if (equalsCaseInsensitive(files[index], filePath))
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

	if (!result.empty())
	{
		std::filesystem::path p(result);
		if (_hideExtensions)
			result = p.stem().string();
		else
			result = p.filename().string();
	}

	return result;
}

void RecentFiles::load()
{
	AlpacaSettings settings(_appName);

	if (!_settingsGroup.empty())
		settings.beginGroup(_settingsGroup);

	_maxFileCount = settings.value("maxFileCount", _maxFileCount).toInt();

	int count = settings.beginReadArray("recentFiles");
	for (int i = 0; i < count; ++i)
	{
		settings.setArrayIndex(i);

		std::string recentFile = settings.value("recentFile").toString();
		if (!recentFile.empty())
		{
			std::error_code ec;
			if (std::filesystem::exists(recentFile, ec))
				_recentFiles.push_back(recentFile);
		}
	}

	settings.endArray();

	if (!_settingsGroup.empty())
		settings.endGroup();
}

void RecentFiles::save()
{
	AlpacaSettings settings(_appName);

	if (!_settingsGroup.empty())
		settings.beginGroup(_settingsGroup);

	settings.remove("recentFiles");

	settings.setValue("maxFileCount", _maxFileCount);

	settings.beginWriteArray("recentFiles");

	for (size_t i = 0; i < _recentFiles.size(); ++i)
	{
		settings.setArrayIndex(static_cast<int>(i));
		settings.setValue("recentFile", _recentFiles.at(i));
	}

	settings.endArray();

	if (!_settingsGroup.empty())
		settings.endGroup();
}
