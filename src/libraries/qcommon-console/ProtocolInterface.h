#ifndef PROTOCOLINTERFACE_H
#define PROTOCOLINTERFACE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

// Author: msimpson

// QCommon
#include "FrameCoder.h"
#include "FramePackage.h"
#include "QCommonConsoleGlobal.h"

// Qt
#include <QByteArray>
#include <QDateTime>
#include <QRecursiveMutex>
#include <QObject>

const quint32 kBadQueueValue(0xFFFFFFFF);

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
	void queueDelay(quint32 delayInMilliSeconds, ReceiveInterface* receiveInterface = Q_NULLPTR);
	void queueLogComment(const QByteArray& comment);
	void queueEndTransaction(ReceiveInterface* receiveInterface = Q_NULLPTR);

	FramePackage getNextFramePackage();
	quint32 queueSize();

	// Data incoming
	bool handleRecievedData(const QByteArray& receivedData);

	// timing
	virtual void idle() = 0;
	virtual void setBaseTime(const QDateTime& baseTime)
	{
		_baseTime = baseTime;
	}

	void setTimeout(quint64 timeout)
	{
		_timeout = (timeout < 750) ? _timeout : 750;
	}

protected:
	QDateTime					_baseTime;
	FrameCoder*					_frameCoder{Q_NULLPTR};
	quint64						_timeout{750};
	
	static quint32				_sendID;

	virtual void frameComplete(const QByteArray& completedFrame) = 0;
	virtual void badFrame(const QByteArray& completedFrame) = 0;

	static void frameCompleteFunc(const QByteArray& completedFrame, ProtocolInterface* userData);
	static void badFrameFunc(const QByteArray& completedFrame, ProtocolInterface* userData);

	quint32 getNextSendID();

private:
	QRecursiveMutex				_pendingFramePackageMutex;
	FramePackage				_pendingFramePackage{FramePackage(Q_NULLPTR)};

	QRecursiveMutex				_outQueueMutex;
	FramePackageList			_outboundData;
};

#define ProtocolInterface_iid "com.qualcomm.simpson.ProtocolInterface"

Q_DECLARE_INTERFACE(ProtocolInterface, ProtocolInterface_iid)

#endif // PROTOCOLINTERFACE_H
