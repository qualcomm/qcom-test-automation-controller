// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "SerialPortLinux.h"

#if !defined(_WIN32) && !defined(Q_OS_WIN)

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <cerrno>
#include <cstring>

static std::string normalizePortName(const std::string& name)
{
	if (name.empty())
		return {};
	if (name[0] == '/')
		return name;
	return "/dev/" + name;
}

static speed_t toBaudSpeed(int32_t baudRate)
{
	switch (baudRate)
	{
	case 1200:   return B1200;
	case 2400:   return B2400;
	case 4800:   return B4800;
	case 9600:   return B9600;
	case 19200:  return B19200;
	case 38400:  return B38400;
	case 57600:  return B57600;
	case 115200: return B115200;
	case 230400: return B230400;
#ifdef B460800
	case 460800: return B460800;
#endif
#ifdef B921600
	case 921600: return B921600;
#endif
	default:     return B115200;
	}
}

SerialPortLinux::SerialPortLinux() :
	_fd(-1)
{
}

SerialPortLinux::SerialPortLinux(const std::string& portName) :
	_portName(portName),
	_fd(-1)
{
}

SerialPortLinux::~SerialPortLinux()
{
	close();
}

bool SerialPortLinux::open()
{
	return open(_portName);
}

bool SerialPortLinux::open(const std::string& portName)
{
	close();

	_portName = portName;
	if (_portName.empty())
	{
		_lastError = SerialPortError::DeviceNotFoundError;
		_lastErrorString = "Serial port name is empty";
		return false;
	}

	std::string path = normalizePortName(_portName);

	_fd = ::open(path.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (_fd < 0)
	{
		int err = errno;
		if (err == ENOENT)
			_lastError = SerialPortError::DeviceNotFoundError;
		else if (err == EACCES)
			_lastError = SerialPortError::PermissionError;
		else
			_lastError = SerialPortError::OpenError;

		_lastErrorString = "Failed to open " + _portName + " (errno: " + std::to_string(err) + " - " + strerror(err) + ")";
		return false;
	}

	if (!configureTermios())
	{
		close();
		return false;
	}

	_lastError = SerialPortError::NoError;
	_lastErrorString.clear();
	return true;
}

bool SerialPortLinux::configureTermios()
{
	if (_fd < 0)
		return false;

	struct termios tty;
	if (::tcgetattr(_fd, &tty) != 0)
	{
		_lastError = SerialPortError::UnsupportedOperationError;
		_lastErrorString = "tcgetattr failed";
		return false;
	}

	speed_t speed = toBaudSpeed(_settings._baudRate);
	::cfsetispeed(&tty, speed);
	::cfsetospeed(&tty, speed);

	// Data bits
	tty.c_cflag &= ~CSIZE;
	switch (_settings._dataBits)
	{
	case SerialDataBits::Data5: tty.c_cflag |= CS5; break;
	case SerialDataBits::Data6: tty.c_cflag |= CS6; break;
	case SerialDataBits::Data7: tty.c_cflag |= CS7; break;
	case SerialDataBits::Data8: tty.c_cflag |= CS8; break;
	default:                    tty.c_cflag |= CS8; break;
	}

	// Parity
	switch (_settings._parity)
	{
	case SerialParity::NoParity:
		tty.c_cflag &= ~(PARENB | PARODD);
		tty.c_iflag &= ~INPCK;
		break;
	case SerialParity::EvenParity:
		tty.c_cflag |= PARENB;
		tty.c_cflag &= ~PARODD;
		tty.c_iflag |= INPCK;
		break;
	case SerialParity::OddParity:
		tty.c_cflag |= (PARENB | PARODD);
		tty.c_iflag |= INPCK;
		break;
	default:
		tty.c_cflag &= ~(PARENB | PARODD);
		break;
	}

	// Stop bits
	switch (_settings._stopBits)
	{
	case SerialStopBits::OneStop:
		tty.c_cflag &= ~CSTOPB;
		break;
	case SerialStopBits::TwoStop:
		tty.c_cflag |= CSTOPB;
		break;
	default:
		tty.c_cflag &= ~CSTOPB;
		break;
	}

	// Flow control
	switch (_settings._flowControl)
	{
	case SerialFlowControl::NoFlowControl:
		tty.c_cflag &= ~CRTSCTS;
		tty.c_iflag &= ~(IXON | IXOFF | IXANY);
		break;
	case SerialFlowControl::HardwareControl:
		tty.c_cflag |= CRTSCTS;
		tty.c_iflag &= ~(IXON | IXOFF | IXANY);
		break;
	case SerialFlowControl::SoftwareControl:
		tty.c_cflag &= ~CRTSCTS;
		tty.c_iflag |= (IXON | IXOFF | IXANY);
		break;
	default:
		break;
	}

	// Raw input/output mode
	tty.c_cflag |= (CLOCAL | CREAD);
	tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHONL | ISIG | IEXTEN);
	tty.c_oflag &= ~OPOST;
	tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

	tty.c_cc[VMIN]  = 0;
	tty.c_cc[VTIME] = 0;

	if (::tcsetattr(_fd, TCSANOW, &tty) != 0)
	{
		_lastError = SerialPortError::UnsupportedOperationError;
		_lastErrorString = "tcsetattr failed";
		return false;
	}

	return true;
}

