/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
