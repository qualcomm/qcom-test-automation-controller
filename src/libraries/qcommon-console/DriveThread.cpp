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

void DriveThread::requestStop()
{
	stopRunning();

	// run() drains the queue before it exits. Without this a queued script
	// delay (up to 120 s from a quick command timing variable) had to run
	// out first, and the caller - the UI thread on Disconnect - hung that
	// long. Skipping the delays still sends the remaining commands, so a
	// held button is still released.
	if (_protocolInterface != Q_NULLPTR)
		_protocolInterface->setSkipDelays(true);
}

void DriveThread::shutDown()
{
	// Also covers a thread that already stopped itself (write failure) or was
	// stopped by requestStop(): wait for run() to actually return either way.
	requestStop();

	// Called on this thread itself (a slot run by a direct connection from
	// run()): waiting for run() to return would wait forever. Stopping is all
	// that can be done here; the owner thread does the blocking shutdown.
	if (QThread::currentThread() == this)
		return;

	while (isRunning() == true)
	{
		QThread::msleep(50);
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
