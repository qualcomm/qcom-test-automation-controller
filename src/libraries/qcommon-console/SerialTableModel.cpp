// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "SerialTableModel.h"

// QCommon
#include "AppCore.h"
#include "SerialPortInfo.h"

// Qt
#include <QSerialPortInfo>

// C++
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <string>
#include <utility>
#include <vector>

typedef std::pair<uint16_t, uint16_t> FilterKey;

template<class T> std::string DecAndHexPrint(T value, int width)
{
	char buf[64];
	std::snprintf(buf, sizeof(buf), "%10u (0x%0*X)", static_cast<unsigned int>(value), width, static_cast<unsigned int>(value));
	return std::string(buf);
}

struct STMImplementation
{
	std::vector<FilterKey>		_filterKeys;
	SerialPortInfos				_serialInfos;
};

// column indexes
const int kPortName(0);
const int kDescription(1);
const int kSerialNumber(2);
const int kVendor(3);
const int kProduct(4);

SerialTableModel::SerialTableModel
(
	QObject* parent // = nullptr
) :
	QAbstractTableModel(parent),
	_impl(nullptr)
{
	_impl = new STMImplementation;

	_columnHeaders = {"Port Name", "Description", "Serial Number", "Vendor", "Product"};
}

SerialTableModel::~SerialTableModel()
{
	if (_impl != nullptr)
	{
		delete _impl;
		_impl = nullptr;
	}
}

void SerialTableModel::addFilterKey
(
	uint16_t vendorID,
	uint16_t productID
)
{
	FilterKey filterKey = std::make_pair(vendorID, productID);

	if (std::find(_impl->_filterKeys.begin(), _impl->_filterKeys.end(), filterKey) == _impl->_filterKeys.end())
	{
		_impl->_filterKeys.push_back(filterKey);
	}
}

bool SerialTableModel::filterProc
(
	uint16_t vendorID,
	uint16_t productID
)
{
	(void)vendorID;
	(void)productID;

	return true;
}

QVariant SerialTableModel::portData(int deviceIndex, int dataIndex)
{
	return data(createIndex(deviceIndex, dataIndex));
}

std::string SerialTableModel::portName(int deviceIndex)
{
	std::string result;

	if (deviceIndex < rowCount())
	{
		result = data(createIndex(deviceIndex, kPortName)).toString().toStdString();
	}

	return result;
}

bool SortFunc(const SerialPortInfo& i, const SerialPortInfo& j)
{
	return (i.portName() < j.portName());
}

void SerialTableModel::refresh()
{
	AppCore* appCore = AppCore::getAppCore();
	bool logging = appCore != nullptr;

	bool filtered = !_impl->_filterKeys.empty();

	SerialPortInfos availablePorts = SerialPortInfo::availablePorts();
	SerialPortInfos serialInfos;

	if (logging)
	{
		for (const auto& serialPortInfo: availablePorts)
		{
			std::string message = "Found serial port with vendor id: " + std::to_string(serialPortInfo.vendorIdentifier()) + " and product id: " + std::to_string(serialPortInfo.productIdentifier()) + "\n";
			appCore->writeToAppLog(message);
		}
	}

	if (filtered)
	{
		for (const auto& serialPortInfo: availablePorts)
		{	
			FilterKey filterKey = std::make_pair(serialPortInfo.vendorIdentifier(), serialPortInfo.productIdentifier());
			if (std::find(_impl->_filterKeys.begin(), _impl->_filterKeys.end(), filterKey) != _impl->_filterKeys.end())
			{
				serialInfos.push_back(serialPortInfo);
			}
		}
	}
	else
	{
		serialInfos = availablePorts;
	}

	if (equal(serialInfos, _impl->_serialInfos))
	{
		return;
	}

	_impl->_serialInfos.clear();

	if (!serialInfos.empty())
	{
		std::sort(serialInfos.begin(), serialInfos.end(), SortFunc);

		for (const auto& serialPortInfo: serialInfos)
		{
			bool add(true);

			if (filtered)
			{
				FilterKey filterKey = std::make_pair(serialPortInfo.vendorIdentifier(), serialPortInfo.productIdentifier());
				add = std::find(_impl->_filterKeys.begin(), _impl->_filterKeys.end(), filterKey) != _impl->_filterKeys.end();
			}

			if (add)
			{
				if (filterProc(serialPortInfo.vendorIdentifier(), serialPortInfo.productIdentifier()))
					_impl->_serialInfos.push_back(serialPortInfo);
			}
		}
	}

	emit modelUpdated();
}

