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
