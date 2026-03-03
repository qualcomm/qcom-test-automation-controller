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
// Copyright 2024 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

// Author:	Biswajit Roy <biswroy@qti.qualcomm.com>

#include "PIC32CXEditorView.h"
#include "PIC32CXPlatformConfiguration.h"
#include "CustomValidator.h"
#include "PlatformConfigurationException.h"
#include "TableComboBox.h"
#include "TableCheckBox.h"

// Qt
#include <QMessageBox>

const int kSetPinColumn{0};
const int kEnabledColumn{1};
const int kInvertColumn{2};
const int kPinLabelColumn{3};
const int kPinTooltipColumn{4};
const int kPinCommandColumn{5};
const int kCommandGroupColumn{6};
const int kTabsColumn{7};
const int kCellLocationColumn{8};

const int kHashRole(Qt::UserRole + 1);


PIC32CXEditorView::PIC32CXEditorView(QWidget *parent)
	: EditorView(parent)
{
	setupUi(this);
}

PIC32CXEditorView::~PIC32CXEditorView()
{
}

void PIC32CXEditorView::setPlatformConfiguration(PlatformConfiguration platformConfiguration)
{
	if (platformConfiguration != Q_NULLPTR)
	{
		EditorView::setPlatformConfiguration(platformConfiguration);

		_pic32cxPlatformConfig = static_cast<_PIC32CXPlatformConfiguration*>(platformConfiguration.data());

		read();
	}
}

void PIC32CXEditorView::resetPlatform()
{
	if (_pic32cxPlatformConfig != Q_NULLPTR)
	{
		read();
	}
}

bool PIC32CXEditorView::read()
{
	bool result{true};

	EditorView::read();

	connect(_pic32CXTable, &PIC32CXEditorView::customContextMenuRequested, this, &PIC32CXEditorView::onCustomContextMenuRequested);

	if (_pic32cxPlatformConfig != Q_NULLPTR)
	{
		int row{0};
		for (const PIC32CXPinData& pinData : _pic32cxPlatformConfig->getAllPins())
		{
			TableCheckBox* tableCheckBox{Q_NULLPTR};
			TableComboBox* tableComboBox{Q_NULLPTR};
			QTableWidgetItem* twi{Q_NULLPTR};

			_pic32CXTable->insertRow(row);

			twi = new QTableWidgetItem();
			if (twi != Q_NULLPTR)
			{
				twi->setData(kHashRole, pinData._hash);
				twi->setText(QString::number(pinData._setPin));
				twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				_pic32CXTable->setItem(row, kSetPinColumn, twi);
			}

			tableCheckBox = new TableCheckBox(_pic32CXTable);
			if (tableCheckBox != Q_NULLPTR)
			{
				tableCheckBox->setProperty(kHash, pinData._hash);
				tableCheckBox->setCheckState(pinData._enabled);
				connect(tableCheckBox, SIGNAL(checkStateChanged(bool)), this, SLOT(onEnableCheckChanged(bool)));
				_pic32CXTable->setCellWidget(row, kEnabledColumn, tableCheckBox);
			}

			tableCheckBox = new TableCheckBox(_pic32CXTable);
			if (tableCheckBox != Q_NULLPTR)
			{
				tableCheckBox->setCheckState(pinData._inverted);
				tableCheckBox->setProperty(kHash, pinData._hash);
				connect(tableCheckBox, SIGNAL(checkStateChanged(bool)), this, SLOT(onInvertCheckChanged(bool)));
				_pic32CXTable->setCellWidget(row, kInvertColumn, tableCheckBox);
			}

			twi = new QTableWidgetItem();
			if (twi != Q_NULLPTR)
			{
				twi->setData(kHashRole, pinData._hash);
				twi->setText(pinData._pinLabel.isEmpty() ? kDefaultPinLabel : pinData._pinLabel);
				twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				_pic32CXTable->setItem(row, kPinLabelColumn, twi);
			}

			twi = new QTableWidgetItem();
			if (twi != Q_NULLPTR)
			{
				twi->setData(kHashRole, pinData._hash);
				twi->setText(pinData._pinTooltip.isEmpty() ? kDefaultPinTooltip : pinData._pinTooltip);
				twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				_pic32CXTable->setItem(row, kPinTooltipColumn, twi);
			}

			twi = new QTableWidgetItem();
			if (twi != Q_NULLPTR)
			{
				twi->setData(kHashRole, pinData._hash);
				twi->setText(pinData._pinCommand.isEmpty() ? kDefaultPinCommand : pinData._pinCommand);
				twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				_pic32CXTable->setItem(row, kPinCommandColumn, twi);
			}

			tableComboBox = new TableComboBox(Q_NULLPTR);
			if (tableComboBox != Q_NULLPTR)
			{
				tableComboBox->setProperty(kHash, pinData._hash);
				tableComboBox->setupItems(CommandGroup::toStringList());
				tableComboBox->setCurrentText(CommandGroup::toString(pinData._commandGroup));

				connect(tableComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(onGroupChanged(QString)));
				_pic32CXTable->setCellWidget(row, kCommandGroupColumn, tableComboBox);
			}

			tableComboBox = new TableComboBox(Q_NULLPTR);
			if (tableComboBox != Q_NULLPTR)
			{
				tableComboBox->setProperty(kHash, pinData._hash);
				tableComboBox->setupItems(Tab::toStringList(_configurableTabs));
				tableComboBox->setCurrentText(pinData._tabName);

				connect(tableComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(onTabsChanged(QString)));
				_pic32CXTable->setCellWidget(row, kTabsColumn, tableComboBox);
			}

			twi = new QTableWidgetItem();
			if (twi != Q_NULLPTR)
			{
				twi->setData(kHashRole, pinData._hash);
				twi->setText(QString::number(pinData._cellLocation.x()) + "," + QString::number(pinData._cellLocation.y()));
				twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				_pic32CXTable->setItem(row, kCellLocationColumn, twi);
			}

			row++;
		}

		connect(_pic32CXTable, &QTableWidget::itemChanged, this, &PIC32CXEditorView::onTableItemChanged);

		setupColumnProperties();
	}

	return result;
}

