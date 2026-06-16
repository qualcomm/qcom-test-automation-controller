#ifndef TACPSOCCODER_H
#define TACPSOCCODER_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

// QCommon
#include "FrameCoder.h"

// Qt
#include <QByteArray>
#include <QVariant>

const QByteArray kCommand("CMD >> ");
const QByteArray kCommandNotRecognized("CMD: Command not recognized.");

class QCOMMONCONSOLE_EXPORT TACPSOCCoder :
	public FrameCoder
{
public:
	TACPSOCCoder();
	virtual ~TACPSOCCoder();

	virtual void reset();

	virtual void decode(const QByteArray& decodeMe);

	virtual QByteArray encode(const QByteArray& encodeMe, const Arguments& arguments);

private:
	Q_DISABLE_COPY(TACPSOCCoder)
	QByteArray					_recieveBuffer;
};

#endif // TACPSOCCODER_H