// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
