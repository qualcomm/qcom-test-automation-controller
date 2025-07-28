#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H
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
