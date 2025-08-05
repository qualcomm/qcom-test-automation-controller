/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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

