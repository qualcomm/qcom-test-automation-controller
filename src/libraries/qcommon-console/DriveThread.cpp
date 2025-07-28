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

void DriveThread::setConsoleInterface
(
	ConsoleInterface* consoleInterface
)
{
	_consoleInterface = consoleInterface;
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
