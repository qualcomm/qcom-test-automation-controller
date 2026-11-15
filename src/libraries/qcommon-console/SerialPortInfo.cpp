// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "SerialPortInfo.h"

// QCommon
#include "StringUtilities.h"

#include <algorithm>
#include <fstream>
#include <string>

#if defined(_WIN32)
#include <windows.h>
#include <setupapi.h>
#pragma comment(lib, "setupapi.lib")

static const GUID kGuidDevClassPorts = { 0x4d36e978, 0xe325, 0x11ce, { 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18 } };

typedef DWORD (WINAPI *FnCM_Get_Parent)(DWORD* pdnDevInst, DWORD dnDevInst, ULONG ulFlags);
typedef DWORD (WINAPI *FnCM_Get_Device_IDA)(DWORD dnDevInst, PSTR Buffer, ULONG BufferLen, ULONG ulFlags);

static std::string getParentDeviceInstanceId(DWORD devInst)
{
	std::string result;
	HMODULE hCfg = GetModuleHandleA("cfgmgr32.dll");
	if (!hCfg)
		hCfg = LoadLibraryA("cfgmgr32.dll");
	if (hCfg)
	{
		auto pCM_Get_Parent = reinterpret_cast<FnCM_Get_Parent>(GetProcAddress(hCfg, "CM_Get_Parent"));
		auto pCM_Get_Device_IDA = reinterpret_cast<FnCM_Get_Device_IDA>(GetProcAddress(hCfg, "CM_Get_Device_IDA"));
		if (pCM_Get_Parent && pCM_Get_Device_IDA)
		{
			DWORD parentInst = 0;
			if (pCM_Get_Parent(&parentInst, devInst, 0) == 0)
			{
				char buf[512] = {0};
				if (pCM_Get_Device_IDA(parentInst, buf, sizeof(buf), 0) == 0)
				{
					result = buf;
				}
			}
		}
	}
	return result;
}
#endif

#if defined(__linux__)
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#endif

SerialPortInfo::SerialPortInfo() :
	_isNull(true)
{
}

SerialPortInfo::SerialPortInfo(const std::string& name) :
	_portName(name),
	_systemLocation(name),
	_isNull(name.empty())
{
}

SerialPortInfo::SerialPortInfo(const std::string& name,
                               const std::string& description,
                               const std::string& manufacturer,
                               const std::string& serialNumber,
                               uint16_t vendorId,
                               uint16_t productId) :
	_portName(name),
	_systemLocation(name),
	_description(description),
	_manufacturer(manufacturer),
	_serialNumber(serialNumber),
	_vendorIdentifier(vendorId),
	_productIdentifier(productId),
	_hasVendorIdentifier(vendorId != 0),
	_hasProductIdentifier(productId != 0),
	_isNull(name.empty())
{
}

uint32_t SerialPortInfo::hash() const
{
	if (_hash == 0 && !_portName.empty())
	{
		_hash = ::strHash(_portName);
	}
	return _hash;
}

