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

#ifndef QTAC_STRINGUTILITIES_H
#define QTAC_STRINGUTILITIES_H

#include <qtac/String.h>
#include <qtac/ByteArray.h>
#include <qtac/Point.h>

#include <cstdint>

using HashType = uint64_t;

namespace qtac {

qtac::String toCamelCase(const qtac::String& camelCaseMe, char splitChar = '_', bool skipEmpty = true);
bool isAlphaNumeric(const qtac::ByteArray& testMe);
qtac::String fromBool(bool value);

qtac::String fromPoint(const qtac::Point& value);
qtac::Point  toPoint(const qtac::String& value);

HashType strHash(const qtac::String& hashMe);
HashType arrayHash(const qtac::ByteArray& hashMe);

// Compile-time version of arrayHash for use in constexpr constants.
// Implements the identical polynomial hash: p=257, m=1e9+9, offset c-'a'+1.
// Wrapping of upper-case chars is reproduced via unsigned 64-bit arithmetic
// (c as unsigned + (2^64 - 96)) which equals static_cast<uint64_t>(c-'a'+1)
// for any char value, including negative ones on signed-char platforms.

// Constants for the hash computation
constexpr uint64_t kHashP = 257;
constexpr uint64_t kHashM = 1000000009ULL;
constexpr uint64_t kHashOffset = static_cast<uint64_t>(-static_cast<int>('a') + 1);

// Helper function for C++11 constexpr compatibility (recursive implementation)
constexpr HashType computeHashStrImpl(const char* str, std::size_t len, std::size_t i,
                                      uint64_t result, uint64_t p_pow) noexcept
{
    return (i >= len) ? result :
        computeHashStrImpl(str, len, i + 1,
            (result + ((static_cast<uint64_t>(static_cast<unsigned char>(str[i])) + kHashOffset) * p_pow)) % kHashM,
            (p_pow * kHashP) % kHashM);
}

constexpr HashType computeHashStr(const char* str, std::size_t len) noexcept
{
    return computeHashStrImpl(str, len, 0, 0, 1);
}

// Helper to compute string length at compile time
constexpr std::size_t constexprStrLen(const char* str, std::size_t i = 0) noexcept
{
    return str[i] ? constexprStrLen(str, i + 1) : i;
}

constexpr HashType computeHashStr(const char* str) noexcept
{
    return computeHashStr(str, constexprStrLen(str));
}

} // namespace qtac

// Bring into global namespace for unqualified use in migrated code
using qtac::strHash;
using qtac::arrayHash;
using qtac::fromBool;
using qtac::fromPoint;
using qtac::toPoint;
using qtac::toCamelCase;
using qtac::isAlphaNumeric;
using qtac::computeHashStr;

#endif // QTAC_STRINGUTILITIES_H
