// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef TACPSOCCODER_H
#define TACPSOCCODER_H

#include "QCommonConsoleGlobal.h"

// QCommon
#include "FrameCoder.h"

// Qt
#include <QByteArray>
#include <QVariant>

class PSOCLegacyCommands;

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

	// Optional legacy firmware translation (see TACPSOCLegacyCommands.h). Not owned.
	void setLegacyCommands(PSOCLegacyCommands* legacyCommands)
	{
		_legacyCommands = legacyCommands;
	}

private:
	Q_DISABLE_COPY(TACPSOCCoder)
	QByteArray					_recieveBuffer;
	PSOCLegacyCommands*			_legacyCommands{Q_NULLPTR};
};

#endif // TACPSOCCODER_H
