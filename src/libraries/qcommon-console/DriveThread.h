#ifndef DRIVETHREAD_H
#define DRIVETHREAD_H
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
// Copyright 2013-2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
