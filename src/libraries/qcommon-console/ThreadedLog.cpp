// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ThreadedLog.h"
#include "StringUtilities.h"

// C++
#include <mutex>
#include <string>

void _ThreadedLog::open
(
	const std::string& filePath
)
{
	_currentLogPath = filePath;
	_logFile.setFileName(QString::fromStdString(filePath));

	_starting = true;

	start(QThread::LowPriority);

	while (_starting)
		QThread::currentThread()->msleep(100);
}

bool _ThreadedLog::isOpen()
{
	return _running;
}

void _ThreadedLog::close()
{
    if (isRunning())
	{
		_running = false;
        _currentLogPath.clear();

        while (isRunning())
        {
            if (isInterruptionRequested() == false)
            {
                requestInterruption();
                quit();
            }

            wait();
        }
	}
}

void _ThreadedLog::addLogEntry
(
	const std::string& logEntry
)
{
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	_logEntries.push_back(logEntry);
}

void _ThreadedLog::run()
{
	std::string logEntry;

	bool opened = _logFile.open(QIODevice::WriteOnly);

	_starting = false;

	_running = opened;
	while (_running)
	{
		{
			std::lock_guard<std::recursive_mutex> lock(_mutex);

			if (!_logEntries.empty())
			{
				logEntry = _logEntries.front();
				_logEntries.erase(_logEntries.begin());
			}
		}

		if (!logEntry.empty())
		{
			_logFile.write(logEntry.data(), logEntry.size());
			_logFile.flush();
			logEntry.clear();
		}
		else
		{
			msleep(100);
		}
	}

	while (!_logEntries.empty())
	{
		std::lock_guard<std::recursive_mutex> lock(_mutex);

		logEntry = _logEntries.front();
		_logEntries.erase(_logEntries.begin());
		_logFile.write(logEntry.data(), logEntry.size());
	}

	_logFile.close();
}

ThreadedLog _ThreadedLog::createThreadedLog()
{
	return std::make_shared<_ThreadedLog>();
}

std::string _ThreadedLog::createLogName(const std::string& prefix)
{
	return prefix + formatDateTime("%m%d_%H%M%S") + ".log";
}
