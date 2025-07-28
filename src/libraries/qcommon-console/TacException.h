#ifndef TACEXCEPTION_H
#define TACEXCEPTION_H

// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
