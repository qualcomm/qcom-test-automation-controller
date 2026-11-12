// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "AlpacaDefines.h"

uint32_t makeFirmwareVersion
(
	uint32_t hw,
	uint32_t major,
	uint32_t minor
)
{
	return (hw << 16) | (major << 8) | minor;
}
