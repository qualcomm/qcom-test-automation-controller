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

#ifndef QTAC_VARIANT_H
#define QTAC_VARIANT_H

#include <qtac/String.h>
#include <qtac/ByteArray.h>
#include <variant>
#include <cstdint>

namespace qtac {

class Variant
{
public:
	enum class Type {
		Invalid,
		Bool,
		Int,
		UInt,
		LongLong,
		ULongLong,
		Double,
		String,
		ByteArray
	};

	Variant();
	Variant(bool value);
	Variant(int value);
	Variant(unsigned int value);
	Variant(long long value);
	Variant(unsigned long long value);
	Variant(double value);
	Variant(const qtac::String& value);
	Variant(const char* value);
	Variant(const ByteArray& value);

	// --- Type checking ---
	bool isValid() const;
	Type type() const;

	// --- Value access ---
	bool toBool() const;
	int toInt(bool* ok = nullptr) const;
	unsigned int toUInt(bool* ok = nullptr) const;
	long long toLongLong(bool* ok = nullptr) const;
	unsigned long long toULongLong(bool* ok = nullptr) const;
	qtac::String toString() const;
	ByteArray toByteArray() const;

	// --- Template value access ---
	template<typename T>
	T value() const;

	// --- Comparison ---
	friend bool operator==(const Variant& lhs, const Variant& rhs);
	friend bool operator!=(const Variant& lhs, const Variant& rhs);

private:
	using Storage = std::variant<
		std::monostate,
		bool,
		int,
		unsigned int,
		long long,
		unsigned long long,
		double,
		qtac::String,
		ByteArray
	>;

	Storage _data;
};

// --- Template specializations ---

template<> inline bool Variant::value<bool>() const { return toBool(); }
template<> inline int Variant::value<int>() const { return toInt(); }
template<> inline unsigned int Variant::value<unsigned int>() const { return toUInt(); }
template<> inline long long Variant::value<long long>() const { return toLongLong(); }
template<> inline unsigned long long Variant::value<unsigned long long>() const { return toULongLong(); }
template<> inline String Variant::value<String>() const { return toString(); }
template<> inline ByteArray Variant::value<ByteArray>() const { return toByteArray(); }

} // namespace qtac

#endif // QTAC_VARIANT_H
