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

// Command hash constants — numeric values match the original Qt implementation.
constexpr HashType kVersionCommandHash          {178099058};
constexpr HashType kGetNameCommandHash          {639527594};
constexpr HashType kSetNameCommandHash          {639527606};
constexpr HashType kGetUUIDCommandHash          {583302694};
constexpr HashType kGetPlatformIDCommandHash    {441876121};
constexpr HashType kGetResetCountCommandHash    {432158534};
constexpr HashType kClearResetCountCommandHash  {845787175};
constexpr HashType kI2CReadRegisterCommandHash  {333696227};
constexpr HashType kI2CReadRegisterValueCommandHash {675332837};
constexpr HashType kI2CWriteRegisterCommandHash {533512490};
constexpr HashType kSetPinCommandHash           {703510564};

// PIC32CX hashes
constexpr HashType kPIC32CXClearBufferHash      {81534733};
constexpr HashType kPIC32CXVersionCommandHash   {22031998};
constexpr HashType kPIC32CXSetPinCommandHash    {15032008};
