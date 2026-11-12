// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SERIALTABLEMODEL_H
#define SERIALTABLEMODEL_H

struct STMImplementation;

#include "QCommonConsoleGlobal.h"

// QCommon
class AppCore;

// Qt
#include <QAbstractTableModel>
#include <QSerialPortInfo>
#include <QString>

#include <cstdint>
#include <string>
#include <vector>

class QCOMMONCONSOLE_EXPORT SerialTableModel :
	public QAbstractTableModel
{
Q_OBJECT

public:
	SerialTableModel(QObject* parent = nullptr);
	~SerialTableModel() override;

	void setAppCore(AppCore* appCore);

	void addFilterKey(uint16_t vendorID, uint16_t productID);

	virtual bool filterProc(uint16_t vendorID, uint16_t productID);

	QVariant portData(int deviceIndex, int dataIndex);
	std::string portName(int deviceIndex);

	QSerialPortInfo getSelectedSerialPort();
	QSerialPortInfo getSerialPortFromName(const std::string& portName);
	QSerialPortInfo getSerialPortFromSerialNumber(const std::string& serialNumber);
	void setSelectedSerialPort(int rowIndex);

	std::string rowData(int row, int column);

	void refresh();

	// QAbstractTableModel
	Q_INVOKABLE virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	Q_INVOKABLE virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	Q_INVOKABLE virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	Q_INVOKABLE virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

signals:
	void modelUpdated();

private:
	AppCore*					_appCore{nullptr};
	STMImplementation*			_impl{nullptr};
	QSerialPortInfo				_selectedSerialPort;
	std::vector<std::string>	_columnHeaders;
};

#endif // SERIALTABLEMODEL_H
