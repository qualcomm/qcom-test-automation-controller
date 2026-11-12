#ifndef PROTOCOLINTERFACE_H
#define PROTOCOLINTERFACE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "FrameCoder.h"
#include "FramePackage.h"
#include "QCommonConsoleGlobal.h"

// Qt
#include <QByteArray>
#include <QDateTime>
#include <QObject>

#include <cstdint>
#include <mutex>

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
	void queueDelay(uint32_t delayInMilliSeconds, ReceiveInterface* receiveInterface = Q_NULLPTR);
	void queueLogComment(const QByteArray& comment);
	void queueEndTransaction(ReceiveInterface* receiveInterface = Q_NULLPTR);

	FramePackage getNextFramePackage();
	uint32_t queueSize();

	// Data incoming
	bool handleRecievedData(const QByteArray& receivedData);

	// timing
	virtual void idle() = 0;
	virtual void setBaseTime(const QDateTime& baseTime)
	{
		_baseTime = baseTime;
	}

	void setTimeout(uint64_t timeout)
	{
		_timeout = (timeout < 750) ? _timeout : 750;
	}

protected:
	QDateTime					_baseTime;
	FrameCoder*					_frameCoder{Q_NULLPTR};
	uint64_t					_timeout{750};

	static uint32_t				_sendID;

	virtual void frameComplete(const QByteArray& completedFrame) = 0;
	virtual void badFrame(const QByteArray& completedFrame) = 0;

	static void frameCompleteFunc(const QByteArray& completedFrame, ProtocolInterface* userData);
	static void badFrameFunc(const QByteArray& completedFrame, ProtocolInterface* userData);

	uint32_t getNextSendID();

private:
	std::recursive_mutex		_pendingFramePackageMutex;
	FramePackage				_pendingFramePackage{FramePackage(Q_NULLPTR)};

	std::recursive_mutex		_outQueueMutex;
	FramePackageList			_outboundData;
};

#define ProtocolInterface_iid "com.qualcomm.simpson.ProtocolInterface"

Q_DECLARE_INTERFACE(ProtocolInterface, ProtocolInterface_iid)

#endif // PROTOCOLINTERFACE_H
