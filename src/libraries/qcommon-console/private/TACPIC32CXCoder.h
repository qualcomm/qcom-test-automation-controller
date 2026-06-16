#ifndef TACPIC32CXCODER_H
#define TACPIC32CXCODER_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

// QCommon
#include "FrameCoder.h"

// Qt
#include <QByteArray>
#include <QVariant>

const QByteArray kPIC32CXCommandError("Error!!! port >");
const QByteArray kPIC32CXCommandNotRecognized("*** Command Processor: unknown command. ***");
const quint8 kValidPIC32CXResponseSize(40);

class QCOMMONCONSOLE_EXPORT TACPIC32CXCoder :
	public FrameCoder
{
public:
	TACPIC32CXCoder();
	virtual ~TACPIC32CXCoder();

	virtual void reset();

	virtual void decode(const QByteArray& decodeMe);

	virtual QByteArray encode(const QByteArray& encodeMe, const Arguments& arguments);

private:
	Q_DISABLE_COPY(TACPIC32CXCoder)
	QByteArray					_recieveBuffer;
};

#endif // TACPIC32CXCODER_H
