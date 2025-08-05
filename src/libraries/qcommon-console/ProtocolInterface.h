#ifndef PROTOCOLINTERFACE_H
#define PROTOCOLINTERFACE_H
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
