// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//         notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//         copyright notice, this list of conditions and the following
//         disclaimer in the documentation and/or other materials provided
//         with the distribution.
//
//     * Neither the name of Qualcomm Technologies, Inc. nor the names of its
//         contributors may be used to endorse or promote products derived
//         from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
// HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Authors: Michael Simpson, Biswajit Roy
// Windows implementation of SerialPortInfo::availablePorts().
//
// libserialport's sp_get_port_transport() does not always detect USB transport
// for ports using the generic Windows usbser.sys driver (shows as "USB Serial
// Device" instead of "USB Serial Port"), causing VID/PID to come back as 0.
// The fallback reads VID/PID and serial number from the Windows device registry
// via SetupDI, which is exactly what Qt's QSerialPortInfo does internally.

#include <qtac/SerialPortInfo.h>

#include <libserialport.h>

#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#pragma comment(lib, "setupapi.lib")

namespace {

// Read VID/PID and revision from the Windows registry for a given COM port name.
// Parses the hardware ID string, e.g. "USB\VID_05C6&PID_9302&REV_0055\...".
static bool readVidPidFromRegistry(const std::string& portName, uint16_t& vid, uint16_t& pid, uint16_t& rev)
{
	vid = 0;
	pid = 0;
	rev = 0;

	HDEVINFO devInfo = SetupDiGetClassDevsA(&GUID_DEVINTERFACE_COMPORT, nullptr, nullptr,
		DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (devInfo == INVALID_HANDLE_VALUE)
		return false;

	SP_DEVINFO_DATA devInfoData{};
	devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	for (DWORD i = 0; SetupDiEnumDeviceInfo(devInfo, i, &devInfoData); ++i)
	{
		HKEY hKey = SetupDiOpenDevRegKey(devInfo, &devInfoData, DICS_FLAG_GLOBAL, 0,
			DIREG_DEV, KEY_READ);
		if (hKey == INVALID_HANDLE_VALUE)
			continue;

		char portNameBuf[256]{};
		DWORD portNameSize = sizeof(portNameBuf);
		DWORD type = 0;
		LONG res = RegQueryValueExA(hKey, "PortName", nullptr, &type,
			reinterpret_cast<LPBYTE>(portNameBuf), &portNameSize);
		RegCloseKey(hKey);

		if (res != ERROR_SUCCESS || type != REG_SZ)
			continue;
		if (_stricmp(portNameBuf, portName.c_str()) != 0)
			continue;

		char hwIdBuf[512]{};
		DWORD hwIdSize = sizeof(hwIdBuf);
		if (!SetupDiGetDeviceRegistryPropertyA(devInfo, &devInfoData, SPDRP_HARDWAREID,
			nullptr, reinterpret_cast<PBYTE>(hwIdBuf), hwIdSize, &hwIdSize))
			break;

		std::string hwId(hwIdBuf);
		auto vidPos = hwId.find("VID_");
		auto pidPos = hwId.find("PID_");
		if (vidPos != std::string::npos && pidPos != std::string::npos)
		{
			vid = static_cast<uint16_t>(std::stoul(hwId.substr(vidPos + 4, 4), nullptr, 16));
			pid = static_cast<uint16_t>(std::stoul(hwId.substr(pidPos + 4, 4), nullptr, 16));
		}
		auto revPos = hwId.find("REV_");
		if (revPos != std::string::npos)
		{
			try { rev = static_cast<uint16_t>(std::stoul(hwId.substr(revPos + 4, 4), nullptr, 16)); }
			catch (...) {}
		}
		break;
	}

	SetupDiDestroyDeviceInfoList(devInfo);
	return vid != 0;
}

// Read serial number from the device instance ID for a COM port.
// Instance ID format: "USB\VID_XXXX&PID_YYYY\SERIALNUMBER"
static std::string readSerialFromRegistry(const std::string& portName)
{
	HDEVINFO devInfo = SetupDiGetClassDevsA(&GUID_DEVINTERFACE_COMPORT, nullptr, nullptr,
		DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (devInfo == INVALID_HANDLE_VALUE)
		return {};

	SP_DEVINFO_DATA devInfoData{};
	devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	std::string result;

	for (DWORD i = 0; SetupDiEnumDeviceInfo(devInfo, i, &devInfoData); ++i)
	{
		HKEY hKey = SetupDiOpenDevRegKey(devInfo, &devInfoData, DICS_FLAG_GLOBAL, 0,
			DIREG_DEV, KEY_READ);
		if (hKey == INVALID_HANDLE_VALUE)
			continue;

		char portNameBuf[256]{};
		DWORD portNameSize = sizeof(portNameBuf);
		DWORD type = 0;
		LONG res = RegQueryValueExA(hKey, "PortName", nullptr, &type,
			reinterpret_cast<LPBYTE>(portNameBuf), &portNameSize);
		RegCloseKey(hKey);

		if (res != ERROR_SUCCESS || _stricmp(portNameBuf, portName.c_str()) != 0)
			continue;

		char instanceId[256]{};
		if (SetupDiGetDeviceInstanceIdA(devInfo, &devInfoData, instanceId,
			sizeof(instanceId), nullptr))
		{
			std::string id(instanceId);
			auto lastSlash = id.rfind('\\');
			if (lastSlash != std::string::npos)
			{
				std::string serial = id.substr(lastSlash + 1);
				// Windows-generated instance IDs contain '&'; real serials do not
				if (serial.find('&') == std::string::npos)
					result = serial;
			}
		}
		break;
	}

	SetupDiDestroyDeviceInfoList(devInfo);
	return result;
}

} // anonymous namespace

namespace qtac {

SerialPortInfos SerialPortInfo::availablePorts()
{
	SerialPortInfos result;

	sp_port** portList = nullptr;
	if (sp_list_ports(&portList) != SP_OK || portList == nullptr)
		return result;

	for (int i = 0; portList[i] != nullptr; ++i)
	{
		SerialPortInfo info;

		const char* name = sp_get_port_name(portList[i]);
		const char* desc = sp_get_port_description(portList[i]);
		if (name) info.setPortName(qtac::String(name));
		if (desc) info.setDescription(qtac::String(desc));

		// Always use the SetupDI registry API for VID/PID and serial number.
		// This matches Qt's QSerialPortInfo behavior for all USB serial devices
		// regardless of driver (FTDI, usbser.sys, etc.).
		if (name)
		{
			uint16_t vid = 0, pid = 0, rev = 0;
			if (readVidPidFromRegistry(name, vid, pid, rev))
			{
				info.setVendorIdentifier(vid);
				info.setProductIdentifier(pid);
				info.setRevision(rev);
			}

			std::string serial = readSerialFromRegistry(name);
			if (!serial.empty())
				info.setSerialNumber(qtac::String(serial));
		}

		result.push_back(info);
	}

	sp_free_port_list(portList);
	return result;
}

} // namespace qtac
