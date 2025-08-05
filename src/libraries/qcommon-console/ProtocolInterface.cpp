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
