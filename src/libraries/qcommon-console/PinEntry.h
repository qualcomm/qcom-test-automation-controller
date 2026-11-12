#ifndef PINENTRY_H
#define PINENTRY_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommonConsole
#include "CommandGroup.h"
#include "PinID.h"
#include "StringUtilities.h"

#include <string>
#include <utility>
#include <vector>

struct PinEntry
{
    PinID						_pin{0};
    bool						_enabled{false};
    HashType					_hash{0};
    std::string					_pinLabel;
    std::string					_pinTooltip;
    bool						_initialValue{false};
    int							_initializationPriority{0};
    bool						_inverted{false};
    std::string					_pinCommand;
    CommandGroups				_commandGroup{eUnknownCommandGroup};
    std::pair<int, int>			_cellLocation{-1, -1};
    std::string					_tabName{"General"};
};

typedef std::vector<PinEntry> Pins;

#endif // PINENTRY_H
