#ifndef TACEXCEPTION_H
#define TACEXCEPTION_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include <QException>

#include <cstdint>
#include <string>

const uint32_t TAC_BUFFER_TOO_SMALL{1};
const uint32_t TAC_COMMAND_NOT_FOUND{3};
const uint32_t TAC_BAD_INDEX{4};
const uint32_t TAC_DEVICE_INACTIVE{5};
const uint32_t TAC_SCRIPT_VARIABLE_NOT_FOUND{6};

class QCOMMONCONSOLE_EXPORT TACException :
	public QException
{
public:
	TACException(uint32_t errorCode, std::string const& message)
	{
		_errorCode = errorCode;
		_message = message;
	}
	~TACException() = default;

	void raise() const
	{
		throw *this;
	}

	uint32_t errorCode() const
	{
		return _errorCode;
	}

	std::string getMessage() const
	{
		return _message;
	}

private:
	std::string             _message;
	uint32_t					_errorCode;
};

#endif // TACEXCEPTION_H
