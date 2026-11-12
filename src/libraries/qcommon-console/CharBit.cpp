// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "CharBit.h"

CharBit::CharBit()
{

}

void CharBit::set
(
	int position,
	bool value
)
{
	unsigned char bit = static_cast<unsigned char>(1 << (position % 8));

	if (value)
		_bits |= bit;
	else
		_bits &= static_cast<unsigned char>(~bit);
}
