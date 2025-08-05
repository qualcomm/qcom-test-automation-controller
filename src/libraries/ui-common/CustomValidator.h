#ifndef CUSTOMVALIDATOR_H
#define CUSTOMVALIDATOR_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "DebugBoardType.h"

// Qt
#include <QItemDelegate>
#include <QRegularExpression>


enum ValidatorType
{
	eDefaultValidator,
	eNumericOnlyValidator,
	eNoSpaceValidator,
	eNoSemiColonValidator,
	eCellLocationValidator,
	ePlatformIdValidator,
	ePINEVersionValidator
};

class CustomValidator : public QItemDelegate
{
public:
	CustomValidator(QWidget *parent = Q_NULLPTR, const ValidatorType validationType = eDefaultValidator, DebugBoardType type = eUnknownDebugBoard);
	virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
	void setCellLocationValidation();
	void setNoSpaceValidation();
	void setNumericOnlyValidation();
	void setNoSemiColonValidation();
	void setPlatformIdLimit(QWidget *parent);
	void setPINEVersionValidation(QWidget *parent);

	QRegularExpression              _re;
	QString                         _tooltip;
	DebugBoardType                  _debugBoardType{eUnknownDebugBoard};
};

#endif // CUSTOMVALIDATOR_H
