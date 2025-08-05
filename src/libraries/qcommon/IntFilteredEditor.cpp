// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "IntFilteredEditor.h"

// Qt
#include <QSet>
#include <QValidator>

class IntValidator:
	public QValidator
{
public:
	IntValidator(QWidget* parent) :
		QValidator(parent)
	{

	}

	void addBannedInt(quint32 bannedInt)
	{
		_bannedInts.insert(bannedInt);
	}

	void removeBannedInt(quint32 unbannedInt)
	{
		_bannedInts.remove(unbannedInt);
	}

	virtual QValidator::State validate(QString& input, int& pos) const
	{
		Q_UNUSED(pos);

		QValidator::State result(QValidator::Invalid);

        bool okay{false};

		int testInt = input.toInt(&okay);
		if (okay == true)
		{
			if (_bannedInts.contains(testInt) == false)
			{
				result = QValidator::Acceptable;
			}
			else
			{
				result = QValidator::Intermediate;
			}
		}

		return result;
	}

private:
	QSet<quint32>					_bannedInts;
};

// IntFilteredEditor

IntFilteredEditor::IntFilteredEditor
(
	QWidget* parent
) : QLineEdit(parent)
{
	_validator = new IntValidator(this);
	setValidator(_validator);
}

IntFilteredEditor::~IntFilteredEditor()
{
	if (_validator != Q_NULLPTR)
	{
		delete _validator;
		_validator = Q_NULLPTR;
	}
}

void IntFilteredEditor::addBannedInt
(
	quint32 banMe
)
{
	_validator->addBannedInt(banMe);
}

void IntFilteredEditor::removeBannedInt
(
	quint32 unBanMe
)
{
	_validator->removeBannedInt(unBanMe);
}
