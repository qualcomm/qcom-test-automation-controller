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

#ifndef QTAC_PLATFORMID_H
#define QTAC_PLATFORMID_H

#include <qtac/String.h>
#include <qtac/ByteArray.h>
#include <qtac/List.h>
#include <qtac/Map.h>
#include <qtac/DebugBoardType.h>
#include <qtac/FTDIPinSet.h>

#include <cstdint>
#include <memory>

using PlatformID = uint32_t;

const PlatformID MICRO_EPM_BOARD_ID_UNKNOWN{0};
const PlatformID ALPACA_LITE_ID{13};
const PlatformID ALPACA_PIC32CX_ID{50};

struct _PlatformEntry
{
	_PlatformEntry()
	{
		for (int i = 0; i < kMaxPinSetCount; ++i)
			_pinSets[i] = NoOptions;
	}

	_PlatformEntry(PlatformID platformID, DebugBoardType boardType,
	               const qtac::String& description,
	               const qtac::String& path = qtac::String(),
	               const qtac::ByteArray& usbDescriptor = qtac::ByteArray())
		: _platformID(platformID)
		, _boardtype(boardType)
		, _description(description)
		, _path(path)
		, _usbDescriptor(usbDescriptor)
	{
		for (int i = 0; i < kMaxPinSetCount; ++i)
			_pinSets[i] = NoOptions;
	}

	PlatformID      _platformID{0};
	DebugBoardType  _boardtype{eUnknownDebugBoard};
	qtac::String    _description;
	qtac::ByteArray _usbDescriptor;
	qtac::String    _path;
	FTDIPinSets     _pinSets[kMaxPinSetCount];
};

using PlatformEntry    = std::shared_ptr<_PlatformEntry>;
using PlatformIDList   = qtac::List<PlatformEntry>;
using PlatformIDs      = qtac::Map<PlatformID, PlatformEntry>;

class PlatformContainer
{
public:
	PlatformContainer() = delete;
	PlatformContainer(const PlatformContainer&) = delete;
	~PlatformContainer() = delete;

	static void initialize();
	static PlatformIDList getEntries();
	static void addEntry(PlatformEntry platformEntry);

	static qtac::String toString(PlatformID platformID);
	static PlatformID fromUSBDescriptor(const qtac::ByteArray& usbDescriptor);
	static DebugBoardType getDebugBoardType(PlatformID platformID);
	static PlatformIDList getDebugBoards();
	static PlatformIDList getDebugBoardsOfType(DebugBoardType debugBoardType);

private:
	static void initializeDynamic();
	static PlatformIDs _platformIds;
};

#endif // QTAC_PLATFORMID_H
