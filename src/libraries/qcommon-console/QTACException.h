#ifndef QTACEXCEPTION_H
#define QTACEXCEPTION_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include <cstdint>
#include <stdexcept>
#include <string>

/**
 * @brief Base exception class for all QTAC / QCommon exceptions.
 * Derived from standard std::runtime_error. Pure Standard C++, zero Qt dependencies.
 */
class QCOMMONCONSOLE_EXPORT QTACException : public std::runtime_error
{
public:
	explicit QTACException(const std::string& message, uint32_t errorCode = 0) :
		std::runtime_error(message),
		_message(message),
		_errorCode(errorCode)
	{
	}

	QTACException(const QTACException& other) :
		std::runtime_error(other._message),
		_message(other._message),
		_errorCode(other._errorCode)
	{
	}

	QTACException& operator=(const QTACException& other)
	{
		if (this != &other)
		{
			std::runtime_error::operator=(other);
			_message = other._message;
			_errorCode = other._errorCode;
		}
		return *this;
	}

	~QTACException() override = default;

	virtual void raise() const
	{
		throw *this;
	}

	virtual QTACException* clone() const
	{
		return new QTACException(*this);
	}

	const std::string& getMessage() const noexcept
	{
		return _message;
	}

	uint32_t errorCode() const noexcept
	{
		return _errorCode;
	}

protected:
	std::string _message;
	uint32_t    _errorCode{0};
};

#endif // QTACEXCEPTION_H
