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

#include <qtac/Variant.h>
#include <sstream>

namespace qtac {

// --- Constructors ---

Variant::Variant() : _data(std::monostate{}) {}
Variant::Variant(bool value) : _data(value) {}
Variant::Variant(int value) : _data(value) {}
Variant::Variant(unsigned int value) : _data(value) {}
Variant::Variant(long long value) : _data(value) {}
Variant::Variant(unsigned long long value) : _data(value) {}
Variant::Variant(double value) : _data(value) {}
Variant::Variant(const qtac::String& value) : _data(value) {}
Variant::Variant(const char* value) : _data(qtac::String(value)) {}
Variant::Variant(const ByteArray& value) : _data(value) {}

// --- Type checking ---

bool Variant::isValid() const
{
	return !std::holds_alternative<std::monostate>(_data);
}

Variant::Type Variant::type() const
{
	if (std::holds_alternative<std::monostate>(_data)) return Type::Invalid;
	if (std::holds_alternative<bool>(_data)) return Type::Bool;
	if (std::holds_alternative<int>(_data)) return Type::Int;
	if (std::holds_alternative<unsigned int>(_data)) return Type::UInt;
	if (std::holds_alternative<long long>(_data)) return Type::LongLong;
	if (std::holds_alternative<unsigned long long>(_data)) return Type::ULongLong;
	if (std::holds_alternative<double>(_data)) return Type::Double;
	if (std::holds_alternative<qtac::String>(_data)) return Type::String;
	if (std::holds_alternative<ByteArray>(_data)) return Type::ByteArray;
	return Type::Invalid;
}

// --- Value access ---

bool Variant::toBool() const
{
	if (auto* v = std::get_if<bool>(&_data)) return *v;
	if (auto* v = std::get_if<int>(&_data)) return *v != 0;
	if (auto* v = std::get_if<unsigned int>(&_data)) return *v != 0;
	if (auto* v = std::get_if<long long>(&_data)) return *v != 0;
	if (auto* v = std::get_if<unsigned long long>(&_data)) return *v != 0;
	if (auto* v = std::get_if<double>(&_data)) return *v != 0.0;
	if (auto* v = std::get_if<qtac::String>(&_data)) return !v->isEmpty();
	if (auto* v = std::get_if<ByteArray>(&_data)) return !v->isEmpty();
	return false;
}

int Variant::toInt(bool* ok) const
{
	if (auto* v = std::get_if<int>(&_data)) { if (ok) *ok = true; return *v; }
	if (auto* v = std::get_if<bool>(&_data)) { if (ok) *ok = true; return *v ? 1 : 0; }
	if (auto* v = std::get_if<unsigned int>(&_data)) { if (ok) *ok = true; return static_cast<int>(*v); }
	if (auto* v = std::get_if<long long>(&_data)) { if (ok) *ok = true; return static_cast<int>(*v); }
	if (auto* v = std::get_if<unsigned long long>(&_data)) { if (ok) *ok = true; return static_cast<int>(*v); }
	if (auto* v = std::get_if<double>(&_data)) { if (ok) *ok = true; return static_cast<int>(*v); }
	if (auto* v = std::get_if<qtac::String>(&_data)) { return v->toInt(ok); }
	if (ok) *ok = false;
	return 0;
}

unsigned int Variant::toUInt(bool* ok) const
{
	if (auto* v = std::get_if<unsigned int>(&_data)) { if (ok) *ok = true; return *v; }
	if (auto* v = std::get_if<bool>(&_data)) { if (ok) *ok = true; return *v ? 1u : 0u; }
	if (auto* v = std::get_if<int>(&_data)) { if (ok) *ok = true; return static_cast<unsigned int>(*v); }
	if (auto* v = std::get_if<long long>(&_data)) { if (ok) *ok = true; return static_cast<unsigned int>(*v); }
	if (auto* v = std::get_if<unsigned long long>(&_data)) { if (ok) *ok = true; return static_cast<unsigned int>(*v); }
	if (auto* v = std::get_if<double>(&_data)) { if (ok) *ok = true; return static_cast<unsigned int>(*v); }
	if (auto* v = std::get_if<qtac::String>(&_data)) { return v->toUInt(ok); }
	if (ok) *ok = false;
	return 0;
}

long long Variant::toLongLong(bool* ok) const
{
	if (auto* v = std::get_if<long long>(&_data)) { if (ok) *ok = true; return *v; }
	if (auto* v = std::get_if<bool>(&_data)) { if (ok) *ok = true; return *v ? 1LL : 0LL; }
	if (auto* v = std::get_if<int>(&_data)) { if (ok) *ok = true; return *v; }
	if (auto* v = std::get_if<unsigned int>(&_data)) { if (ok) *ok = true; return *v; }
	if (auto* v = std::get_if<unsigned long long>(&_data)) { if (ok) *ok = true; return static_cast<long long>(*v); }
	if (auto* v = std::get_if<double>(&_data)) { if (ok) *ok = true; return static_cast<long long>(*v); }
	if (auto* v = std::get_if<qtac::String>(&_data)) { return v->toLongLong(ok); }
	if (ok) *ok = false;
	return 0;
}

unsigned long long Variant::toULongLong(bool* ok) const
{
	if (auto* v = std::get_if<unsigned long long>(&_data)) { if (ok) *ok = true; return *v; }
	if (auto* v = std::get_if<bool>(&_data)) { if (ok) *ok = true; return *v ? 1ULL : 0ULL; }
	if (auto* v = std::get_if<int>(&_data)) { if (ok) *ok = true; return static_cast<unsigned long long>(*v); }
	if (auto* v = std::get_if<unsigned int>(&_data)) { if (ok) *ok = true; return *v; }
	if (auto* v = std::get_if<long long>(&_data)) { if (ok) *ok = true; return static_cast<unsigned long long>(*v); }
	if (auto* v = std::get_if<double>(&_data)) { if (ok) *ok = true; return static_cast<unsigned long long>(*v); }
	if (auto* v = std::get_if<qtac::String>(&_data)) { return v->toULongLong(ok); }
	if (ok) *ok = false;
	return 0;
}

qtac::String Variant::toString() const
{
	if (auto* v = std::get_if<qtac::String>(&_data)) return *v;
	if (auto* v = std::get_if<bool>(&_data)) return qtac::String(*v ? "true" : "false");
	if (auto* v = std::get_if<int>(&_data)) return qtac::String::number(*v);
	if (auto* v = std::get_if<unsigned int>(&_data)) return qtac::String::number(*v);
	if (auto* v = std::get_if<long long>(&_data)) return qtac::String::number(*v);
	if (auto* v = std::get_if<unsigned long long>(&_data)) return qtac::String::number(*v);
	if (auto* v = std::get_if<double>(&_data)) return qtac::String::number(*v);
	if (auto* v = std::get_if<ByteArray>(&_data)) return qtac::String(*v);
	return qtac::String();
}

ByteArray Variant::toByteArray() const
{
	if (auto* v = std::get_if<ByteArray>(&_data)) return *v;
	return toString().toLatin1();
}

// --- Comparison ---

bool operator==(const Variant& lhs, const Variant& rhs)
{
	return lhs._data == rhs._data;
}

bool operator!=(const Variant& lhs, const Variant& rhs)
{
	return !(lhs == rhs);
}

} // namespace qtac
