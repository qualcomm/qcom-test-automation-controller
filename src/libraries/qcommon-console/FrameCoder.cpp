// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "FrameCoder.h"

void FrameCoder::reset()
{
	_frame.clear();
}

void FrameCoder::decode
(
	const std::string& decodeMe
)
{
	Q_ASSERT(_frameFunction != nullptr);

	_frameFunction(decodeMe, _protocolInterface);
}

std::string FrameCoder::encode
(
	const std::string& encodeMe,
	const Arguments& arguments
)
{
	Q_UNUSED(arguments)

	return encodeMe;
}

void FrameCoder::setupCallbackFunctions
(
	ProtocolInterface* protocolInterface,
	FrameCompleteFunc frameFunc,
	BadFrameFunc badFrameFunc
)
{
	_protocolInterface = protocolInterface;
	_frameFunction = frameFunc;
	_badFrameFunction = badFrameFunc;
}
