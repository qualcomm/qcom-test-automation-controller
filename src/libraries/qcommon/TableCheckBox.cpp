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

#include "TableCheckBox.h"

// Qt
#include <QCheckBox>
#include <QHBoxLayout>

TableCheckBox::TableCheckBox
(
	QWidget* parent
) :
	QWidget{parent}
{
	// Initialize the check-box
	_checkBox = new QCheckBox(this);

	connect(_checkBox, &QCheckBox::checkStateChanged, this, [=, this](bool newState){ emit checkStateChanged(newState); });

	// Prepare check box layout
	QHBoxLayout* layoutCheckBox = new QHBoxLayout(this);

	// Add the checkbox to the layout
	layoutCheckBox->addWidget(_checkBox);

	layoutCheckBox->setAlignment(Qt::AlignCenter);
	layoutCheckBox->setContentsMargins(0,0,0,0);
}

void TableCheckBox::setCheckState(bool state)
{
	Qt::CheckState checkState = state ? Qt::Checked : Qt::Unchecked;

	if (_checkBox->checkState() != checkState)
	{
		_checkBox->setCheckState(checkState);

		emit checkStateChanged(state);
	}
}

void TableCheckBox::setCheckState(Qt::CheckState state)
{
	if (_checkBox->checkState() != state)
	{
		_checkBox->setCheckState(Qt::Checked);

		emit checkStateChanged(Qt::Checked);
	}
}
