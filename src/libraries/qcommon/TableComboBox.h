#ifndef TABLECOMBOBOX_H
#define TABLECOMBOBOX_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
