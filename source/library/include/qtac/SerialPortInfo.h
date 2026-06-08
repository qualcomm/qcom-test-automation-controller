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
// Adapted from tac-core/SerialPortInfo.h — uses qtac:: types

#ifndef QTAC_SERIALPORTINFO_H
#define QTAC_SERIALPORTINFO_H

#include <qtac/String.h>

#include <cstdint>
#include <vector>

namespace qtac {

class SerialPortInfo;
using SerialPortInfos = std::vector<SerialPortInfo>;

class SerialPortInfo
{
public:
	SerialPortInfo() = default;
	explicit SerialPortInfo(const qtac::String& portName) : _portName(portName) {}
	SerialPortInfo(const SerialPortInfo&) = default;
	SerialPortInfo& operator=(const SerialPortInfo&) = default;

	bool isNull() const { return _portName.isEmpty(); }

	qtac::String portName()     const { return _portName; }
	qtac::String serialNumber() const { return _serialNumber; }
	qtac::String description()  const { return _description; }
	uint16_t vendorIdentifier()  const { return _vendorId; }
	uint16_t productIdentifier() const { return _productId; }

	void setPortName(const qtac::String& s)     { _portName = s; }
	void setSerialNumber(const qtac::String& s) { _serialNumber = s; }
	void setDescription(const qtac::String& s)  { _description = s; }
	void setVendorIdentifier(uint16_t vid)      { _vendorId = vid; }
	void setProductIdentifier(uint16_t pid)     { _productId = pid; }

	bool matchesVidPid(uint16_t vid, uint16_t pid) const
	{
		return _vendorId == vid && _productId == pid;
	}

	// Enumerate available serial ports via libserialport.
	// Platform-specific implementation in SerialPortInfo_win32.cpp / SerialPortInfo_posix.cpp
	static SerialPortInfos availablePorts();

	bool operator==(const SerialPortInfo& o) const { return _portName == o._portName; }
	bool operator< (const SerialPortInfo& o) const { return _portName <  o._portName; }
	bool operator<=(const SerialPortInfo& o) const { return _portName <= o._portName; }

private:
	qtac::String _portName;
	qtac::String _serialNumber;
	qtac::String _description;
	uint16_t     _vendorId{0};
	uint16_t     _productId{0};
};

bool equal(const SerialPortInfos& si1, const SerialPortInfos& si2);

} // namespace qtac

#endif // QTAC_SERIALPORTINFO_H
