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


// Prompt strings sent by the PSOC firmware.
static const qtac::ByteArray kCommand              {"CMD >> "};
static const qtac::ByteArray kCommandNotRecognized {"CMD: Command not recognized."};

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
// frames (matching the original Qt logic), then deliver an empty sentinel
// that tells TACPSOCProtocol::frameComplete() to commit the packet.
// -----------------------------------------------------------------------
void TACPSOCCoder::decode(const qtac::ByteArray& decodeMe)
{
    static const qtac::ByteArray kDelimiter{"\r\n"};

    _receiveBuffer += decodeMe;

    // Check whether the prompt or "not recognized" tag is present.
    const bool hasPrompt =
        _receiveBuffer.contains(kCommand) ||
        _receiveBuffer.contains(kCommandNotRecognized);

    if (!hasPrompt)
        return;

    // Split on \r\n, dropping empty tokens.
    std::vector<qtac::ByteArray> frames;
    {
        const std::string buf      = _receiveBuffer.toStdString();
        const std::string delim    = kDelimiter.toStdString();
        std::string::size_type start = 0;
        while (start < buf.size())
        {
            auto end = buf.find(delim, start);
            if (end == std::string::npos)
            {
                const std::string tok = buf.substr(start);
                if (!tok.empty())
                    frames.push_back(qtac::ByteArray(tok));
                break;
            }
            const std::string tok = buf.substr(start, end - start);
            if (!tok.empty())
                frames.push_back(qtac::ByteArray(tok));
            start = end + delim.size();
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
            _frameFunction(qtac::ByteArray(" "), _protocolInterface);
        ++count;
    }

    // Empty sentinel — tells frameComplete() to commit the packet.
    if (_frameFunction)
        _frameFunction(qtac::ByteArray(), _protocolInterface);

    _receiveBuffer.clear();
}

// -----------------------------------------------------------------------
// encode
//
// Maps the high-level command name (identified by its hash) to the raw
// serial string that the PSOC firmware understands.
// -----------------------------------------------------------------------
qtac::ByteArray TACPSOCCoder::encode(const qtac::ByteArray& encodeMe, const Arguments& arguments)
{
    qtac::ByteArray result = encodeMe;

    const HashType h = arrayHash(encodeMe);

    switch (h)
    {
    case kVersionCommandHash:
        result = "version\r";
        break;

    case kGetNameCommandHash:
        result = "getname\r";
        break;

    case kSetNameCommandHash:
        if (!arguments.empty())
            result = qtac::ByteArray("setname ") + arguments.at(0).asString().c_str() + "\r";
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
            result = qtac::ByteArray("i2c readRegisterBytes ") + arguments.at(0).asString().c_str() + " 1";
        break;

    case kI2CReadRegisterValueCommandHash:
        result = "i2c receive";
        break;

    case kI2CWriteRegisterCommandHash:
        if (!arguments.empty())
            result = qtac::ByteArray("i2c writeByte ") + arguments.at(0).asString().c_str();
        break;

    case kSetPinCommandHash:
        if (arguments.size() == 2)
        {
            const qtac::ByteArray stateStr = argumentToBoolString(arguments.at(0));
            const qtac::ByteArray pinStr   = qtac::ByteArray(std::to_string(arguments.at(1).asUInt32()));
            result = qtac::ByteArray("pin ") + stateStr + " " + pinStr + "\r";
        }
        break;

    default:
        result = encodeMe;
        break;
    }

    // Ensure trailing \r (matches original behaviour).
    if (result.isEmpty() || result[result.size() - 1] != '\r')
        result += '\r';

    return result;
}

} // namespace qtac
