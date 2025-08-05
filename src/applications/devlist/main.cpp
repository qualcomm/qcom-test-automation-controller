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
