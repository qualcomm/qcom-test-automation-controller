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
// Adapted from tac-core/SerialPort.h — uses qtac:: types, removes QCommonConsoleGlobal.h

#ifndef QTAC_SERIALPORT_H
#define QTAC_SERIALPORT_H

#include <qtac/ByteArray.h>
#include <qtac/String.h>
#include <qtac/SerialPortSettings.h>

#include <cstdint>
#include <functional>

// libserialport forward declaration — avoids pulling the full header into every TU
struct sp_port;

namespace qtac {

class SerialPortInfo;

class SerialPort
{
public:
	explicit SerialPort(const qtac::String& portName);
	explicit SerialPort(const SerialPortInfo& portInfo);
	~SerialPort();

	SerialPort(const SerialPort&) = delete;
	SerialPort& operator=(const SerialPort&) = delete;

	SerialPortSettings settings() const { return _settings; }
	void setSettings(const SerialPortSettings& s) { _settings = s; }

	bool open();
	void close();
	bool isOpen() const { return _port != nullptr; }

	// Blocking read — waits up to timeout_ms for at least 1 byte, then drains
	qtac::ByteArray readAll(uint32_t timeout_ms = 10);

	// Blocking write — waits up to timeout_ms for the write to complete
	int write(const qtac::ByteArray& data, uint32_t timeout_ms = 1000);

	// Returns true if data arrives within timeout_ms (does not consume bytes)
	bool waitForReadyRead(uint32_t timeout_ms = 10);

	// Flush input and output buffers
	bool clear();

	qtac::String portName() const { return _portName; }
	qtac::String errorString() const { return _lastError; }

	// Callbacks (replace Qt signals; set before calling open())
	std::function<void()>    onPortOpened;
	std::function<void()>    onPortClosed;
	std::function<void()>    onReadyRead;
	std::function<void(int)> onErrorOccurred;

private:
	sp_port*           _port{nullptr};
	qtac::String       _portName;
	SerialPortSettings _settings;
	qtac::String       _lastError;

	void applySettings();
};

} // namespace qtac

#endif // QTAC_SERIALPORT_H
