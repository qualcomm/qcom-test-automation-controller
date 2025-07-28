// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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

