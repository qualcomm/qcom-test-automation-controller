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
// Copyright 2022 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "PSOCEditorView.h"

#include "CustomValidator.h"

#include "CommandGroup.h"
#include "PlatformConfigurationException.h"

// QCommon
#include "StringUtilities.h"
#include "TableCheckBox.h"
#include "TableComboBox.h"

// QT
#include <QCheckBox>
#include <QFile>
#include <QMenu>
#include <QMessageBox>

const int kPinColumn{0};
const int kEnabledColumn{1};
const int kInitialPinValueColumn{2};
const int kInitializationPriorityColumn{3};
const int kInvertColumn{4};
const int kPinLabelColumn{5};
const int kPinTooltipColumn{6};
const int kPinCommandColumn{7};
const int kClassicActionColumn{8};
const int kCommandGroupColumn{9};
const int kTabsColumn{10};
const int kCellLocationColumn{11};

const int kHashRole(Qt::UserRole + 1);

PSOCEditorView::PSOCEditorView(QWidget *parent):
	  EditorView(parent)
{
	setupUi(this);

	// No semicolon allowed in the label column
	_psocTable->setItemDelegateForColumn(kPinLabelColumn, new CustomValidator(_psocTable, eNoSemiColonValidator));

	// No semicolon allowed in the tooltip column
	_psocTable->setItemDelegateForColumn(kPinTooltipColumn, new CustomValidator(_psocTable, eNoSemiColonValidator));

	// Limit spaces in the pin command string
	_psocTable->setItemDelegateForColumn(kPinCommandColumn, new CustomValidator(_psocTable, eNoSpaceValidator));

	// Only accept input in yy,xx format
	_psocTable->setItemDelegateForColumn(kCellLocationColumn, new CustomValidator(_psocTable, eCellLocationValidator));

	_psocTable->setFocusPolicy(Qt::NoFocus);
}

PSOCEditorView::~PSOCEditorView()
{
	// do not delete, belongs to a smart pointer that will do the job
	_psocPlatformConfig = Q_NULLPTR;
}

void PSOCEditorView::setPlatformConfiguration
(
	PlatformConfiguration platformConfiguration
)
{
	if (platformConfiguration != Q_NULLPTR)
	{
		EditorView::setPlatformConfiguration(platformConfiguration);

		_psocPlatformConfig = static_cast<_PSOCPlatformConfiguration*>(platformConfiguration.data());

		read();
	}
}

void PSOCEditorView::resetPlatform()
{
	if (_psocPlatformConfig != Q_NULLPTR)
	{
		read();
	}
}

