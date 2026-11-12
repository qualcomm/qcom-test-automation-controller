#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "QCommonConsoleGlobal.h"

#include <map>
#include <string>
#include <vector>

typedef std::map<std::string, std::string> KeyValuePairs;

class QCOMMONCONSOLE_EXPORT CommandLineParser
{
public:
	CommandLineParser(const std::vector<std::string>& args);

	void setVersion(const std::string& version);
	bool versionRequested();
	std::string version();

	bool helpRequested();

	void addHelpLine(const std::string& helpText);
	void addHelpLine();
	const std::string helpText();

	bool isSet(const std::vector<std::string>& keys);
	std::string value(const std::string& key);

protected:
	std::vector<std::string>	_args;
	KeyValuePairs				_values;

	bool						_helpRequested{false};
	std::string					_helpText;

	bool						_versionRequested{false};
	std::string					_version;
};

#endif // COMMANDLINEPARSER_H
