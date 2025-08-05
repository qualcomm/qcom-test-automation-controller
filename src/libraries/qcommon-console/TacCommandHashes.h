#ifndef COMMAND_HASHES_H
#define COMMAND_HASHES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include <QtGlobal>
#include "StringUtilities.h"

const HashType kVersionCommandHash(178099058);
const HashType kGetNameCommandHash(639527594);
const HashType kSetNameCommandHash(639527606);
const HashType kGetUUIDCommandHash(583302694);
const HashType kGetPlatformIDCommandHash(441876121);
const HashType kGetResetCountCommandHash(432158534);
const HashType kClearResetCountCommandHash(845787175);
const HashType kI2CReadRegisterCommandHash(333696227);
const HashType kI2CReadRegisterValueCommandHash(675332837);
const HashType kI2CWriteRegisterCommandHash(533512490);
const HashType kSetPinCommandHash(703510564);

// PIC32CX Hashes
const HashType kPIC32CXClearBufferHash(81534733);
const HashType kPIC32CXVersionCommandHash(22031998);
const HashType kPIC32CXSetPinCommandHash(15032008);

#endif // COMMAND_HASHES_H
