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

// Author: Biswajit Roy

#include <qtac/TACPIC32CXCoder.h>
#include <qtac/TACCommandHashes.h>
#include <qtac/StringUtilities.h>

#include <variant>

// PIC32CX firmware prompt / error strings.
static const std::string kPIC32CXCommandError          {"Error!!! port >"};
static const std::string kPIC32CXCommandNotRecognized  {"*** Command Processor: unknown command. ***"};

namespace qtac {

TACPIC32CXCoder::TACPIC32CXCoder()  = default;
TACPIC32CXCoder::~TACPIC32CXCoder() = default;

void TACPIC32CXCoder::reset()
{
    _receiveBuffer.clear();
    FrameCoder::reset();
}

// -----------------------------------------------------------------------
// decode
//
// Mirrors the original Qt logic:
//   • If the buffer starts with the error prompt or contains "unknown command"
//     → send empty sentinel immediately (bad frame).
//   • Else if the buffer exceeds kValidPIC32CXResponseSize
//     → split on \r\n, deliver frames[1] as the response line, then send
//       the empty sentinel to commit.
//   • Otherwise → send empty sentinel (no data yet / ignore).
// In all cases the receive buffer is cleared afterward.
// -----------------------------------------------------------------------
void TACPIC32CXCoder::decode(const std::string& decodeMe)
{
    _receiveBuffer += decodeMe;

    if (_receiveBuffer.find(kPIC32CXCommandError)         == 0 ||
        _receiveBuffer.find(kPIC32CXCommandNotRecognized) != std::string::npos)
    {
        if (_frameFunction)
            _frameFunction(std::string(), _protocolInterface);
        _receiveBuffer.clear();
        return;
    }

    if (_receiveBuffer.size() > kValidPIC32CXResponseSize)
    {
        // Split on \r\n, skip empty tokens — mirrors Qt::SkipEmptyParts.
        static const std::string kDelimiter{"\r\n"};
        std::vector<std::string> frames;
        std::string::size_type   start = 0;
        while (start < _receiveBuffer.size())
        {
            auto end = _receiveBuffer.find(kDelimiter, start);
            if (end == std::string::npos)
            {
                const std::string tok = _receiveBuffer.substr(start);
                if (!tok.empty()) frames.push_back(tok);
                break;
            }
            const std::string tok = _receiveBuffer.substr(start, end - start);
            if (!tok.empty()) frames.push_back(tok);
            start = end + kDelimiter.size();
        }

        // Deliver frames[1] (index 1) if it exists, matching original `frames[1].toLatin1()`.
        if (frames.size() > 1 && _frameFunction)
            _frameFunction(frames[1], _protocolInterface);

        if (_frameFunction)
            _frameFunction(std::string(), _protocolInterface);

        _receiveBuffer.clear();
        return;
    }

    // Buffer not yet complete — send empty sentinel and clear.
    if (_frameFunction)
        _frameFunction(std::string(), _protocolInterface);
    _receiveBuffer.clear();
}

// -----------------------------------------------------------------------
// encode
//
// Only kPIC32CXSetPinCommandHash has a special encoding:
//   CONF:DIG:ON <1|0> (@<pin>)
// where the pin string is zero-padded to at least 3 chars to handle
// "port 0" pins (original: prepend '0' if size < 3).
// All other commands get a trailing '\n'.
// -----------------------------------------------------------------------
std::string TACPIC32CXCoder::encode(const std::string& encodeMe, const Arguments& arguments)
{
    std::string result = encodeMe;

    if (arrayHash(qtac::ByteArray(encodeMe)) == kPIC32CXSetPinCommandHash)
    {
        if (arguments.size() == 2)
        {
            std::string pinStr = std::to_string(std::get<uint32_t>(arguments.at(1)));

            // Prepend '0' if the pin string is fewer than 3 characters
            // (matches original: "if (argumentStr.size() < 3) argumentStr = '0' + argumentStr").
            if (pinStr.size() < 3)
                pinStr = "0" + pinStr;

            const std::string stateStr = argumentToBoolString(arguments.at(0));
            result = encodeMe + " " + stateStr + " (@" + pinStr + ")";
        }
    }

    if (result.empty() || result.back() != '\n')
        result += '\n';

    return result;
}

} // namespace qtac
