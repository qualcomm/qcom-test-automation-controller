#ifndef DRIVETHREAD_H
#define DRIVETHREAD_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "ProtocolInterface.h"
#include "SendInterface.h"
#include "QCommonConsoleGlobal.h"

// Standard C++
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <map>
#include <mutex>
#include <stop_token>
#include <string>
#include <thread>

class QCOMMONCONSOLE_EXPORT DriveThread :
	public SendInterface
{
public:
	DriveThread();
	virtual ~DriveThread();

	void start();
	void wait();
	bool isRunning() const
	{
		return weAreRunning();
	}

	bool hasFinished() const noexcept
	{
		return _finished.load();
	}

	bool weAreRunning() const
	{
		return _running.load();
	}

	bool stopRequested() const noexcept
	{
		return !_running.load() || _stopToken.stop_requested();
	}

	std::stop_token getStopToken() const noexcept
	{
		return _stopToken;
	}

	void requestStop() noexcept;
	void shutDown();
	void wake() noexcept;

	// Interruptible wait: sleeps up to 'ms', but returns immediately if awakened or stop requested.
	// Returns true if timed out normally without stop; false if interrupted or stop requested.
	bool sleepForOrStop(std::chrono::milliseconds ms);

	const std::string& name() const
	{
		return _driveTrainName;
	}

	int id() const
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

	// Lifecycle hooks for derived classes
	virtual void onStarting() {}
	virtual void onStopping() {}
	virtual void onFinished() {}

	// Send Interface
	virtual uint32_t send(const std::string& sendMe, const Arguments& arguments, bool console, ReceiveInterface* recieveInterface, bool store = true) = 0;
	virtual void addDelay(uint32_t delayInMilliSeconds, ReceiveInterface* recieveInterface);
	virtual void addLogComment(const std::string& comment);
	virtual void addEndTransaction(ReceiveInterface* receiveInterface);

protected:
	void startRunning()
	{
		_running.store(true);
	}

	bool stopRunning()
	{
		return _running.exchange(false);
	}

	static std::atomic<int>		_driveTrainIDs;

	int							_driveTrainID{0};
	ProtocolInterface*			_protocolInterface{nullptr};

	std::string					_driveTrainName{"<unnamed>"};
	std::string                 _lastErrorMessage;

private:
	std::jthread				_workerThread;
	std::stop_token				_stopToken;
	std::atomic<bool>			_running{false};
	std::atomic<bool>			_finished{false};
	std::mutex					_lifecycleMutex;
	std::condition_variable		_wakeCv;
	std::mutex					_wakeMutex;
};

typedef std::map<int, DriveThread*> DriveTrains;

#endif // DRIVETHREAD_H
