#ifndef CUSTOMVALIDATOR_H
#define CUSTOMVALIDATOR_H

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
