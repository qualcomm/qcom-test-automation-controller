#ifndef TACLITECODER_H
#define TACLITECODER_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

// QCommon
#include "FrameCoder.h"

#include <string>

class QCOMMONCONSOLE_EXPORT TACLiteCoder :
	public FrameCoder
{
public:
	TACLiteCoder(const TACLiteCoder&) = delete;
	TACLiteCoder& operator=(const TACLiteCoder&) = delete;

	TACLiteCoder();
	virtual ~TACLiteCoder();

	virtual void reset() override;

	virtual void decode(const std::string& decodeMe) override;
	virtual std::string encode(const std::string& encodeMe, const Arguments& arguments) override;

private:
	std::string					_recieveBuffer;
};

#endif // TACLITECODER_H
