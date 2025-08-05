#ifndef FTDIEDITORVIEW_H
#define FTDIEDITORVIEW_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
