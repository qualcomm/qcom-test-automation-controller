#ifndef CONSOLEAPPLICATIONENHANCEMENTS_H
#define CONSOLEAPPLICATIONENHANCEMENTS_H
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

// QCommon
#include "QCommonConsoleGlobal.h"

// QT
#include <QSettings>
#include <QString>


QString QCOMMONCONSOLE_EXPORT applicationBinPath();
QString QCOMMONCONSOLE_EXPORT applicationDataPath();
QString QCOMMONCONSOLE_EXPORT documentsDataPath(const QString& append);
QString QCOMMONCONSOLE_EXPORT defaultGlobalLoggingPath();
QString QCOMMONCONSOLE_EXPORT defaultLoggingPath(const QString& appName);

QString QCOMMONCONSOLE_EXPORT killOneDrive(const QString& testPath, const QString& revertPath);

QString QCOMMONCONSOLE_EXPORT createFilenameTimeStamp();

bool QCOMMONCONSOLE_EXPORT isNewInstall(QSettings& settings, const QString& currentInstallGuuid);
void QCOMMONCONSOLE_EXPORT renewInstallRegistry(QSettings& settings, const QString& newInstallGuid);

QString QCOMMONCONSOLE_EXPORT getModuleFilePath(const QString& moduleFileName);
QString QCOMMONCONSOLE_EXPORT expandPath(const QString& filePath);

QString QCOMMONCONSOLE_EXPORT tacConfigRoot(bool expandPath = true);
QString QCOMMONCONSOLE_EXPORT epmConfigRoot();

void kickIt();
void cleanIt(const QString& pathToClean);

bool QCOMMONCONSOLE_EXPORT isUserPrivileged();
bool QCOMMONCONSOLE_EXPORT executeBinaryAsAdministrator(const QString &binary, const QString &cmdArgs);

#endif

