#ifndef INISETTINGS_H
#define INISETTINGS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

class QCOMMONCONSOLE_EXPORT IniSettings
{
public:
	class QCOMMONCONSOLE_EXPORT Value
	{
	public:
		Value();
		Value(const std::string& v, bool exists = true);
		Value(const char* v);
		Value(int v);
		Value(unsigned int v);
		Value(int64_t v);
		Value(double v);
		Value(bool v);

		std::string toString() const;
		int toInt(int defaultVal = 0) const;
		unsigned int toUInt(unsigned int defaultVal = 0) const;
		int64_t toLongLong(int64_t defaultVal = 0) const;
		double toDouble(double defaultVal = 0.0) const;
		double toReal(double defaultVal = 0.0) const;
		bool toBool(bool defaultVal = false) const;
		bool isNull() const;
		bool isValid() const;

	private:
		std::string _val;
		bool _exists{false};
	};

	IniSettings();
	explicit IniSettings(const std::string& appName);
	IniSettings(const std::string& organization, const std::string& appName);
	virtual ~IniSettings();

	void setFilePath(const std::string& path);
	std::string filePath() const;

	void beginGroup(const std::string& group);
	void endGroup();
	std::string currentGroup() const;

	void setValue(const std::string& key, const std::string& value);
	void setValue(const std::string& key, const char* value);
	void setValue(const std::string& key, int value);
	void setValue(const std::string& key, unsigned int value);
	void setValue(const std::string& key, int64_t value);
	void setValue(const std::string& key, double value);
	void setValue(const std::string& key, bool value);

	Value value(const std::string& key, const Value& defaultValue = Value()) const;
	void remove(const std::string& key);
	bool contains(const std::string& key) const;

	int beginReadArray(const std::string& prefix);
	void beginWriteArray(const std::string& prefix, int size = -1);
	void setArrayIndex(int i);
	void endArray();

	void sync();
	void load();
	void save();

private:
	std::string fullKey(const std::string& key) const;
	static std::string defaultSettingsPath(const std::string& organization, const std::string& appName);

	std::string                                                 _appName;
	std::string                                                 _organization;
	std::string                                                 _filePath;
	std::vector<std::string>                                    _groupStack;
	std::string                                                 _arrayPrefix;
	int                                                         _arrayIndex{-1};
	bool                                                        _inArray{false};
	bool                                                        _dirty{false};
	std::map<std::string, std::map<std::string, std::string>> _sections;
};

#endif // INISETTINGS_H
