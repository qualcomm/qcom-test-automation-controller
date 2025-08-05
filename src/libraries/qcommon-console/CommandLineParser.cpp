// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "CommandLineParser.h"

// c++
#include <utility>

CommandLineParser::CommandLineParser
(
	const QStringList &args
) :
	_args(args)
{
	for (const QString& arg: std::as_const(_args))
	{
		QStringList argElements = arg.split("=");
		if (argElements.isEmpty() == false)
		{
			if (argElements.count() == 1)
				_values[argElements.at(0).toLower()] = "true";
			else
				_values[argElements.at(0).toLower()] = argElements.at(1);
		}
	}

	_helpRequested = isSet(QStringList() << "-h" << "help" << "-help" << "--help" << "/help" << "/?");
	_versionRequested = isSet(QStringList() << "-v" << "version");
}

void CommandLineParser::setVersion
(
	const QByteArray& version
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
	if (_version.isEmpty() == false)
		return _versionRequested;

	return false;
}

QByteArray CommandLineParser::version()
{
	return _version;
}

void CommandLineParser::addHelpLine
(
	const QString& helpText
)
{
	_helpText += helpText + "\n";
}

void CommandLineParser::addHelpLine()
{
	addHelpLine("   -h, help, -help --help /help /?  -- Displays Help Text");
	addHelpLine("   -v, version                      -- Displays Application Version");
}

const QString CommandLineParser::helpText()
{
	return _helpText;
}

bool CommandLineParser::isSet
(
	const QStringList& keys
)
{
	bool result{false};

	for (const auto& key: std::as_const(keys))
	{
		auto keyInLowerCase = key.toLower();
		if (_values.find(keyInLowerCase) != _values.end())
		{
			result = true;
			break;
		}
	}

	return result;
}

QString CommandLineParser::value
(
	const QString& key
)
{
	QString result;

	QString keyInLowerCase{key.toLower()};
	if (_values.find(keyInLowerCase) != _values.end())
	{
		result = _values[keyInLowerCase];
	}

	return result;
}

