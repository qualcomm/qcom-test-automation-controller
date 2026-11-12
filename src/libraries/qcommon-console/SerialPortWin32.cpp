// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "SerialPortWin32.h"

#include <chrono>
#include <thread>

#if defined(_WIN32) || defined(Q_OS_WIN)

static std::string normalizePortName(const std::string& name)
{
	if (name.empty())
		return {};
	if (name.rfind("\\\\.\\", 0) == 0)
		return name;
	return "\\\\.\\" + name;
}

SerialPortWin32::SerialPortWin32() :
	_handle(INVALID_HANDLE_VALUE)
{
}

SerialPortWin32::SerialPortWin32(const std::string& portName) :
	_portName(portName),
	_handle(INVALID_HANDLE_VALUE)
{
}

SerialPortWin32::~SerialPortWin32()
{
	close();
}

bool SerialPortWin32::open()
{
	return open(_portName);
}

bool SerialPortWin32::open(const std::string& portName)
{
	close();

	_portName = portName;
	if (_portName.empty())
	{
		_lastError = SerialPortError::DeviceNotFoundError;
		_lastErrorString = "Serial port name is empty";
		return false;
	}

	std::string winPath = normalizePortName(_portName);

	_handle = CreateFileA(
		winPath.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,              // Exclusive access
		nullptr,        // Default security
		OPEN_EXISTING,  // Must exist
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);

	if (_handle == INVALID_HANDLE_VALUE)
	{
		DWORD err = GetLastError();
		if (err == ERROR_FILE_NOT_FOUND)
			_lastError = SerialPortError::DeviceNotFoundError;
		else if (err == ERROR_ACCESS_DENIED)
			_lastError = SerialPortError::PermissionError;
		else
			_lastError = SerialPortError::OpenError;

		_lastErrorString = "Failed to open serial port " + _portName + " (Win32 error: " + std::to_string(err) + ")";
		return false;
	}

	if (!configureCommPort())
	{
		close();
		return false;
	}

	_lastError = SerialPortError::NoError;
	_lastErrorString.clear();
	return true;
}

bool SerialPortWin32::configureCommPort()
{
	if (_handle == INVALID_HANDLE_VALUE)
		return false;

	DCB dcb;
	SecureZeroMemory(&dcb, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);

	if (!GetCommState(_handle, &dcb))
	{
		_lastError = SerialPortError::UnsupportedOperationError;
		_lastErrorString = "GetCommState failed (Win32 error: " + std::to_string(GetLastError()) + ")";
		return false;
	}

	dcb.BaudRate = static_cast<DWORD>(_settings._baudRate);
	dcb.ByteSize = static_cast<BYTE>(_settings._dataBits);

	switch (_settings._parity)
	{
	case SerialParity::NoParity:
		dcb.Parity = NOPARITY;
		dcb.fParity = FALSE;
		break;
	case SerialParity::EvenParity:
		dcb.Parity = EVENPARITY;
		dcb.fParity = TRUE;
		break;
	case SerialParity::OddParity:
		dcb.Parity = ODDPARITY;
		dcb.fParity = TRUE;
		break;
	case SerialParity::MarkParity:
		dcb.Parity = MARKPARITY;
		dcb.fParity = TRUE;
		break;
	case SerialParity::SpaceParity:
		dcb.Parity = SPACEPARITY;
		dcb.fParity = TRUE;
		break;
	default:
		dcb.Parity = NOPARITY;
		dcb.fParity = FALSE;
		break;
	}

	switch (_settings._stopBits)
	{
	case SerialStopBits::OneStop:
		dcb.StopBits = ONESTOPBIT;
		break;
	case SerialStopBits::OneAndHalfStop:
		dcb.StopBits = ONE5STOPBITS;
		break;
	case SerialStopBits::TwoStop:
		dcb.StopBits = TWOSTOPBITS;
		break;
	default:
		dcb.StopBits = ONESTOPBIT;
		break;
	}

	switch (_settings._flowControl)
	{
	case SerialFlowControl::NoFlowControl:
		dcb.fOutxCtsFlow = FALSE;
		dcb.fRtsControl = RTS_CONTROL_ENABLE;
		dcb.fOutX = FALSE;
		dcb.fInX = FALSE;
		break;
	case SerialFlowControl::HardwareControl:
		dcb.fOutxCtsFlow = TRUE;
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
		dcb.fOutX = FALSE;
		dcb.fInX = FALSE;
		break;
	case SerialFlowControl::SoftwareControl:
		dcb.fOutxCtsFlow = FALSE;
		dcb.fRtsControl = RTS_CONTROL_ENABLE;
		dcb.fOutX = TRUE;
		dcb.fInX = TRUE;
		break;
	default:
		break;
	}

	dcb.fBinary = TRUE;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;

	if (!SetCommState(_handle, &dcb))
	{
		_lastError = SerialPortError::UnsupportedOperationError;
		_lastErrorString = "SetCommState failed (Win32 error: " + std::to_string(GetLastError()) + ")";
		return false;
	}

	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = MAXDWORD;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 5000;

	if (!SetCommTimeouts(_handle, &timeouts))
	{
		_lastError = SerialPortError::UnsupportedOperationError;
		_lastErrorString = "SetCommTimeouts failed (Win32 error: " + std::to_string(GetLastError()) + ")";
		return false;
	}

	return true;
}