void SerialPortLinux::close()
{
	if (_fd >= 0)
	{
		::close(_fd);
		_fd = -1;
	}
}

bool SerialPortLinux::isOpen() const
{
	return _fd >= 0;
}

std::string SerialPortLinux::portName() const
{
	return _portName;
}

void SerialPortLinux::setPortName(const std::string& name)
{
	_portName = name;
}

SerialPortSettings SerialPortLinux::getSerialPortSettings() const
{
	return _settings;
}

bool SerialPortLinux::setSerialPortSettings(const SerialPortSettings& settings)
{
	_settings = settings;
	if (isOpen())
		return configureTermios();
	return true;
}

bool SerialPortLinux::setBaudRate(int32_t baudRate)
{
	_settings._baudRate = baudRate;
	if (isOpen())
		return configureTermios();
	return true;
}

int32_t SerialPortLinux::baudRate() const
{
	return _settings._baudRate;
}

bool SerialPortLinux::setDataBits(SerialDataBits dataBits)
{
	_settings._dataBits = dataBits;
	if (isOpen())
		return configureTermios();
	return true;
}

SerialDataBits SerialPortLinux::dataBits() const
{
	return _settings._dataBits;
}

bool SerialPortLinux::setParity(SerialParity parity)
{
	_settings._parity = parity;
	if (isOpen())
		return configureTermios();
	return true;
}

SerialParity SerialPortLinux::parity() const
{
	return _settings._parity;
}

bool SerialPortLinux::setStopBits(SerialStopBits stopBits)
{
	_settings._stopBits = stopBits;
	if (isOpen())
		return configureTermios();
	return true;
}

SerialStopBits SerialPortLinux::stopBits() const
{
	return _settings._stopBits;
}

bool SerialPortLinux::setFlowControl(SerialFlowControl flowControl)
{
	_settings._flowControl = flowControl;
	if (isOpen())
		return configureTermios();
	return true;
}

SerialFlowControl SerialPortLinux::flowControl() const
{
	return _settings._flowControl;
}

int64_t SerialPortLinux::write(const char* data, size_t size)
{
	if (!isOpen() || data == nullptr || size == 0)
		return 0;

	ssize_t bytesWritten = ::write(_fd, data, size);
	if (bytesWritten < 0)
	{
		_lastError = SerialPortError::WriteError;
		_lastErrorString = "write failed (errno: " + std::to_string(errno) + ")";
		return -1;
	}

	return bytesWritten;
}

int64_t SerialPortLinux::write(const std::string& data)
{
	return write(data.data(), data.size());
}

int64_t SerialPortLinux::read(char* buffer, size_t maxSize)
{
	if (!isOpen() || buffer == nullptr || maxSize == 0)
		return 0;

	ssize_t bytesRead = ::read(_fd, buffer, maxSize);
	if (bytesRead < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return 0;

		_lastError = SerialPortError::ReadError;
		_lastErrorString = "read failed (errno: " + std::to_string(errno) + ")";
		return -1;
	}

	return bytesRead;
}

