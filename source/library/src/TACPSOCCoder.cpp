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

#include <qtac/TACPSOCCoder.h>
#include <qtac/TACCommandHashes.h>
#include <qtac/StringUtilities.h>
#include <qtac/ProtocolInterface.h>

#include <algorithm>
#include <variant>

// Prompt strings sent by the PSOC firmware.
static const std::string kCommand              {"CMD >> "};
static const std::string kCommandNotRecognized {"CMD: Command not recognized."};

namespace qtac {

TACPSOCCoder::TACPSOCCoder()  = default;
TACPSOCCoder::~TACPSOCCoder() = default;

void TACPSOCCoder::reset()
{
    _receiveBuffer.clear();
    FrameCoder::reset();
}

// -----------------------------------------------------------------------
// decode
//
// Accumulates raw bytes in _receiveBuffer.  When the buffer contains the
// PSOC command prompt (kCommand) or the "not recognized" string, we split
// on "\r\n", deliver each non-prompt line as a frame, pad to at least 3
// frames (matching the original Qt logic), then deliver an empty-string
// sentinel that tells TACPSOCProtocol::frameComplete() to commit the packet.
// -----------------------------------------------------------------------
void TACPSOCCoder::decode(const std::string& decodeMe)
{
    static const std::string kDelimiter{"\r\n"};

    _receiveBuffer += decodeMe;

    // Check whether the prompt or "not recognized" tag is present.
    const bool hasPrompt =
        _receiveBuffer.find(kCommand) != std::string::npos ||
        _receiveBuffer.find(kCommandNotRecognized) != std::string::npos;

    if (!hasPrompt)
        return;

    // Split on \r\n, dropping empty tokens.
    std::vector<std::string> frames;
    {
        std::string::size_type start = 0;
        while (start < _receiveBuffer.size())
        {
            auto end = _receiveBuffer.find(kDelimiter, start);
            if (end == std::string::npos)
            {
                const std::string tok = _receiveBuffer.substr(start);
                if (!tok.empty())
                    frames.push_back(tok);
                break;
            }
            const std::string tok = _receiveBuffer.substr(start, end - start);
            if (!tok.empty())
                frames.push_back(tok);
            start = end + kDelimiter.size();
        }
    }

    // Deliver each frame.
    int count = 0;
    for (const auto& f : frames)
    {
        if (_frameFunction)
            _frameFunction(f, _protocolInterface);
        ++count;
    }

    // Pad to at least 3 (original logic: "while (count < 3)").
    while (count < 3)
    {
        if (_frameFunction)
            _frameFunction(" ", _protocolInterface);
        ++count;
    }

    // Empty-string sentinel — tells frameComplete() to commit the packet.
    if (_frameFunction)
        _frameFunction(std::string(), _protocolInterface);

    _receiveBuffer.clear();
}

// -----------------------------------------------------------------------
// encode
//
// Maps the high-level command name (identified by its hash) to the raw
// serial string that the PSOC firmware understands.
// -----------------------------------------------------------------------
std::string TACPSOCCoder::encode(const std::string& encodeMe, const Arguments& arguments)
{
    std::string result = encodeMe;

    switch (arrayHash(qtac::ByteArray(encodeMe)))
    {
    case kVersionCommandHash:
        result = "version\r";
        break;

    case kGetNameCommandHash:
        result = "getname\r";
        break;

    case kSetNameCommandHash:
        if (!arguments.empty())
            result = "setname " + std::get<std::string>(arguments.at(0)) + "\r";
        break;

    case kGetUUIDCommandHash:
        result = "sys getFSUUID\r";
        break;

    case kGetPlatformIDCommandHash:
        result = "getboardid\r";
        break;

    case kGetResetCountCommandHash:
        result = "getresetcount\r";
        break;

    case kClearResetCountCommandHash:
        result = "clearresetcount\r";
        break;

    case kI2CReadRegisterCommandHash:
        if (!arguments.empty())
            result = "i2c readRegisterBytes " + std::get<std::string>(arguments.at(0)) + " 1";
        break;

    case kI2CReadRegisterValueCommandHash:
        result = "i2c receive";
        break;

    case kI2CWriteRegisterCommandHash:
        if (!arguments.empty())
            result = "i2c writeByte " + std::get<std::string>(arguments.at(0));
        break;

    case kSetPinCommandHash:
        if (arguments.size() == 2)
        {
            const std::string stateStr = argumentToBoolString(arguments.at(0));
            const std::string pinStr   = std::to_string(std::get<uint32_t>(arguments.at(1)));
            result = "pin " + stateStr + " " + pinStr + "\r";
        }
        break;

    default:
        result = encodeMe;
        break;
    }

    // Ensure trailing \r (matches original behaviour).
    if (result.empty() || result.back() != '\r')
        result += '\r';

    return result;
}

} // namespace qtac
