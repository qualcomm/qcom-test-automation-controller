// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "CommandLineParser.h"
#include "StringUtilities.h"

// c++
#include <cctype>
#include <utility>

CommandLineParser::CommandLineParser
(
	const std::vector<std::string> &args
) :
	_args(args)
{
	for (const std::string& arg: _args)
	{
		std::vector<std::string> argElements = split(arg, '=');
		if (argElements.empty() == false)
		{
			if (argElements.size() == 1)
				_values[toLower(argElements.at(0))] = "true";
			else
				_values[toLower(argElements.at(0))] = argElements.at(1);
		}
	}

	_helpRequested = isSet({"-h", "help", "-help", "--help", "/help", "/?"});
	_versionRequested = isSet({"-v", "version"});
}

void CommandLineParser::setVersion
(
	const std::string& version
)
{
	_version = version;
}

bool CommandLineParser::helpRequested()
{
	return _helpRequested;
}

bool CommandLineParser::versionRequested()
{
	if (_version.empty() == false)
		return _versionRequested;

	return false;
}

std::string CommandLineParser::version()
{
	return _version;
}

void CommandLineParser::addHelpLine
(
	const std::string& helpText
)
{
	_helpText += helpText + "\n";
}

void CommandLineParser::addHelpLine()
{
	addHelpLine("   -h, help, -help --help /help /?  -- Displays Help Text");
	addHelpLine("   -v, version                      -- Displays Application Version");
}

const std::string CommandLineParser::helpText()
{
	return _helpText;
}

bool CommandLineParser::isSet
(
	const std::vector<std::string>& keys
)
{
	bool result{false};

	for (const auto& key: keys)
	{
		auto keyInLowerCase = toLowerCase(key);
		if (_values.find(keyInLowerCase) != _values.end())
		{
			result = true;
			break;
		}
	}

	return result;
}

std::string CommandLineParser::value
(
	const std::string& key
)
{
	std::string result;

	std::string keyInLowerCase{toLowerCase(key)};
	if (_values.find(keyInLowerCase) != _values.end())
	{
		result = _values[keyInLowerCase];
	}

	return result;
}

