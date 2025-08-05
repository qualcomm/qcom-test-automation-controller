#ifndef DRIVETHREAD_H
#define DRIVETHREAD_H
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
