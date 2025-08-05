#ifndef FRAMECODER_H
#define FRAMECODER_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