int SerialTableModel::rowCount
(
	const QModelIndex& parent // = QModelIndex()
) const
{
	(void)parent;

	return static_cast<int>(_impl->_serialInfos.size());
}

int SerialTableModel::columnCount
(
	const QModelIndex& parent // = QModelIndex()
) const
{
	(void)parent;

	return static_cast<int>(_columnHeaders.size());
}

QVariant SerialTableModel::headerData
(
	int section,
	Qt::Orientation orientation,
	int role // = Qt::DisplayRole
) const
{
	(void)orientation;

	switch (role)
	{
	case Qt::DisplayRole:
		if (section >= 0 && section < static_cast<int>(_columnHeaders.size()))
			return QString::fromStdString(_columnHeaders.at(section));
		break;

	default:
		break;
	}

	return QVariant();
}

QVariant SerialTableModel::data
(
	const QModelIndex& index,
	int role // = Qt::DisplayRole
) const
{
	QSerialPortInfo serialInfo = _impl->_serialInfos.at(index.row());

	switch (role)
	{
	case Qt::DisplayRole:
		switch (index.column())
		{
		case kPortName: return serialInfo.portName();
		case kDescription: return serialInfo.description();
		case kSerialNumber: return serialInfo.serialNumber();
		case kVendor:
		{
			return QString::fromStdString(DecAndHexPrint(serialInfo.vendorIdentifier(), 4));
		}
		case kProduct:
		{
			return QString::fromStdString(DecAndHexPrint(serialInfo.productIdentifier(), 4));
		}

		default:
			return QString("-");
		}

	default:
		break;
	}

	return QVariant();
}

QSerialPortInfo SerialTableModel::getSelectedSerialPort()
{
	return _selectedSerialPort;
}

QSerialPortInfo SerialTableModel::getSerialPortFromName
(
	const std::string &portName
)
{
	QSerialPortInfo result;

	refresh();

	for (const auto& serialInfo: _impl->_serialInfos)
	{
		std::string infoPort = serialInfo.portName().toStdString();
		if (infoPort.size() == portName.size())
		{
			bool match = true;
			for (size_t i = 0; i < infoPort.size(); ++i)
			{
				if (std::tolower(static_cast<unsigned char>(infoPort[i])) != std::tolower(static_cast<unsigned char>(portName[i])))
				{
					match = false;
					break;
				}
			}
			if (match)
			{
				result = serialInfo;
				break;
			}
		}
	}

	return result;
}

QSerialPortInfo SerialTableModel::getSerialPortFromSerialNumber
(
	const std::string &serialNumber
)
{
	QSerialPortInfo result;

	refresh();

	for (const auto& serialInfo: _impl->_serialInfos)
	{
		std::string infoSerial = serialInfo.serialNumber().toStdString();
		if (infoSerial.size() == serialNumber.size())
		{
			bool match = true;
			for (size_t i = 0; i < infoSerial.size(); ++i)
			{
				if (std::tolower(static_cast<unsigned char>(infoSerial[i])) != std::tolower(static_cast<unsigned char>(serialNumber[i])))
				{
					match = false;
					break;
				}
			}
			if (match)
			{
				result = serialInfo;
				break;
			}
		}
	}

	return result;
}

void SerialTableModel::setSelectedSerialPort(int rowIndex)
{
	if (rowIndex == -1)
		_selectedSerialPort = QSerialPortInfo();
	else
		_selectedSerialPort = _impl->_serialInfos.at(rowIndex);
}

std::string SerialTableModel::rowData
(
	int row,
	int column
)
{
	std::string result;

	if (row >= 0 && row < static_cast<int>(_impl->_serialInfos.size()))
	{
		if (column >= 0 && column < static_cast<int>(_columnHeaders.size()))
		{
			QModelIndex modelIndex = index(row, column);
			result = data(modelIndex).toString().toStdString();
		}
	}

	return result;
}
