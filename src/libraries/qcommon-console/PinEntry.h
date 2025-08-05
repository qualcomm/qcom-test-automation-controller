#ifndef PINENTRY_H
#define PINENTRY_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
    Author: Michael Simpson (msimpson@qti.qualcomm.com)
            Biswajit Roy (biswroy@qti.qualcomm.com)
*/


// qcommon-console
#include "CommandGroup.h"
#include "PinID.h"
#include "StringUtilities.h"

struct PinEntry
{
    PinID						_pin{0};
    bool						_enabled{false};
    HashType					_hash{0};
    QString						_pinLabel;
    QString						_pinTooltip;
    bool						_initialValue{false};
    int							_initializationPriority{0};
    bool						_inverted{false};
    QString						_pinCommand;
    CommandGroups				_commandGroup{eUnknownCommandGroup};
    QPoint						_cellLocation{QPoint(-1,-1)};
    QString						_tabName{"General"};
};

typedef QList<PinEntry> Pins;

#endif // PINENTRY_H
