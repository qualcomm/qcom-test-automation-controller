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
