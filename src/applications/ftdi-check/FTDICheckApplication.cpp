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

#include "FTDICheckApplication.h"

// QCommon
#include "version.h"

// Qt
#include <QDir>
#include <QProcess>
#include <QSettings>

// Windows
#ifdef Q_OS_WINDOWS
	#include <psapi.h>
#endif
#ifdef Q_OS_LINUX
	#include <sys/stat.h>
#endif

// C++
#include <iostream>

// Windows driver paths
const QString kWindowsDriverDir(QStringLiteral("C:\\Windows\\INF\\"));

const QString kInstallerX64FTDIBusDriverPath(QStringLiteral("C:\\ProgramData\\Qualcomm\\QTAC\\FTDI\\x64\\ftdibus.inf"));
const QString kInstallerX64FTDIPortDriverPath(QStringLiteral("C:\\ProgramData\\Qualcomm\\QTAC\\FTDI\\x64\\ftdiport.inf"));

const QString kInstallerARMFTDIBusDriverPath(QStringLiteral("C:\\ProgramData\\Qualcomm\\QTAC\\FTDI\\arm\\ARM64\\Release\\ftdibus.inf"));
const QString kInstallerARMFTDIPortDriverPath(QStringLiteral("C:\\ProgramData\\Qualcomm\\QTAC\\FTDI\\arm\\ARM64\\Release\\ftdiport.inf"));

const QString kWindowsFTDIBusDriverPath(QStringLiteral("C:\\Windows\\System32\\drivers\\ftdibus.sys"));
const QString kWindowsFTDIPortDriverPath(QStringLiteral("C:\\Windows\\System32\\drivers\\ftser2k.sys"));

// Linux driver paths
const QString kLinuxDriverDir(QStringLiteral("/usr/local/lib/"));
const QString kLinuxDriverPath(QStringLiteral("/usr/local/lib/libftd2xx.so.1.4.27"));
const QString kLinuxDriverLinkPath(QStringLiteral("/usr/local/lib/libftd2xx.so"));
const QString kInstallerDriverDir(QStringLiteral("~/QTAC/FTDI/release/build"));


FTDICheckApplication::FTDICheckApplication()
{
	_expectedProductVersionMap =
	{
		{"ftbusui.dll", "2.12.36.4"},
		{"ftcserco.dll", "2.12.36.4"},
		{"ftd2xx.dll", "2.12.36.4"},
		{"ftlang.dll", "2.12.36.4"},
		{"ftserui2.dll", "2.12.36.4"},
		{"ftdibus.sys", "2.12.36.4"},
		{"ftser2k.sys", "2.12.36.4"}
	};
}

FTDICheckApplication::~FTDICheckApplication()
{

}

/**
 * @brief FTDICheckApplication::exec - The public method which is responsible for validating the presence of the appropriate driver.
 * @return 0 if there are no errors.
 */
int FTDICheckApplication::defaultActivity()
{
	bool result{false};

	result = verifyInstalledDriver();

	if (!result)
	{
		uninstallDriver();

		if (installDriver())
		{
			verifyInstalledDriver();
		}
	}
	return result;
}

void FTDICheckApplication::showVersion()
{
	std::cout << "Version " << FTDI_CHECK_VERSION << std::endl;
}

bool FTDICheckApplication::verifyInstalledDriver()
{
	bool result{true};

#ifdef Q_OS_WINDOWS
	QStringList expectedLibrariesList;

	std::cout << "\nVerifying installed driver...\nLooking for the FTDI driver: CDM v2.12.36.4\n";

	for (auto &fileData : _expectedProductVersionMap.keys())
	{
		// Do not add driver files to the library list as they are driver files
		if (fileData.compare("ftdibus.sys") != 0 && fileData.compare("ftser2k.sys") != 0)
			expectedLibrariesList.push_back(fileData);
	}

	// Add ftd2xx64.dll to the library list if the processor is not AMD64-based.
	if (getArchitecture() == ArchType::eX32)
		expectedLibrariesList.push_back("ftd2xx64.dll");

	for (const auto &libName : expectedLibrariesList)
	{
		if (!result)
			break;

		result = detectLibrary(libName);
	}

	for (auto& libPath : _detectedFiles)
	{
		if (!result)
			break;

		result = checkLibraryVersion(libPath);
	}

	if (result)
		result = isWindowsFTDIDriverFilesPresent() && isDriverLatest();
#endif
#ifdef Q_OS_LINUX
	std::cout << "\nVerifying installed driver...\nLooking for the FTDI driver: CDM v1.4.27\n";

	QFile expectedDriverFile(kLinuxDriverPath);
	QFile linkDriverFile(kLinuxDriverLinkPath);

	// If the file exists and is writable, check if the linked file points toward expectedDriverFilePath
	if (expectedDriverFile.exists())
	{
		if (expectedDriverFile.isWritable())
		{
			if (linkDriverFile.exists())
			{
				if (linkDriverFile.isWritable())
				{
					QString targetLinkPath = linkDriverFile.symLinkTarget();
					if (targetLinkPath != expectedDriverFile.fileName())
						result = false;
				}
				else
				{
					std::cout << "\nThe driver link file was found but is not writable by current user. Fixing permissions...\n";
					result = false;
				}
			}
		}
		else
		{
			std::cout << "\nThe driver file was found but is not writable by current user. Fixing permissions...\n";
			result = false;
		}
	}
	else
	{
		std::cout << "\nCould not find the expected FTDI driver. Installing...\n";
		result = false;
	}
#endif
	return result;
}

