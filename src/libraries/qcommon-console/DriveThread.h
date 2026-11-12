#ifndef DRIVETHREAD_H
#define DRIVETHREAD_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "ProtocolInterface.h"
#include "SendInterface.h"
#include "QCommonConsoleGlobal.h"

// Qt
#include <QObject>

// Standard C++
#include <atomic>
#include <chrono>
#include <cstdint>
#include <map>
#include <mutex>
#include <string>
#include <thread>

class QCOMMONCONSOLE_EXPORT DriveThread :
	public QObject,
	public SendInterface
{
Q_OBJECT

public:
	DriveThread();
	virtual ~DriveThread();

	void start();
	void wait();
	bool isRunning()
	{
		return weAreRunning();
	}

	bool weAreRunning()
	{
		std::lock_guard<std::mutex> lock(_runningMutex);
		return _running;
	}

	void shutDown();

	std::string name()
	{
		return _driveTrainName;
	}

	int id()
	{
		return _driveTrainID;
	}

	std::string lastErrorMessage()
	{
		std::string result(_lastErrorMessage);
		_lastErrorMessage.clear();
		return result;
	}

	void setProtocolInterface(ProtocolInterface* protocolInterface);

	virtual void run() = 0;

	// Send Interface
	virtual uint32_t send(const std::string& sendMe, const Arguments& arguments, bool console, ReceiveInterface* recieveInterface, bool store = true) = 0;
	virtual void addDelay(uint32_t delayInMilliSeconds, ReceiveInterface* recieveInterface);
	virtual void addLogComment(const std::string& comment);
	virtual void addEndTransaction(ReceiveInterface* receiveInterface);

protected:
	void startRunning()
	{
		std::lock_guard<std::mutex> lock(_runningMutex);
		_running = true;
	}

	bool stopRunning()
	{
		bool result;

		std::lock_guard<std::mutex> lock(_runningMutex);
		result = _running;
		_running = false;

		return result;
	}

	static int					_driveTrainIDs;

	int							_driveTrainID{0};
	ProtocolInterface*			_protocolInterface{nullptr};

	std::string					_driveTrainName{"<unnamed>"};
	std::string                 _lastErrorMessage;

private:
	std::thread					_workerThread;
	std::mutex					_runningMutex;
	bool						_running{false};
};

typedef std::map<int, DriveThread*> DriveTrains;

#endif // DRIVETHREAD_H
