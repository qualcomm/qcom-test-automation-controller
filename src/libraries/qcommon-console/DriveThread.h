#ifndef DRIVETHREAD_H
#define DRIVETHREAD_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// QCommon
#include "ConsoleInterface.h"
#include "ProtocolInterface.h"
#include "SendInterface.h"
#include "QCommonConsoleGlobal.h"

// QT
#include <QFile>
#include <QIODevice>
#include <QMap>
#include <QMutex>
#include <QThread>

class QCOMMONCONSOLE_EXPORT DriveThread :
	public QThread,
	public SendInterface
{
Q_OBJECT

public:
	DriveThread();
	~DriveThread();
	
	// QThread has an isRunning, not working so adding this
	bool weAreRunning()
	{
		QMutexLocker lock(&_runningMutex);
		return _running;
	}

	void shutDown();

	QString name()
	{
		return _driveTrainName;
	}

	int id()
	{
		return _driveTrainID;
	}

	QByteArray lastErrorMessage()
	{
		QByteArray result(_lastErrorMessage);
		_lastErrorMessage.clear();
		return result;
	}


	void setConsoleInterface(ConsoleInterface* consoleInterface);
	void setProtocolInterface(ProtocolInterface* protocolInterface);

	virtual void run() = 0;

	// Send Interface
	virtual quint32 send(const QByteArray& sendMe, const Arguments& arguments, bool console, ReceiveInterface* recieveInterface, bool store = true) = 0;
	virtual void addDelay(quint32 delayInMilliSeconds, ReceiveInterface* recieveInterface);
	virtual void addLogComment(const QByteArray& comment);
	virtual void addEndTransaction(ReceiveInterface* receiveInterface);

protected:
	void startRunning()
	{
		QMutexLocker lock(&_runningMutex);
		_running = true;
	}

	bool stopRunning()
	{
		bool result;

		QMutexLocker lock(&_runningMutex);
		result = _running;
		_running = false;

		return result;
	}

	static int					_driveTrainIDs;

	int							_driveTrainID{0};
	ConsoleInterface*			_consoleInterface{Q_NULLPTR};
	ProtocolInterface*			_protocolInterface{Q_NULLPTR};
	
	QString						_driveTrainName{"<unnamed>"};
	QByteArray                  _lastErrorMessage;

private:
	QMutex						_runningMutex;
	bool						_running{false};

};

typedef QMap<int, DriveThread*> DriveTrains;

#endif // DRIVETHREAD_H
