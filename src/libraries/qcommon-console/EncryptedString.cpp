// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "EncryptedString.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>

std::string EncryptedString::_key;

static std::string hexDecode(const std::string& hexString)
{
	std::string result;

	for (size_t index = 0; index + 1 < hexString.size(); index += 2)
	{
		std::string byteString = hexString.substr(index, 2);
		result.push_back(static_cast<char>(std::strtoul(byteString.c_str(), nullptr, 16)));
	}

	return result;
}

static std::string hexEncode(const std::string& rawString)
{
	std::string result;

	for (const auto& rawChar: rawString)
	{
		char buffer[3];

		std::snprintf(buffer, sizeof(buffer), "%02x", static_cast<unsigned char>(rawChar));

		result += buffer;
	}

	return result;
}

EncryptedString::EncryptedString
(
	const std::string& encryptedString
)
{
	_encryptedString = hexDecode(encryptedString);
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

	return hexEncode(result);
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
