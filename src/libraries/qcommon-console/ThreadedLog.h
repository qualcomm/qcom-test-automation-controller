#ifndef THREADEDLOG_H
#define THREADEDLOG_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

class _ThreadedLog;

#include "QCommonConsoleGlobal.h"

// Standard C++
#include <atomic>
#include <condition_variable>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

typedef std::shared_ptr<_ThreadedLog> ThreadedLog;

class QCOMMONCONSOLE_EXPORT _ThreadedLog
{
public:
	_ThreadedLog();
	~_ThreadedLog();

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

protected:
	void workerLoop();

	std::atomic<bool>			_running{false};
	std::thread					_workerThread;
	std::mutex					_mutex;
	std::condition_variable		_cv;
	std::vector<std::string>	_logEntries;
	std::string					_currentLogPath;
	std::ofstream				_logFile;
};

#endif // THREADEDLOG_H
