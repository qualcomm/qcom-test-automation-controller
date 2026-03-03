// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2013-2018 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "SerialTableModel.h"

// QCommon
#include "AppCore.h"
#include "SerialPortInfo.h"

// Qt
#include <QSerialPortInfo>
#include <QSet>
#include <QSize>

// C++
#include <algorithm>

typedef QPair<quint16, quint16> FilterKey;

template<class T> QByteArray DecAndHexPrint(T value, int width)
{
	return QString("%1 (0x%2)").arg(value, 10, 10, QChar(' ')).arg(value, width, 16, QChar('0')).toLatin1();
}

struct STMImplementation
{
	QList<FilterKey>			_filterKeys;
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
	_impl(Q_NULLPTR)
{
	_impl = new STMImplementation;

	_columnHeaders << tr("Port Name") << tr("Description") << tr("Serial Number") << tr("Vendor") << tr("Product");
}

SerialTableModel::~SerialTableModel()
{
	if (_impl != Q_NULLPTR)
	{
		delete _impl;
		_impl = Q_NULLPTR;
	}
}

void SerialTableModel::addFilterKey
(
	quint16 vendorID,
	quint16 productID
)
{
	FilterKey filterKey = qMakePair(vendorID, productID);

	if (_impl->_filterKeys.contains(filterKey) == false)
	{
		_impl->_filterKeys.append(filterKey);
	}
}

bool SerialTableModel::filterProc
(
	quint16 vendorID,
	quint16 productID
)
{
	Q_UNUSED(vendorID)
	Q_UNUSED(productID)

	return true;
}

QVariant SerialTableModel::portData(int deviceIndex, int dataIndex)
{
	return data(createIndex(deviceIndex, dataIndex));
}

QString SerialTableModel::portName(int deviceIndex)
{
	QString result;

	if (deviceIndex < rowCount())
	{
		result = data(createIndex(deviceIndex, kPortName)).toString();
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
	bool logging = appCore != Q_NULLPTR;

	bool filtered = _impl->_filterKeys.empty() == false;

	SerialPortInfos availablePorts = SerialPortInfo::availablePorts();
	SerialPortInfos serialInfos;

	if (logging)
	{
		for (const auto& serialPortInfo: std::as_const(availablePorts))
		{
			QString message = QString("Found serial port with vendor id: %1 and product id: %2\n").arg(serialPortInfo.vendorIdentifier()).arg(serialPortInfo.productIdentifier());
			appCore->writeToAppLog(message);
		}
	}

	if (filtered)
	{
		for (const auto& serialPortInfo: std::as_const(availablePorts))
		{	
			FilterKey filterKey = qMakePair(serialPortInfo.vendorIdentifier(), serialPortInfo.productIdentifier());
			if (_impl->_filterKeys.contains(filterKey))
			{
				serialInfos.append(serialPortInfo);
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

	if (serialInfos.count() != 0)
	{
		std::sort(serialInfos.begin(), serialInfos.end(), SortFunc);

		for (const auto& serialPortInfo: std::as_const(serialInfos))
		{
			bool add(true);

			if (filtered)
			{
				FilterKey filterKey = qMakePair(serialPortInfo.vendorIdentifier(), serialPortInfo.productIdentifier());
				add = _impl->_filterKeys.contains(filterKey);
			}

			if (add)
			{
				if (filterProc(serialPortInfo.vendorIdentifier(), serialPortInfo.productIdentifier()))
					_impl->_serialInfos.append(serialPortInfo);
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
	Q_UNUSED(parent);

	return _impl->_serialInfos.count();
}

int SerialTableModel::columnCount
(
	const QModelIndex& parent // = QModelIndex()
) const
{
	Q_UNUSED(parent);

	return _columnHeaders.count();
}

QVariant SerialTableModel::headerData
(
	int section,
	Qt::Orientation orientation,
	int role // = Qt::DisplayRole
) const
{
	Q_UNUSED(orientation);

	switch (role)
	{
	case Qt::DisplayRole:
		return _columnHeaders.at(section);

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
			QString vendorString(DecAndHexPrint(serialInfo.vendorIdentifier(), 4));
			return vendorString;
		}
		case kProduct:
		{
			QString prodString(DecAndHexPrint(serialInfo.productIdentifier(), 4));
			return prodString;
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
	const QByteArray &portName
)
{
	QSerialPortInfo result;

	refresh();

	for (const auto& serialInfo: std::as_const(_impl->_serialInfos))
	{
		if (serialInfo.portName().toLower() == portName.toLower())
		{
			result = serialInfo;
			break;
		}
	}

	return result;
}

QSerialPortInfo SerialTableModel::getSerialPortFromSerialNumber
(
	const QByteArray &serialNumber
)
{
	QSerialPortInfo result;

	refresh();

	for (const auto& serialInfo: std::as_const(_impl->_serialInfos))
	{
		if (serialInfo.serialNumber().toLower() == serialNumber.toLower())
		{
			result = serialInfo;
			break;
		}
	}

	return result;
}

void SerialTableModel::setSelectedSerialPort(int rowIndex)
{
	if (rowIndex ==-1)
		_selectedSerialPort = QSerialPortInfo();
	else
		_selectedSerialPort = _impl->_serialInfos.at(rowIndex);
}

QString SerialTableModel::rowData
(
	int row,
	int column
)
{
	QString result;

	if (row < _impl->_serialInfos.count())
	{
		if (column < _columnHeaders.count())
		{
			QModelIndex modelIndex = index(row, column);
			result = data(modelIndex).toString();
		}
	}

	return result;
}
