// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "DriveThread.h"

int DriveThread::_driveTrainIDs(1);

DriveThread::DriveThread()
{
	_driveTrainID = _driveTrainIDs++;
}

DriveThread::~DriveThread()
{

}

void DriveThread::shutDown()
{
	if (stopRunning() == true)
	{
		while (isRunning() == true)
		{
			QThread::sleep(1);
		}
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