bool FTDICheckApplication::uninstallDriver()
{
	bool result{true};
	std::cout << "\nAttempting to uninstall driver...\n\n";

#ifdef Q_OS_WINDOWS
	QDir windowsINFDir(kWindowsDriverDir);

	if (windowsINFDir.exists() && windowsINFDir.isReadable())
	{
		windowsINFDir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
		windowsINFDir.setNameFilters({"oem*.inf", "ftdibus.inf", "ftdiport.inf"});

		bool ftdiInfFound{false};

		for (auto &filePath : windowsINFDir.entryList())
		{
			QString absoluteFilePath = kWindowsDriverDir + filePath;
			if (isFTDIDriverInfFile(absoluteFilePath))
			{
				bool status = deleteWindowsDriver(filePath);
				ftdiInfFound = true;

				if (!status)
				{
					std::cout << "Could not uninstall driver. Are you root? Is pnputil program installed on the machine?\n";
					result = false;
					break;
				}
			}
		}

		if (!ftdiInfFound)
		{
			std::cout << "No FTDI drivers were found. Nothing to uninstall\n";
		}

		if (windowsINFDir.entryList().size() == 0)
		{
			std::cout << "Could not find any driver files at path: " << kWindowsDriverDir.toStdString() << std::endl;
			result = false;
		}
	}
	else
	{
		std::cout << "The Windows INF directory does not exists at: " << kWindowsDriverDir.toStdString() << " or is not accessible. Are you root?\n";
		result = false;
	}
#endif
#ifdef Q_OS_LINUX
	result = deleteLinuxDriver();
#endif
	return result;
}

bool FTDICheckApplication::installDriver()
{
	bool result{false};
#ifdef Q_OS_WINDOWS
	QStringList driverInfPathList;

	if (getArchitecture() == ArchType::eX64)
		driverInfPathList = {kInstallerX64FTDIBusDriverPath, kInstallerX64FTDIPortDriverPath};
	else if (getArchitecture() == ArchType::eARM)
		driverInfPathList = {kInstallerARMFTDIBusDriverPath, kInstallerARMFTDIPortDriverPath};

	std::cout << "\nAttempting to install driver...\n\n";

	for (const auto &driverFile : driverInfPathList)
		result = addWindowsDriver(driverFile);
#endif
#ifdef Q_OS_LINUX
	result = addLinuxDriver();
#endif
	return result;
}

/**
 * @brief FTDICheckApplication::getArchitecture - Identify the native architecture of host system.
 * @return ArchType defined enum value based on architecture
 */
ArchType FTDICheckApplication::getArchitecture()
{
	// List of native architecture constants available at: https://learn.microsoft.com/en-us/windows/win32/sysinfo/image-file-machine-constants
	// Why this check? https://ftdichip.com/faq/with-the-latest-drivers-two-sets-are-supplied-for-i386-and-amd64-for-win-64-and-intel-processor-should-we-be-using-the-i386-drivers/

	ArchType archType{ArchType::eUnknown};

#ifdef Q_OS_WINDOWS
	USHORT processMachine, nativeMachine;
	HANDLE currProcess = GetCurrentProcess();

	IsWow64Process2(currProcess, &processMachine, &nativeMachine);
	switch(nativeMachine)
	{
		case IMAGE_FILE_MACHINE_I386:
			archType = ArchType::eX32;
			break;
		case IMAGE_FILE_MACHINE_AMD64:
			archType = ArchType::eX64;
			break;
		case IMAGE_FILE_MACHINE_ARM64:
			archType = ArchType::eARM;
			break;
	}

#endif
	return archType;
}

