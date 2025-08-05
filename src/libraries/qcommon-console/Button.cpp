// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "Button.h"

HashType Button::makeHash(const Button &button)
{
	HashType hash{0};

	hash = strHash(button._label + button._tab);

	return hash;
}
