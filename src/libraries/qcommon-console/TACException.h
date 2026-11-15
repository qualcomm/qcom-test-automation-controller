#ifndef TACEXCEPTION_H
#define TACEXCEPTION_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QTACException.h"

#include <cstdint>
#include <string>

const uint32_t TAC_BUFFER_TOO_SMALL{1};
const uint32_t TAC_COMMAND_NOT_FOUND{3};
const uint32_t TAC_BAD_INDEX{4};
const uint32_t TAC_DEVICE_INACTIVE{5};
const uint32_t TAC_SCRIPT_VARIABLE_NOT_FOUND{6};

class QCOMMONCONSOLE_EXPORT TACException :
	public QTACException
{
public:
	TACException(uint32_t errorCode, const std::string& message) :
		QTACException(message, errorCode)
	{
	}

	~TACException() override = default;

	void raise() const override
	{
		throw *this;
	}

	TACException* clone() const override
	{
		return new TACException(*this);
	}
};

#endif // TACEXCEPTION_H
