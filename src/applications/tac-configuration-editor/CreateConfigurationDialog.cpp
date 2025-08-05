/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
