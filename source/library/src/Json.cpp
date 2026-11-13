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

// boost::json is confined to this translation unit only.
// All other source files use the opaque qtac::JsonValue API from Json.h.

#include <qtac/Json.h>

#pragma warning(push)
#pragma warning(disable: 4714)
#include <boost/json.hpp>
#pragma warning(pop)

namespace qtac {

// ---------------------------------------------------------------------------
// Pimpl structs (defined here — boost types never escape to headers)
// ---------------------------------------------------------------------------

struct JsonValue::Impl
{
    boost::json::value v;
    Impl() : v(nullptr) {}
    explicit Impl(boost::json::value val) : v(std::move(val)) {}
};

struct JsonObject::Impl
{
    boost::json::object obj;
    Impl() {}
    explicit Impl(boost::json::object o) : obj(std::move(o)) {}
};

struct JsonArray::Impl
{
    boost::json::array arr;
    Impl() {}
    explicit Impl(boost::json::array a) : arr(std::move(a)) {}
};

// ---------------------------------------------------------------------------
// JsonValue
// ---------------------------------------------------------------------------

JsonValue::JsonValue()
    : _d(std::make_shared<Impl>())
{}

JsonValue::JsonValue(const JsonValue& o)
    : _d(o._d ? std::make_shared<Impl>(*o._d) : std::make_shared<Impl>())
{}

JsonValue::JsonValue(JsonValue&& o) noexcept
    : _d(std::move(o._d))
{
    o._d = std::make_shared<Impl>();
}

JsonValue& JsonValue::operator=(const JsonValue& o)
{
    if (this != &o)
        _d = o._d ? std::make_shared<Impl>(*o._d) : std::make_shared<Impl>();
    return *this;
}

JsonValue& JsonValue::operator=(JsonValue&& o) noexcept
{
    if (this != &o) {
        _d = std::move(o._d);
        o._d = std::make_shared<Impl>();
    }
    return *this;
}

JsonValue::~JsonValue() = default;

JsonValue::JsonValue(std::shared_ptr<Impl> d)
    : _d(std::move(d))
{}

JsonValue JsonValue::parse(const std::string& text)
{
    try {
        boost::json::value v = boost::json::parse(text);
        return JsonValue(std::make_shared<Impl>(std::move(v)));
    } catch (...) {
        return JsonValue();
    }
}

bool JsonValue::isObject() const { return _d && _d->v.is_object(); }
bool JsonValue::isArray()  const { return _d && _d->v.is_array(); }
bool JsonValue::isString() const { return _d && _d->v.is_string(); }
bool JsonValue::isInt()    const { return _d && (_d->v.is_int64() || _d->v.is_uint64()); }
bool JsonValue::isBool()   const { return _d && _d->v.is_bool(); }
bool JsonValue::isNull()   const { return !_d || _d->v.is_null(); }

JsonObject JsonValue::asObject() const
{
    if (!isObject()) return JsonObject();
    return JsonObject(std::make_shared<JsonObject::Impl>(_d->v.as_object()));
}

JsonArray JsonValue::asArray() const
{
    if (!isArray()) return JsonArray();
    return JsonArray(std::make_shared<JsonArray::Impl>(_d->v.as_array()));
}

std::string JsonValue::asString() const
{
    if (!isString()) return {};
    const auto& s = _d->v.as_string();
    return std::string(s.data(), s.size());
}

int JsonValue::asInt() const
{
    if (!_d) return 0;
    if (_d->v.is_int64())  return static_cast<int>(_d->v.as_int64());
    if (_d->v.is_uint64()) return static_cast<int>(_d->v.as_uint64());
    return 0;
}

unsigned JsonValue::asUInt() const
{
    if (!_d) return 0u;
    if (_d->v.is_uint64()) return static_cast<unsigned>(_d->v.as_uint64());
    if (_d->v.is_int64())  return static_cast<unsigned>(_d->v.as_int64());
    return 0u;
}

bool JsonValue::asBool() const
{
    if (!isBool()) return false;
    return _d->v.as_bool();
}

JsonValue JsonValue::fromString(const std::string& v)
{
    return JsonValue(std::make_shared<Impl>(
        boost::json::value(boost::json::string(v.data(), v.size()))));
}

JsonValue JsonValue::fromInt(int v)
{
    return JsonValue(std::make_shared<Impl>(
        boost::json::value(static_cast<std::int64_t>(v))));
}

JsonValue JsonValue::fromUInt(unsigned int v)
{
    return JsonValue(std::make_shared<Impl>(
        boost::json::value(static_cast<std::uint64_t>(v))));
}

JsonValue JsonValue::fromBool(bool v)
{
    return JsonValue(std::make_shared<Impl>(boost::json::value(v)));
}

JsonValue JsonValue::makeObject()
{
    return JsonValue(std::make_shared<Impl>(boost::json::object()));
}

JsonValue JsonValue::makeArray()
{
    return JsonValue(std::make_shared<Impl>(boost::json::array()));
}

void JsonValue::setKey(const std::string& key, const JsonValue& val)
{
    if (!_d) return;
    if (!_d->v.is_object())
        _d->v = boost::json::object();
    _d->v.as_object()[key] = val._d ? val._d->v : boost::json::value(nullptr);
}

void JsonValue::setKey(const std::string& key, const std::string& val)
{
    setKey(key, fromString(val));
}

void JsonValue::setKey(const std::string& key, int val)
{
    setKey(key, fromInt(val));
}

void JsonValue::setKey(const std::string& key, unsigned int val)
{
    setKey(key, fromUInt(val));
}

void JsonValue::setKey(const std::string& key, bool val)
{
    setKey(key, fromBool(val));
}

void JsonValue::append(const JsonValue& val)
{
    if (!_d) return;
    if (!_d->v.is_array())
        _d->v = boost::json::array();
    _d->v.as_array().push_back(val._d ? val._d->v : boost::json::value(nullptr));
}

std::string JsonValue::serialize() const
{
    if (!_d) return "null";
    return boost::json::serialize(_d->v);
}

// ---------------------------------------------------------------------------
// JsonObject
// ---------------------------------------------------------------------------

JsonObject::JsonObject()
    : _d(std::make_shared<Impl>())
{}

JsonObject::JsonObject(const JsonObject& o)
    : _d(o._d ? std::make_shared<Impl>(*o._d) : std::make_shared<Impl>())
{}

JsonObject& JsonObject::operator=(const JsonObject& o)
{
    if (this != &o)
        _d = o._d ? std::make_shared<Impl>(*o._d) : std::make_shared<Impl>();
    return *this;
}

JsonObject::~JsonObject() = default;

JsonObject::JsonObject(std::shared_ptr<Impl> d)
    : _d(std::move(d))
{}

bool JsonObject::contains(const std::string& key) const
{
    if (!_d) return false;
    return _d->obj.contains(key);
}

JsonValue JsonObject::at(const std::string& key) const
{
    if (!_d || !_d->obj.contains(key)) return JsonValue();
    return JsonValue(std::make_shared<JsonValue::Impl>(_d->obj.at(key)));
}

// ---------------------------------------------------------------------------
// JsonArray
// ---------------------------------------------------------------------------

JsonArray::JsonArray()
    : _d(std::make_shared<Impl>())
{}

JsonArray::JsonArray(const JsonArray& o)
    : _d(o._d ? std::make_shared<Impl>(*o._d) : std::make_shared<Impl>())
{}

JsonArray& JsonArray::operator=(const JsonArray& o)
{
    if (this != &o)
        _d = o._d ? std::make_shared<Impl>(*o._d) : std::make_shared<Impl>();
    return *this;
}

JsonArray::~JsonArray() = default;

JsonArray::JsonArray(std::shared_ptr<Impl> d)
    : _d(std::move(d))
{}

std::size_t JsonArray::size() const
{
    if (!_d) return 0;
    return _d->arr.size();
}

JsonValue JsonArray::at(std::size_t i) const
{
    if (!_d || i >= _d->arr.size()) return JsonValue();
    return JsonValue(std::make_shared<JsonValue::Impl>(_d->arr[i]));
}

// ---------------------------------------------------------------------------
// Free functions
// ---------------------------------------------------------------------------

namespace json {

std::string getString(const JsonObject& obj, const std::string& key, const std::string& def)
{
    if (!obj.contains(key)) return def;
    const JsonValue v = obj.at(key);
    return v.isString() ? v.asString() : def;
}

int getInt(const JsonObject& obj, const std::string& key, int def)
{
    if (!obj.contains(key)) return def;
    const JsonValue v = obj.at(key);
    return v.isInt() ? v.asInt() : def;
}

bool getBool(const JsonObject& obj, const std::string& key, bool def)
{
    if (!obj.contains(key)) return def;
    const JsonValue v = obj.at(key);
    return v.isBool() ? v.asBool() : def;
}

std::string toString(const JsonValue& v) { return v.isString() ? v.asString() : std::string(); }
int         toInt   (const JsonValue& v) { return v.isInt()    ? v.asInt()    : 0; }
bool        toBool  (const JsonValue& v) { return v.isBool()   ? v.asBool()   : false; }
unsigned    toUInt  (const JsonValue& v) { return v.isInt()    ? v.asUInt()   : 0u; }

} // namespace json
} // namespace qtac
