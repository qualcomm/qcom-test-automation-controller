// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
