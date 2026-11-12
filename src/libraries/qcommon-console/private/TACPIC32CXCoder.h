// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef TACPIC32CXCODER_H
#define TACPIC32CXCODER_H

#include "QCommonConsoleGlobal.h"

// QCommon
#include "FrameCoder.h"

#include <cstdint>
#include <string>

const std::string kPIC32CXCommandError{"Error!!! port >"};
const std::string kPIC32CXCommandNotRecognized{"*** Command Processor: unknown command. ***"};
const uint8_t kValidPIC32CXResponseSize(40);

class QCOMMONCONSOLE_EXPORT TACPIC32CXCoder :
	public FrameCoder
{
public:
	TACPIC32CXCoder();
	virtual ~TACPIC32CXCoder();

	virtual void reset() override;

	virtual void decode(const std::string& decodeMe) override;

	virtual std::string encode(const std::string& encodeMe, const Arguments& arguments) override;

private:
	Q_DISABLE_COPY(TACPIC32CXCoder)
	std::string					_recieveBuffer;
};

#endif // TACPIC32CXCODER_H
