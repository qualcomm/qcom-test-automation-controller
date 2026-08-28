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
#include <new>

namespace qtac {

// Helpers for placement-new access to non-trivial union members
static inline qtac::String* strPtr(Variant::Storage* s)
{
    return reinterpret_cast<qtac::String*>(s->str);
}
static inline const qtac::String* strPtr(const Variant::Storage* s)
{
    return reinterpret_cast<const qtac::String*>(s->str);
}
static inline ByteArray* baPtr(Variant::Storage* s)
{
    return reinterpret_cast<ByteArray*>(s->ba);
}
static inline const ByteArray* baPtr(const Variant::Storage* s)
{
    return reinterpret_cast<const ByteArray*>(s->ba);
}

// --- Lifecycle helpers ---

void Variant::_destroy()
{
    if (_type == Type::String)
        strPtr(&_data)->~String();
    else if (_type == Type::ByteArray)
        baPtr(&_data)->~ByteArray();
    _type = Type::Invalid;
}

void Variant::_copyFrom(const Variant& other)
{
    _type = other._type;
    switch (_type) {
    case Type::Invalid:                                             break;
    case Type::Bool:      _data.b   = other._data.b;              break;
    case Type::Int:       _data.i   = other._data.i;              break;
    case Type::UInt:      _data.u   = other._data.u;              break;
    case Type::LongLong:  _data.ll  = other._data.ll;             break;
    case Type::ULongLong: _data.ull = other._data.ull;            break;
    case Type::Double:    _data.d   = other._data.d;              break;
    case Type::String:
        new (_data.str) qtac::String(*strPtr(&other._data));
        break;
    case Type::ByteArray:
        new (_data.ba) ByteArray(*baPtr(&other._data));
        break;
    }
}

// --- Constructors ---

Variant::Variant() : _type(Type::Invalid) {}
Variant::Variant(bool value)               : _type(Type::Bool)      { _data.b   = value; }
Variant::Variant(int value)                : _type(Type::Int)       { _data.i   = value; }
Variant::Variant(unsigned int value)       : _type(Type::UInt)      { _data.u   = value; }
Variant::Variant(long long value)          : _type(Type::LongLong)  { _data.ll  = value; }
Variant::Variant(unsigned long long value) : _type(Type::ULongLong) { _data.ull = value; }
Variant::Variant(double value)             : _type(Type::Double)    { _data.d   = value; }

Variant::Variant(const qtac::String& value) : _type(Type::String)
{
    new (_data.str) qtac::String(value);
}

Variant::Variant(const char* value) : _type(Type::String)
{
    new (_data.str) qtac::String(value);
}

Variant::Variant(const ByteArray& value) : _type(Type::ByteArray)
{
    new (_data.ba) ByteArray(value);
}

Variant::Variant(const Variant& other) : _type(Type::Invalid)
{
    _copyFrom(other);
}

Variant& Variant::operator=(const Variant& other)
{
    if (this != &other) {
        _destroy();
        _copyFrom(other);
    }
    return *this;
}

Variant::~Variant()
{
    _destroy();
}

// --- Type checking ---

bool Variant::isValid() const
{
    return _type != Type::Invalid;
}

Variant::Type Variant::type() const
{
    return _type;
}

// --- Value access ---

bool Variant::toBool() const
{
    switch (_type) {
    case Type::Bool:      return _data.b;
    case Type::Int:       return _data.i != 0;
    case Type::UInt:      return _data.u != 0;
    case Type::LongLong:  return _data.ll != 0;
    case Type::ULongLong: return _data.ull != 0;
    case Type::Double:    return _data.d != 0.0;
    case Type::String:    return !strPtr(&_data)->isEmpty();
    case Type::ByteArray: return !baPtr(&_data)->isEmpty();
    default:              return false;
    }
}

int Variant::toInt(bool* ok) const
{
    if (ok) *ok = true;
    switch (_type) {
    case Type::Int:       return _data.i;
    case Type::Bool:      return _data.b ? 1 : 0;
    case Type::UInt:      return static_cast<int>(_data.u);
    case Type::LongLong:  return static_cast<int>(_data.ll);
    case Type::ULongLong: return static_cast<int>(_data.ull);
    case Type::Double:    return static_cast<int>(_data.d);
    case Type::String:    return strPtr(&_data)->toInt(ok);
    default:              if (ok) *ok = false; return 0;
    }
}

unsigned int Variant::toUInt(bool* ok) const
{
    if (ok) *ok = true;
    switch (_type) {
    case Type::UInt:      return _data.u;
    case Type::Bool:      return _data.b ? 1u : 0u;
    case Type::Int:       return static_cast<unsigned int>(_data.i);
    case Type::LongLong:  return static_cast<unsigned int>(_data.ll);
    case Type::ULongLong: return static_cast<unsigned int>(_data.ull);
    case Type::Double:    return static_cast<unsigned int>(_data.d);
    case Type::String:    return strPtr(&_data)->toUInt(ok);
    default:              if (ok) *ok = false; return 0;
    }
}

long long Variant::toLongLong(bool* ok) const
{
    if (ok) *ok = true;
    switch (_type) {
    case Type::LongLong:  return _data.ll;
    case Type::Bool:      return _data.b ? 1LL : 0LL;
    case Type::Int:       return _data.i;
    case Type::UInt:      return _data.u;
    case Type::ULongLong: return static_cast<long long>(_data.ull);
    case Type::Double:    return static_cast<long long>(_data.d);
    case Type::String:    return strPtr(&_data)->toLongLong(ok);
    default:              if (ok) *ok = false; return 0;
    }
}

unsigned long long Variant::toULongLong(bool* ok) const
{
    if (ok) *ok = true;
    switch (_type) {
    case Type::ULongLong: return _data.ull;
    case Type::Bool:      return _data.b ? 1ULL : 0ULL;
    case Type::Int:       return static_cast<unsigned long long>(_data.i);
    case Type::UInt:      return _data.u;
    case Type::LongLong:  return static_cast<unsigned long long>(_data.ll);
    case Type::Double:    return static_cast<unsigned long long>(_data.d);
    case Type::String:    return strPtr(&_data)->toULongLong(ok);
    default:              if (ok) *ok = false; return 0;
    }
}

qtac::String Variant::toString() const
{
    switch (_type) {
    case Type::String:    return *strPtr(&_data);
    case Type::Bool:      return qtac::String(_data.b ? "true" : "false");
    case Type::Int:       return qtac::String::number(_data.i);
    case Type::UInt:      return qtac::String::number(_data.u);
    case Type::LongLong:  return qtac::String::number(_data.ll);
    case Type::ULongLong: return qtac::String::number(_data.ull);
    case Type::Double:    return qtac::String::number(_data.d);
    case Type::ByteArray: return qtac::String(*baPtr(&_data));
    default:              return qtac::String();
    }
}

ByteArray Variant::toByteArray() const
{
    if (_type == Type::ByteArray)
        return *baPtr(&_data);
    return toString().toLatin1();
}

// --- Comparison ---

bool operator==(const Variant& lhs, const Variant& rhs)
{
    if (lhs._type != rhs._type)
        return false;
    switch (lhs._type) {
    case Variant::Type::Invalid:   return true;
    case Variant::Type::Bool:      return lhs._data.b   == rhs._data.b;
    case Variant::Type::Int:       return lhs._data.i   == rhs._data.i;
    case Variant::Type::UInt:      return lhs._data.u   == rhs._data.u;
    case Variant::Type::LongLong:  return lhs._data.ll  == rhs._data.ll;
    case Variant::Type::ULongLong: return lhs._data.ull == rhs._data.ull;
    case Variant::Type::Double:    return lhs._data.d   == rhs._data.d;
    case Variant::Type::String:
        return *strPtr(&lhs._data) == *strPtr(&rhs._data);
    case Variant::Type::ByteArray:
        return *baPtr(&lhs._data) == *baPtr(&rhs._data);
    }
    return false;
}

bool operator!=(const Variant& lhs, const Variant& rhs)
{
    return !(lhs == rhs);
}

} // namespace qtac
