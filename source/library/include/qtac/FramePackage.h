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

// Authors: Michael Simpson, Biswajit Roy
// Qt-free reimplementation of FramePackage.h from qcommon-console.

#pragma once

#include <qtac/ByteArray.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace qtac {

class ReceiveInterface;

// Heterogeneous argument — mirrors the QList<QVariant> usage: bool, uint32, or string.
struct Argument
{
    enum class Kind { Bool, UInt32, String };

    // Implicit constructors so existing push_back(bool/uint32_t/string) calls compile unchanged.
    Argument(bool v)               : _kind(Kind::Bool),   _b(v) {}
    Argument(uint32_t v)           : _kind(Kind::UInt32), _u(v) {}
    Argument(const std::string& v) : _kind(Kind::String), _s(v) {}

    Kind               kind()     const { return _kind; }
    bool               asBool()   const { return _b; }
    uint32_t           asUInt32() const { return _u; }
    const std::string& asString() const { return _s; }

private:
    Kind        _kind;
    bool        _b  = false;
    uint32_t    _u  = 0;
    std::string _s;
};

using Arguments = std::vector<Argument>;class FramePackageData
{
public:
    FramePackageData()  = default;
    ~FramePackageData() = default;

    // Non-copyable; always used through shared_ptr.
    FramePackageData(const FramePackageData&)            = delete;
    FramePackageData& operator=(const FramePackageData&) = delete;

    qtac::ByteArray              lastError;
    qtac::ByteArray              request;
    uint32_t                     requestHash{0};
    Arguments                    arguments;
    qtac::ByteArray              synonym;
    qtac::ByteArray              codedRequest;
    std::vector<qtac::ByteArray> responses;
    uint32_t                     packetID{0};
    qtac::ByteArray              comment;

    bool                    endTransaction{false};
    bool                    valid{true};
    bool                    console{false};
    bool                    shouldStore{false};
    ReceiveInterface*       receiveInterface{nullptr};
    uint64_t                tickcount{0};
    uint32_t                delayInMilliSeconds{0};
};

using FramePackage     = std::shared_ptr<FramePackageData>;
using FramePackageList = std::vector<FramePackage>;

inline FramePackage makeFramePackage()
{
    return std::make_shared<FramePackageData>();
}

} // namespace qtac
