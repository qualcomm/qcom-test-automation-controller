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

#include <qtac/StringUtilities.h>
#include <qtac/Point.h>
#include <cassert>
#include <cstdio>

// ===== toCamelCase =====

static void test_toCamelCase()
{
    // Basic underscore split
    assert(toCamelCase(qtac::String("hello_world")) == "HelloWorld");
    assert(toCamelCase(qtac::String("foo_bar_baz")) == "FooBarBaz");

    // Single word (no split char)
    assert(toCamelCase(qtac::String("hello")) == "Hello");

    // Already single capital token
    assert(toCamelCase(qtac::String("Hello")) == "Hello");

    // Empty string
    assert(toCamelCase(qtac::String("")).isEmpty());

    // Leading/trailing underscores skipped with skipEmpty=true (default)
    assert(toCamelCase(qtac::String("_hello_world_")) == "HelloWorld");

    // skipEmpty=false preserves empty segments (capitalises nothing for them)
    // Single underscore between words — with skipEmpty=false the empty part is
    // still an empty string and capitalising its [0] is skipped by the inner guard
    assert(toCamelCase(qtac::String("hello_world"), '_', false) == "HelloWorld");

    // Custom split char
    assert(toCamelCase(qtac::String("hello.world"), '.') == "HelloWorld");
    assert(toCamelCase(qtac::String("a-b-c"), '-') == "ABC");
}

// ===== fromBool =====

static void test_fromBool()
{
    assert(fromBool(true)  == "true");
    assert(fromBool(false) == "false");
}

// ===== fromPoint / toPoint =====

static void test_fromPoint_toPoint()
{
    // Basic round-trip
    qtac::Point p(3, 7);
    qtac::String s = fromPoint(p);
    assert(s == "3,7");
    qtac::Point back = toPoint(s);
    assert(back.x() == 3);
    assert(back.y() == 7);

    // Negative coords (used as "no location" sentinel throughout the codebase)
    qtac::Point neg(-1, -1);
    qtac::String ns = fromPoint(neg);
    assert(ns == "-1,-1");
    qtac::Point negBack = toPoint(ns);
    assert(negBack.x() == -1);
    assert(negBack.y() == -1);

    // Zero
    qtac::Point zero(0, 0);
    qtac::String zs = fromPoint(zero);
    assert(zs == "0,0");
    qtac::Point zBack = toPoint(zs);
    assert(zBack.x() == 0 && zBack.y() == 0);

    // Malformed string → sentinel (-1,-1)
    qtac::Point bad = toPoint(qtac::String("notapoint"));
    assert(bad.x() == -1 && bad.y() == -1);

    // Missing second component → sentinel
    qtac::Point bad2 = toPoint(qtac::String("5"));
    assert(bad2.x() == -1 && bad2.y() == -1);
}

// ===== strHash =====

static void test_strHash()
{
    // Same input → same hash
    HashType h1 = strHash(qtac::String("battery"));
    HashType h2 = strHash(qtac::String("battery"));
    assert(h1 == h2);

    // Different inputs → different hashes (with very high probability for these strings)
    HashType ha = strHash(qtac::String("battery"));
    HashType hb = strHash(qtac::String("usb0"));
    assert(ha != hb);

    // Empty string is stable
    HashType hempty1 = strHash(qtac::String(""));
    HashType hempty2 = strHash(qtac::String(""));
    assert(hempty1 == hempty2);

    // Numeric strings used by PSOC/PIC32CX pin hashing
    HashType h53  = strHash(qtac::String("53"));
    HashType h54  = strHash(qtac::String("54"));
    assert(h53 != h54);
}

// ===== arrayHash =====

static void test_arrayHash()
{
    // Same input → same hash
    HashType h1 = arrayHash(qtac::ByteArray("hello"));
    HashType h2 = arrayHash(qtac::ByteArray("hello"));
    assert(h1 == h2);

    // Different inputs
    HashType ha = arrayHash(qtac::ByteArray("hello"));
    HashType hb = arrayHash(qtac::ByteArray("world"));
    assert(ha != hb);

    // strHash and arrayHash are consistent (strHash delegates to arrayHash via toLatin1)
    qtac::String s("test123");
    assert(strHash(s) == arrayHash(s.toLatin1()));
}

// ===== isAlphaNumeric =====

static void test_isAlphaNumeric()
{
    assert(isAlphaNumeric(qtac::ByteArray("abc123")));
    assert(isAlphaNumeric(qtac::ByteArray("ABC")));
    assert(isAlphaNumeric(qtac::ByteArray("123")));
    assert(!isAlphaNumeric(qtac::ByteArray("abc 123")));  // space
    assert(!isAlphaNumeric(qtac::ByteArray("abc-123"))); // hyphen
    assert(!isAlphaNumeric(qtac::ByteArray("abc_123"))); // underscore
    // Empty is trivially true (no non-alphanumeric chars)
    assert(isAlphaNumeric(qtac::ByteArray("")));
}

int main()
{
    test_toCamelCase();
    test_fromBool();
    test_fromPoint_toPoint();
    test_strHash();
    test_arrayHash();
    test_isAlphaNumeric();

    std::printf("All StringUtilities tests passed!\n");
    return 0;
}
