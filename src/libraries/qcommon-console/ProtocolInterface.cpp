// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "ProtocolInterface.h"
#include "TickCount.h"

#include <chrono>
#include <mutex>
#include <thread>

#include <cstdint>

uint32_t ProtocolInterface::_sendID(0);

ProtocolInterface::~ProtocolInterface()
{
	std::lock_guard<std::recursive_mutex> lock(_outQueueMutex);
	_outboundData.clear();

	if (_frameCoder != nullptr)
	{
		delete _frameCoder;
		_frameCoder = nullptr;
	}
}

uint32_t ProtocolInterface::getNextSendID()
{
	std::lock_guard<std::recursive_mutex> lock(_outQueueMutex);

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
	std::lock_guard<std::recursive_mutex> lock(_pendingFramePackageMutex);

	if (_pendingFramePackage != nullptr)
		_pendingFramePackage = nullptr;
}

FramePackage& ProtocolInterface::pendingFramePackage()
{
	std::lock_guard<std::recursive_mutex> lock(_pendingFramePackageMutex);
	return _pendingFramePackage;
}

void ProtocolInterface::queueCommand
(
	const FramePackage& framePackage
)
{		
	std::lock_guard<std::recursive_mutex> lock(_outQueueMutex);
	_outboundData.push_back(framePackage);
}

void ProtocolInterface::queueDelay
(
	uint32_t delayInMilliSeconds,
	ReceiveInterface* recieveInterface
)
{
	if (delayInMilliSeconds > 0)
	{
		FramePackage framePackage = FramePackage(new _FramePackage);

		framePackage->_recieveInterface = recieveInterface;
		framePackage->_delayInMilliSeconds = delayInMilliSeconds;
		framePackage->_request = "delay: " + std::to_string(delayInMilliSeconds);

		std::lock_guard<std::recursive_mutex> lock(_outQueueMutex);
		_outboundData.push_back(framePackage);
	}
}

void ProtocolInterface::queueLogComment
(
	const std::string& comment
)
{
	FramePackage framePackage = FramePackage(new _FramePackage);

	framePackage->_comment = comment;

	std::lock_guard<std::recursive_mutex> lock(_outQueueMutex);
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
	framePackage->_request = "End Transaction";

	std::lock_guard<std::recursive_mutex> lock(_outQueueMutex);
	_outboundData.push_back(framePackage);
}

FramePackage ProtocolInterface::getNextFramePackage()
{
	FramePackage result;
	bool noPendingFrame;

	{
		std::lock_guard<std::recursive_mutex> lock(_pendingFramePackageMutex);
		noPendingFrame = (_pendingFramePackage == nullptr);
	}

	if (noPendingFrame)
	{
		std::lock_guard<std::recursive_mutex> lock(_outQueueMutex);

		if (_outboundData.empty() == false)
		{
			FramePackage framePackage = _outboundData.front();
			_outboundData.erase(_outboundData.begin());

			if (framePackage->_delayInMilliSeconds != 0)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(framePackage->_delayInMilliSeconds));
				result = framePackage;
			}
			else if (framePackage->_endTransaction)
			{
				result = framePackage;
			}
			else if (framePackage->_comment.empty() == false)
			{
				result = framePackage;
			}
			else
			{
				result = framePackage;
				framePackage->_tickcount = tickCount();

				std::lock_guard<std::recursive_mutex> lock(_pendingFramePackageMutex);
				_pendingFramePackage = framePackage;
			}
		}
	}

	return result;
}

uint32_t ProtocolInterface::queueSize()
{
	std::lock_guard<std::recursive_mutex> lock(_outQueueMutex);

	return static_cast<uint32_t>(_outboundData.size());
}

bool ProtocolInterface::handleRecievedData
(
	const std::string& receivedData
)
{
	_frameCoder->decode(receivedData);

	return true;
}

void ProtocolInterface::frameCompleteFunc
(
	const std::string& completedFrame, 
	ProtocolInterface* protocolInterface
)
{
	protocolInterface->frameComplete(completedFrame);
}

void ProtocolInterface::badFrameFunc
(
	const std::string& completedFrame,
	ProtocolInterface* protocolInterface
)
{
	protocolInterface->clearPendingFrame();

	protocolInterface->badFrame(completedFrame);
}
