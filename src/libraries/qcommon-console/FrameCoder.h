#ifndef FRAMECODER_H
#define FRAMECODER_H
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

#include "QCommonConsoleGlobal.h"

// QCommon
#include "SendInterface.h"

// Qt
#include <QByteArray>
#include <QObject>
#include <QPair>
#include <QVariant>

typedef QPair<QString, QVariant> ErrorParameter;
typedef QList<ErrorParameter> ErrorParameters;

class ProtocolInterface;

typedef void (*FrameCompleteFunc)(const QByteArray& completedFrame, ProtocolInterface* userData);
typedef void (*BadFrameFunc)(const QByteArray& completedFrame, ProtocolInterface* userData);

class QCOMMONCONSOLE_EXPORT FrameCoder
{
public:
	FrameCoder() = default;
	virtual ~FrameCoder() = default;

	virtual void reset();

	void setupCallbackFunctions(ProtocolInterface* userData, FrameCompleteFunc frameFunc, BadFrameFunc badFrameFunc);

	virtual void decode(const QByteArray& decodeMe);
	virtual QByteArray encode(const QByteArray& encodeMe, const Arguments& arguments);

protected:
	Q_DISABLE_COPY(FrameCoder)

	QByteArray variantToBoolString(const QVariant& state) const;

	QByteArray					_frame;

	ProtocolInterface*			_protocolInterface{Q_NULLPTR};
	FrameCompleteFunc			_frameFunction{Q_NULLPTR};
	BadFrameFunc				_badFrameFunction{Q_NULLPTR};
};

#endif // FRAMECODER_H
