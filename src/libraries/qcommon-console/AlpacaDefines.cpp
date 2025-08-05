// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "AlpacaDefines.h"

quint32 makeFirmwareVersion
(
	quint32 hw,
	quint32 major,
	quint32 minor
)
{
	return (hw << 16) | (major << 8) | minor;
}
