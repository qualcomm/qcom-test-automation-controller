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

class QCOMMONCONSOLE_EXPORT SerialTableModel :
	public QAbstractTableModel
{
Q_OBJECT

public:
	SerialTableModel(QObject* parent = Q_NULLPTR);
	~SerialTableModel() override;

	void setAppCore(AppCore* appCore);

	void addFilterKey(quint16 vendorID, quint16 productID);

	virtual bool filterProc(quint16 vendorID, quint16 productID);

	QVariant portData(int deviceIndex, int dataIndex);
	QString portName(int deviceIndex);

	QSerialPortInfo getSelectedSerialPort();
	QSerialPortInfo getSerialPortFromName(const QByteArray& portName);
	QSerialPortInfo getSerialPortFromSerialNumber(const QByteArray& serialNumber);
	void setSelectedSerialPort(int rowIndex);

	QString rowData(int row, int column);

	void refresh();

	// QAbstractTableModel
	Q_INVOKABLE virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	Q_INVOKABLE virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	Q_INVOKABLE virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	Q_INVOKABLE virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

signals:
	void modelUpdated();

private:
	AppCore*					_appCore{Q_NULLPTR};
	STMImplementation*			_impl{Q_NULLPTR};
	QSerialPortInfo				_selectedSerialPort;
	QStringList					_columnHeaders;
};

#endif // SERIALLISTMODEL_H

