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

#ifndef QTAC_FTDIPINSET_H
#define QTAC_FTDIPINSET_H

#include <cstdint>

namespace qtac {

const int kMaxPinSetCount{4};

enum FTDIPinSet : uint32_t
{
	NoOptions = 0x0,
	eA        = 0x1,
	eB        = 0x2,
	eC        = 0x4,
	eD        = 0x8
};

// Replaces Q_DECLARE_FLAGS(FTDIPinSets, FTDIPinSet)
using FTDIPinSets = uint32_t;

constexpr FTDIPinSets operator|(FTDIPinSet a, FTDIPinSet b)
{
	return static_cast<uint32_t>(a) | static_cast<uint32_t>(b);
}

constexpr FTDIPinSets operator|(FTDIPinSets a, FTDIPinSet b)
{
	return a | static_cast<uint32_t>(b);
}

inline bool testFlag(FTDIPinSets set, FTDIPinSet flag)
{
	return (set & static_cast<uint32_t>(flag)) != 0;
}

} // namespace qtac

// Bring into global namespace so code using unqualified FTDIPinSet/FTDIPinSets still compiles
using qtac::FTDIPinSet;
using qtac::FTDIPinSets;
using qtac::NoOptions;
using qtac::eA;
using qtac::eB;
using qtac::eC;
using qtac::eD;
using qtac::testFlag;
using qtac::kMaxPinSetCount;

#endif // QTAC_FTDIPINSET_H
