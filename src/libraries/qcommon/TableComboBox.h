#ifndef TABLECOMBOBOX_H
#define TABLECOMBOBOX_H
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

#include "QCommonGlobal.h"

// QCommon
class CustomComboBox;
#include <QWidget>

class QCOMMON_EXPORT TableComboBox :
	public QWidget
{
Q_OBJECT

public:
	TableComboBox(QWidget* parent);

	void setupItems(const QStringList& items, const QString& currentText = QString());
	QString currentText();

public slots:
	void clear();
	void clearEditText();
	void setCurrentIndex(int index);
	void setCurrentText(const QString &text);
	void setEditText(const QString &text);

	void onActivated(int index);
	void onCurrentIndexChanged(int index);
	void onCurrentTextChanged(const QString &text);
	void onEditTextChanged(const QString &text);
	void onHighlighted(int index);
	void onTextActivated(const QString &text);
	void onTextHighlighted(const QString &text);

signals:
	void activated(int index);
	void currentIndexChanged(int index);
	void currentTextChanged(const QString &text);
	void editTextChanged(const QString &text);
	void highlighted(int index);
	void textActivated(const QString &text);
	void textHighlighted(const QString &text);


private:
    CustomComboBox*					_comboBox{Q_NULLPTR};
};

#endif // TABLECOMBOBOX_H
