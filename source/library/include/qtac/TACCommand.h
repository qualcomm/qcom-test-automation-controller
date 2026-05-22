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

#ifndef QTAC_TACCOMMAND_H
#define QTAC_TACCOMMAND_H

#include <qtac/String.h>
#include <qtac/ByteArray.h>
#include <qtac/List.h>
#include <qtac/Map.h>
#include <qtac/PinID.h>

class TACCommand;

using TACCommands   = qtac::List<TACCommand>;
using TACCommandMap = qtac::Map<qtac::String, TACCommand>;

class TACCommand
{
public:
	TACCommand() = default;
	TACCommand(const TACCommand&) = default;
	TACCommand(TACCommand&&) = default;
	~TACCommand() = default;

	TACCommand& operator=(const TACCommand&) = default;
	TACCommand& operator=(TACCommand&&) = default;

	qtac::String command() const { return _command; }

	static bool contains(const qtac::String& command, const TACCommands& tacCommands)
	{
		for (const auto& tc : tacCommands)
			if (tc._command == command)
				return true;
		return false;
	}

	static TACCommand find(const qtac::String& command, const TACCommands& tacCommands)
	{
		for (const auto& tc : tacCommands)
			if (tc._command == command)
				return tc;
		return TACCommand{};
	}

	static TACCommand find(PinID pin, const TACCommands& tacCommands)
	{
		for (const auto& tc : tacCommands)
			if (tc._pin == pin)
				return tc;
		return TACCommand{};
	}

	PinID           _pin{static_cast<PinID>(-1)};
	qtac::String    _command;
	qtac::String    _helpText;
	bool            _currentState{false};
	bool            _isInverted{false};
	qtac::ByteArray _tabName;
	qtac::ByteArray _groupName;
	qtac::ByteArray _cellLocation;
};

#endif // QTAC_TACCOMMAND_H
