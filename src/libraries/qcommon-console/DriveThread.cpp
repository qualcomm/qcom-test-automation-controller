// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "DriveThread.h"
#include "AppCore.h"

std::atomic<int> DriveThread::_driveTrainIDs(1);

DriveThread::DriveThread()
{
	_driveTrainID = _driveTrainIDs.fetch_add(1);
}

DriveThread::~DriveThread()
{
	shutDown();
}

void DriveThread::start()
{
	std::lock_guard<std::mutex> lock(_lifecycleMutex);
	if (weAreRunning())
		return;

	if (_workerThread.joinable())
		_workerThread.join();

	_finished.store(false);
	startRunning();

	_workerThread = std::jthread([this](std::stop_token stopToken) {
		_stopToken = stopToken;
		try
		{
			onStarting();
			run();
		}
		catch (const std::exception& e)
		{
			_lastErrorMessage = std::string("DriveThread exception in '") + _driveTrainName + "': " + e.what();
			AppCore::writeToApplicationLogLine(_lastErrorMessage);
		}
		catch (...)
		{
			_lastErrorMessage = std::string("DriveThread unknown exception in '") + _driveTrainName + "'";
			AppCore::writeToApplicationLogLine(_lastErrorMessage);
		}
		stopRunning();
		wake();
		try
		{
			onFinished();
		}
		catch (const std::exception& e)
		{
			AppCore::writeToApplicationLogLine("DriveThread exception in onFinished(): " + std::string(e.what()));
		}
		catch (...)
		{
			AppCore::writeToApplicationLogLine("DriveThread unknown exception in onFinished()");
		}
		_finished.store(true);
	});
}

void DriveThread::wait()
{
	if (_workerThread.joinable())
		_workerThread.join();
	_finished.store(true);
}

void DriveThread::requestStop() noexcept
{
	stopRunning();
	_workerThread.request_stop();
	wake();
}

void DriveThread::wake() noexcept
{
	_wakeCv.notify_all();
}

bool DriveThread::sleepForOrStop(std::chrono::milliseconds ms)
{
	std::unique_lock<std::mutex> lock(_wakeMutex);
	bool stopWoke = _wakeCv.wait_for(lock, ms, [this]() {
		return stopRequested();
	});
	return !stopWoke;
}

void DriveThread::shutDown()
{
	std::lock_guard<std::mutex> lock(_lifecycleMutex);
	if (stopRunning())
	{
		_workerThread.request_stop();
		wake();
		try
		{
			onStopping();
		}
		catch (const std::exception& e)
		{
			AppCore::writeToApplicationLogLine("DriveThread exception in onStopping(): " + std::string(e.what()));
		}
		catch (...)
		{
			AppCore::writeToApplicationLogLine("DriveThread unknown exception in onStopping()");
		}
	}

	if (_workerThread.joinable())
		_workerThread.join();
	_finished.store(true);
}

void DriveThread::addDelay
(
	uint32_t delayInMilliSeconds,
	ReceiveInterface* recieveInterface
)
{
	if (_protocolInterface != nullptr)
	{
		_protocolInterface->queueDelay(delayInMilliSeconds, recieveInterface);
		wake();
	}
}

void DriveThread::addLogComment(const std::string& comment)
{
	if (_protocolInterface != nullptr)
	{
		_protocolInterface->queueLogComment(comment);
		wake();
	}
}

void DriveThread::addEndTransaction
(
	ReceiveInterface* receiveInterface
)
{
	if (_protocolInterface != nullptr)
	{
		_protocolInterface->queueEndTransaction(receiveInterface);
		wake();
	}
}

void DriveThread::setProtocolInterface
(
	ProtocolInterface* protocolInterface
)
{
	_protocolInterface = protocolInterface;
}
