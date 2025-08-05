#ifndef EDITORVIEW_H
#define EDITORVIEW_H
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
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

// libTAC
#include "Tabs.h"
#include "PlatformConfiguration.h"

// QT
#include <QComboBox>
#include <QFrame>
#include <QTableWidgetItem>
#include <QWidget>

const QByteArray kHash(QByteArrayLiteral("hash"));
const QByteArray kChipIndex(QByteArrayLiteral("chipIndex"));

const QString kDefaultTabString(QStringLiteral("General"));

const QString kDefaultPinLabel(QStringLiteral("<type a label name>"));
const QString kDefaultPinTooltip(QStringLiteral("<add a tooltip>"));
const QString kDefaultPinCommand(QStringLiteral("<type a command>"));
const QString kDefaultPinPriority(QStringLiteral("-1"));
const QString kDefaultClassicAction(QStringLiteral("<type an action>"));
const QString kDefaultCellLocation(QStringLiteral("-1,-1"));

class EditorView:
	public QWidget
{
	Q_OBJECT
public:
	EditorView(QWidget* parent = Q_NULLPTR);
	~EditorView();

	virtual void setPlatformConfiguration(PlatformConfiguration platformConfiguration);
	virtual void resetPlatform() = 0;

protected:
	virtual bool read();
	QString sanitizeText(const QString& inputText);

	Tabs						_configurableTabs;

private:
	PlatformConfiguration		_platformConfiguration;
};

#endif // EDITORVIEW_H