void PSOCEditorView::setRowEnabled(int rowId, bool state)
{
	if (_psocTable == Q_NULLPTR || rowId < 0 || rowId >= _psocTable->rowCount())
		return;

	QTableWidgetItem* twi{Q_NULLPTR};
	TableCheckBox* checkBoxWidget{Q_NULLPTR};
	TableComboBox* comboBoxWidget{Q_NULLPTR};

	checkBoxWidget = static_cast<TableCheckBox*>(_psocTable->cellWidget(rowId, kEnabledColumn));
	if (checkBoxWidget)
	{
		checkBoxWidget->setCheckState(false);
		checkBoxWidget->setEnabled(state);
	}

	checkBoxWidget = static_cast<TableCheckBox*>(_psocTable->cellWidget(rowId, kInitialPinValueColumn));
	if (checkBoxWidget)
	{
		checkBoxWidget->setCheckState(false);
		checkBoxWidget->setEnabled(state);
	}

	checkBoxWidget = static_cast<TableCheckBox*>(_psocTable->cellWidget(rowId, kInvertColumn));
	if (checkBoxWidget)
	{
		checkBoxWidget->setCheckState(false);
		checkBoxWidget->setEnabled(state);
	}

	twi = _psocTable->item(rowId, kInitializationPriorityColumn);
	if (twi)
	{
		twi->setText(kDefaultPinPriority);
		if (state == false)
			twi->setFlags(twi->flags() & ~Qt::ItemIsEditable);
		else
			twi->setFlags(twi->flags() | Qt::ItemIsEditable);
	}

	twi = _psocTable->item(rowId, kPinLabelColumn);
	if (twi)
	{
		twi->setText(kDefaultPinLabel);
		if (state == false)
			twi->setFlags(twi->flags() & ~Qt::ItemIsEditable);
		else
			twi->setFlags(twi->flags() | Qt::ItemIsEditable);
	}

	twi = _psocTable->item(rowId, kPinTooltipColumn);
	if (twi)
	{
		twi->setText(kDefaultPinTooltip);
		if (state == false)
			twi->setFlags(twi->flags() & ~Qt::ItemIsEditable);
		else
			twi->setFlags(twi->flags() | Qt::ItemIsEditable);
	}

	twi = _psocTable->item(rowId, kPinCommandColumn);
	if (twi)
	{
		twi->setText(kDefaultPinCommand);
		if (state == false)
			twi->setFlags(twi->flags() & ~Qt::ItemIsEditable);
		else
			twi->setFlags(twi->flags() | Qt::ItemIsEditable);
	}

	comboBoxWidget = static_cast<TableComboBox*>(_psocTable->cellWidget(rowId, kCommandGroupColumn));
	if (comboBoxWidget)
	{
		comboBoxWidget->setCurrentIndex(0);
		comboBoxWidget->setEnabled(state);
	}

	comboBoxWidget = static_cast<TableComboBox*>(_psocTable->cellWidget(rowId, kTabsColumn));
	if (comboBoxWidget)
	{
		comboBoxWidget->setCurrentText(kDefaultTabString);
		comboBoxWidget->setEnabled(state);
	}

	twi = _psocTable->item(rowId, kCellLocationColumn);
	if (twi)
	{
		twi->setText(kDefaultCellLocation);
		if (state == false)
			twi->setFlags(twi->flags() & ~Qt::ItemIsEditable);
		else
			twi->setFlags(twi->flags() | Qt::ItemIsEditable);
	}
}

void PSOCEditorView::onEnableCheckChanged(bool newState)
{
	TableCheckBox* obj = qobject_cast<TableCheckBox*>(sender());
	if (obj != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();

		_psocPlatformConfig->setPinEnableState(hash, newState);
	}
}

void PSOCEditorView::onInitialPinValueChanged(bool newState)
{
	TableCheckBox* obj = qobject_cast<TableCheckBox*>(sender());
	if (obj != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();

		_psocPlatformConfig->setInitialPinValue(hash, newState);
	}
}

void PSOCEditorView::onInvertCheckChanged(bool newState)
{
	TableCheckBox* obj = qobject_cast<TableCheckBox*>(sender());
	if (obj != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();

		_psocPlatformConfig->setPinInvertedState(hash, newState);
	}
}

void PSOCEditorView::onGroupChanged(QString newText)
{
	TableComboBox* obj = qobject_cast<TableComboBox*>(sender());
	if (obj != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();

		try
		{
			_psocPlatformConfig->setPinGroup(hash, CommandGroup::fromString(newText));
		}
		catch (const PlatformConfigurationException& e)
		{
			QMessageBox::critical(this, "TAC Configuration Editor - cannot set pin group", e.getMessage());
		}
	}
}

void PSOCEditorView::onTabsChanged(QString newText)
{
	TableComboBox* obj = qobject_cast<TableComboBox*>(sender());
	if (obj != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();

		_psocPlatformConfig->setTabName(hash, newText);
	}
}

void PSOCEditorView::onTableItemChanged(QTableWidgetItem* twi)
{
	HashType hash = twi->data(kHashRole).toInt();
	QString itemText = twi->text();
	switch(twi->column())
	{
	case kInitializationPriorityColumn:
		_psocPlatformConfig->setPinInitializationPriority(hash, itemText.toInt());
		break;
	case kPinLabelColumn:
		_psocPlatformConfig->setPinLabel(hash, sanitizeText(itemText));
		break;
	case kPinTooltipColumn:
		_psocPlatformConfig->setPinTooltip(hash, sanitizeText(itemText));
		break;
	case kPinCommandColumn:
		_psocPlatformConfig->setPinCommand(hash, sanitizeText(itemText));
		break;
	case kCellLocationColumn:
	{
		QStringList rowColString = itemText.split(",");
		if (rowColString.size() == 2)
		{
			int row = rowColString.at(0).toInt();
			int col = rowColString.at(1).toInt();

			if (row < 0 || col < 0)
			{
				twi->setText(kDefaultCellLocation);
				_psocPlatformConfig->setPinCellLocation(hash, QPoint(-1,-1));
			}
			else
				_psocPlatformConfig->setPinCellLocation(hash, QPoint(row, col));
		}
		else
		{
			twi->setText(kDefaultCellLocation);
			_psocPlatformConfig->setPinCellLocation(hash, QPoint(-1,-1));
		}
		break;
	}
	default:
		break;
	}
}

