// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "STM32EditorView.h"
#include "STM32PlatformConfiguration.h"
#include "CustomValidator.h"
#include "PlatformConfigurationException.h"
#include "TableComboBox.h"
#include "TableCheckBox.h"

// Qt
#include <QMessageBox>

const int kPinColumn{0};
const int kEnabledColumn{1};
const int kInvertColumn{2};
const int kPinLabelColumn{3};
const int kPinTooltipColumn{4};
const int kPinCommandColumn{5};
const int kCommandGroupColumn{6};
const int kTabsColumn{7};
const int kCellLocationColumn{8};

const int kHashRole(Qt::UserRole + 1);

STM32EditorView::STM32EditorView(QWidget *parent)
	: EditorView(parent)
{
	setupUi(this);
}

STM32EditorView::~STM32EditorView()
{
}

void STM32EditorView::setPlatformConfiguration(PlatformConfiguration platformConfiguration)
{
	if (platformConfiguration != Q_NULLPTR)
	{
		EditorView::setPlatformConfiguration(platformConfiguration);

		_stm32PlatformConfig = static_cast<_STM32PlatformConfiguration*>(platformConfiguration.data());

		read();
	}
}

void STM32EditorView::resetPlatform()
{
	if (_stm32PlatformConfig != Q_NULLPTR)
	{
		read();
	}
}

bool STM32EditorView::read()
{
	bool result{true};

	EditorView::read();

	connect(tableWidget, &STM32EditorView::customContextMenuRequested, this, &STM32EditorView::onCustomContextMenuRequested);

	if (_stm32PlatformConfig != Q_NULLPTR)
	{
		tableWidget->clearContents();
		tableWidget->setRowCount(0);

		int row{0};
		for (const STM32PinData& pinData : _stm32PlatformConfig->getAllPins())
		{
			TableCheckBox* tableCheckBox{Q_NULLPTR};
			TableComboBox* tableComboBox{Q_NULLPTR};
			QTableWidgetItem* twi{Q_NULLPTR};

			tableWidget->insertRow(row);

			twi = new QTableWidgetItem();
			if (twi != Q_NULLPTR)
			{
				twi->setData(kHashRole, pinData._hash);
				twi->setText(QString::number(pinData._pin));
				twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				twi->setFlags(twi->flags() & ~Qt::ItemIsEditable);
				tableWidget->setItem(row, kPinColumn, twi);
			}

			tableCheckBox = new TableCheckBox(tableWidget);
			if (tableCheckBox != Q_NULLPTR)
			{
				tableCheckBox->setProperty(kHash, pinData._hash);
				tableCheckBox->setCheckState(pinData._enabled);
				connect(tableCheckBox, SIGNAL(checkStateChanged(bool)), this, SLOT(onEnableCheckChanged(bool)));
				tableWidget->setCellWidget(row, kEnabledColumn, tableCheckBox);
			}

			tableCheckBox = new TableCheckBox(tableWidget);
			if (tableCheckBox != Q_NULLPTR)
			{
				tableCheckBox->setProperty(kHash, pinData._hash);
				tableCheckBox->setCheckState(pinData._inverted);
				connect(tableCheckBox, SIGNAL(checkStateChanged(bool)), this, SLOT(onInvertCheckChanged(bool)));
				tableWidget->setCellWidget(row, kInvertColumn, tableCheckBox);
			}

			twi = new QTableWidgetItem();
			if (twi != Q_NULLPTR)
			{
				twi->setData(kHashRole, pinData._hash);
				twi->setText(pinData._pinLabel.isEmpty() ? kDefaultPinLabel : pinData._pinLabel);
				twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				tableWidget->setItem(row, kPinLabelColumn, twi);
			}

			twi = new QTableWidgetItem();
			if (twi != Q_NULLPTR)
			{
				twi->setData(kHashRole, pinData._hash);
				twi->setText(pinData._pinTooltip.isEmpty() ? kDefaultPinTooltip : pinData._pinTooltip);
				twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				tableWidget->setItem(row, kPinTooltipColumn, twi);
			}

			twi = new QTableWidgetItem();
			if (twi != Q_NULLPTR)
			{
				twi->setData(kHashRole, pinData._hash);
				twi->setText(pinData._pinCommand.isEmpty() ? kDefaultPinCommand : pinData._pinCommand);
				twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				tableWidget->setItem(row, kPinCommandColumn, twi);
			}

			tableComboBox = new TableComboBox(Q_NULLPTR);
			if (tableComboBox != Q_NULLPTR)
			{
				tableComboBox->setProperty(kHash, pinData._hash);
				tableComboBox->setupItems(CommandGroup::toStringList());
				tableComboBox->setCurrentText(CommandGroup::toString(pinData._commandGroup));

				connect(tableComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(onGroupChanged(QString)));
				tableWidget->setCellWidget(row, kCommandGroupColumn, tableComboBox);
			}

			tableComboBox = new TableComboBox(Q_NULLPTR);
			if (tableComboBox != Q_NULLPTR)
			{
				tableComboBox->setProperty(kHash, pinData._hash);
				tableComboBox->setupItems(Tab::toStringList(_configurableTabs));
				tableComboBox->setCurrentText(pinData._tabName);

				connect(tableComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(onTabsChanged(QString)));
				tableWidget->setCellWidget(row, kTabsColumn, tableComboBox);
			}

			twi = new QTableWidgetItem();
			if (twi != Q_NULLPTR)
			{
				twi->setData(kHashRole, pinData._hash);
				twi->setText(QString::number(pinData._cellLocation.x()) + "," + QString::number(pinData._cellLocation.y()));
				twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				tableWidget->setItem(row, kCellLocationColumn, twi);
			}

			row++;
		}

		connect(tableWidget, &QTableWidget::itemChanged, this, &STM32EditorView::onTableItemChanged);

		setupColumnProperties();
	}

	return result;
}

