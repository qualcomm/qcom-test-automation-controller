// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "IniSettings.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>

static std::string trim(const std::string& str)
{
	size_t first = str.find_first_not_of(" \t\r\n");
	if (first == std::string::npos)
		return "";
	size_t last = str.find_last_not_of(" \t\r\n");
	return str.substr(first, (last - first + 1));
}

IniSettings::Value::Value()
	: _val(""), _exists(false)
{
}

IniSettings::Value::Value(const std::string& v, bool exists)
	: _val(v), _exists(exists)
{
}

IniSettings::Value::Value(const char* v)
	: _val(v ? v : ""), _exists(v != nullptr)
{
}

IniSettings::Value::Value(int v)
	: _val(std::to_string(v)), _exists(true)
{
}

IniSettings::Value::Value(unsigned int v)
	: _val(std::to_string(v)), _exists(true)
{
}

IniSettings::Value::Value(int64_t v)
	: _val(std::to_string(v)), _exists(true)
{
}

IniSettings::Value::Value(double v)
	: _val(std::to_string(v)), _exists(true)
{
}

IniSettings::Value::Value(bool v)
	: _val(v ? "true" : "false"), _exists(true)
{
}

std::string IniSettings::Value::toString() const
{
	return _val;
}

int IniSettings::Value::toInt(int defaultVal) const
{
	if (!_exists || _val.empty())
		return defaultVal;
	try
	{
		return std::stoi(_val, nullptr, 0);
	}
	catch (...)
	{
		return defaultVal;
	}
}

unsigned int IniSettings::Value::toUInt(unsigned int defaultVal) const
{
	if (!_exists || _val.empty())
		return defaultVal;
	try
	{
		return static_cast<unsigned int>(std::stoul(_val, nullptr, 0));
	}
	catch (...)
	{
		return defaultVal;
	}
}

int64_t IniSettings::Value::toLongLong(int64_t defaultVal) const
{
	if (!_exists || _val.empty())
		return defaultVal;
	try
	{
		return std::stoll(_val, nullptr, 0);
	}
	catch (...)
	{
		return defaultVal;
	}
}

double IniSettings::Value::toDouble(double defaultVal) const
{
	if (!_exists || _val.empty())
		return defaultVal;
	try
	{
		return std::stod(_val);
	}
	catch (...)
	{
		return defaultVal;
	}
}

double IniSettings::Value::toReal(double defaultVal) const
{
	return toDouble(defaultVal);
}

bool IniSettings::Value::toBool(bool defaultVal) const
{
	if (!_exists || _val.empty())
		return defaultVal;

	std::string s = _val;
	for (char &c : s)
		c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

	if (s == "true" || s == "1" || s == "yes" || s == "on")
		return true;
	if (s == "false" || s == "0" || s == "no" || s == "off")
		return false;

	return defaultVal;
}

bool IniSettings::Value::isNull() const
{
	return !_exists;
}

bool IniSettings::Value::isValid() const
{
	return _exists;
}

std::string IniSettings::defaultSettingsPath(const std::string& organization, const std::string& appName)
{
	std::string org = organization.empty() ? "Qualcomm" : organization;
	std::string app = appName.empty() ? "Alpaca" : appName;

#if defined(_WIN32)
	const char* appData = std::getenv("APPDATA");
	if (appData && *appData)
	{
		return (std::filesystem::path(appData) / org / app / "settings.ini").string();
	}
	const char* userProfile = std::getenv("USERPROFILE");
	if (userProfile && *userProfile)
	{
		return (std::filesystem::path(userProfile) / "AppData" / "Roaming" / org / app / "settings.ini").string();
	}
	return (std::filesystem::path(".") / org / app / "settings.ini").string();
#else
	const char* xdgConfig = std::getenv("XDG_CONFIG_HOME");
	if (xdgConfig && *xdgConfig)
	{
		return (std::filesystem::path(xdgConfig) / org / app / "settings.ini").string();
	}
	const char* home = std::getenv("HOME");
	if (home && *home)
	{
		return (std::filesystem::path(home) / ".config" / org / app / "settings.ini").string();
	}
	return (std::filesystem::path(".") / ".config" / org / app / "settings.ini").string();
#endif
}

IniSettings::IniSettings()
	: _appName("Alpaca"), _organization("Qualcomm")
{
	_filePath = defaultSettingsPath(_organization, _appName);
	load();
}

IniSettings::IniSettings(const std::string& appName)
	: _appName(appName.empty() ? "Alpaca" : appName), _organization("Qualcomm")
{
	_filePath = defaultSettingsPath(_organization, _appName);
	load();
}

IniSettings::IniSettings(const std::string& organization, const std::string& appName)
	: _appName(appName.empty() ? "Alpaca" : appName), _organization(organization.empty() ? "Qualcomm" : organization)
{
	_filePath = defaultSettingsPath(_organization, _appName);
	load();
}

IniSettings::~IniSettings()
{
	if (_dirty)
	{
		save();
	}
}

void IniSettings::setFilePath(const std::string& path)
{
	if (_dirty)
		save();
	_filePath = path;
	load();
}

std::string IniSettings::filePath() const
{
	return _filePath;
}

void IniSettings::beginGroup(const std::string& group)
{
	if (!group.empty())
		_groupStack.push_back(group);
}

void IniSettings::endGroup()
{
	if (!_groupStack.empty())
		_groupStack.pop_back();
}

