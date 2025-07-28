#ifndef THREADEDLOG_H
#define THREADEDLOG_H
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
// Copyright 2013-2018 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

class _ThreadedLog;

#include "QCommonConsoleGlobal.h"

// Qt
#include <QFile>
#include <QRecursiveMutex>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QThread>

// C++
#include <atomic>

typedef QSharedPointer<_ThreadedLog>  ThreadedLog;

class QCOMMONCONSOLE_EXPORT _ThreadedLog :
	public QThread
{
public:
	_ThreadedLog() = default;

	static ThreadedLog createThreadedLog();
	static QString createLogName(const QString& prefix);

	void open(const QString& filePath);
	bool isOpen();
	void close();

	QString currentLogPath()
	{
		return _currentLogPath;
	}

	void addLogEntry(const QString& logEntry);

	virtual void run() override;

protected:
	std::atomic<bool>			_running{false};
	std::atomic<bool>			_starting{false};
	QRecursiveMutex				_mutex;
	QStringList					_logEntries;
	QString						_currentLogPath;
	QFile						_logFile;
};

#endif // THREADEDLOG_H