bool PSOCEditorView::read()
{
	bool result{true};

	EditorView::read();

	_psocTable->clearContents();
	_psocTable->setRowCount(0);
	connect(_psocTable, &PSOCEditorView::customContextMenuRequested, this, &PSOCEditorView::onCustomContextMenuRequested);

	int row = 0;

	if (_psocPlatformConfig != Q_NULLPTR)
	{
		for (auto& pin : _psocPlatformConfig->getAllPins())
		{
			TableCheckBox* tableCheckBox{Q_NULLPTR};
			TableComboBox* tableComboBox{Q_NULLPTR};
			QTableWidgetItem* twi{Q_NULLPTR};

			_psocTable->insertRow(row);

			HashType hash{static_cast<HashType>(pin._pin)};

			twi = new QTableWidgetItem();
			twi->setData(kHashRole, hash);
			twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			twi->setText(QString::number(static_cast<quint16>(pin._pin)));
			twi->setFlags(twi->flags() & ~Qt::ItemIsEditable);
			_psocTable->setItem(row, kPinColumn, twi);

			tableCheckBox = new TableCheckBox(Q_NULLPTR);
			tableCheckBox->setProperty(kHash, hash);
			tableCheckBox->setCheckState(pin._enabled);
			connect(tableCheckBox, SIGNAL(checkStateChanged(bool)), this, SLOT(onEnableCheckChanged(bool)));
			_psocTable->setCellWidget(row, kEnabledColumn, tableCheckBox);

			tableCheckBox = new TableCheckBox(Q_NULLPTR);
			tableCheckBox->setProperty(kHash, hash);
			tableCheckBox->setCheckState(pin._initialValue);
			connect(tableCheckBox, SIGNAL(checkStateChanged(bool)), this, SLOT(onInitialPinValueChanged(bool)));
			_psocTable->setCellWidget(row, kInitialPinValueColumn, tableCheckBox);

			twi = new QTableWidgetItem();
			twi->setData(kHashRole, hash);
			twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			twi->setText(QString::number(pin._initializationPriority));
			_psocTable->setItem(row, kInitializationPriorityColumn, twi);

			tableCheckBox = new TableCheckBox(Q_NULLPTR);
			tableCheckBox->setCheckState(pin._inverted);
			tableCheckBox->setProperty(kHash, hash);
			connect(tableCheckBox, SIGNAL(checkStateChanged(bool)), this, SLOT(onInvertCheckChanged(bool)));
			_psocTable->setCellWidget(row, kInvertColumn, tableCheckBox);

			twi = new QTableWidgetItem();
			twi->setData(kHashRole, hash);
			twi->setText(pin._pinLabel.isEmpty() ? kDefaultPinLabel : pin._pinLabel);
			twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			_psocTable->setItem(row, kPinLabelColumn, twi);

			twi = new QTableWidgetItem();
			twi->setData(kHashRole, hash);
			twi->setText(pin._pinTooltip.isEmpty() ? kDefaultPinTooltip : pin._pinTooltip);
			twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			_psocTable->setItem(row, kPinTooltipColumn, twi);

			twi = new QTableWidgetItem();
			twi->setData(kHashRole, hash);
			twi->setText(pin._pinCommand.isEmpty() ? kDefaultPinCommand : pin._pinCommand);
			twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			_psocTable->setItem(row, kPinCommandColumn, twi);

			tableComboBox = new TableComboBox(Q_NULLPTR);
			tableComboBox->setProperty(kHash, hash);
			tableComboBox->setupItems(CommandGroup::toStringList());
			tableComboBox->setCurrentText(CommandGroup::toString(pin._commandGroup));
			connect(tableComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(onGroupChanged(QString)));
			_psocTable->setCellWidget(row, kCommandGroupColumn, tableComboBox);

			tableComboBox = new TableComboBox(Q_NULLPTR);
			tableComboBox->setProperty(kHash, hash);

			tableComboBox->setupItems(Tab::toStringList(_configurableTabs));
			tableComboBox->setCurrentText(pin._tabName);
			connect(tableComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(onTabsChanged(QString)));
			_psocTable->setCellWidget(row, kTabsColumn, tableComboBox);

			twi = new QTableWidgetItem();
			twi->setData(kHashRole, hash);
			twi->setText(pin._classicAction);
			twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			twi->setFlags(twi->flags() & ~Qt::ItemIsEditable);
			_psocTable->setItem(row, kClassicActionColumn, twi);

			twi = new QTableWidgetItem();
			twi->setData(kHashRole, hash);
			twi->setText(QString::number(pin._cellLocation.x()) + "," + QString::number(pin._cellLocation.y()));
			twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			_psocTable->setItem(row, kCellLocationColumn, twi);

			row++;
		}

		connect(_psocTable, &QTableWidget::itemChanged, this, &PSOCEditorView::onTableItemChanged);

		setupColumnProperties();
	}

	return result;
}

