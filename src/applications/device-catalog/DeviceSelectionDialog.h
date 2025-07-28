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
