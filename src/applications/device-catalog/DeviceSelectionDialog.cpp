// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "DeviceSelectionDialog.h"
#include "ui_DeviceSelectionDialog.h"

#include <QPushButton>

const QByteArray kDefaultDevice(QByteArrayLiteral("<select a device>"));


DeviceSelectionDialog::DeviceSelectionDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::DeviceSelectionDialog)
{
	ui->setupUi(this);
	ui->_selectionDialogBtnBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

DeviceSelectionDialog::~DeviceSelectionDialog()
{
	delete ui;
}

void DeviceSelectionDialog::setDevices(AlpacaDevices alpacaDevices)
{
	QStringList deviceList;
	deviceList << kDefaultDevice;

	for (AlpacaDevice device : alpacaDevices)
	{
		deviceList << device->portName() + " : " + device->serialNumber();
	}

	ui->_deviceSelectionBox->addItems(deviceList);
}

QString DeviceSelectionDialog::currentSerialNumber()
{
	QString currentSerialNumber;

	QString currentDevice = ui->_deviceSelectionBox->currentText();

	if (currentDevice.compare(kDefaultDevice) != 0)
		currentSerialNumber = currentDevice.split(" : ", Qt::SkipEmptyParts).at(1);

	return currentSerialNumber;
}

void DeviceSelectionDialog::on__deviceSelectionBox_currentIndexChanged(int index)
{
	if (index > 0)
		ui->_selectionDialogBtnBox->button(QDialogButtonBox::Ok)->setEnabled(true);
	else
		ui->_selectionDialogBtnBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}
