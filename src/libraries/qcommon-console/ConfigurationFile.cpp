// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ConfigurationFile.h"
#include "private/nlohmann/json.hpp"

#include <cctype>
#include <fstream>
#include <sstream>

const ConfigurationTable ConfigurationFile::_emptyTable;
const std::vector<int> ConfigurationFile::_emptyIntList;
const std::vector<std::string> ConfigurationFile::_emptyStringList;

static bool isIntegerString(const std::string& s)
{
	if (s.empty())
		return false;
	size_t start = 0;
	if (s[0] == '-' || s[0] == '+')
	{
		if (s.length() == 1)
			return false;
		start = 1;
	}
	for (size_t i = start; i < s.length(); ++i)
	{
		if (!std::isdigit(static_cast<unsigned char>(s[i])))
			return false;
	}
	return true;
}

static void parseValueToRecord(const std::string& key, const nlohmann::json& val, ConfigurationRecord& record)
{
	if (val.is_string())
		record[key] = val.get<std::string>();
	else if (val.is_boolean())
		record[key] = val.get<bool>() ? "true" : "false";
	else if (val.is_number_integer())
		record[key] = std::to_string(val.get<int64_t>());
	else if (val.is_number_unsigned())
		record[key] = std::to_string(val.get<uint64_t>());
	else if (val.is_number_float())
		record[key] = std::to_string(val.get<double>());
	else if (val.is_primitive())
		record[key] = val.dump();
}

static void addFieldToJson(nlohmann::json& obj, const std::string& key, const std::string& value)
{
	if (value == "true")
	{
		obj[key] = true;
	}
	else if (value == "false")
	{
		obj[key] = false;
	}
	else if (isIntegerString(value))
	{
		try
		{
			if (value[0] == '-')
				obj[key] = std::stoll(value);
			else
				obj[key] = std::stoull(value);
		}
		catch (...)
		{
			obj[key] = value;
		}
	}
	else
	{
		obj[key] = value;
	}
}

bool ConfigurationFile::load(const std::string& filePath)
{
	clear();

	std::ifstream file(filePath);
	if (!file.is_open())
		return false;

	try
	{
		nlohmann::json root = nlohmann::json::parse(file, nullptr, false);
		if (root.is_discarded() || !root.is_object())
			return false;

		for (auto it = root.begin(); it != root.end(); ++it)
		{
			const std::string& key = it.key();
			const auto& val = it.value();

			if (val.is_array())
			{
				if (val.empty())
				{
					_tables[key] = ConfigurationTable();
				}
				else if (val[0].is_object())
				{
					ConfigurationTable table;
					for (const auto& item : val)
					{
						if (item.is_object())
						{
							ConfigurationRecord record;
							for (auto fieldIt = item.begin(); fieldIt != item.end(); ++fieldIt)
							{
								parseValueToRecord(fieldIt.key(), fieldIt.value(), record);
							}
							table.push_back(record);
						}
					}
					_tables[key] = table;
				}
				else if (val[0].is_number_integer() || val[0].is_number_unsigned())
				{
					std::vector<int> intList;
					for (const auto& item : val)
					{
						if (item.is_number())
							intList.push_back(item.get<int>());
					}
					_intLists[key] = intList;
				}
				else if (val[0].is_string())
				{
					std::vector<std::string> strList;
					for (const auto& item : val)
					{
						if (item.is_string())
							strList.push_back(item.get<std::string>());
					}
					_stringLists[key] = strList;
				}
			}
			else if (val.is_primitive())
			{
				parseValueToRecord(key, val, _properties);
			}
		}

		return true;
	}
	catch (...)
	{
		clear();
		return false;
	}
}

bool ConfigurationFile::save(const std::string& filePath, bool indented) const
{
	std::ofstream file(filePath);
	if (!file.is_open())
		return false;

	try
	{
		std::string jsonStr = writeToString(indented);
		file << jsonStr << "\n";
		return file.good();
	}
	catch (...)
	{
		return false;
	}
}

