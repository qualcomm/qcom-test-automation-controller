#ifndef DEVICECATALOG_H
#define DEVICECATALOG_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "ui_DeviceCatalog.h"
#include "AlpacaDevice.h"

// Qt
#include <QDialog>


class DeviceCatalog :
	public QDialog,
	public Ui::DeviceCatalog
{
	Q_OBJECT

public:
	DeviceCatalog(QWidget *parent = nullptr);
	~DeviceCatalog();

private slots:
	void onConfigurationLinkClicked(QTableWidgetItem* twi);
	void onCustomContextMenuRequested(const QPoint& pos);
	void on__deviceTable_itemClicked(QTableWidgetItem *item);
	void on__programBtn_clicked();

	void on__firmwareUpdateBtn_clicked();
	void on__docsBtn_clicked();

private:
	AlpacaDevices enumerateDevices(const DebugBoardType type);

	void deviceSelectionDialog(const DebugBoardType type);

	void processProgramming(const DebugBoardType &type, const QPoint &pos, int row);

	void invokeProgrammer(const DebugBoardType type);

	void invokeLiteProgrammer(const QString &serialNumber, const PlatformID platformId);
	void invokePSOCProgrammer(const QString &serialNumber, const PlatformID platformId);

	QString                     _currentSerialNumber;
	PlatformID                  _currentPlatformId;
};
#endif // DEVICECATALOG_H
