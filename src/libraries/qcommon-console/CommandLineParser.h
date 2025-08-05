#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// QCommon
#include "QCommonConsoleGlobal.h"

// QT
#include <QMap>
#include <QString>
#include <QStringList>

typedef QMap<QString, QString> KeyValuePairs;

class QCOMMONCONSOLE_EXPORT CommandLineParser
{
public:
	CommandLineParser(const QStringList& args);

	void setVersion(const QByteArray& version);
	bool versionRequested();
	QByteArray version();

	bool helpRequested();

	void addHelpLine(const QString& helpText);
	void addHelpLine();
	const QString helpText();

	bool isSet(const QStringList& keys);
	QString value(const QString& key);

protected:
	QStringList					_args;
	KeyValuePairs				_values;

	bool						_helpRequested{false};
	QString						_helpText;

	bool						_versionRequested{false};
	QByteArray					_version;
};

#endif // COMMANDLINEPARSER_H
