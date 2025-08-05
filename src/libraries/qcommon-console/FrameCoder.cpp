// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

// Author: msimpson

#include "FrameCoder.h"

void FrameCoder::reset()
{
	_frame.clear();
}

void FrameCoder::decode
(
	const QByteArray& decodeMe
)
{
	Q_ASSERT(_frameFunction != Q_NULLPTR);

	_frameFunction(decodeMe, _protocolInterface);
}

QByteArray FrameCoder::encode
(
	const QByteArray& encodeMe,
	const Arguments& arguments
)
{
	Q_UNUSED(arguments)

	return encodeMe;
}

QByteArray FrameCoder::variantToBoolString(const QVariant& state) const
{
	if (state.toBool() == true)
		return "1";

	return "0";
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
