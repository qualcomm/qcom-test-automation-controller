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

// Thin helpers over boost::json to replicate the nlohmann patterns used in this project.

#pragma once

#include <boost/json.hpp>
#include <string>

using json_t = boost::json::value;

namespace qtac {
namespace json_util {

// Replicates nlohmann's j.value(key, default) — returns default if key absent or wrong type.
inline std::string valueString(const boost::json::object& obj,
                               const std::string& key,
                               const std::string& def = "")
{
    auto it = obj.find(key);
    if (it == obj.end()) return def;
    if (const auto* s = it->value().if_string())
        return std::string(s->data(), s->size());
    return def;
}

inline int valueInt(const boost::json::object& obj, const std::string& key, int def = 0)
{
    auto it = obj.find(key);
    if (it == obj.end()) return def;
    const auto& v = it->value();
    if (v.is_int64())  return static_cast<int>(v.as_int64());
    if (v.is_uint64()) return static_cast<int>(v.as_uint64());
    return def;
}

inline bool valueBool(const boost::json::object& obj, const std::string& key, bool def = false)
{
    auto it = obj.find(key);
    if (it == obj.end()) return def;
    if (const auto* b = it->value().if_bool())
        return *b;
    return def;
}

// Safe string extraction from a json value
inline std::string toString(const boost::json::value& v)
{
    if (const auto* s = v.if_string())
        return std::string(s->data(), s->size());
    return {};
}

// Safe int extraction from a json value
inline int toInt(const boost::json::value& v)
{
    if (v.is_int64())  return static_cast<int>(v.as_int64());
    if (v.is_uint64()) return static_cast<int>(v.as_uint64());
    return 0;
}

// Safe bool extraction from a json value
inline bool toBool(const boost::json::value& v)
{
    if (const auto* b = v.if_bool()) return *b;
    return false;
}

// Safe uint32 extraction from a json value
inline unsigned int toUInt(const boost::json::value& v)
{
    if (v.is_int64())  return static_cast<unsigned int>(v.as_int64());
    if (v.is_uint64()) return static_cast<unsigned int>(v.as_uint64());
    return 0u;
}

} // namespace json_util
} // namespace qtac