void STM32EditorView::setupColumnProperties()
{
	tableWidget->setItemDelegateForColumn(kPinLabelColumn, new CustomValidator(tableWidget, eNoSemiColonValidator));
	tableWidget->setItemDelegateForColumn(kPinTooltipColumn, new CustomValidator(tableWidget, eNoSemiColonValidator));
	tableWidget->setItemDelegateForColumn(kPinCommandColumn, new CustomValidator(tableWidget, eNoSpaceValidator));
	tableWidget->setItemDelegateForColumn(kCellLocationColumn, new CustomValidator(tableWidget, eCellLocationValidator));

	tableWidget->setFocusPolicy(Qt::NoFocus);
	QHeaderView *headerView = tableWidget->horizontalHeader();
	tableWidget->resizeColumnToContents(kPinColumn);

	headerView->setSectionResizeMode(kPinColumn, QHeaderView::Fixed);
	tableWidget->resizeColumnToContents(kEnabledColumn);

	headerView->setSectionResizeMode(kEnabledColumn, QHeaderView::Fixed);
	tableWidget->resizeColumnToContents(kInvertColumn);

	headerView->setSectionResizeMode(kInvertColumn, QHeaderView::Fixed);
	tableWidget->setColumnWidth(kPinLabelColumn, 160);
	headerView->setSectionResizeMode(kPinLabelColumn, QHeaderView::Interactive);

	tableWidget->setColumnWidth(kPinTooltipColumn, 220);
	headerView->setSectionResizeMode(kPinTooltipColumn, QHeaderView::Stretch);

	tableWidget->setColumnWidth(kPinCommandColumn, 160);
	headerView->setSectionResizeMode(kPinCommandColumn, QHeaderView::Interactive);

	tableWidget->setColumnWidth(kCommandGroupColumn, 120);
	headerView->setSectionResizeMode(kCommandGroupColumn, QHeaderView::Fixed);

	tableWidget->setColumnWidth(kTabsColumn, 120);
	headerView->setSectionResizeMode(kTabsColumn, QHeaderView::Fixed);

	tableWidget->setColumnWidth(kCellLocationColumn, 100);
	headerView->setSectionResizeMode(kCellLocationColumn, QHeaderView::Fixed);
}

void STM32EditorView::onCustomContextMenuRequested()
{

}

void STM32EditorView::onEnableCheckChanged(bool newState)
{
	TableCheckBox* obj = qobject_cast<TableCheckBox*>(sender());
	if (obj != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();

		_stm32PlatformConfig->setPinEnableState(hash, newState);
	}
}

void STM32EditorView::onInvertCheckChanged(bool newState)
{
	TableCheckBox* obj = qobject_cast<TableCheckBox*>(sender());
	if (obj != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();

		_stm32PlatformConfig->setPinInvertedState(hash, newState);
	}
}

void STM32EditorView::onGroupChanged(const QString &newText)
{
	TableComboBox* obj = qobject_cast<TableComboBox*>(sender());
	if (obj != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();

		try
		{
			_stm32PlatformConfig->setPinGroup(hash, CommandGroup::fromString(newText));
		}
		catch (const PlatformConfigurationException& e)
		{
			QMessageBox::critical(this, "TAC Configuration Editor - cannot set pin group", e.getMessage());
		}
	}
}

void STM32EditorView::onTabsChanged(const QString &newText)
{
	TableComboBox* obj = qobject_cast<TableComboBox*>(sender());
	if (obj != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();

		try
		{
			_stm32PlatformConfig->setTabName(hash, newText);
		}
		catch (const PlatformConfigurationException& e)
		{
			QMessageBox::critical(this, "TAC Configuration Editor - cannot set tab", e.getMessage());
		}
	}
}

void STM32EditorView::onTableItemChanged(QTableWidgetItem* twi)
{
	HashType hash = twi->data(kHashRole).toInt();
	QString itemText = twi->text();
	switch(twi->column())
	{
	case kPinLabelColumn:
		_stm32PlatformConfig->setPinLabel(hash, sanitizeText(itemText));
		break;
	case kPinTooltipColumn:
		_stm32PlatformConfig->setPinTooltip(hash, sanitizeText(itemText));
		break;
	case kPinCommandColumn:
		_stm32PlatformConfig->setPinCommand(hash, sanitizeText(itemText));
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
				_stm32PlatformConfig->setPinCellLocation(hash, QPoint(-1,-1));
			}
			else
				_stm32PlatformConfig->setPinCellLocation(hash, QPoint(row, col));
		}
		else
		{
			twi->setText(kDefaultCellLocation);
			_stm32PlatformConfig->setPinCellLocation(hash, QPoint(-1,-1));
		}
		break;
	}
	default:
		break;
	}
}
