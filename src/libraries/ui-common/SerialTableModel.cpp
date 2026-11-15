// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "SerialTableModel.h"

// QCommon
#include "AppCore.h"
#include "SerialPortInfo.h"
#include "StringUtilities.h"

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
	_impl(new STMImplementation)
{
	_columnHeaders.push_back("Port Name");
	_columnHeaders.push_back("Description");
	_columnHeaders.push_back("Serial Number");
	_columnHeaders.push_back("Vendor");
	_columnHeaders.push_back("Product");
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
	if (_impl != nullptr)
	{
		FilterKey filterKey = std::make_pair(vendorID, productID);
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
	return data(createIndex(deviceIndex,  dataIndex));
}

std::string SerialTableModel::portName(int deviceIndex)
{
	std::string result;

	if (_impl != nullptr)
	{
		if (deviceIndex < static_cast<int>(_impl->_serialInfos.size()))
		{
			result = _impl->_serialInfos.at(deviceIndex).portName();
		}
	}

	return result;
}

bool SortFunc(const SerialPortInfo& i, const SerialPortInfo& j)
{
	return (i.portName() < j.portName());
}

void SerialTableModel::refresh()
{
	if (_impl == nullptr)
		return;

	_impl->_serialInfos.clear();

	SerialPortInfos availablePorts = SerialPortInfo::availablePorts();
	SerialPortInfos serialInfos;

	if (_impl->_filterKeys.empty())
	{
		for (const auto& serialPortInfo: availablePorts)
		{
			serialInfos.push_back(serialPortInfo);
		}
	}
	else
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

	std::sort(serialInfos.begin(), serialInfos.end(), SortFunc);

	beginResetModel();

	if (serialInfos.size() > 0)
	{
		for (const auto& serialPortInfo: serialInfos)
		{
			if (_impl->_filterKeys.empty())
			{
				_impl->_serialInfos.push_back(serialPortInfo);
			}
			else
			{
				FilterKey filterKey = std::make_pair(serialPortInfo.vendorIdentifier(), serialPortInfo.productIdentifier());
				if (std::find(_impl->_filterKeys.begin(), _impl->_filterKeys.end(), filterKey) != _impl->_filterKeys.end())
				{
					if (filterProc(serialPortInfo.vendorIdentifier(), serialPortInfo.productIdentifier()))
						_impl->_serialInfos.push_back(serialPortInfo);
				}
			}
		}
	}

	endResetModel();

	emit modelUpdated();
}

int SerialTableModel::rowCount
(
	const QModelIndex& parent // = QModelIndex()
) const
{
	(void)parent;
	int result(0);

	if (_impl != nullptr)
	{
		result = static_cast<int>(_impl->_serialInfos.size());
	}

	return result;
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
	switch (role)
	{
	case Qt::DisplayRole:
		switch (orientation)
		{
		case Qt::Horizontal:
			return QString::fromStdString(_columnHeaders.at(section));

		case Qt::Vertical:
			return QString::fromStdString(std::to_string(section + 1));
		}
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
	SerialPortInfo serialInfo = _impl->_serialInfos.at(index.row());

	switch (role)
	{
	case Qt::DisplayRole:
		switch (index.column())
		{
		case kPortName: return QString::fromStdString(serialInfo.portName());
		case kDescription: return QString::fromStdString(serialInfo.description());
		case kSerialNumber: return QString::fromStdString(serialInfo.serialNumber());
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

SerialPortInfo SerialTableModel::getSelectedSerialPort()
{
	return _selectedSerialPort;
}

SerialPortInfo SerialTableModel::getSerialPortFromName
(
	const std::string &portName
)
{
	SerialPortInfo result;

	refresh();

	for (const auto& serialInfo: _impl->_serialInfos)
	{
		if (equalsIgnoreCase(serialInfo.portName(), portName))
		{
			result = serialInfo;
			break;
		}
	}

	return result;
}

SerialPortInfo SerialTableModel::getSerialPortFromSerialNumber
(
	const std::string &serialNumber
)
{
	SerialPortInfo result;

	refresh();

	for (const auto& serialInfo: _impl->_serialInfos)
	{
		if (equalsIgnoreCase(serialInfo.serialNumber(), serialNumber))
		{
			result = serialInfo;
			break;
		}
	}

	return result;
}

void SerialTableModel::setSelectedSerialPort(int rowIndex)
{
	if (rowIndex == -1)
		_selectedSerialPort = SerialPortInfo();
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
			switch (column)
			{
			case kPortName:
				result = _impl->_serialInfos.at(row).portName();
				break;
			case kDescription:
				result = _impl->_serialInfos.at(row).description();
				break;
			case kSerialNumber:
				result = _impl->_serialInfos.at(row).serialNumber();
				break;
			case kVendor:
				result = DecAndHexPrint(_impl->_serialInfos.at(row).vendorIdentifier(), 4);
				break;
			case kProduct:
				result = DecAndHexPrint(_impl->_serialInfos.at(row).productIdentifier(), 4);
				break;
			default:
				result = "-";
				break;
			}
		}
	}

	return result;
}
