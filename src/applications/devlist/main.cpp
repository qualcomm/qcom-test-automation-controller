// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/


// qcommon-console
#include "ConsoleApplicationEnhancements.h"
#include "USBDescriptors.h"

// Qt
#include <QCoreApplication>
#include <QDir>

// C++
#include <iostream>
#include <iomanip>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QString kUSBDescriptorPath = tacConfigRoot() + "devicelist.json";

	USBDescriptors usbDescriptors;

	if (usbDescriptors.load(kUSBDescriptorPath) == true)
	{
		std::cout << "TAC Descriptors: " << std::endl;

		DescriptorList descriptorList = usbDescriptors.getDescriptors();

		std::cout << std::setw(25) << "Name" << std::setw(10) << "BoardType"  << std::setw(13) << "Platform ID" << std::setw(35) << "USB Descriptor" << std::setw(9) << "Revision" << " File Path" << std::endl;;
		std::cout << "_________________________" << " __________"  << " ____________" << " _________________________________" << " ________" << "  _______________________________" << std::endl;


		for (const auto& descriptor: std::as_const(descriptorList))
		{
			std::cout << std::setw(25) << descriptor._name.toLatin1().data();
			std::cout << std::setw(10) << debugBoardTypeToString(descriptor._debugBoardType).toLatin1().data();
			std::cout <<  std::setw(13) << descriptor._platformID;
			if (descriptor._usbDescriptor.isEmpty() == false)
				std::cout << std::setw(35) << descriptor._usbDescriptor.data();
			else
				std::cout << std::setw(35) << "-";

			std::cout << std::setw(9) << descriptor._revision;
			std::cout << " " << descriptor._configurationFilePath.data();
			std::cout << std::endl;
		}
	}
	else
	{
		std::cout << "Failed to open " << kUSBDescriptorPath.toLatin1().data() << std::endl;
	}
}
