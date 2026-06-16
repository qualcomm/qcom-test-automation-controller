#ifndef TACLITECODER_H
#define TACLITECODER_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

// QCommon
#include "FrameCoder.h"

// Qt
#include <QByteArray>

class QCOMMONCONSOLE_EXPORT TACLiteCoder :
	public FrameCoder
{
public:
	TACLiteCoder();
	virtual ~TACLiteCoder();

	virtual void reset();

	virtual void decode(const QByteArray& decodeMe);
	virtual QByteArray encode(const QByteArray& encodeMe, const Arguments& arguments);

private:
	Q_DISABLE_COPY(TACLiteCoder)
	QByteArray					_recieveBuffer;
};

#endif // TACLITECODER_H
