#ifndef FTDIEDITORVIEW_H
#define FTDIEDITORVIEW_H
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

#include "ui_FTDIEditorView.h"
#include "EditorView.h"

// libTAC
#include "FTDIPlatformConfiguration.h"

// QT
#include <QMap>
#include <QWidget>

struct FTDIModelIndex
{
	FTDIModelIndex() = default;
	FTDIModelIndex(const FTDIModelIndex& copyMe) = default;

	quint16			_chip;
	QChar			_bus;
	quint16			_pin;
};

typedef QMap<quint32, FTDIModelIndex> FTDIModelIndexes;

class FTDIEditorView :
	public EditorView,
	private Ui::FTDIEditorView
{
Q_OBJECT

public:
	explicit FTDIEditorView(QWidget* parent = Q_NULLPTR);
	~FTDIEditorView();

	virtual void setPlatformConfiguration(PlatformConfiguration platformConfiguration);
	virtual void resetPlatform();

protected slots:
	void onBusFunctionChanged(const QString& text);
	void onTableItemChanged(QTableWidgetItem *twi);
	void onEnableCheckChanged(bool newState);
	void onInputCheckChanged(bool newState);
	void onInitialPinValueChanged(bool newState);
	void onInvertCheckChanged(bool newState);
	void onGroupChanged(QString newText);
	void onTabsChanged(QString newText);
	void onCustomContextMenuRequested(const QPoint &pos);

protected:
	virtual bool read();

private:
	void setupChipTabs();

	QTableWidget* selectChipTable(ChipIndex chipIndex);
	void setupDelegates(ChipIndex chipIndex);
	void setupFunctionRows(ChipIndex chipIndex);
	int getPinRowOffset(const Bus& bus);
	void setupPinRows(ChipIndex chipIndex, const Bus& bus);
	void updateBusState(ChipIndex chipindex, HashType hash, bool newState);
	void updatePinsState(QTableWidget* chipTable, ChipIndex chipindex, Bus busId, PinID pinId, bool newState);

	QString getSetPinIndex(const ChipIndex chipindex, const Bus& bus, PinID pinId);

	void setTableWidgetItemEnabled(QTableWidgetItem* twi, bool enabled);

	void clearRow(QTableWidget* tableWidget);
	void resetToDefault(QTableWidget* tableWidget);

	_FTDIPlatformConfiguration*			_ftdiPlatformConfiguration{Q_NULLPTR};
};

#endif // FTDIEDITORVIEW_H
