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
// Copyright 2024 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