void SerialPortWin32::close()
{
	if (_handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(_handle);
		_handle = INVALID_HANDLE_VALUE;
	}
}

bool SerialPortWin32::isOpen() const
{
	return _handle != INVALID_HANDLE_VALUE;
}

std::string SerialPortWin32::portName() const
{
	return _portName;
}

void SerialPortWin32::setPortName(const std::string& name)
{
	_portName = name;
}

SerialPortSettings SerialPortWin32::getSerialPortSettings() const
{
	return _settings;
}

bool SerialPortWin32::setSerialPortSettings(const SerialPortSettings& settings)
{
	_settings = settings;
	if (isOpen())
		return configureCommPort();
	return true;
}

bool SerialPortWin32::setBaudRate(int32_t baudRate)
{
	_settings._baudRate = baudRate;
	if (isOpen())
		return configureCommPort();
	return true;
}

int32_t SerialPortWin32::baudRate() const
{
	return _settings._baudRate;
}

bool SerialPortWin32::setDataBits(SerialDataBits dataBits)
{
	_settings._dataBits = dataBits;
	if (isOpen())
		return configureCommPort();
	return true;
}

SerialDataBits SerialPortWin32::dataBits() const
{
	return _settings._dataBits;
}

bool SerialPortWin32::setParity(SerialParity parity)
{
	_settings._parity = parity;
	if (isOpen())
		return configureCommPort();
	return true;
}

SerialParity SerialPortWin32::parity() const
{
	return _settings._parity;
}

bool SerialPortWin32::setStopBits(SerialStopBits stopBits)
{
	_settings._stopBits = stopBits;
	if (isOpen())
		return configureCommPort();
	return true;
}

SerialStopBits SerialPortWin32::stopBits() const
{
	return _settings._stopBits;
}

bool SerialPortWin32::setFlowControl(SerialFlowControl flowControl)
{
	_settings._flowControl = flowControl;
	if (isOpen())
		return configureCommPort();
	return true;
}

SerialFlowControl SerialPortWin32::flowControl() const
{
	return _settings._flowControl;
}

int64_t SerialPortWin32::write(const char* data, size_t size)
{
	if (!isOpen() || data == nullptr || size == 0)
		return 0;

	DWORD bytesWritten = 0;
	if (!WriteFile(_handle, data, static_cast<DWORD>(size), &bytesWritten, nullptr))
	{
		_lastError = SerialPortError::WriteError;
		_lastErrorString = "WriteFile failed (Win32 error: " + std::to_string(GetLastError()) + ")";
		return -1;
	}

	return static_cast<int64_t>(bytesWritten);
}

int64_t SerialPortWin32::write(const std::string& data)
{
	return write(data.data(), data.size());
}

int64_t SerialPortWin32::read(char* buffer, size_t maxSize)
{
	if (!isOpen() || buffer == nullptr || maxSize == 0)
		return 0;

	DWORD bytesRead = 0;
	if (!ReadFile(_handle, buffer, static_cast<DWORD>(maxSize), &bytesRead, nullptr))
	{
		_lastError = SerialPortError::ReadError;
		_lastErrorString = "ReadFile failed (Win32 error: " + std::to_string(GetLastError()) + ")";
		return -1;
	}

	return static_cast<int64_t>(bytesRead);
}

