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
// POSIX (Linux/macOS) implementation of SerialPortInfo::availablePorts().
// libserialport reads VID/PID and serial number from sysfs on Linux,
// which works correctly without any additional registry fallback.

#include <qtac/SerialPortInfo.h>

#include <libserialport.h>

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

		if (sp_get_port_transport(portList[i]) == SP_TRANSPORT_USB)
		{
			const char* serial = sp_get_port_usb_serial(portList[i]);
			if (serial) info.setSerialNumber(qtac::String(serial));

			int vid = 0, pid = 0;
			sp_get_port_usb_vid_pid(portList[i], &vid, &pid);
			info.setVendorIdentifier(static_cast<uint16_t>(vid));
			info.setProductIdentifier(static_cast<uint16_t>(pid));
		}

		result.push_back(info);
	}

	sp_free_port_list(portList);
	return result;
}

} // namespace qtac
