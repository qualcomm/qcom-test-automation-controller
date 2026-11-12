#ifndef THREADEDLOG_H
#define THREADEDLOG_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

class _ThreadedLog;

#include "QCommonConsoleGlobal.h"

// Qt
#include <QFile>
#include <QThread>

// C++
#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

typedef std::shared_ptr<_ThreadedLog> ThreadedLog;

class QCOMMONCONSOLE_EXPORT _ThreadedLog :
	public QThread
{
public:
	_ThreadedLog() = default;

	static ThreadedLog createThreadedLog();
	static std::string createLogName(const std::string& prefix);

	void open(const std::string& filePath);
	bool isOpen();
	void close();

	std::string currentLogPath()
	{
		return _currentLogPath;
	}

	void addLogEntry(const std::string& logEntry);

	virtual void run() override;

protected:
	std::atomic<bool>			_running{false};
	std::atomic<bool>			_starting{false};
	std::recursive_mutex		_mutex;
	std::vector<std::string>	_logEntries;
	std::string					_currentLogPath;
	QFile						_logFile;
};

#endif // THREADEDLOG_H
