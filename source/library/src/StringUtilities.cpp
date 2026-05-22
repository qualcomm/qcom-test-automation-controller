// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//         notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//         copyright notice, this list of conditions and the following
//         disclaimer in the documentation and/or other materials provided
//         with the distribution.
//
//     * Neither the name of Qualcomm Technologies, Inc. nor the names of its
//         contributors may be used to endorse or promote products derived
//         from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
// HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: Michael Simpson

#include <qtac/StringUtilities.h>
#include <qtac/StringList.h>

#include <cctype>
#include <cstdint>

namespace qtac {

qtac::String toCamelCase(const qtac::String& camelCaseMe, char splitChar, bool skipEmpty)
{
	qtac::StringList parts = camelCaseMe.split(splitChar);
	qtac::String result;
	for (int i = 0; i < static_cast<int>(parts.size()); ++i)
	{
		qtac::String part = parts[i];
		if (skipEmpty && part.isEmpty())
			continue;
		if (!part.isEmpty())
		{
			std::string s = part.toStdString();
			s[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(s[0])));
			result += qtac::String(s);
		}
	}
	return result;
}

bool isAlphaNumeric(const qtac::ByteArray& testMe)
{
	for (char c : testMe)
	{
		if (!std::isalnum(static_cast<unsigned char>(c)))
			return false;
	}
	return true;
}

qtac::String fromBool(bool value)
{
	return value ? qtac::String("true") : qtac::String("false");
}

qtac::String fromPoint(const qtac::Point& value)
{
	return qtac::String::number(value.x()) + qtac::String(",") + qtac::String::number(value.y());
}

qtac::Point toPoint(const qtac::String& value)
{
	qtac::StringList temp = value.split(',');
	if (static_cast<int>(temp.size()) >= 2)
		return qtac::Point(temp[0].toInt(), temp[1].toInt());
	return qtac::Point(-1, -1);
}

HashType strHash(const qtac::String& hashMe)
{
	return arrayHash(hashMe.toLatin1());
}

HashType arrayHash(const qtac::ByteArray& hashMe)
{
	uint64_t result{0};
	const uint64_t p = 257;
	const uint64_t m = 1000000009ULL;
	uint64_t p_pow = 1;
	for (char c : hashMe)
	{
		result = (result + static_cast<uint64_t>(c - 'a' + 1) * p_pow) % m;
		p_pow = (p_pow * p) % m;
	}
	return result;
}

} // namespace qtac
