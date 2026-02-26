#ifndef SERIALTABLEMODEL_H
#define SERIALTABLEMODEL_H

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

