// Confidential and Proprietary – Qualcomm Technologies, Inc.

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
// Copyright ©2018 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

// Author: msimpson

// QCommon
#include "ProtocolInterface.h"
#include "TickCount.h"

// QT
#include <QDir>
#include <QMutexLocker>
#include <QThread>


quint32 ProtocolInterface::_sendID(0);

ProtocolInterface::~ProtocolInterface()
{
	QMutexLocker lock(&_outQueueMutex);
	_outboundData.clear();

	if (_frameCoder != nullptr)
	{
		delete _frameCoder;
		_frameCoder = nullptr;
	}
}

quint32 ProtocolInterface::getNextSendID()
{
	QMutexLocker lock(&_outQueueMutex);

	if (ProtocolInterface::_sendID >= kBadQueueValue - 1)
		ProtocolInterface::_sendID = 1;
	else
		ProtocolInterface::_sendID++;

	return ProtocolInterface::_sendID;
}

void ProtocolInterface::setFrameCoder
(
	FrameCoder* frameCoder
)
{
	_frameCoder = frameCoder;
	_frameCoder->setupCallbackFunctions(this, ProtocolInterface::frameCompleteFunc, ProtocolInterface::badFrameFunc);
}

void ProtocolInterface::clearPendingFrame()
{
	QMutexLocker lock(&_pendingFramePackageMutex);

	if (_pendingFramePackage.isNull() == false)
		_pendingFramePackage = FramePackage(Q_NULLPTR);
}

FramePackage& ProtocolInterface::pendingFramePackage()
{
	QMutexLocker lock(&_pendingFramePackageMutex);
	return _pendingFramePackage;
}

void ProtocolInterface::queueCommand
(
	const FramePackage& framePackage
)
{		
	QMutexLocker lock(&_outQueueMutex);
	_outboundData.push_back(framePackage);
}

void ProtocolInterface::queueDelay
(
	quint32 delayInMilliSeconds,
	ReceiveInterface* recieveInterface
)
{
	if (delayInMilliSeconds > 0)
	{
		FramePackage framePackage = FramePackage(new _FramePackage);

		framePackage->_recieveInterface = recieveInterface;
		framePackage->_delayInMilliSeconds = delayInMilliSeconds;
		framePackage->_request = QString("delay: %1").arg(delayInMilliSeconds).toLatin1();

		QMutexLocker lock(&_outQueueMutex);
		_outboundData.push_back(framePackage);
	}
}

void ProtocolInterface::queueLogComment
(
	const QByteArray& comment
)
{
	FramePackage framePackage = FramePackage(new _FramePackage);

	framePackage->_comment = comment;

	QMutexLocker lock(&_outQueueMutex);
	_outboundData.push_back(framePackage);
}

void ProtocolInterface::queueEndTransaction
(
	ReceiveInterface* receiveInterface
)
{
	FramePackage framePackage = FramePackage(new _FramePackage);

	framePackage->_recieveInterface = receiveInterface;
	framePackage->_endTransaction = true;
	framePackage->_request = QByteArray("End Transaction");

	QMutexLocker lock(&_outQueueMutex);
	_outboundData.push_back(framePackage);
}

FramePackage ProtocolInterface::getNextFramePackage()
{
	FramePackage result;
	bool noPendingFrame;

	{
		QMutexLocker lock(&_pendingFramePackageMutex);
		noPendingFrame = _pendingFramePackage.isNull() == true;
	}

	if (noPendingFrame)
	{
		QMutexLocker lock(&_outQueueMutex);

		if (_outboundData.isEmpty() == false)
		{
			FramePackage framePackage = _outboundData.first();
			_outboundData.pop_front();

			if (framePackage->_delayInMilliSeconds != 0)
			{
				QThread::currentThread()->msleep(framePackage->_delayInMilliSeconds);
				result = framePackage;
			}
			else if (framePackage->_endTransaction)
			{
				result = framePackage;
			}
			else if (framePackage->_comment.isEmpty() == false)
			{
				result = framePackage;
			}
			else
			{
				result = framePackage;
				framePackage->_tickcount = tickCount();

				QMutexLocker lock(&_pendingFramePackageMutex);
				_pendingFramePackage = framePackage;
			}
		}
	}

	return result;
}

quint32 ProtocolInterface::queueSize()
{
	QMutexLocker lock(&_outQueueMutex);

	return static_cast<quint32>(_outboundData.count());
}

bool ProtocolInterface::handleRecievedData
(
	const QByteArray& receivedData
)
{
	_frameCoder->decode(receivedData);

	return true;
}

void ProtocolInterface::frameCompleteFunc
(
	const QByteArray& completedFrame, 
	ProtocolInterface* protocolInterface
)
{
	protocolInterface->frameComplete(completedFrame);
}

void ProtocolInterface::badFrameFunc
(
	const QByteArray& completedFrame,
	ProtocolInterface* protocolInterface
)
{
	protocolInterface->clearPendingFrame();

	protocolInterface->badFrame(completedFrame);
}
