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
// Copyright 2022 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "CreateConfigurationDialog.h"

// QT
#include <QMessageBox>
#include <QMetaEnum>
#include <QSpinBox>

CreateConfigurationDialog::CreateConfigurationDialog
(
	QWidget* parent
) :
	QDialog(parent)
{
	setupUi(this);

	// mapping combo-box elements with the enum
	_platformComboBox->insertItem(_platformComboBox->count(), "<select a platform>", eUnknownDebugBoard);
	_platformComboBox->insertItem(_platformComboBox->count(), "FTDI", eFTDI);

	_chipCount->setMinimum(1);
	_chipCount->setMaximum(4);

	_chipCount->setEnabled(false);
	_countLabel->setEnabled(false);
}

CreateConfigurationDialog::~CreateConfigurationDialog()
{
}

DebugBoardType CreateConfigurationDialog::getPlatform()
{
	return _platformType;
}

int CreateConfigurationDialog::getChipCount()
{
	return _chipCount->value();
}

void CreateConfigurationDialog::on__platformComboBox_currentIndexChanged(int index)
{
	bool enabled{false};

	_platformType = static_cast<DebugBoardType>(_platformComboBox->itemData(index).toInt());

	if (_platformType == eFTDI)
		enabled = true;

	_chipCount->setEnabled(enabled);
	_countLabel->setEnabled(enabled);
}
