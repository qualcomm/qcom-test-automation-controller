#ifndef FTDIPINSET_H
#define FTDIPINSET_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

// Author: msimpson

// QT
#include <QtGlobal>

const int kMaxPinSetCount{4};

enum FTDIPinSet
{
		NoOptions = 0x0,
		eA = 0x1,
		eB = 0x2,
		eC = 0x4,
		eD = 0x8
};
Q_DECLARE_FLAGS(FTDIPinSets, FTDIPinSet)

#endif // FTDIDEVICE_H