std::string SerialPortLinux::read(size_t maxSize)
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

std::string SerialPortLinux::readAll()
{
	int64_t avail = bytesAvailable();
	if (avail <= 0)
		avail = 4096;
	return read(static_cast<size_t>(avail));
}

int64_t SerialPortLinux::bytesAvailable() const
{
	if (!isOpen())
		return 0;

	int bytes = 0;
	if (::ioctl(_fd, FIONREAD, &bytes) == 0)
	{
		return bytes;
	}
	return 0;
}

bool SerialPortLinux::flush()
{
	if (!isOpen())
		return false;

	return ::tcdrain(_fd) == 0;
}

void SerialPortLinux::clearBuffers()
{
	if (isOpen())
	{
		::tcflush(_fd, TCIOFLUSH);
	}
}

bool SerialPortLinux::waitForReadyRead(int timeoutMs)
{
	if (!isOpen())
		return false;

	if (bytesAvailable() > 0)
		return true;

	struct pollfd pfd;
	pfd.fd = _fd;
	pfd.events = POLLIN;
	pfd.revents = 0;

	int ret = ::poll(&pfd, 1, timeoutMs);
	return (ret > 0 && (pfd.revents & POLLIN));
}

SerialPortError SerialPortLinux::error() const
{
	return _lastError;
}

std::string SerialPortLinux::errorString() const
{
	return _lastErrorString;
}

#else // Windows fallback stubs

SerialPortLinux::SerialPortLinux() {}
SerialPortLinux::SerialPortLinux(const std::string& portName) : _portName(portName) {}
SerialPortLinux::~SerialPortLinux() {}
bool SerialPortLinux::open() { return false; }
bool SerialPortLinux::open(const std::string&) { return false; }
void SerialPortLinux::close() {}
bool SerialPortLinux::isOpen() const { return false; }
std::string SerialPortLinux::portName() const { return _portName; }
void SerialPortLinux::setPortName(const std::string& name) { _portName = name; }
SerialPortSettings SerialPortLinux::getSerialPortSettings() const { return _settings; }
bool SerialPortLinux::setSerialPortSettings(const SerialPortSettings& s) { _settings = s; return true; }
bool SerialPortLinux::setBaudRate(int32_t b) { _settings._baudRate = b; return true; }
int32_t SerialPortLinux::baudRate() const { return _settings._baudRate; }
bool SerialPortLinux::setDataBits(SerialDataBits d) { _settings._dataBits = d; return true; }
SerialDataBits SerialPortLinux::dataBits() const { return _settings._dataBits; }
bool SerialPortLinux::setParity(SerialParity p) { _settings._parity = p; return true; }
SerialParity SerialPortLinux::parity() const { return _settings._parity; }
bool SerialPortLinux::setStopBits(SerialStopBits s) { _settings._stopBits = s; return true; }
SerialStopBits SerialPortLinux::stopBits() const { return _settings._stopBits; }
bool SerialPortLinux::setFlowControl(SerialFlowControl f) { _settings._flowControl = f; return true; }
SerialFlowControl SerialPortLinux::flowControl() const { return _settings._flowControl; }
int64_t SerialPortLinux::write(const char*, size_t) { return -1; }
int64_t SerialPortLinux::write(const std::string&) { return -1; }
int64_t SerialPortLinux::read(char*, size_t) { return -1; }
std::string SerialPortLinux::read(size_t) { return {}; }
std::string SerialPortLinux::readAll() { return {}; }
int64_t SerialPortLinux::bytesAvailable() const { return 0; }
bool SerialPortLinux::flush() { return false; }
void SerialPortLinux::clearBuffers() {}
bool SerialPortLinux::waitForReadyRead(int) { return false; }
SerialPortError SerialPortLinux::error() const { return SerialPortError::UnsupportedOperationError; }
std::string SerialPortLinux::errorString() const { return "Linux serial port is not supported on Windows"; }
bool SerialPortLinux::configureTermios() { return false; }

#endif