/**
 * @brief FTDICheckApplication::detectLibrary - figures out the absolute path of the expected library and updates the `_detectedFiles` list with their absolute paths
 * @param libraryName - the name of the library we're looking for
 * @return true if the library was found
 */
bool FTDICheckApplication::detectLibrary(const QString &libraryName)
{
	bool result{true};

#ifdef Q_OS_WINDOWS
	HINSTANCE hinstLib;

	hinstLib = LoadLibraryA(libraryName.toStdString().c_str());

	if (hinstLib != NULL)
	{
		char* buffer = new char[1024];
		GetModuleFileNameA(hinstLib, buffer, 1024);

		_detectedFiles.push_back(buffer);
	}
	else
	{
		std::cout << "The '" << libraryName.toStdString() << "' library was not found!" << std::endl;
		result = false;
	}
#endif
	return result;
}

/**
 * @brief FTDICheckApplication::checkLibraryVersion - determines whether the current version of file is appropriate
 * @param filePath - The path to the file whose versions need to be validated
 * @return true if the machine contains the expected version of the file
 */
bool FTDICheckApplication::checkLibraryVersion(const QString& filePath)
{
	bool result{false};

#ifdef Q_OS_WINDOWS
	DWORD dwHandle;
	BYTE buffer[1024];
	PUINT versionSize = 0;
	VS_FIXEDFILEINFO* versionInfo = NULL;

	QString fileName = filePath.split('\\').back();

	DWORD bufferSize = GetFileVersionInfoSizeA(filePath.toStdString().c_str(), &dwHandle);

	if ((bufferSize > 0) && GetFileVersionInfoA(filePath.toStdString().c_str(), dwHandle, bufferSize, buffer))
	{
		VerQueryValue(buffer, L"\\", (LPVOID*)&versionInfo, versionSize);
	}

	if (versionInfo != nullptr)
	{
		char buf[25];

		sprintf(buf, "%hu.%hu.%hu.%hu", HIWORD(versionInfo->dwProductVersionMS), LOWORD(versionInfo->dwProductVersionMS), HIWORD(versionInfo->dwProductVersionLS), LOWORD(versionInfo->dwProductVersionLS));
		result = isExpectedProductVersion(fileName, buf);
	}
	else
	{
		std::cout << "Could not find the version information for '" << fileName.toStdString() << "'" << std::endl;
	}
#endif
	return result;
}

/**
 * @brief FTDICheckApplication::isExpectedProductVersion - determines whether the product version matches the expected version
 * @param fileName - the name of the file whose version needs to be checked
 * @param version - the product-version of the file as found on the machine
 * @return true if version is the expected version
 */
bool FTDICheckApplication::isExpectedProductVersion(const QString& fileName, const QString& version)
{
	bool result{false};
	auto fileData = _expectedProductVersionMap.find(fileName);
	if (fileData != _expectedProductVersionMap.end())
	{
		if (fileData.value() == version)
			result = true;
	}
	else
		std::cout << "Could not find the expected product version for: " << fileName.toStdString() << std::endl;

	if(!result)
		std::cout << "Found an unexpected product version '" << version.toStdString() << "' for file: " << fileName.toStdString() << std::endl;

	return result;
}

bool FTDICheckApplication::isFTDIDriverInfFile(const QString& filePath)
{
	bool result{false};

	QSettings* infSettings = new QSettings(filePath, QSettings::IniFormat);

	infSettings->beginGroup("Strings");
	QString ftdiString = infSettings->value("Ftdi", "null").toString();
	infSettings->endGroup();

	if (ftdiString == "FTDI")
		result = true;

	return result;
}

