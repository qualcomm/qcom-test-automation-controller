#ifndef THREADEDLOG_H
#define THREADEDLOG_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
