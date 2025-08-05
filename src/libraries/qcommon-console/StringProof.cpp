/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
