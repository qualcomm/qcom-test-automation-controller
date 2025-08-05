#ifndef ALPACADEFINES_H
#define ALPACADEFINES_H
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
*/

#include "QCommonConsoleGlobal.h"
#include "StringProof.h"
#include "version.h"

#include <QString>

const QString kProductName(QStringLiteral("QTAC"));
const char kProductID[] =	"1e48f695-c109-11ec-aebb-063166a9270b"; // QTAC External
const char kCoreFeature[] = "1e71efc7-c109-11ec-aebb-063166a9270b";

const QString kOrganizationName(QStringLiteral("Qualcomm, Inc."));
const QString kProductVersion(QStringLiteral(ALPACA_VERSION));
const QString kCompileDate(QStringLiteral(__DATE__));
const QString kCompileTime(QStringLiteral(__TIME__));
const QString kBuildTime(kCompileDate + QStringLiteral(" ") + kCompileTime);
const QString kVersionGUID(QStringLiteral("{6F01E0AB-1962-4054-C061-3CA7CA4397E0}")); // produced by GUID Maker

quint32 QCOMMONCONSOLE_EXPORT makeFirmwareVersion(quint32 hw, quint32 major, quint32 minor);

#endif // ALPACADEFINES_H
