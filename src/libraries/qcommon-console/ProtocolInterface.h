#ifndef PROTOCOLINTERFACE_H
#define PROTOCOLINTERFACE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "FrameCoder.h"
#include "FramePackage.h"
#include "QCommonConsoleGlobal.h"

// Qt
#include <QObject>

#include <chrono>
#include <cstdint>
#include <mutex>
#include <string>

const uint32_t kBadQueueValue(0xFFFFFFFF);

class QCOMMONCONSOLE_EXPORT ProtocolInterface
{
public:
	ProtocolInterface() = default;
	virtual ~ProtocolInterface();

	void setFrameCoder(FrameCoder* frameCoder);

	// Out going data
	void clearPendingFrame();
	FramePackage& pendingFramePackage();

	void queueCommand(const FramePackage& framePackage);
	void queueDelay(uint32_t delayInMilliSeconds, ReceiveInterface* receiveInterface = nullptr);
	void queueLogComment(const std::string& comment);
	void queueEndTransaction(ReceiveInterface* receiveInterface = nullptr);

	FramePackage getNextFramePackage();
	uint32_t queueSize();

	// Data incoming
	bool handleRecievedData(const std::string& receivedData);

	// timing
	virtual void idle() = 0;
	virtual void setBaseTime(const std::chrono::system_clock::time_point& baseTime = std::chrono::system_clock::now())
	{
		_baseTime = baseTime;
	}

	void setTimeout(uint64_t timeout)
	{
		_timeout = (timeout < 750) ? _timeout : 750;
	}

protected:
	std::chrono::system_clock::time_point	_baseTime;
	FrameCoder*								_frameCoder{nullptr};
	uint64_t								_timeout{750};

	static uint32_t							_sendID;

	virtual void frameComplete(const std::string& completedFrame) = 0;
	virtual void badFrame(const std::string& completedFrame) = 0;

	static void frameCompleteFunc(const std::string& completedFrame, ProtocolInterface* userData);
	static void badFrameFunc(const std::string& completedFrame, ProtocolInterface* userData);

	uint32_t getNextSendID();

private:
	std::recursive_mutex		_pendingFramePackageMutex;
	FramePackage				_pendingFramePackage{nullptr};

	std::recursive_mutex		_outQueueMutex;
	FramePackageList			_outboundData;
};

#define ProtocolInterface_iid "com.qualcomm.simpson.ProtocolInterface"

Q_DECLARE_INTERFACE(ProtocolInterface, ProtocolInterface_iid)

#endif // PROTOCOLINTERFACE_H