void PSOCEditorView::setupColumnProperties()
{
	_psocTable->setFocusPolicy(Qt::NoFocus);

	QHeaderView* headerView = _psocTable->horizontalHeader();

	_psocTable->resizeColumnToContents(kPinColumn);
	headerView->setSectionResizeMode(kPinColumn, QHeaderView::Fixed);

	_psocTable->resizeColumnToContents(kEnabledColumn);
	headerView->setSectionResizeMode(kEnabledColumn, QHeaderView::Fixed);

	_psocTable->resizeColumnToContents(kInitialPinValueColumn);
	headerView->setSectionResizeMode(kInitialPinValueColumn, QHeaderView::Fixed);

	_psocTable->resizeColumnToContents(kInitializationPriorityColumn);
	headerView->setSectionResizeMode(kInitializationPriorityColumn, QHeaderView::Fixed);

	_psocTable->resizeColumnToContents(kInvertColumn);
	headerView->setSectionResizeMode(kInvertColumn, QHeaderView::Fixed);

	_psocTable->setColumnWidth(kPinLabelColumn, 160);
	headerView->setSectionResizeMode(kPinLabelColumn, QHeaderView::Interactive);

	_psocTable->setColumnWidth(kPinTooltipColumn, 220);
	headerView->setSectionResizeMode(kPinTooltipColumn, QHeaderView::Stretch);

	_psocTable->setColumnWidth(kPinCommandColumn, 160);
	headerView->setSectionResizeMode(kPinCommandColumn, QHeaderView::Interactive);

	_psocTable->setColumnWidth(kCommandGroupColumn, 120);
	headerView->setSectionResizeMode(kCommandGroupColumn, QHeaderView::Fixed);

	_psocTable->setColumnWidth(kTabsColumn, 120);
	headerView->setSectionResizeMode(kTabsColumn, QHeaderView::Fixed);

	_psocTable->resizeColumnToContents(kClassicActionColumn);
	headerView->setSectionResizeMode(kClassicActionColumn, QHeaderView::Fixed);

	_psocTable->setColumnWidth(kCellLocationColumn, 100);
	headerView->setSectionResizeMode(kCellLocationColumn, QHeaderView::Fixed);
}

void PSOCEditorView::onCustomContextMenuRequested(const QPoint& pos)
{
	QTableWidget* tableWidget = qobject_cast<QTableWidget*>(sender());

	if (tableWidget != Q_NULLPTR)
	{
		QMenu menu;
		QAction* clearAction = menu.addAction("Clear");
		QAction* resetAction = menu.addAction("Reset to default");
		QAction* result = menu.exec(tableWidget->mapToGlobal(pos));

		if (result != Q_NULLPTR)
		{
			if (result == clearAction)
				clearRow();
			else if (result == resetAction)
				resetToDefault();
		}
	}
}

