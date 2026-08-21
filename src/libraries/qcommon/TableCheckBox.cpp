// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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

	// QCheckBox::checkStateChanged() replaced stateChanged() in Qt 6.7; stateChanged()
	// is compiled out on newer Qt by QT_DISABLE_DEPRECATED_UP_TO, so branch on version.
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect(_checkBox, &QCheckBox::checkStateChanged, this, [=, this](bool newState){ emit checkStateChanged(newState); });
#else
	connect(_checkBox, &QCheckBox::stateChanged, this, [=, this](bool newState){ emit checkStateChanged(newState); });
#endif

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
