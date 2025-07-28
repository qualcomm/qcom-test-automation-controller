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
// Copyright 2020-2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
