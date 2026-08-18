// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "CreateConfigurationDialog.h"

// QT
#include <QMessageBox>
#include <QMetaEnum>
#include <QSpinBox>

const QByteArray kPSOCVariant("PSOCVariant");

CreateConfigurationDialog::CreateConfigurationDialog
(
	QWidget* parent
) :
	QDialog(parent)
{
	setupUi(this);

	// mapping combo-box elements with the enum
	_platformComboBox->insertItem(_platformComboBox->count(), "<select a platform>", eUnknownDebugBoard);
	_platformComboBox->insertItem(_platformComboBox->count(), "PSOC", ePSOC);
	_platformComboBox->insertItem(_platformComboBox->count(), "FTDI", eFTDI);
	_platformComboBox->insertItem(_platformComboBox->count(), "PIC32CX (Automotive)", ePIC32CXAuto);
	_platformComboBox->insertItem(_platformComboBox->count(), "FT232H (Arduino V1)", eFT232H);

	_chipCount->setMinimum(1);
	_chipCount->setMaximum(4);

	_gpioRadio->setChecked(true);

	_psocConfiguration->hide();
	_ftdiConfiguration->hide();

	_configurationGroup->hide();
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
	if (_platformType != eFTDI)
		return 0;

	return _chipCount->value();
}

PSOCVariant CreateConfigurationDialog::getPSOCVariant()
{
	PSOCVariant result{ePSOCUnknown};

	if (_platformType == ePSOC)
		result = psocVariantFromString(_psocGPIOConfiguration->checkedButton()->text());

	return result;
}

void CreateConfigurationDialog::on__platformComboBox_currentIndexChanged(int index)
{
	_platformType = static_cast<DebugBoardType>(_platformComboBox->itemData(index).toInt());

	if (_platformType == eFTDI)
	{
		_psocConfiguration->hide();
		_ftdiConfiguration->show();

		_configurationGroup->show();
	}
	else if (_platformType == ePSOC)
	{
		_ftdiConfiguration->hide();
		_psocConfiguration->show();

		_configurationGroup->show();
	}
	else
		_configurationGroup->hide();
}
