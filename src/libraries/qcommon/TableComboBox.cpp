// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
		Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "TableComboBox.h"

// QCommon
#include "CustomComboBox.h"

TableComboBox::TableComboBox
(
	QWidget* parent
) :
	QWidget(parent)
{
	// Initialize the check-box
    _comboBox = new CustomComboBox(this);
	connect(_comboBox, &QComboBox::activated, this,  &TableComboBox::onActivated);
	connect(_comboBox, &QComboBox::currentIndexChanged, this,  &TableComboBox::onCurrentIndexChanged);
	connect(_comboBox, &QComboBox::currentTextChanged, this,  &TableComboBox::onCurrentTextChanged);
	connect(_comboBox, &QComboBox::editTextChanged, this,  &TableComboBox::onEditTextChanged);
	connect(_comboBox, &QComboBox::highlighted, this,  &TableComboBox::onHighlighted);
	connect(_comboBox, &QComboBox::textActivated, this,  &TableComboBox::onTextActivated);
	connect(_comboBox, &QComboBox::textHighlighted, this,  &TableComboBox::onTextHighlighted);
}

void TableComboBox::setupItems
(
	const QStringList &items,
	const QString& currentText
)
{
	_comboBox->clear();
	_comboBox->addItems(items);
	if (currentText.isEmpty() == false)
		_comboBox->setCurrentText(currentText);
}

QString TableComboBox::currentText()
{
	QString result;

	if (_comboBox != Q_NULLPTR && _comboBox->count() > 0)
	{
		result = _comboBox->currentText();
	}

	return result;
}

void TableComboBox::clear()
{
	_comboBox->clear();
}

void TableComboBox::clearEditText()
{
	_comboBox->clearEditText();
}

void TableComboBox::setCurrentIndex(int index)
{
	_comboBox->setCurrentIndex(index);
}

void TableComboBox::setCurrentText(const QString& text)
{
	_comboBox->setCurrentText(text);
}

void TableComboBox::setEditText(const QString& text)
{
	_comboBox->setEditText(text);
}

void TableComboBox::onActivated(int index)
{
	emit activated(index);
}

void TableComboBox::onCurrentIndexChanged(int index)
{
	emit currentIndexChanged(index);
}

void TableComboBox::onCurrentTextChanged(const QString &text)
{
	emit currentTextChanged(text);
}

void TableComboBox::onEditTextChanged(const QString &text)
{
	emit editTextChanged(text);
}

void TableComboBox::onHighlighted(int index)
{
	emit highlighted(index);
}

void TableComboBox::onTextActivated(const QString &text)
{
	emit textActivated(text);
}

void TableComboBox::onTextHighlighted(const QString &text)
{
	emit textHighlighted(text);
}
