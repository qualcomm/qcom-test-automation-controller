#ifndef TACSTM32CODER_H
#define TACSTM32CODER_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

// QCommon
#include "FrameCoder.h"

// Qt
#include <QByteArray>

class QCOMMONCONSOLE_EXPORT TACSTM32Coder :
	public FrameCoder
{
public:
	TACSTM32Coder();
	virtual ~TACSTM32Coder();

	virtual void reset();

	virtual void decode(const QByteArray& decodeMe);
	virtual QByteArray encode(const QByteArray& encodeMe, const Arguments& arguments);

	quint8 gpioState() const
	{
		return _gpioState;
	}

private:
	Q_DISABLE_COPY(TACSTM32Coder)

	quint8						_gpioState{0};
};

#endif // TACSTM32CODER_H
