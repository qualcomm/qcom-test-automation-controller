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
// Adapted from tac-core/SerialPort.cpp — uses qtac:: types

#include <qtac/SerialPort.h>
#include <qtac/SerialPortInfo.h>

#include <chrono>
#include <thread>

// Include libserialport only in the .cpp to avoid polluting headers
#include <libserialport.h>

namespace qtac {

SerialPort::SerialPort(const qtac::String& portName)
	: _portName(portName)
{
}

SerialPort::SerialPort(const SerialPortInfo& portInfo)
	: _portName(portInfo.portName())
{
}

SerialPort::~SerialPort()
{
	close();
}

bool SerialPort::open()
{
	sp_port* port = nullptr;
	if (sp_get_port_by_name(_portName.constData(), &port) != SP_OK)
	{
		_lastError = "Port not found: " + _portName;
		return false;
	}

	if (sp_open(port, SP_MODE_READ_WRITE) != SP_OK)
	{
		_lastError = sp_last_error_message();
		sp_free_port(port);
		return false;
	}

	_port = port;
	applySettings();

	if (onPortOpened) onPortOpened();
	return true;
}

void SerialPort::close()
{
	if (_port != nullptr)
	{
		sp_close(_port);
		sp_free_port(_port);
		_port = nullptr;
		if (onPortClosed) onPortClosed();
	}
}

void SerialPort::applySettings()
{
	if (_port == nullptr) return;

	sp_set_baudrate(_port, _settings.baudRate);
	sp_set_bits(_port, _settings.dataBits);
	sp_set_parity(_port, static_cast<sp_parity>(_settings.parity));
	sp_set_stopbits(_port, _settings.stopBits);
	sp_set_flowcontrol(_port, static_cast<sp_flowcontrol>(_settings.flowControl));

	// Assert DTR and RTS — QSerialPort does this by default on open().
	// Many USB-serial devices (PSoC, PIC32CX) require DTR high to enable communication.
	sp_set_dtr(_port, SP_DTR_ON);
	sp_set_rts(_port, SP_RTS_ON);
}

qtac::ByteArray SerialPort::readAll(uint32_t timeout_ms)
{
	if (_port == nullptr) return {};

	char buf[4096];
	sp_return n = sp_blocking_read_next(_port, buf, sizeof(buf), timeout_ms);
	if (n <= 0) return {};

	qtac::ByteArray result(buf, static_cast<int>(n));

	// Drain any remaining bytes non-blocking
	sp_return extra;
	while ((extra = sp_nonblocking_read(_port, buf, sizeof(buf))) > 0)
		result.append(buf, static_cast<int>(extra));

	return result;
}

int SerialPort::write(const qtac::ByteArray& data, uint32_t timeout_ms)
{
	if (_port == nullptr) return -1;
	sp_return n = sp_blocking_write(_port, data.constData(),
	                                static_cast<size_t>(data.size()), timeout_ms);
	return static_cast<int>(n);
}

bool SerialPort::waitForReadyRead(uint32_t timeout_ms)
{
	if (_port == nullptr) return false;

	uint32_t elapsed = 0;
	const uint32_t pollInterval = 1; // ms

	while (elapsed < timeout_ms)
	{
		sp_return waiting = sp_input_waiting(_port);
		if (waiting > 0) return true;
		if (waiting < 0) return false; // error

		std::this_thread::sleep_for(std::chrono::milliseconds(pollInterval));
		elapsed += pollInterval;
	}

	return sp_input_waiting(_port) > 0;
}

bool SerialPort::clear()
{
	if (_port == nullptr) return false;
	return sp_flush(_port, SP_BUF_BOTH) == SP_OK;
}

} // namespace qtac