bool ConfigurationFile::readFromString(const std::string& content)
{
	clear();
	try
	{
		nlohmann::json root = nlohmann::json::parse(content, nullptr, false);
		if (root.is_discarded() || !root.is_object())
			return false;

		for (auto it = root.begin(); it != root.end(); ++it)
		{
			const std::string& key = it.key();
			const auto& val = it.value();

			if (val.is_array())
			{
				if (val.empty())
				{
					_tables[key] = ConfigurationTable();
				}
				else if (val[0].is_object())
				{
					ConfigurationTable table;
					for (const auto& item : val)
					{
						if (item.is_object())
						{
							ConfigurationRecord record;
							for (auto fieldIt = item.begin(); fieldIt != item.end(); ++fieldIt)
							{
								parseValueToRecord(fieldIt.key(), fieldIt.value(), record);
							}
							table.push_back(record);
						}
					}
					_tables[key] = table;
				}
				else if (val[0].is_number_integer() || val[0].is_number_unsigned())
				{
					std::vector<int> intList;
					for (const auto& item : val)
					{
						if (item.is_number())
							intList.push_back(item.get<int>());
					}
					_intLists[key] = intList;
				}
				else if (val[0].is_string())
				{
					std::vector<std::string> strList;
					for (const auto& item : val)
					{
						if (item.is_string())
							strList.push_back(item.get<std::string>());
					}
					_stringLists[key] = strList;
				}
			}
			else if (val.is_primitive())
			{
				parseValueToRecord(key, val, _properties);
			}
		}

		return true;
	}
	catch (...)
	{
		clear();
		return false;
	}
}

std::string ConfigurationFile::writeToString(bool indented) const
{
	nlohmann::json root = nlohmann::json::object();

	// Write top-level properties
	for (const auto& [key, value] : _properties)
	{
		addFieldToJson(root, key, value);
	}

	// Write tables
	for (const auto& [tableName, table] : _tables)
	{
		nlohmann::json arr = nlohmann::json::array();
		for (const auto& record : table)
		{
			nlohmann::json recObj = nlohmann::json::object();
			for (const auto& [field, val] : record)
			{
				addFieldToJson(recObj, field, val);
			}
			arr.push_back(recObj);
		}
		root[tableName] = arr;
	}

	// Write int lists
	for (const auto& [key, list] : _intLists)
	{
		root[key] = list;
	}

	// Write string lists
	for (const auto& [key, list] : _stringLists)
	{
		root[key] = list;
	}

	return indented ? root.dump(4) : root.dump();
}

bool ConfigurationFile::hasProperty(const std::string& key) const
{
	return _properties.find(key) != _properties.end();
}

bool ConfigurationFile::hasTable(const std::string& tableName) const
{
	return _tables.find(tableName) != _tables.end();
}

bool ConfigurationFile::hasIntList(const std::string& key) const
{
	return _intLists.find(key) != _intLists.end();
}

bool ConfigurationFile::hasStringList(const std::string& key) const
{
	return _stringLists.find(key) != _stringLists.end();
}

std::string ConfigurationFile::getProperty(const std::string& key, const std::string& defaultValue) const
{
	auto it = _properties.find(key);
	if (it != _properties.end())
		return it->second;
	return defaultValue;
}

int ConfigurationFile::getInt(const std::string& key, int defaultValue) const
{
	auto it = _properties.find(key);
	if (it != _properties.end())
	{
		try { return std::stoi(it->second); } catch (...) { return defaultValue; }
	}
	return defaultValue;
}

uint32_t ConfigurationFile::getUInt(const std::string& key, uint32_t defaultValue) const
{
	auto it = _properties.find(key);
	if (it != _properties.end())
	{
		try { return static_cast<uint32_t>(std::stoul(it->second)); } catch (...) { return defaultValue; }
	}
	return defaultValue;
}

int64_t ConfigurationFile::getInt64(const std::string& key, int64_t defaultValue) const
{
	auto it = _properties.find(key);
	if (it != _properties.end())
	{
		try { return std::stoll(it->second); } catch (...) { return defaultValue; }
	}
	return defaultValue;
}

uint64_t ConfigurationFile::getUInt64(const std::string& key, uint64_t defaultValue) const
{
	auto it = _properties.find(key);
	if (it != _properties.end())
	{
		try { return std::stoull(it->second); } catch (...) { return defaultValue; }
	}
	return defaultValue;
}

bool ConfigurationFile::getBool(const std::string& key, bool defaultValue) const
{
	auto it = _properties.find(key);
	if (it != _properties.end())
	{
		return (it->second == "true" || it->second == "1" || it->second == "TRUE");
	}
	return defaultValue;
}

double ConfigurationFile::getDouble(const std::string& key, double defaultValue) const
{
	auto it = _properties.find(key);
	if (it != _properties.end())
	{
		try { return std::stod(it->second); } catch (...) { return defaultValue; }
	}
	return defaultValue;
}

