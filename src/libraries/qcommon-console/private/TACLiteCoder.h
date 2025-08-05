#ifndef TACLITECODER_H
#define TACLITECODER_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

// QCommon
#include "FrameCoder.h"

// Qt
#include <QByteArray>

class QCOMMONCONSOLE_EXPORT TacLiteCoder :
	public FrameCoder
{
public:
	TacLiteCoder();
	virtual ~TacLiteCoder();

	virtual void reset();

	virtual void decode(const QByteArray& decodeMe);
	virtual QByteArray encode(const QByteArray& encodeMe, const Arguments& arguments);

private:
	Q_DISABLE_COPY(TacLiteCoder)
	QByteArray					_recieveBuffer;
};

#endif // TACLITECODER_H
