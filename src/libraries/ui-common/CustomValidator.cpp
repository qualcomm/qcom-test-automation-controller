// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "CustomValidator.h"

// Qt
#include <QLineEdit>
#include <QRegularExpressionValidator>


const int kMaxCharacterLength(200);

CustomValidator::CustomValidator(QWidget *parent, const ValidatorType validationType, DebugBoardType type):
	QItemDelegate(parent)
{
	switch (validationType)
	{
	case eDefaultValidator:
		break;
	case eNumericOnlyValidator:
		setNumericOnlyValidation();
		break;
	case eNoSpaceValidator:
		setNoSpaceValidation();
		break;
	case eNoSemiColonValidator:
		setNoSemiColonValidation();
		break;
	case eCellLocationValidator:
		_debugBoardType = type;
		setCellLocationValidation();
		break;
	case ePlatformIdValidator:
		_debugBoardType = type;
		setPlatformIdLimit(parent);
		break;
	case ePINEVersionValidator:
		setPINEVersionValidation(parent);
		break;
	}
}

QWidget *CustomValidator::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(option)
	Q_UNUSED(index)

	QLineEdit* customLineEdit = new QLineEdit(parent);

	// set the maximum character limit for the QLineEdit
	customLineEdit->setMaxLength(kMaxCharacterLength);

	customLineEdit->setToolTip(_tooltip);


	QRegularExpressionValidator* validator = new QRegularExpressionValidator(_re, parent);
	customLineEdit->setValidator(validator);

	return customLineEdit;
}

void CustomValidator::setCellLocationValidation()
{
	_re.setPattern("[-]{0,1}[0-9]{1,2},[-]{0,1}[0-9]{1,2}");
	_tooltip = "Allowed format y,x or yy,xx or -yy,-xx";
}

void CustomValidator::setNoSpaceValidation()
{
	// regex to match only alphabets, numbers, underscore and hyphen in the input string
	_re.setPattern("^([a-zA-Z0-9_-])+$");
	_tooltip = "AlphaNumeric characters allowed. Spaces and special characters are not allowed except - and _";
}

void CustomValidator::setNumericOnlyValidation()
{
	_re.setPattern("[-]{0,1}[0-9]{10}");
	_tooltip = "Only positive and negative whole numbers are allowed";
}

void CustomValidator::setNoSemiColonValidation()
{
	_re.setPattern("[\\w\\d/\\-:.,_$%\x27\"()<>#$%^&+=\x2A !\\[\\]\\{\\}@]{1,}");
	_tooltip = "Alphanumeric and spaces allowed. ;|\\ are not allowed";
}

void CustomValidator::setPlatformIdLimit(QWidget *parent)
{
	if (parent != Q_NULLPTR)
	{
		QLineEdit* lew = qobject_cast<QLineEdit*>(parent);

		if (lew != Q_NULLPTR)
		{
			if (_debugBoardType == eFTDI)
			{
				_re.setPattern("[0-9]{1,5}");
				_tooltip = "Platform id > 90000 is dedicated for experiments";

				QRegularExpressionValidator* validator = new QRegularExpressionValidator(_re, parent);
				lew->setValidator(validator);
				lew->setToolTip(_tooltip);
			}
		}
	}
}

void CustomValidator::setPINEVersionValidation(QWidget *parent)
{
	if (parent != Q_NULLPTR)
	{
		QLineEdit* lew = qobject_cast<QLineEdit*>(parent);

		if (lew != Q_NULLPTR)
		{
			_re.setPattern("[0-9]{1,5}");
			_tooltip = "Maximum pine version can be 99999";

			QRegularExpressionValidator validator(_re, parent);
			lew->setValidator(&validator);
		}
	}
}
