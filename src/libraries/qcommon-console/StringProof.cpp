// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.
	
// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// Qualcomm Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
            Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "StringProof.h"
#include "EncryptedString.h"

const EncryptedString kWindowsTACConfigPath(QByteArrayLiteral("380d3a604b0e064b564070541504716514030a4e575d54646c5e43595057694250053b5a0e131d5e014365"));
const EncryptedString kLinuxTACConfigPath(QByteArrayLiteral("380d3a604b0e064b564070541504716514030a4e575d54646c5e43595057694250053b5a0e131d5e014365"));
const EncryptedString kWindowsEPMConfigPath(QByteArrayLiteral("380d3a604b0e064b564070541504716514030a4e575d54646c5e435950576953410b3b5a0e131d5e014365"));
const EncryptedString kLinuxEPMConfigPath(QByteArrayLiteral("380d3a604b0e064b564070541504716514030a4e575d54646c5e435950576953410b3b5a0e131d5e014365"));
const EncryptedString kLicenseCheckFailed(QByteArrayLiteral("375e055557120419744551560a456b55080e0349"));
const EncryptedString kLicenseManagerFailed(QByteArrayLiteral("375e0555571204197a4c5a5406005f1408114643574419485f57405d5d42"));

void initializeStringProof()
{
	EncryptedString::setKey("{7f09aa97-45ae-4abf-8098-23836561fd9a}");
}

QByteArray licenseCheckFailed()
{
	return kLicenseCheckFailed.decode();
}

QByteArray licenseManagerFailed()
{
	return kLicenseManagerFailed.decode();
}
