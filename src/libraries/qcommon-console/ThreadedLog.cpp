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
