#ifndef ENCRYPTEDSTRING_H
#define ENCRYPTEDSTRING_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include <string>

#include "QCommonConsoleGlobal.h"

class QCOMMONCONSOLE_EXPORT EncryptedString
{
public:
	EncryptedString() = default;
	EncryptedString(const std::string& encryptedString);
	EncryptedString(const EncryptedString& copyMe);

	static void setKey(const std::string& key);
	static std::string encode(const std::string& encodeMe);
	std::string decode() const;

private:
	static std::string			_key;
	std::string					_encryptedString;
};

#endif // ENCRYPTEDSTRING_H
