#ifndef FRAMECODER_H
#define FRAMECODER_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

// QCommon
#include "SendInterface.h"

// Qt
#include <QObject>

#include <string>

class ProtocolInterface;

typedef void (*FrameCompleteFunc)(const std::string& completedFrame, ProtocolInterface* userData);
typedef void (*BadFrameFunc)(const std::string& completedFrame, ProtocolInterface* userData);

class QCOMMONCONSOLE_EXPORT FrameCoder
{
public:
	FrameCoder() = default;
	virtual ~FrameCoder() = default;

	virtual void reset();

	void setupCallbackFunctions(ProtocolInterface* userData, FrameCompleteFunc frameFunc, BadFrameFunc badFrameFunc);

	virtual void decode(const std::string& decodeMe);
	virtual std::string encode(const std::string& encodeMe, const Arguments& arguments);

protected:
	Q_DISABLE_COPY(FrameCoder)

	std::string					_frame;

	ProtocolInterface*			_protocolInterface{nullptr};
	FrameCompleteFunc			_frameFunction{nullptr};
	BadFrameFunc				_badFrameFunction{nullptr};
};

#endif // FRAMECODER_H
