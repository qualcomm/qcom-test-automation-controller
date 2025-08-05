#ifndef ENCRYPTEDSTRING_H
#define ENCRYPTEDSTRING_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include <QByteArray>

#include "QCommonConsoleGlobal.h"

class QCOMMONCONSOLE_EXPORT EncryptedString
{
public:
	EncryptedString() = default;
	EncryptedString(const QByteArray& encryptedString);
	EncryptedString(const EncryptedString& copyMe);

	static void setKey(const QByteArray& key);
	static QByteArray encode(const QByteArray& encodeMe);
	QByteArray decode() const;

private:
	static QByteArray			_key;
	QByteArray					_encryptedString;
};

#endif // ENCRYPTEDSTRING_H