void PSOCEditorView::clearRow()
{
	QTableWidgetItem* twi{Q_NULLPTR};
	TableCheckBox* checkBoxWidget{Q_NULLPTR};
	TableComboBox* comboBoxWidget{Q_NULLPTR};

	int rowId = _psocTable->currentRow();

	checkBoxWidget = static_cast<TableCheckBox*>(_psocTable->cellWidget(rowId, kEnabledColumn));
	if (checkBoxWidget)
		checkBoxWidget->setCheckState(false);

	checkBoxWidget = static_cast<TableCheckBox*>(_psocTable->cellWidget(rowId, kInitialPinValueColumn));
	if (checkBoxWidget)
		checkBoxWidget->setCheckState(false);

	checkBoxWidget = static_cast<TableCheckBox*>(_psocTable->cellWidget(rowId, kInvertColumn));
	if (checkBoxWidget)
		checkBoxWidget->setCheckState(false);

	twi = _psocTable->item(rowId, kInitializationPriorityColumn);
	if (twi)
		twi->setText(kDefaultPinPriority);

	twi = _psocTable->item(rowId, kPinLabelColumn);
	if (twi)
		twi->setText(kDefaultPinLabel);

	twi = _psocTable->item(rowId, kPinTooltipColumn);
	if (twi)
		twi->setText(kDefaultPinTooltip);

	twi = _psocTable->item(rowId, kPinCommandColumn);
	if (twi)
		twi->setText(kDefaultPinCommand);

	comboBoxWidget = static_cast<TableComboBox*>(_psocTable->cellWidget(rowId, kCommandGroupColumn));
	if (comboBoxWidget)
		comboBoxWidget->setCurrentIndex(0);

	comboBoxWidget = static_cast<TableComboBox*>(_psocTable->cellWidget(rowId, kTabsColumn));
	if (comboBoxWidget)
		comboBoxWidget->setCurrentText(kDefaultTabString);

	twi = _psocTable->item(rowId, kCellLocationColumn);
	if (twi)
		twi->setText(kDefaultCellLocation);
}

void PSOCEditorView::resetToDefault()
{
	QTableWidgetItem* twi{Q_NULLPTR};
	TableCheckBox* checkBoxWidget{Q_NULLPTR};
	TableComboBox* comboBoxWidget{Q_NULLPTR};

	_PSOCPlatformConfiguration psocConfig;
	for (auto& pinData : psocConfig.getPins())
	{
		PinID pin{0};

		int rowId = _psocTable->currentRow();

		twi = _psocTable->item(rowId, kPinColumn);
		if (twi)
			pin = twi->text().toUInt();

		if (pin == pinData._pin)
		{
			checkBoxWidget = static_cast<TableCheckBox*>(_psocTable->cellWidget(rowId, kEnabledColumn));
			if (checkBoxWidget)
				checkBoxWidget->setCheckState(pinData._enabled);

			checkBoxWidget = static_cast<TableCheckBox*>(_psocTable->cellWidget(rowId, kInitialPinValueColumn));
			if (checkBoxWidget)
				checkBoxWidget->setCheckState(pinData._initialValue);

			checkBoxWidget = static_cast<TableCheckBox*>(_psocTable->cellWidget(rowId, kInvertColumn));
			if (checkBoxWidget)
				checkBoxWidget->setCheckState(pinData._inverted);

			twi = _psocTable->item(rowId, kPinLabelColumn);
			if (twi)
				twi->setText(pinData._pinLabel);

			twi = _psocTable->item(rowId, kPinTooltipColumn);
			if (twi)
				twi->setText(pinData._pinTooltip);

			twi = _psocTable->item(rowId, kPinCommandColumn);
			if (twi)
				twi->setText(pinData._pinCommand);

			comboBoxWidget = static_cast<TableComboBox*>(_psocTable->cellWidget(rowId, kCommandGroupColumn));
			if (comboBoxWidget)
				comboBoxWidget->setCurrentText(CommandGroup::toString(pinData._commandGroup));

			comboBoxWidget = static_cast<TableComboBox*>(_psocTable->cellWidget(rowId, kTabsColumn));
			if (comboBoxWidget)
				comboBoxWidget->setCurrentText(pinData._tabName);

			twi = _psocTable->item(rowId, kCellLocationColumn);
			if (twi)
			{
				QString cellLocationString = QString::number(pinData._cellLocation.x()) + "," + QString::number(pinData._cellLocation.y());
				twi->setText(cellLocationString);
			}
		}
	}
}
