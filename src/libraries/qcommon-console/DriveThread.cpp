// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "DriveThread.h"

// QT
#include <QDateTime>
#include <QDir>

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
	quint32 delayInMilliSeconds,
	ReceiveInterface* recieveInterface
)
{
	if (_protocolInterface != Q_NULLPTR)
		_protocolInterface->queueDelay(delayInMilliSeconds, recieveInterface);
}

void DriveThread::addLogComment(const QByteArray& comment)
{
	if (_protocolInterface != Q_NULLPTR)
		_protocolInterface->queueLogComment(comment);
}

void DriveThread::addEndTransaction
(
	ReceiveInterface* receiveInterface
)
{
	if (_protocolInterface != Q_NULLPTR)
		_protocolInterface->queueEndTransaction(receiveInterface);
}

void DriveThread::setProtocolInterface
(
	ProtocolInterface* protocolInterface
)
{
	_protocolInterface = protocolInterface;
}
