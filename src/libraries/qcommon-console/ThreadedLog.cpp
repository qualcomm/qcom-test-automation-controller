// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ThreadedLog.h"
#include "StringUtilities.h"

#include <chrono>

_ThreadedLog::_ThreadedLog()
{
}

_ThreadedLog::~_ThreadedLog()
{
	close();
}

void _ThreadedLog::open(const std::string& filePath)
{
	close();

	_currentLogPath = filePath;
	_logFile.open(filePath, std::ios::out | std::ios::app);
	if (!_logFile.is_open())
	{
		_currentLogPath.clear();
		return;
	}

	_running = true;
	_workerThread = std::thread(&_ThreadedLog::workerLoop, this);
}

bool _ThreadedLog::isOpen()
{
	return _running && _logFile.is_open();
}

void _ThreadedLog::close()
{
	if (_running)
	{
		_running = false;
		_cv.notify_all();

		if (_workerThread.joinable())
			_workerThread.join();

		_currentLogPath.clear();
	}
}

void _ThreadedLog::addLogEntry(const std::string& logEntry)
{
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_logEntries.push_back(logEntry);
	}
	_cv.notify_one();
}

void _ThreadedLog::workerLoop()
{
	while (_running)
	{
		std::vector<std::string> entries;
		{
			std::unique_lock<std::mutex> lock(_mutex);
			_cv.wait_for(lock, std::chrono::milliseconds(100), [this]() {
				return !_running || !_logEntries.empty();
			});

			entries.swap(_logEntries);
		}

		for (const auto& entry : entries)
		{
			if (_logFile.is_open())
				_logFile.write(entry.data(), entry.size());
		}

		if (_logFile.is_open() && !entries.empty())
			_logFile.flush();
	}

	// Drain remaining entries upon shutdown
	std::vector<std::string> remaining;
	{
		std::lock_guard<std::mutex> lock(_mutex);
		remaining.swap(_logEntries);
	}

	for (const auto& entry : remaining)
	{
		if (_logFile.is_open())
			_logFile.write(entry.data(), entry.size());
	}

	if (_logFile.is_open())
	{
		_logFile.flush();
		_logFile.close();
	}
}

ThreadedLog _ThreadedLog::createThreadedLog()
{
	return std::make_shared<_ThreadedLog>();
}

std::string _ThreadedLog::createLogName(const std::string& prefix)
{
	return prefix + formatDateTime("%m%d_%H%M%S") + ".log";
}
