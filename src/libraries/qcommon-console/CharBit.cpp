// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "CharBit.h"

#include <QBitArray>

CharBit::CharBit()
{

}

void CharBit::set
(
	int position,
	bool value
)
{
	QBitArray bitArray = QBitArray::fromBits(reinterpret_cast<char*>(&_bits), 8);

	bitArray.setBit(position % 8, value);

	_bits = *bitArray.bits();
}