std::string IniSettings::currentGroup() const
{
	std::string grp;
	for (size_t i = 0; i < _groupStack.size(); ++i)
	{
		if (i > 0) grp += "/";
		grp += _groupStack[i];
	}
	return grp;
}

std::string IniSettings::fullKey(const std::string& key) const
{
	std::string k = key;
	if (_inArray && !_arrayPrefix.empty())
	{
		k = _arrayPrefix + "/" + std::to_string(_arrayIndex + 1) + "/" + key;
	}
	return k;
}

void IniSettings::setValue(const std::string& key, const std::string& value)
{
	std::string grp = currentGroup();
	std::string k = fullKey(key);

	_sections[grp][k] = value;
	_dirty = true;

	if (_inArray && !_arrayPrefix.empty())
	{
		int currentSize = this->value(_arrayPrefix + "/size", 0).toInt();
		if (_arrayIndex + 1 > currentSize)
		{
			_sections[grp][_arrayPrefix + "/size"] = std::to_string(_arrayIndex + 1);
		}
	}
}

void IniSettings::setValue(const std::string& key, const char* value)
{
	setValue(key, std::string(value ? value : ""));
}

void IniSettings::setValue(const std::string& key, int value)
{
	setValue(key, std::to_string(value));
}

void IniSettings::setValue(const std::string& key, unsigned int value)
{
	setValue(key, std::to_string(value));
}

void IniSettings::setValue(const std::string& key, int64_t value)
{
	setValue(key, std::to_string(value));
}

void IniSettings::setValue(const std::string& key, double value)
{
	setValue(key, std::to_string(value));
}

void IniSettings::setValue(const std::string& key, bool value)
{
	setValue(key, value ? std::string("true") : std::string("false"));
}

IniSettings::Value IniSettings::value(const std::string& key, const Value& defaultValue) const
{
	std::string grp = currentGroup();
	std::string k = fullKey(key);

	auto secIt = _sections.find(grp);
	if (secIt != _sections.end())
	{
		auto keyIt = secIt->second.find(k);
		if (keyIt != secIt->second.end())
		{
			return Value(keyIt->second, true);
		}
	}

	return defaultValue;
}

void IniSettings::remove(const std::string& key)
{
	std::string grp = currentGroup();
	auto secIt = _sections.find(grp);
	if (secIt != _sections.end())
	{
		std::string prefix = key + "/";
		auto it = secIt->second.begin();
		while (it != secIt->second.end())
		{
			if (it->first == key || it->first.rfind(prefix, 0) == 0)
			{
				it = secIt->second.erase(it);
				_dirty = true;
			}
			else
			{
				++it;
			}
		}
	}
}

bool IniSettings::contains(const std::string& key) const
{
	return value(key).isValid();
}

int IniSettings::beginReadArray(const std::string& prefix)
{
	_arrayPrefix = prefix;
	_inArray = true;
	_arrayIndex = 0;
	return value(_arrayPrefix + "/size", 0).toInt();
}

void IniSettings::beginWriteArray(const std::string& prefix, int size)
{
	_arrayPrefix = prefix;
	_inArray = true;
	_arrayIndex = 0;
	if (size >= 0)
	{
		setValue(_arrayPrefix + "/size", size);
	}
}

void IniSettings::setArrayIndex(int i)
{
	_arrayIndex = i;
}

void IniSettings::endArray()
{
	_inArray = false;
	_arrayPrefix.clear();
	_arrayIndex = -1;
}

void IniSettings::sync()
{
	if (_dirty)
		save();
}

void IniSettings::load()
{
	_sections.clear();
	_dirty = false;

	if (_filePath.empty() || !std::filesystem::exists(_filePath))
		return;

	std::ifstream file(_filePath);
	if (!file.is_open())
		return;

	std::string line;
	std::string currentSec;

	while (std::getline(file, line))
	{
		std::string trimmed = trim(line);
		if (trimmed.empty() || trimmed[0] == ';' || trimmed[0] == '#')
			continue;

		if (trimmed.front() == '[' && trimmed.back() == ']')
		{
			currentSec = trim(trimmed.substr(1, trimmed.size() - 2));
		}
		else
		{
			size_t eqPos = trimmed.find('=');
			if (eqPos != std::string::npos)
			{
				std::string k = trim(trimmed.substr(0, eqPos));
				std::string v = trim(trimmed.substr(eqPos + 1));
				_sections[currentSec][k] = v;
			}
		}
	}
}

void IniSettings::save()
{
	if (_filePath.empty())
		return;

	try
	{
		std::filesystem::path p(_filePath);
		if (p.has_parent_path())
		{
			std::filesystem::create_directories(p.parent_path());
		}

		std::ofstream file(_filePath, std::ios::trunc);
		if (!file.is_open())
			return;

		// Write global/empty section first
		auto emptySec = _sections.find("");
		if (emptySec != _sections.end())
		{
			for (const auto& kv : emptySec->second)
			{
				file << kv.first << "=" << kv.second << "\n";
			}
			file << "\n";
		}

		// Write all named sections
		for (const auto& sec : _sections)
		{
			if (sec.first.empty())
				continue;

			file << "[" << sec.first << "]\n";
			for (const auto& kv : sec.second)
			{
				file << kv.first << "=" << kv.second << "\n";
			}
			file << "\n";
		}

		_dirty = false;
	}
	catch (...)
	{
		// Silently catch filesystem exceptions
	}
}
