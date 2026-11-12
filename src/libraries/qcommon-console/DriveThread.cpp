// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "DriveThread.h"

int DriveThread::_driveTrainIDs(1);

DriveThread::DriveThread() :
	QObject(nullptr)
{
	_driveTrainID = _driveTrainIDs++;
}

DriveThread::~DriveThread()
{
	shutDown();
}

void DriveThread::start()
{
	startRunning();
	if (_workerThread.joinable())
		_workerThread.join();

	_workerThread = std::thread([this]() {
		run();
		stopRunning();
	});
}

void DriveThread::wait()
{
	if (_workerThread.joinable())
		_workerThread.join();
}

void DriveThread::shutDown()
{
	if (stopRunning() == true)
	{
		if (_workerThread.joinable())
			_workerThread.join();
	}
}

void DriveThread::addDelay
(
	uint32_t delayInMilliSeconds,
	ReceiveInterface* recieveInterface
)
{
	if (_protocolInterface != nullptr)
		_protocolInterface->queueDelay(delayInMilliSeconds, recieveInterface);
}

void DriveThread::addLogComment(const std::string& comment)
{
	if (_protocolInterface != nullptr)
		_protocolInterface->queueLogComment(comment);
}

void DriveThread::addEndTransaction
(
	ReceiveInterface* receiveInterface
)
{
	if (_protocolInterface != nullptr)
		_protocolInterface->queueEndTransaction(receiveInterface);
}

void DriveThread::setProtocolInterface
(
	ProtocolInterface* protocolInterface
)
{
	_protocolInterface = protocolInterface;
}
