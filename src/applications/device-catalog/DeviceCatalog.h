#ifndef DEVICECATALOG_H
#define DEVICECATALOG_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ui_DeviceCatalog.h"

// QCommon
#include "AlpacaDevice.h"
#include "MediaPlaybackFrame.h"

class DeviceCatalog :
	public QDialog,
	private Ui::DeviceCatalog
{
	Q_OBJECT

public:
	DeviceCatalog(QWidget* parent = Q_NULLPTR);
	~DeviceCatalog();

private slots:
	void onConfigurationLinkClicked(QTableWidgetItem* twi);
	void onCustomContextMenuRequested(const QPoint& pos);
	void on__deviceTable_itemClicked(QTableWidgetItem* item);
	void on__programBtn_clicked();
	void on__firmwareUpdateBtn_clicked();
	void on__docsBtn_clicked();
	void on__firmwareSelect_currentTextChanged(const QString &firmwareVersion);

	void onInfoGroupCloseBtnClicked();
	void onInfoGroupLinkClicked(const QString& link);

private:
	AlpacaDevices enumerateDevices(const DebugBoardType type);
	void deviceSelectionDialog(const DebugBoardType type);
	void processProgramming(const DebugBoardType& type, const QPoint& pos, int row);
	void invokeProgrammer(const DebugBoardType type);
	void invokeLiteProgrammer(const QString& serialNumber, const PlatformID platformId);
	void invokePSOCProgrammer(const QString& serialNumber, const PlatformID platformId);

	QByteArray					_firmwareDir;
	QString						_currentSerialNumber;
	PlatformID					_currentPlatformId;

	MediaPlaybackFrame			_player;
};

#endif // DEVICECATALOG_H
