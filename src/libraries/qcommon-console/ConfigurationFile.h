#ifndef CONFIGURATIONFILE_H
#define CONFIGURATIONFILE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

// Standard C++ data types: a record is a key-value map, and a table is a list of records
using ConfigurationRecord = std::map<std::string, std::string>;
using ConfigurationTable  = std::vector<ConfigurationRecord>;

// Convenient aliases
using ConfigRecord = ConfigurationRecord;
using ConfigTable  = ConfigurationTable;

class QCOMMONCONSOLE_EXPORT ConfigurationFile
{
public:
	ConfigurationFile() = default;
	~ConfigurationFile() = default;

	// Bulk Read and Write APIs: reads/writes file to standard maps and lists
	bool load(const std::string& filePath);
	bool save(const std::string& filePath, bool indented = true) const;

	bool readFromString(const std::string& content);
	std::string writeToString(bool indented = true) const;

	// Structure queries
	bool hasProperty(const std::string& key) const;
	bool hasTable(const std::string& tableName) const;
	bool hasIntList(const std::string& key) const;
	bool hasStringList(const std::string& key) const;

	// Query individual top-level properties
	std::string getProperty(const std::string& key, const std::string& defaultValue = "") const;
	int getInt(const std::string& key, int defaultValue = 0) const;
	uint32_t getUInt(const std::string& key, uint32_t defaultValue = 0) const;
	int64_t getInt64(const std::string& key, int64_t defaultValue = 0) const;
	uint64_t getUInt64(const std::string& key, uint64_t defaultValue = 0) const;
	bool getBool(const std::string& key, bool defaultValue = false) const;
	double getDouble(const std::string& key, double defaultValue = 0.0) const;

	// Set individual top-level properties
	void setProperty(const std::string& key, const std::string& value);
	void setInt(const std::string& key, int value);
	void setUInt(const std::string& key, uint32_t value);
	void setInt64(const std::string& key, int64_t value);
	void setUInt64(const std::string& key, uint64_t value);
	void setBool(const std::string& key, bool value);
	void setDouble(const std::string& key, double value);

	const std::map<std::string, std::string>& properties() const { return _properties; }

	// Tables / Array of records (e.g. "pins", "tabs", "buttons", "catalog")
	const ConfigurationTable& getTable(const std::string& tableName) const;
	void setTable(const std::string& tableName, const ConfigurationTable& table);
	void appendRecord(const std::string& tableName, const ConfigurationRecord& record);

	// Integer lists (e.g. "min_firmware_version")
	const std::vector<int>& getIntList(const std::string& key) const;
	void setIntList(const std::string& key, const std::vector<int>& list);
	void appendInt(const std::string& key, int value);

	// String lists
	const std::vector<std::string>& getStringList(const std::string& key) const;
	void setStringList(const std::string& key, const std::vector<std::string>& list);
	void appendString(const std::string& key, const std::string& value);

	void clear();

	// Record helpers: extract typed values from a ConfigurationRecord
	static std::string getRecordString(const ConfigurationRecord& record, const std::string& key, const std::string& defaultValue = "");
	static int getRecordInt(const ConfigurationRecord& record, const std::string& key, int defaultValue = 0);
	static uint32_t getRecordUInt(const ConfigurationRecord& record, const std::string& key, uint32_t defaultValue = 0);
	static int64_t getRecordInt64(const ConfigurationRecord& record, const std::string& key, int64_t defaultValue = 0);
	static uint64_t getRecordUInt64(const ConfigurationRecord& record, const std::string& key, uint64_t defaultValue = 0);
	static bool getRecordBool(const ConfigurationRecord& record, const std::string& key, bool defaultValue = false);
	static double getRecordDouble(const ConfigurationRecord& record, const std::string& key, double defaultValue = 0.0);

private:
	std::map<std::string, std::string>              _properties;
	std::map<std::string, ConfigurationTable>       _tables;
	std::map<std::string, std::vector<int>>         _intLists;
	std::map<std::string, std::vector<std::string>> _stringLists;

	static const ConfigurationTable                 _emptyTable;
	static const std::vector<int>                   _emptyIntList;
	static const std::vector<std::string>           _emptyStringList;
};

#endif // CONFIGURATIONFILE_H
