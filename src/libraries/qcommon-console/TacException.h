#ifndef TACEXCEPTION_H
#define TACEXCEPTION_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

#include <QException>

const quint32 TAC_BUFFER_TOO_SMALL{1};
const quint32 TAC_COMMAND_NOT_FOUND{3};
const quint32 TAC_BAD_INDEX{4};
const quint32 TAC_DEVICE_INACTIVE{5};
const quint32 TAC_SCRIPT_VARIABLE_NOT_FOUND{6};


class QCOMMONCONSOLE_EXPORT TacException :
	public QException
{
public:
	TacException(quint32 errorCode, QString const& message)
	{
		_errorCode = errorCode;
		_message = message.toLatin1();
	}
	~TacException() = default;

	void raise() const
	{
		throw *this;
	}

	quint32 errorCode() const
	{
		return _errorCode;
	}

	QByteArray getMessage() const
	{
		return _message;
	}

private:
	QByteArray              _message;
	quint32					_errorCode;
};

#endif // TACEXCEPTION_H