bool FTDICheckApplication::isWindowsFTDIDriverFilesPresent()
{
	int driverCount;
	bool result{false};

	bool ftdiBusPresent{false};
	bool ftser2kPresent{false};

#ifdef Q_OS_WINDOWS
	LPVOID driverList[1024];
	DWORD actualDriverListSize;

	if (EnumDeviceDrivers(driverList, sizeof(driverList), &actualDriverListSize))
	{
	  TCHAR driverName[1024];

	  driverCount = actualDriverListSize/sizeof(driverList[0]);

	  for (int i=0; i < driverCount; i++)
	  {
		if (GetDeviceDriverBaseName(driverList[i], driverName, sizeof(driverName)/sizeof(driverName[0])))
		{
			char buffer[25];
			sprintf(buffer, "%ls", driverName);

			if (std::string("ftdibus.sys").compare(buffer) == 0)
				ftdiBusPresent = true;

			else if (std::string("ftser2k.sys").compare(buffer) == 0)
				ftser2kPresent = true;
		}
	  }
	}
	else
		std::cout << "isFTDIDriverFilesPresent() failed. EnumDeviceDrivers() needs an array size of: " << actualDriverListSize/sizeof(LPVOID) << std::endl;

	if (ftdiBusPresent && ftser2kPresent)
		result = true;
	else
		std::cout << "Could not find the required drivers." << std::endl;
#endif
	return result;
}

bool FTDICheckApplication::isLinuxFTDIDriverFilesPresent()
{
	bool result{false};
	QDir driverPackage(kInstallerDriverDir);

	driverPackage.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
	driverPackage.setNameFilters({"libftd2xx.*"});

	if (driverPackage.exists())
	{
		int fileCount = driverPackage.entryList().count();
		if (fileCount == 3)
		result = true;
	}
	return result;
}

bool FTDICheckApplication::isDriverLatest()
{
	bool result{false};
	QStringList  driverPaths = {kWindowsFTDIBusDriverPath, kWindowsFTDIPortDriverPath};

	for (const auto &filePath : driverPaths)
	{
		result = checkLibraryVersion(filePath);
	}

	if (result)
		std::cout << "\nThe expected driver version is already installed. Nothing to install." << std::endl;

	return result;
}

bool FTDICheckApplication::deleteWindowsDriver(const QString &infFile)
{
	bool result{true};

	std::cout << "Attempting to delete driver using driver path: " << infFile.toStdString() << std::endl;

	QProcess* process = new QProcess();
	int exitCode = process->execute("pnputil", {"/delete-driver", infFile, "/uninstall", "/force"});
	process->waitForFinished();

	if (exitCode != 0)
	{
		std::cout << "\nThe driver delete operation was unsuccessful. Exit code: " << exitCode;
		result = false;
	}

	std::cout << std::endl;

	return result;
}

bool FTDICheckApplication::addWindowsDriver(const QString &infFile)
{
	bool result{true};

	std::cout << "Attempting to install driver from driver path: " << infFile.toStdString() << std::endl;

	QProcess* process = new QProcess();
	int exitCode = process->execute("pnputil", {"/add-driver", infFile, "/install"});
	process->waitForFinished();

	if (exitCode != 0)
	{
		std::cout << "\nThe driver installation operation was unsuccessful. Exit code: " << exitCode;
		result = false;
	}

	std::cout << std::endl;

	return result;
}

bool FTDICheckApplication::deleteLinuxDriver()
{
	bool result{true};

	if (!isLinuxFTDIDriverFilesPresent())
	{
		std::cout << "The linux driver package was not found at '"<< kInstallerDriverDir.toStdString() << "'. Uninstall will not proceed for safety reasons." << std::endl;
		result = false;
	}
	else
	{
		QDir driverDirectory(kLinuxDriverDir);
		driverDirectory.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
		driverDirectory.setNameFilters({"libftd2xx.*"});

		for (const auto &fileName: driverDirectory.entryList())
		{
			result = QFile(kLinuxDriverDir + QDir::separator() + fileName).remove();
			if (!result)
			std::cout << "Could not remove file with name: '" << fileName.toStdString() << "'. Try running as root.\n";
		}
	}
	return result;
}

bool FTDICheckApplication::addLinuxDriver()
{
	bool result{true};
	QStringList driverList = {"libftd2xx.a", "libftd2xx.so.1.4.27", "libftd2xx.txt"};

	if (isLinuxFTDIDriverFilesPresent())
	{
		for (auto& fileName : driverList)
		{
			QFile driverFile(kInstallerDriverDir + QDir::separator() + fileName);
			driverFile.copy(kLinuxDriverDir + QDir::separator() + fileName);
			result = driverFile.setPermissions(QFileDevice::ExeOther);
			if (!result)
			{
				std::cout << "Failed to set global execute permisssion for '" + fileName.toStdString() + "'\n";
				break;
			}
		}
	}
	else
	{
		std::cout << "The linux driver package was not found at '"<< kInstallerDriverDir.toStdString() << "'\n";
		result = false;
	}

	return result;
}
