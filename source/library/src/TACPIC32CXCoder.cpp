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
static const qtac::ByteArray kPIC32CXCommandError          {"Error!!! port >"};
static const qtac::ByteArray kPIC32CXCommandNotRecognized  {"*** Command Processor: unknown command. ***"};

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
void TACPIC32CXCoder::decode(const qtac::ByteArray& decodeMe)
{
    _receiveBuffer += decodeMe;

    if (_receiveBuffer.startsWith(kPIC32CXCommandError) ||
        _receiveBuffer.contains(kPIC32CXCommandNotRecognized))
    {
        if (_frameFunction)
            _frameFunction(qtac::ByteArray(), _protocolInterface);
        _receiveBuffer.clear();
        return;
    }

    if (static_cast<size_t>(_receiveBuffer.size()) > kValidPIC32CXResponseSize)
    {
        // Split on \r\n, skip empty tokens — mirrors Qt::SkipEmptyParts.
        static const qtac::ByteArray kDelimiter{"\r\n"};
        std::vector<qtac::ByteArray> frames;
        {
            const std::string buf   = _receiveBuffer.toStdString();
            const std::string delim = kDelimiter.toStdString();
            std::string::size_type start = 0;
            while (start < buf.size())
            {
                auto end = buf.find(delim, start);
                if (end == std::string::npos)
                {
                    const std::string tok = buf.substr(start);
                    if (!tok.empty()) frames.push_back(qtac::ByteArray(tok));
                    break;
                }
                const std::string tok = buf.substr(start, end - start);
                if (!tok.empty()) frames.push_back(qtac::ByteArray(tok));
                start = end + delim.size();
            }
        }

        // Deliver frames[1] (index 1) if it exists, matching original `frames[1].toLatin1()`.
        if (frames.size() > 1 && _frameFunction)
            _frameFunction(frames[1], _protocolInterface);

        if (_frameFunction)
            _frameFunction(qtac::ByteArray(), _protocolInterface);

        _receiveBuffer.clear();
        return;
    }

    // Buffer not yet complete — send empty sentinel and clear.
    if (_frameFunction)
        _frameFunction(qtac::ByteArray(), _protocolInterface);
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
qtac::ByteArray TACPIC32CXCoder::encode(const qtac::ByteArray& encodeMe, const Arguments& arguments)
{
    qtac::ByteArray result = encodeMe;

    if (arrayHash(encodeMe) == kPIC32CXSetPinCommandHash)
    {
        if (arguments.size() == 2)
        {
            std::string pinStr = std::to_string(std::get<uint32_t>(arguments.at(1)));

            // Prepend '0' if the pin string is fewer than 3 characters
            // (matches original: "if (argumentStr.size() < 3) argumentStr = '0' + argumentStr").
            if (pinStr.size() < 3)
                pinStr = "0" + pinStr;

            const qtac::ByteArray stateStr = argumentToBoolString(arguments.at(0));
            result = encodeMe + " " + stateStr + " (@" + pinStr.c_str() + ")";
        }
    }

    if (result.isEmpty() || result[result.size() - 1] != '\n')
        result += '\n';

    return result;
}

} // namespace qtac
