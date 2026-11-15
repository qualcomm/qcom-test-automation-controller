// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef TACPSOCCODER_H
#define TACPSOCCODER_H

#include "QCommonConsoleGlobal.h"

// QCommon
#include "FrameCoder.h"

#include <string>

const std::string kCommand{"CMD >> "};
const std::string kCommandNotRecognized{"CMD: Command not recognized."};

class QCOMMONCONSOLE_EXPORT TACPSOCCoder :
	public FrameCoder
{
public:
	TACPSOCCoder(const TACPSOCCoder&) = delete;
	TACPSOCCoder& operator=(const TACPSOCCoder&) = delete;

	TACPSOCCoder();
	virtual ~TACPSOCCoder();

	virtual void reset() override;

	virtual void decode(const std::string& decodeMe) override;

	virtual std::string encode(const std::string& encodeMe, const Arguments& arguments) override;

private:
	std::string					_recieveBuffer;
};

#endif // TACPSOCCODER_H