std::string SerialPortWin32::read(size_t maxSize)
{
	if (maxSize == 0)
		return {};

	std::string buffer(maxSize, '\0');
	int64_t bytesRead = read(&buffer[0], maxSize);
	if (bytesRead > 0)
	{
		buffer.resize(static_cast<size_t>(bytesRead));
		return buffer;
	}
	return {};
}

std::string SerialPortWin32::readAll()
{
	int64_t avail = bytesAvailable();
	if (avail <= 0)
		avail = 4096;
	return read(static_cast<size_t>(avail));
}

int64_t SerialPortWin32::bytesAvailable() const
{
	if (!isOpen())
		return 0;

	COMSTAT stat;
	DWORD errors = 0;
	if (ClearCommError(_handle, &errors, &stat))
	{
		return static_cast<int64_t>(stat.cbInQue);
	}

	return 0;
}

bool SerialPortWin32::flush()
{
	if (!isOpen())
		return false;

	return FlushFileBuffers(_handle) != FALSE;
}

void SerialPortWin32::clearBuffers()
{
	if (isOpen())
	{
		PurgeComm(_handle, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
	}
}

bool SerialPortWin32::waitForReadyRead(int timeoutMs)
{
	if (!isOpen())
		return false;

	if (bytesAvailable() > 0)
		return true;

	auto start = std::chrono::steady_clock::now();
	while (true)
	{
		if (bytesAvailable() > 0)
			return true;

		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now() - start).count();

		if (elapsed >= timeoutMs)
			break;

		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}

	return bytesAvailable() > 0;
}

SerialPortError SerialPortWin32::error() const
{
	return _lastError;
}

std::string SerialPortWin32::errorString() const
{
	return _lastErrorString;
}

#else // Non-Windows fallback stubs

SerialPortWin32::SerialPortWin32() {}
SerialPortWin32::SerialPortWin32(const std::string& portName) : _portName(portName) {}
SerialPortWin32::~SerialPortWin32() {}
bool SerialPortWin32::open() { return false; }
bool SerialPortWin32::open(const std::string&) { return false; }
void SerialPortWin32::close() {}
bool SerialPortWin32::isOpen() const { return false; }
std::string SerialPortWin32::portName() const { return _portName; }
void SerialPortWin32::setPortName(const std::string& name) { _portName = name; }
SerialPortSettings SerialPortWin32::getSerialPortSettings() const { return _settings; }
bool SerialPortWin32::setSerialPortSettings(const SerialPortSettings& s) { _settings = s; return true; }
bool SerialPortWin32::setBaudRate(int32_t b) { _settings._baudRate = b; return true; }
int32_t SerialPortWin32::baudRate() const { return _settings._baudRate; }
bool SerialPortWin32::setDataBits(SerialDataBits d) { _settings._dataBits = d; return true; }
SerialDataBits SerialPortWin32::dataBits() const { return _settings._dataBits; }
bool SerialPortWin32::setParity(SerialParity p) { _settings._parity = p; return true; }
SerialParity SerialPortWin32::parity() const { return _settings._parity; }
bool SerialPortWin32::setStopBits(SerialStopBits s) { _settings._stopBits = s; return true; }
SerialStopBits SerialPortWin32::stopBits() const { return _settings._stopBits; }
bool SerialPortWin32::setFlowControl(SerialFlowControl f) { _settings._flowControl = f; return true; }
SerialFlowControl SerialPortWin32::flowControl() const { return _settings._flowControl; }
int64_t SerialPortWin32::write(const char*, size_t) { return -1; }
int64_t SerialPortWin32::write(const std::string&) { return -1; }
int64_t SerialPortWin32::read(char*, size_t) { return -1; }
std::string SerialPortWin32::read(size_t) { return {}; }
std::string SerialPortWin32::readAll() { return {}; }
int64_t SerialPortWin32::bytesAvailable() const { return 0; }
bool SerialPortWin32::flush() { return false; }
void SerialPortWin32::clearBuffers() {}
bool SerialPortWin32::waitForReadyRead(int) { return false; }
SerialPortError SerialPortWin32::error() const { return SerialPortError::UnsupportedOperationError; }
std::string SerialPortWin32::errorString() const { return "Win32 serial port is not supported on this platform"; }
bool SerialPortWin32::configureCommPort() { return false; }

#endif
