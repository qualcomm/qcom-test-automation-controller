#ifndef PROTOCOLINTERFACE_H
#define PROTOCOLINTERFACE_H
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
// Copyright ©2018-2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
