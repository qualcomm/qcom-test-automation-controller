// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#ifndef DEVICESELECTIONDIALOG_H
#define DEVICESELECTIONDIALOG_H

#include "AlpacaDevice.h"
#include <QDialog>

namespace Ui {
class DeviceSelectionDialog;
}

class DeviceSelectionDialog : public QDialog
{
	Q_OBJECT

public:
	explicit DeviceSelectionDialog(QWidget *parent = nullptr);
	~DeviceSelectionDialog();

	void setDevices(AlpacaDevices deviceList);

	QString currentSerialNumber();

private slots:
	void on__deviceSelectionBox_currentIndexChanged(int index);

private:
	Ui::DeviceSelectionDialog *ui;
};

#endif // DEVICESELECTIONDIALOG_H
