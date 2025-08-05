#ifndef CHARBIT_H
#define CHARBIT_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

class QCOMMONCONSOLE_EXPORT CharBit
{
public:
	CharBit();

	void set(int position, bool value = true);
	unsigned char* value()
	{
		return &_bits;
	}

private:
	unsigned char	_bits{0};
};

#endif // CHARBIT_H
