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
// Copyright 2024 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
            Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "ThreadedLog.h"

// Qt
#include <QDateTime>
#include <QMutexLocker>

void _ThreadedLog::open
(
	const QString& filePath
)
{
	_currentLogPath = filePath;
	_logFile.setFileName(filePath);

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
	const QString& logEntry
)
{
	QMutexLocker lock(&_mutex);

	_logEntries.append(logEntry);
}

void _ThreadedLog::run()
{
	QString logEntry;

	bool opened = _logFile.open(QIODevice::WriteOnly);

	_starting = false;

	_running = opened;
	while (_running)
	{
		{
			QMutexLocker lock(&_mutex);

			if (_logEntries.empty() == false)
			{
				logEntry = _logEntries.takeFirst();
			}
		}

		if (logEntry.isEmpty() == false)
		{
			_logFile.write(logEntry.toLatin1());
			_logFile.flush();
			logEntry.clear();
		}
		else
		{
			msleep(100);
		}
	}

	while (_logEntries.isEmpty() == false)
	{
		QMutexLocker lock(&_mutex);

		logEntry = _logEntries.takeFirst();
		_logFile.write(logEntry.toLatin1());
	}

	_logFile.close();
}

ThreadedLog _ThreadedLog::createThreadedLog()
{
	return ThreadedLog(new _ThreadedLog);
}

QString _ThreadedLog::createLogName(const QString& prefix)
{
	return prefix + QDateTime::currentDateTime().toString("MMdd_HHmmss") + ".log";
}