SerialPortInfos SerialPortInfo::availablePorts()
{
	SerialPortInfos result;

#if defined(_WIN32)
	HDEVINFO hDevInfo = SetupDiGetClassDevsA(&kGuidDevClassPorts, nullptr, nullptr, DIGCF_PRESENT);
	if (hDevInfo != INVALID_HANDLE_VALUE)
	{
		SP_DEVINFO_DATA devInfoData;
		devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

		for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); ++i)
		{
			HKEY hDevKey = SetupDiOpenDevRegKey(hDevInfo, &devInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
			if (hDevKey == INVALID_HANDLE_VALUE)
				continue;

			char portName[256] = {0};
			DWORD portNameSize = sizeof(portName);
			DWORD type = 0;
			LONG regStatus = RegQueryValueExA(hDevKey, "PortName", nullptr, &type, reinterpret_cast<LPBYTE>(portName), &portNameSize);
			RegCloseKey(hDevKey);

			if (regStatus != ERROR_SUCCESS || portName[0] == '\0')
				continue;

			char friendlyName[256] = {0};
			if (!SetupDiGetDeviceRegistryPropertyA(hDevInfo, &devInfoData, SPDRP_FRIENDLYNAME, nullptr,
			                                      reinterpret_cast<PBYTE>(friendlyName), sizeof(friendlyName), nullptr))
			{
				SetupDiGetDeviceRegistryPropertyA(hDevInfo, &devInfoData, SPDRP_DEVICEDESC, nullptr,
				                                  reinterpret_cast<PBYTE>(friendlyName), sizeof(friendlyName), nullptr);
			}

			char mfg[256] = {0};
			SetupDiGetDeviceRegistryPropertyA(hDevInfo, &devInfoData, SPDRP_MFG, nullptr,
			                                  reinterpret_cast<PBYTE>(mfg), sizeof(mfg), nullptr);

			char instanceId[512] = {0};
			SetupDiGetDeviceInstanceIdA(hDevInfo, &devInfoData, instanceId, sizeof(instanceId), nullptr);

			char hardwareId[512] = {0};
			SetupDiGetDeviceRegistryPropertyA(hDevInfo, &devInfoData, SPDRP_HARDWAREID, nullptr,
			                                  reinterpret_cast<PBYTE>(hardwareId), sizeof(hardwareId), nullptr);

			std::string idStr = instanceId;
			if (idStr.find("VID_") == std::string::npos && idStr.find("VID+") == std::string::npos && hardwareId[0] != '\0')
				idStr = hardwareId;

			uint16_t vid = 0;
			uint16_t pid = 0;
			std::string serialNumber;

			size_t vidPos = idStr.find("VID_");
			if (vidPos == std::string::npos)
				vidPos = idStr.find("VID+");
			if (vidPos != std::string::npos && vidPos + 8 <= idStr.size())
			{
				try {
					vid = static_cast<uint16_t>(std::stoul(idStr.substr(vidPos + 4, 4), nullptr, 16));
				} catch (...) {}
			}

			size_t pidPos = idStr.find("PID_");
			if (pidPos == std::string::npos)
				pidPos = idStr.find("PID+");
			if (pidPos != std::string::npos && pidPos + 8 <= idStr.size())
			{
				try {
					pid = static_cast<uint16_t>(std::stoul(idStr.substr(pidPos + 4, 4), nullptr, 16));
				} catch (...) {}
			}

			// Check for parent USB device to obtain true serial number if composite
			std::string parentId = getParentDeviceInstanceId(devInfoData.DevInst);
			if (!parentId.empty())
			{
				size_t parentSlash = parentId.rfind('\\');
				if (parentSlash != std::string::npos && parentSlash + 1 < parentId.size())
				{
					std::string candidate = parentId.substr(parentSlash + 1);
					if (candidate.find('&') == std::string::npos)
					{
						serialNumber = candidate;
					}
				}
			}

			if (serialNumber.empty())
			{
				std::string inst(instanceId);
				size_t lastSlash = inst.rfind('\\');
				if (lastSlash != std::string::npos && lastSlash + 1 < inst.size())
				{
					std::string candidate = inst.substr(lastSlash + 1);
					if (candidate.find('&') == std::string::npos)
					{
						serialNumber = candidate;
					}
				}
			}

			std::string desc = (friendlyName[0] != '\0') ? friendlyName : portName;
			std::string manufacturer = mfg;

			SerialPortInfo info(portName, desc, manufacturer, serialNumber, vid, pid);
			info._systemLocation = "\\\\.\\" + std::string(portName);
			result.push_back(info);
		}

		SetupDiDestroyDeviceInfoList(hDevInfo);
	}

	if (result.empty())
	{
		HKEY hKey;
		if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			char valueName[256];
			char data[256];
			DWORD valueNameSize;
			DWORD dataSize;
			DWORD type;
			DWORD index = 0;

			while (true)
			{
				valueNameSize = sizeof(valueName);
				dataSize = sizeof(data);

				LONG status = RegEnumValueA(hKey, index, valueName, &valueNameSize, nullptr, &type,
				                            reinterpret_cast<LPBYTE>(data), &dataSize);
				if (status != ERROR_SUCCESS)
					break;

				if (type == REG_SZ && dataSize > 0)
				{
					std::string portName(data);
					std::string desc(valueName);
					SerialPortInfo info(portName);
					info._description = desc;
					info._systemLocation = "\\\\.\\" + portName;
					result.push_back(info);
				}

				++index;
			}

			RegCloseKey(hKey);
		}
	}
#elif defined(__linux__)
	DIR* dir = opendir("/sys/class/tty");
	if (dir != nullptr)
	{
		struct dirent* entry;
		while ((entry = readdir(dir)) != nullptr)
		{
			std::string dname = entry->d_name;
			if (dname == "." || dname == "..")
				continue;

			// Check for device symlink
			std::string devPath = "/sys/class/tty/" + dname + "/device";
			if (access(devPath.c_str(), F_OK) == 0)
			{
				std::string ueventPath = devPath + "/uevent";
				std::ifstream ueventFile(ueventPath);
				uint16_t vid = 0;
				uint16_t pid = 0;
				std::string serial;

				if (ueventFile.is_open())
				{
					std::string line;
					while (std::getline(ueventFile, line))
					{
						if (line.rfind("PRODUCT=", 0) == 0)
						{
							// Format: PRODUCT=vid/pid/rev
							std::string prod = line.substr(8);
							auto slash1 = prod.find('/');
							if (slash1 != std::string::npos)
							{
								try {
									vid = static_cast<uint16_t>(std::stoul(prod.substr(0, slash1), nullptr, 16));
									auto slash2 = prod.find('/', slash1 + 1);
									if (slash2 != std::string::npos)
										pid = static_cast<uint16_t>(std::stoul(prod.substr(slash1 + 1, slash2 - slash1 - 1), nullptr, 16));
								} catch (...) {}
							}
						}
					}
				}

				SerialPortInfo info(dname, dname, "", serial, vid, pid);
				info._systemLocation = "/dev/" + dname;
				result.push_back(info);
			}
		}
		closedir(dir);
	}
#endif

	return result;
}

SerialPortInfo SerialPortInfo::getPortByName(const std::string& name)
{
	auto ports = availablePorts();
	for (const auto& port : ports)
	{
		if (port.portName() == name)
			return port;
	}
	return SerialPortInfo();
}

SerialPortInfo SerialPortInfo::getPortBySerialNumber(const std::string& serialNumber)
{
	auto ports = availablePorts();
	for (const auto& port : ports)
	{
		if (port.serialNumber() == serialNumber)
			return port;
	}
	return SerialPortInfo();
}

bool SerialPortInfo::operator==(const SerialPortInfo& other) const
{
	return _portName == other._portName;
}

bool SerialPortInfo::operator!=(const SerialPortInfo& other) const
{
	return _portName != other._portName;
}

bool SerialPortInfo::operator<(const SerialPortInfo& other) const
{
	return _portName < other._portName;
}

bool SerialPortInfo::operator<=(const SerialPortInfo& other) const
{
	return _portName <= other._portName;
}

bool equal(const SerialPortInfos& si1, const SerialPortInfos& si2)
{
	if (si1.size() != si2.size())
		return false;

	for (const auto& item : si1)
	{
		if (std::find(si2.begin(), si2.end(), item) == si2.end())
			return false;
	}
	return true;
}
