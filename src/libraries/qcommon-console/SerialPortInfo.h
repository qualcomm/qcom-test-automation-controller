#ifndef SERIALPORTINFO_H
#define SERIALPORTINFO_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include <cstdint>
#include <string>
#include <vector>

class SerialPortInfo;
typedef std::vector<SerialPortInfo> SerialPortInfos;

/**
 * @brief Pure Standard C++ cross-platform serial port metadata and enumeration.
 * Completely free of Qt libraries.
 */
class QCOMMONCONSOLE_EXPORT SerialPortInfo
{
public:
	SerialPortInfo();
	explicit SerialPortInfo(const std::string& name);
	SerialPortInfo(const std::string& name,
	               const std::string& description,
	               const std::string& manufacturer,
	               const std::string& serialNumber,
	               uint16_t vendorId = 0,
	               uint16_t productId = 0);
	SerialPortInfo(const SerialPortInfo& copyMe) = default;
	SerialPortInfo& operator=(const SerialPortInfo& copyMe) = default;
	~SerialPortInfo() = default;

	bool isNull() const { return _isNull; }

	const std::string& portName() const { return _portName; }
	const std::string& systemLocation() const { return _systemLocation; }
	const std::string& description() const { return _description; }
	const std::string& manufacturer() const { return _manufacturer; }
	const std::string& serialNumber() const { return _serialNumber; }

	uint16_t vendorIdentifier() const { return _vendorIdentifier; }
	uint16_t productIdentifier() const { return _productIdentifier; }
	bool hasVendorIdentifier() const { return _hasVendorIdentifier; }
	bool hasProductIdentifier() const { return _hasProductIdentifier; }

	uint32_t hash() const;

	static SerialPortInfos availablePorts();
	static SerialPortInfo getPortByName(const std::string& name);
	static SerialPortInfo getPortBySerialNumber(const std::string& serialNumber);

	bool operator==(const SerialPortInfo& other) const;
	bool operator!=(const SerialPortInfo& other) const;
	bool operator<(const SerialPortInfo& other) const;
	bool operator<=(const SerialPortInfo& other) const;

private:
	std::string _portName;
	std::string _systemLocation;
	std::string _description;
	std::string _manufacturer;
	std::string _serialNumber;
	uint16_t    _vendorIdentifier{0};
	uint16_t    _productIdentifier{0};
	bool        _hasVendorIdentifier{false};
	bool        _hasProductIdentifier{false};
	bool        _isNull{true};
	mutable uint32_t _hash{0};
};

bool equal(const SerialPortInfos& si1, const SerialPortInfos& si2);

#endif // SERIALPORTINFO_H