void ConfigurationFile::setProperty(const std::string& key, const std::string& value)
{
	_properties[key] = value;
}

void ConfigurationFile::setInt(const std::string& key, int value)
{
	_properties[key] = std::to_string(value);
}

void ConfigurationFile::setUInt(const std::string& key, uint32_t value)
{
	_properties[key] = std::to_string(value);
}

void ConfigurationFile::setInt64(const std::string& key, int64_t value)
{
	_properties[key] = std::to_string(value);
}

void ConfigurationFile::setUInt64(const std::string& key, uint64_t value)
{
	_properties[key] = std::to_string(value);
}

void ConfigurationFile::setBool(const std::string& key, bool value)
{
	_properties[key] = (value ? "true" : "false");
}

void ConfigurationFile::setDouble(const std::string& key, double value)
{
	_properties[key] = std::to_string(value);
}

const ConfigurationTable& ConfigurationFile::getTable(const std::string& tableName) const
{
	auto it = _tables.find(tableName);
	if (it != _tables.end())
		return it->second;
	return _emptyTable;
}

void ConfigurationFile::setTable(const std::string& tableName, const ConfigurationTable& table)
{
	_tables[tableName] = table;
}

void ConfigurationFile::appendRecord(const std::string& tableName, const ConfigurationRecord& record)
{
	_tables[tableName].push_back(record);
}

const std::vector<int>& ConfigurationFile::getIntList(const std::string& key) const
{
	auto it = _intLists.find(key);
	if (it != _intLists.end())
		return it->second;
	return _emptyIntList;
}

void ConfigurationFile::setIntList(const std::string& key, const std::vector<int>& list)
{
	_intLists[key] = list;
}

void ConfigurationFile::appendInt(const std::string& key, int value)
{
	_intLists[key].push_back(value);
}

const std::vector<std::string>& ConfigurationFile::getStringList(const std::string& key) const
{
	auto it = _stringLists.find(key);
	if (it != _stringLists.end())
		return it->second;
	return _emptyStringList;
}

void ConfigurationFile::setStringList(const std::string& key, const std::vector<std::string>& list)
{
	_stringLists[key] = list;
}

void ConfigurationFile::appendString(const std::string& key, const std::string& value)
{
	_stringLists[key].push_back(value);
}

void ConfigurationFile::clear()
{
	_properties.clear();
	_tables.clear();
	_intLists.clear();
	_stringLists.clear();
}

std::string ConfigurationFile::getRecordString(const ConfigurationRecord& record, const std::string& key, const std::string& defaultValue)
{
	auto it = record.find(key);
	if (it != record.end())
		return it->second;
	return defaultValue;
}

int ConfigurationFile::getRecordInt(const ConfigurationRecord& record, const std::string& key, int defaultValue)
{
	auto it = record.find(key);
	if (it != record.end())
	{
		try { return std::stoi(it->second); } catch (...) { return defaultValue; }
	}
	return defaultValue;
}

uint32_t ConfigurationFile::getRecordUInt(const ConfigurationRecord& record, const std::string& key, uint32_t defaultValue)
{
	auto it = record.find(key);
	if (it != record.end())
	{
		try { return static_cast<uint32_t>(std::stoul(it->second)); } catch (...) { return defaultValue; }
	}
	return defaultValue;
}

int64_t ConfigurationFile::getRecordInt64(const ConfigurationRecord& record, const std::string& key, int64_t defaultValue)
{
	auto it = record.find(key);
	if (it != record.end())
	{
		try { return std::stoll(it->second); } catch (...) { return defaultValue; }
	}
	return defaultValue;
}

uint64_t ConfigurationFile::getRecordUInt64(const ConfigurationRecord& record, const std::string& key, uint64_t defaultValue)
{
	auto it = record.find(key);
	if (it != record.end())
	{
		try { return std::stoull(it->second); } catch (...) { return defaultValue; }
	}
	return defaultValue;
}

bool ConfigurationFile::getRecordBool(const ConfigurationRecord& record, const std::string& key, bool defaultValue)
{
	auto it = record.find(key);
	if (it != record.end())
	{
		return (it->second == "true" || it->second == "1" || it->second == "TRUE");
	}
	return defaultValue;
}

double ConfigurationFile::getRecordDouble(const ConfigurationRecord& record, const std::string& key, double defaultValue)
{
	auto it = record.find(key);
	if (it != record.end())
	{
		try { return std::stod(it->second); } catch (...) { return defaultValue; }
	}
	return defaultValue;
}
