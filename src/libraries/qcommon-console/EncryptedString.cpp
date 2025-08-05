// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "EncryptedString.h"

QByteArray EncryptedString::_key;

EncryptedString::EncryptedString
(
	const QByteArray& encryptedString
)
{
	_encryptedString = QByteArray::fromHex(encryptedString);
}

EncryptedString::EncryptedString(const EncryptedString &copyMe)
{
	_encryptedString = copyMe._encryptedString;
}

void EncryptedString::setKey
(
	const QByteArray& key
)
{
	_key = key;
}

QByteArray EncryptedString::encode
(
	const QByteArray& encodeMe
)
{
	Q_ASSERT(_key.isEmpty() == false);

	QByteArray result;
	int encoderIndex{0};

	for (const auto& encodeMeChar: encodeMe)
	{
		result += encodeMeChar ^ _key[encoderIndex++];
		if (encoderIndex >= _key.length())
			encoderIndex = 0;
	}

	return result.toHex();
}

QByteArray EncryptedString::decode() const
{
	Q_ASSERT(_key.isEmpty() == false);

	QByteArray result;
	int encoderIndex{0};

	for (const auto& decodeMeChar: std::as_const(_encryptedString))
	{
		result += decodeMeChar ^ _key[encoderIndex++];
		if (encoderIndex >= _key.length())
			encoderIndex = 0;
	}

	return result;
}