void PIC32CXEditorView::setupColumnProperties()
{
	_pic32CXTable->setItemDelegateForColumn(kPinLabelColumn, new CustomValidator(_pic32CXTable, eNoSemiColonValidator));
	_pic32CXTable->setItemDelegateForColumn(kPinTooltipColumn, new CustomValidator(_pic32CXTable, eNoSemiColonValidator));
	_pic32CXTable->setItemDelegateForColumn(kPinCommandColumn, new CustomValidator(_pic32CXTable, eNoSpaceValidator));
	_pic32CXTable->setItemDelegateForColumn(kCellLocationColumn, new CustomValidator(_pic32CXTable, eCellLocationValidator));

	_pic32CXTable->setFocusPolicy(Qt::NoFocus);
	QHeaderView *headerView = _pic32CXTable->horizontalHeader();
	_pic32CXTable->resizeColumnToContents(kSetPinColumn);

	headerView->setSectionResizeMode(kSetPinColumn, QHeaderView::Fixed);
	_pic32CXTable->resizeColumnToContents(kEnabledColumn);

	headerView->setSectionResizeMode(kEnabledColumn, QHeaderView::Fixed);
	_pic32CXTable->resizeColumnToContents(kInvertColumn);

	headerView->setSectionResizeMode(kInvertColumn, QHeaderView::Fixed);
	_pic32CXTable->setColumnWidth(kPinLabelColumn, 160);
	headerView->setSectionResizeMode(kPinLabelColumn, QHeaderView::Interactive);

	_pic32CXTable->setColumnWidth(kPinTooltipColumn, 220);
	headerView->setSectionResizeMode(kPinTooltipColumn, QHeaderView::Stretch);

	_pic32CXTable->setColumnWidth(kPinCommandColumn, 160);
	headerView->setSectionResizeMode(kPinCommandColumn, QHeaderView::Interactive);

	_pic32CXTable->setColumnWidth(kCommandGroupColumn, 120);
	headerView->setSectionResizeMode(kCommandGroupColumn, QHeaderView::Fixed);

	_pic32CXTable->setColumnWidth(kTabsColumn, 120);
	headerView->setSectionResizeMode(kTabsColumn, QHeaderView::Fixed);

	_pic32CXTable->setColumnWidth(kCellLocationColumn, 100);
	headerView->setSectionResizeMode(kCellLocationColumn, QHeaderView::Fixed);
}

void PIC32CXEditorView::onCustomContextMenuRequested()
{

}

void PIC32CXEditorView::onEnableCheckChanged(bool newState)
{
	TableCheckBox* obj = qobject_cast<TableCheckBox*>(sender());
	if (obj != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();

		_pic32cxPlatformConfig->setPinEnableState(hash, newState);
	}
}

void PIC32CXEditorView::onInvertCheckChanged(bool newState)
{
	TableCheckBox* obj = qobject_cast<TableCheckBox*>(sender());
	if (obj != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();

		_pic32cxPlatformConfig->setPinInvertedState(hash, newState);
	}
}

void PIC32CXEditorView::onGroupChanged(const QString &newText)
{
	TableComboBox* obj = qobject_cast<TableComboBox*>(sender());
	if (obj != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();

		try
		{
			_pic32cxPlatformConfig->setPinGroup(hash, CommandGroup::fromString(newText));
		}
		catch (const PlatformConfigurationException& e)
		{
			QMessageBox::critical(this, "TAC Configuration Editor - cannot set pin group", e.getMessage());
		}
	}
}

void PIC32CXEditorView::onTabsChanged(const QString &newText)
{
	TableComboBox* obj = qobject_cast<TableComboBox*>(sender());
	if (obj != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();

		try
		{
			_pic32cxPlatformConfig->setTabName(hash, newText);
		}
		catch (const PlatformConfigurationException& e)
		{
			QMessageBox::critical(this, "TAC Configuration Editor - cannot set tab", e.getMessage());
		}
	}
}

void PIC32CXEditorView::onTableItemChanged(QTableWidgetItem* twi)
{
	HashType hash = twi->data(kHashRole).toInt();
	QString itemText = twi->text();
	switch(twi->column())
	{
	case kPinLabelColumn:
		_pic32cxPlatformConfig->setPinLabel(hash, sanitizeText(itemText));
		break;
	case kPinTooltipColumn:
		_pic32cxPlatformConfig->setPinTooltip(hash, sanitizeText(itemText));
		break;
	case kPinCommandColumn:
		_pic32cxPlatformConfig->setPinCommand(hash, sanitizeText(itemText));
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
				_pic32cxPlatformConfig->setPinCellLocation(hash, QPoint(-1,-1));
			}
			else
				_pic32cxPlatformConfig->setPinCellLocation(hash, QPoint(row, col));
		}
		else
		{
			twi->setText(kDefaultCellLocation);
			_pic32cxPlatformConfig->setPinCellLocation(hash, QPoint(-1,-1));
		}
		break;
	}
	default:
		break;
	}
}
