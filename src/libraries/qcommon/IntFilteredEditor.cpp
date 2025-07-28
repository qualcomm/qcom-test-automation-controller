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
// Copyright 2019 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
