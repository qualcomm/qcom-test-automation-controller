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
// Qt-free reimplementation of TACCommandHashes.h from qcommon-console.

#pragma once

#include <qtac/StringUtilities.h>

// Command hash constants — generated at compile time by computeHashStr().
// The strings here are the canonical command names passed to encode().
constexpr HashType kVersionCommandHash              {computeHashStr("Version")};
constexpr HashType kGetNameCommandHash              {computeHashStr("Get Name")};
constexpr HashType kSetNameCommandHash              {computeHashStr("Set Name")};
constexpr HashType kGetUUIDCommandHash              {computeHashStr("Get UUID")};
constexpr HashType kGetPlatformIDCommandHash        {computeHashStr("Get Platform ID")};
constexpr HashType kGetResetCountCommandHash        {computeHashStr("Get Reset Count")};
constexpr HashType kClearResetCountCommandHash      {computeHashStr("Clear Reset Count")};
constexpr HashType kI2CReadRegisterCommandHash      {computeHashStr("I2C Read Register")};
constexpr HashType kI2CReadRegisterValueCommandHash {computeHashStr("I2C Read Register Value")};
constexpr HashType kI2CWriteRegisterCommandHash     {computeHashStr("I2C Write Register")};
constexpr HashType kSetPinCommandHash               {computeHashStr("SetPin")};

// PIC32CX hashes
constexpr HashType kPIC32CXClearBufferHash          {computeHashStr("echo 1")};
// kPIC32CXVersionCommandHash == kVersionCommandHash (both "Version")
constexpr HashType kPIC32CXSetPinCommandHash        {computeHashStr("CONF:DIG:ON")};
