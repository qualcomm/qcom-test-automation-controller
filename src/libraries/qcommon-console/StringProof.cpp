// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "StringProof.h"
#include "EncryptedString.h"

const EncryptedString kWindowsTACConfigPath(QByteArrayLiteral("380d3a604b0e064b564070541504716514030a4e575d54646c5e43595057694250053b5a0e131d5e014365"));
const EncryptedString kLinuxTACConfigPath(QByteArrayLiteral("380d3a604b0e064b564070541504716514030a4e575d54646c5e43595057694250053b5a0e131d5e014365"));

void initializeStringProof()
{
	EncryptedString::setKey("{7f09aa97-45ae-4abf-8098-23836561fd9a}");
}
