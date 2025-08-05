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
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "ConsoleApplicationEnhancements.h"
#include "FTDITemplateCompiler.h"
#include "PlatformID.h"

// Qt
#include <QDir>
#include <QFile>
#include <QTextStream>

// C++
#include <bitset>
#include <iostream>

#ifdef Q_OS_WIN
	#ifdef DEBUG
		const QString xmlTemplatePath(QStringLiteral("C:\\github\\AlpacaRepos\\__Builds\\x64\\Debug\\bin\\ftdi-template.xml"));
	#else
		const QString xmlTemplatePath(QStringLiteral("C:\\Program Files (x86)\\Qualcomm\\QTAC\\ftdi-template.xml"));
	#endif
#endif

#ifdef Q_OS_LINUX
	#ifdef DEBUG
		const QString xmlTemplatePath(QStringLiteral("/local/mnt/workspace/github/AlpacaRepos/__Builds/Linux/Debug/bin/ftdi-template.xml"));
	#else
		const QString xmlTemplatePath(QStringLiteral("/opt/qcom/QTAC/bin/ftdi-template.xml"));
	#endif
#endif

const QString kUSBDescriptorPattern(QStringLiteral("%%USB_DESCRIPTOR%%"));


FTDITemplateCompiler::FTDITemplateCompiler()
{

}

void FTDITemplateCompiler::setDeviceListDir(const QString &fileDir)
{
	_fileDir = fileDir;
}

void FTDITemplateCompiler::setVerbosity(bool verbose)
{
	_verbose = verbose;
}

void FTDITemplateCompiler::write()
{
	bool result{true};

	if (_verbose)
		std::cout << "\nGenerating FTDI programmable template for debug boards...\n";

	result = load();
	if(!result)
		std::cout << "Error: " << _lastError.toStdString() << "\n";
	else
	{
		PlatformIDList platformIDList = PlatformContainer::getDebugBoards();
		for (auto& platformID: platformIDList)
		{
			DebugBoardType boardType = platformID->_boardtype;
			if (boardType == eFTDI)
			{
				QString xmlDocumentName = QString("TAC_%1_%2").arg(debugBoardTypeToString(boardType)).arg(platformID->_platformID);
				QString usbDescriptorString = platformID->_usbDescriptor;

				save(xmlDocumentName, usbDescriptorString, platformID->_pinSets);

				if (_verbose)
					std::cout << "   Saved FTDI programmer template for: " << platformID->_usbDescriptor.data() << std::endl;
			}
		}
	}
}

bool FTDITemplateCompiler::load()
{
	bool result{false};
	QFile xmlFile(xmlTemplatePath);

	if (_template.isNull() && xmlFile.open(QIODevice::ReadOnly))
	{
		QTextStream stream(&xmlFile);
		_template = stream.readAll();
		xmlFile.close();
		result = true;
	}
	else
		_lastError = "The FTDI template was not found.";

	if (_verbose && result)
		std::cout << "The FTDI template was loaded successfully\n";

	return result;
}

void FTDITemplateCompiler::save(const QString& xmlDocumentName, const QString& usbDescriptorString, FTDIPinSets* pinSets)
{
	QDir saveDir(_fileDir);
	QFile xmlDocument;

	if (_fileDir.isEmpty())
		saveDir = QDir::cleanPath(tacConfigRoot());

	// TODO: Code rework is required if multiple chipsets are supported on FTDI
	// Currently, only 1 chip's data is accounted for in the FTDI XML template
	int firstChipBusComposition = pinSets[0];
	std::bitset<4> chipSet1(firstChipBusComposition);

	if (!_template.isEmpty())
	{
		QString xmlData = _template;
		xmlData.replace(kUSBDescriptorPattern, usbDescriptorString);

		for (int i{0}; i<kMaxPinSetCount; i++)
		{
			if (chipSet1.test(i))
			{
				xmlData.replace(QString("%%Port_%1_D2XX%%").arg((char)('A'+i)), "true");
				xmlData.replace(QString("%%Port_%1_VCP%%").arg((char)('A'+i)), "false");
			}
			else
			{
				xmlData.replace(QString("%%Port_%1_D2XX%%").arg((char)('A'+i)), "false");
				xmlData.replace(QString("%%Port_%1_VCP%%").arg((char)('A'+i)), "true");
			}
		}

		xmlDocument.setFileName(saveDir.absoluteFilePath(xmlDocumentName + ".xml"));
		if (xmlDocument.open(QIODevice::WriteOnly))
		{
			xmlDocument.write(xmlData.toLatin1());
			xmlDocument.close();
		}
	}
}
