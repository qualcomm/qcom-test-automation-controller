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
		Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "TableCheckBox.h"

// Qt
#include <QCheckBox>
#include <QHBoxLayout>

TableCheckBox::TableCheckBox
(
	QWidget* parent
) :
	QWidget{parent}
{
	// Initialize the check-box
	_checkBox = new QCheckBox(this);

	connect(_checkBox, &QCheckBox::checkStateChanged, this, [=, this](bool newState){ emit checkStateChanged(newState); });

	// Prepare check box layout
	QHBoxLayout* layoutCheckBox = new QHBoxLayout(this);

	// Add the checkbox to the layout
	layoutCheckBox->addWidget(_checkBox);

	layoutCheckBox->setAlignment(Qt::AlignCenter);
	layoutCheckBox->setContentsMargins(0,0,0,0);
}

void TableCheckBox::setCheckState(bool state)
{
	Qt::CheckState checkState = state ? Qt::Checked : Qt::Unchecked;

	if (_checkBox->checkState() != checkState)
	{
		_checkBox->setCheckState(checkState);

		emit checkStateChanged(state);
	}
}

void TableCheckBox::setCheckState(Qt::CheckState state)
{
	if (_checkBox->checkState() != state)
	{
		_checkBox->setCheckState(Qt::Checked);

		emit checkStateChanged(Qt::Checked);
	}
}
