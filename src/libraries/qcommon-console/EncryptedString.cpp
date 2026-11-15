// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "EncryptedString.h"
#include "StringUtilities.h"

#include <cassert>

std::string EncryptedString::_key;

EncryptedString::EncryptedString
(
	const std::string& encryptedString
)
{
	_encryptedString = fromHexString(encryptedString);
}

EncryptedString::EncryptedString(const EncryptedString &copyMe)
{
	_encryptedString = copyMe._encryptedString;
}

void EncryptedString::setKey
(
	const std::string& key
)
{
	_key = key;
}

std::string EncryptedString::encode
(
	const std::string& encodeMe
)
{
	assert(_key.empty() == false);

	std::string result;
	size_t encoderIndex{0};

	for (const auto& encodeMeChar: encodeMe)
	{
		result.push_back(static_cast<char>(encodeMeChar ^ _key[encoderIndex++]));
		if (encoderIndex >= _key.length())
			encoderIndex = 0;
	}

	return toHexString(result, false);
}

std::string EncryptedString::decode() const
{
	assert(_key.empty() == false);

	std::string result;
	size_t encoderIndex{0};

	for (const auto& decodeMeChar: _encryptedString)
	{
		result.push_back(static_cast<char>(decodeMeChar ^ _key[encoderIndex++]));
		if (encoderIndex >= _key.length())
			encoderIndex = 0;
	}

	return result;
}
