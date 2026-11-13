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

// Opaque JSON value types — no boost headers exposed to callers.
//
// Usage (parsing):
//   qtac::JsonValue root = qtac::JsonValue::parse(text);
//   if (!root.isObject()) return;
//   qtac::JsonObject obj = root.asObject();
//   std::string name = qtac::json::getString(obj, "name", "");
//
// Usage (building):
//   qtac::JsonValue entry = qtac::JsonValue::makeObject();
//   entry.setKey("count", 3);
//   entry.setKey("label", std::string("hello"));
//
// Implementation: source/library/src/Json.cpp (boost::json kept private there).

#pragma once

#include <string>
#include <memory>
#include <cstddef>

namespace qtac {

class JsonObject;
class JsonArray;

// ---------------------------------------------------------------------------
// JsonValue — a single JSON value (object, array, string, int, bool, or null).
// Copying a JsonValue performs a deep copy of the underlying data.
// ---------------------------------------------------------------------------
class JsonValue
{
public:
    JsonValue();
    JsonValue(const JsonValue&);
    JsonValue(JsonValue&&) noexcept;
    JsonValue& operator=(const JsonValue&);
    JsonValue& operator=(JsonValue&&) noexcept;
    ~JsonValue();

    // Parse JSON text. Returns a null JsonValue on parse error.
    static JsonValue parse(const std::string& text);

    // Type predicates
    bool isObject() const;
    bool isArray()  const;
    bool isString() const;
    bool isInt()    const;  // true for integer (int64 or uint64) values
    bool isBool()   const;
    bool isNull()   const;

    // Type-safe accessors (return a default if the type doesn't match)
    JsonObject  asObject() const;    // returns empty JsonObject if not an object
    JsonArray   asArray()  const;    // returns empty JsonArray if not an array
    std::string asString() const;    // returns "" if not a string
    int         asInt()    const;    // returns 0 if not an integer
    unsigned    asUInt()   const;    // returns 0 if not an integer
    bool        asBool()   const;    // returns false if not a bool

    // Factories for building JSON values
    static JsonValue fromString(const std::string& v);
    static JsonValue fromInt(int v);
    static JsonValue fromUInt(unsigned int v);
    static JsonValue fromBool(bool v);
    static JsonValue makeObject();   // creates an empty JSON object {}
    static JsonValue makeArray();    // creates an empty JSON array []

    // Object mutation — no-op if this is not an object (auto-converts null → object)
    void setKey(const std::string& key, const JsonValue& val);
    void setKey(const std::string& key, const std::string& val);
    void setKey(const std::string& key, int val);
    void setKey(const std::string& key, unsigned int val);
    void setKey(const std::string& key, bool val);

    // Array mutation — appends a copy (auto-converts null → array)
    void append(const JsonValue& val);

    // Serialise to compact JSON text
    std::string serialize() const;

    struct Impl;

private:
    explicit JsonValue(std::shared_ptr<Impl> d);
    std::shared_ptr<Impl> _d;

    friend class JsonObject;
    friend class JsonArray;
};

// ---------------------------------------------------------------------------
// JsonObject — a JSON key-value mapping.
// ---------------------------------------------------------------------------
class JsonObject
{
public:
    JsonObject();
    JsonObject(const JsonObject&);
    JsonObject& operator=(const JsonObject&);
    ~JsonObject();

    // Returns true if the key exists.
    bool contains(const std::string& key) const;

    // Returns the value for key, or a null JsonValue if absent.
    JsonValue at(const std::string& key) const;

    struct Impl;

private:
    explicit JsonObject(std::shared_ptr<Impl> d);
    std::shared_ptr<Impl> _d;

    friend class JsonValue;
};

// ---------------------------------------------------------------------------
// JsonArray — an ordered list of JSON values.
// Supports range-based for loops.
// ---------------------------------------------------------------------------
class JsonArray
{
public:
    JsonArray();
    JsonArray(const JsonArray&);
    JsonArray& operator=(const JsonArray&);
    ~JsonArray();

    std::size_t size() const;

    // Returns element at index i, or a null JsonValue if out of range.
    JsonValue at(std::size_t i) const;

    // Range-for support (index-based, copies each element on dereference)
    struct Iterator
    {
        const JsonArray* _arr;
        std::size_t      _idx;

        JsonValue  operator*()  const { return _arr->at(_idx); }
        Iterator&  operator++()       { ++_idx; return *this; }
        bool       operator!=(const Iterator& o) const { return _idx != o._idx; }
    };
    Iterator begin() const { return {this, 0}; }
    Iterator end()   const { return {this, size()}; }

    struct Impl;

private:
    explicit JsonArray(std::shared_ptr<Impl> d);
    std::shared_ptr<Impl> _d;

    friend class JsonValue;
};

// ---------------------------------------------------------------------------
// Convenience free functions — use instead of raw JsonValue accessors when
// reading from a JsonObject with a default fallback.
// ---------------------------------------------------------------------------
namespace json {

// Object key accessors with default fallback
std::string getString(const JsonObject& obj, const std::string& key,
                      const std::string& def = "");
int         getInt  (const JsonObject& obj, const std::string& key, int def = 0);
bool        getBool (const JsonObject& obj, const std::string& key, bool def = false);

// Value extractors (return default if type doesn't match)
std::string toString(const JsonValue& v);
int         toInt   (const JsonValue& v);
bool        toBool  (const JsonValue& v);
unsigned    toUInt  (const JsonValue& v);

} // namespace json
} // namespace qtac
