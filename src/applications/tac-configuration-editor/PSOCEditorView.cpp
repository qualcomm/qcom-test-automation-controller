// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "PSOCEditorView.h"

#include "CustomValidator.h"

#include "CommandGroup.h"
#include "PlatformConfigurationException.h"

// QCommon
#include "StringUtilities.h"
#include "TableCheckBox.h"
#include "TableComboBox.h"

// Qt
#include <QCheckBox>
#include <QFile>
#include <QMenu>
#include <QMessageBox>

// C++
#include <iterator>

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

const int kI2CPinColumn{0};
const int kI2CEnabledColumn{1};
const int kI2CInvertColumn{2};
const int kI2CWriteAddressColumn{3};
const int kI2CPinLabelColumn{4};
const int kI2CPinTooltipColumn{5};
const int kI2CPinCommandColumn{6};
const int kI2CCommandGroupColumn{7};
const int kI2CTabsColumn{8};
const int kI2CCellLocationColumn{9};

const int kHashRole(Qt::UserRole + 1);

const ColumnResizeSpec kGPIOColumnSpecs[] = {
	{ kPinColumn,                   -1,  QHeaderView::Fixed },
	{ kEnabledColumn,               -1,  QHeaderView::Fixed },
	{ kInitialPinValueColumn,       -1,  QHeaderView::Fixed },
	{ kInitializationPriorityColumn,-1,  QHeaderView::Fixed },
	{ kInvertColumn,                -1,  QHeaderView::Fixed },
	{ kPinLabelColumn,              160, QHeaderView::Interactive },
	{ kPinTooltipColumn,            220, QHeaderView::Stretch },
	{ kPinCommandColumn,            160, QHeaderView::Interactive },
	{ kCommandGroupColumn,          120, QHeaderView::Fixed },
	{ kTabsColumn,                  120, QHeaderView::Fixed },
	{ kClassicActionColumn,         -1,  QHeaderView::Fixed },
	{ kCellLocationColumn,          100, QHeaderView::Fixed },
};

const ColumnResizeSpec kI2CColumnSpecs[] = {
	{ kI2CPinColumn,          -1,  QHeaderView::Fixed },
	{ kI2CEnabledColumn,      -1,  QHeaderView::Fixed },
	{ kI2CInvertColumn,       -1,  QHeaderView::Fixed },
	{ kI2CWriteAddressColumn, -1,  QHeaderView::Fixed },
	{ kI2CPinLabelColumn,     160, QHeaderView::Interactive },
	{ kI2CPinTooltipColumn,   220, QHeaderView::Stretch },
	{ kI2CPinCommandColumn,   160, QHeaderView::Interactive },
	{ kI2CCommandGroupColumn, 120, QHeaderView::Fixed },
	{ kI2CTabsColumn,         120, QHeaderView::Fixed },
	{ kI2CCellLocationColumn, 100, QHeaderView::Fixed },
};


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

void PSOCEditorView::onI2CEnableCheckChanged(bool newState)
{
	TableCheckBox* obj = qobject_cast<TableCheckBox*>(sender());
	if (obj != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();

		_psocPlatformConfig->setI2CPinEnableState(hash, newState);
	}
}

void PSOCEditorView::onI2CInvertCheckChanged(bool newState)
{
	TableCheckBox* obj = qobject_cast<TableCheckBox*>(sender());
	if (obj != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();

		_psocPlatformConfig->setI2CPinInvertedState(hash, newState);
	}
}

void PSOCEditorView::onI2CGroupChanged(QString newText)
{
	TableComboBox* obj = qobject_cast<TableComboBox*>(sender());
	if (obj != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();

		try
		{
			_psocPlatformConfig->setI2CPinGroup(hash, CommandGroup::fromString(newText));
		}
		catch (const PlatformConfigurationException& e)
		{
			QMessageBox::critical(this, "TAC Configuration Editor - cannot set pin group", e.getMessage());
		}
	}
}

void PSOCEditorView::onI2CTabsChanged(QString newText)
{
	TableComboBox* obj = qobject_cast<TableComboBox*>(sender());
	if (obj != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();

		_psocPlatformConfig->setI2CTabName(hash, newText);
	}
}

void PSOCEditorView::onI2CTableItemChanged(QTableWidgetItem* twi)
{
	HashType hash = twi->data(kHashRole).toULongLong();
	QString itemText = twi->text();
	switch(twi->column())
	{
	case kI2CPinLabelColumn:
		_psocPlatformConfig->setI2CPinLabel(hash, sanitizeText(itemText));
		break;
	case kI2CPinTooltipColumn:
		_psocPlatformConfig->setI2CPinTooltip(hash, sanitizeText(itemText));
		break;
	case kI2CPinCommandColumn:
		_psocPlatformConfig->setI2CPinCommand(hash, sanitizeText(itemText));
		break;
	case kI2CCellLocationColumn:
	{
		QStringList rowColString = itemText.split(",");
		if (rowColString.size() == 2)
		{
			int row = rowColString.at(0).toInt();
			int col = rowColString.at(1).toInt();

			if (row < 0 || col < 0)
			{
				twi->setText(kDefaultCellLocation);
				_psocPlatformConfig->setI2CPinCellLocation(hash, QPoint(-1,-1));
			}
			else
				_psocPlatformConfig->setI2CPinCellLocation(hash, QPoint(row, col));
		}
		else
		{
			twi->setText(kDefaultCellLocation);
			_psocPlatformConfig->setI2CPinCellLocation(hash, QPoint(-1,-1));
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

	_slaveI2CTables.clear();

	int row = 0;

	if (_psocPlatformConfig != Q_NULLPTR)
	{
		PSOCI2CSlaves slaves = _psocPlatformConfig->getSlaveConfigs();

		if (slaves.count() > 0)
			_psocTabWgt->setTabVisible(1, 0);

		for (auto& s : slaves)
		{
			QWidget* wgt = new QWidget(_psocTabWgt);
			QHBoxLayout* hbl = new QHBoxLayout(wgt);

			QTableWidget* slaveTable = createI2CTableFromTemplate(wgt);
			hbl->addWidget(slaveTable);
			_slaveI2CTables.append(slaveTable);

			_psocTabWgt->addTab(wgt, _PSOCPlatformConfiguration::i2cSlaveTabName(s));
		}

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

		PSOCI2CEntries i2cEntries = _psocPlatformConfig->getI2CEntries();

		if (slaves.isEmpty())
		{
			populateI2CTable(_i2cTable, i2cEntries.values());
			connect(_i2cTable, &QTableWidget::itemChanged, this, &PSOCEditorView::onI2CTableItemChanged);
		}
		else
		{
			for (int i = 0; i < slaves.count(); ++i)
			{
				QList<PSOCI2CData> slaveEntries = _psocPlatformConfig->getI2CEntriesForSlave(slaves.at(i));
				populateI2CTable(_slaveI2CTables.at(i), slaveEntries);
			}
		}

		connect(_psocTable, &QTableWidget::itemChanged, this, &PSOCEditorView::onTableItemChanged);

		setupColumnProperties();
	}

	return result;
}

QTableWidget* PSOCEditorView::createI2CTableFromTemplate(QWidget* parent)
{
	QTableWidget* table = new QTableWidget(parent);

	table->setColumnCount(_i2cTable->columnCount());
	table->setContextMenuPolicy(_i2cTable->contextMenuPolicy());
	table->setFocusPolicy(_i2cTable->focusPolicy());
	table->horizontalHeader()->setStretchLastSection(_i2cTable->horizontalHeader()->stretchLastSection());
	table->verticalHeader()->setVisible(_i2cTable->verticalHeader()->isVisible());
	table->verticalHeader()->setDefaultSectionSize(_i2cTable->verticalHeader()->defaultSectionSize());

	for (int col = 0; col < _i2cTable->columnCount(); ++col)
	{
		QTableWidgetItem* templateHeader = _i2cTable->horizontalHeaderItem(col);
		QTableWidgetItem* headerItem = new QTableWidgetItem();

		if (templateHeader != Q_NULLPTR)
		{
			headerItem->setText(templateHeader->text());
			headerItem->setToolTip(templateHeader->toolTip());

			if (templateHeader->textAlignment() != 0)
				headerItem->setTextAlignment(static_cast<Qt::Alignment>(templateHeader->textAlignment()));
		}

		table->setHorizontalHeaderItem(col, headerItem);
	}

	connect(table, &QTableWidget::itemChanged, this, &PSOCEditorView::onI2CTableItemChanged);

	return table;
}

void PSOCEditorView::populateI2CTable(QTableWidget* table, const QList<PSOCI2CData>& entries)
{
	int row = 0;

	for (auto& i2cData : entries)
	{
		TableCheckBox* tableCheckBox{Q_NULLPTR};
		TableComboBox* tableComboBox{Q_NULLPTR};
		QTableWidgetItem* twi{Q_NULLPTR};

		table->insertRow(row);

		HashType hash{i2cData._hash};

		twi = new QTableWidgetItem();
		twi->setData(kHashRole, hash);
		twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		twi->setText(QString::number(static_cast<quint16>(i2cData._pin)));
		twi->setFlags(twi->flags() & ~Qt::ItemIsEditable);
		table->setItem(row, kI2CPinColumn, twi);

		tableCheckBox = new TableCheckBox(Q_NULLPTR);
		tableCheckBox->setProperty(kHash, hash);
		tableCheckBox->setCheckState(i2cData._enabled);
		connect(tableCheckBox, SIGNAL(checkStateChanged(bool)), this, SLOT(onI2CEnableCheckChanged(bool)));
		table->setCellWidget(row, kI2CEnabledColumn, tableCheckBox);

		tableCheckBox = new TableCheckBox(Q_NULLPTR);
		tableCheckBox->setCheckState(i2cData._inverted);
		tableCheckBox->setProperty(kHash, hash);
		connect(tableCheckBox, SIGNAL(checkStateChanged(bool)), this, SLOT(onI2CInvertCheckChanged(bool)));
		table->setCellWidget(row, kI2CInvertColumn, tableCheckBox);

		twi = new QTableWidgetItem();
		twi->setData(kHashRole, hash);
		twi->setText(QString::number(i2cData._writeAddress, 16));
		twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		table->setItem(row, kI2CWriteAddressColumn, twi);

		twi = new QTableWidgetItem();
		twi->setData(kHashRole, hash);
		twi->setText(i2cData._pinLabel.isEmpty() ? kDefaultPinLabel : i2cData._pinLabel);
		twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		table->setItem(row, kI2CPinLabelColumn, twi);

		twi = new QTableWidgetItem();
		twi->setData(kHashRole, hash);
		twi->setText(i2cData._pinTooltip.isEmpty() ? kDefaultPinTooltip : i2cData._pinTooltip);
		twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		table->setItem(row, kI2CPinTooltipColumn, twi);

		twi = new QTableWidgetItem();
		twi->setData(kHashRole, hash);
		twi->setText(i2cData._pinCommand.isEmpty() ? kDefaultPinCommand : i2cData._pinCommand);
		twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		table->setItem(row, kI2CPinCommandColumn, twi);

		tableComboBox = new TableComboBox(Q_NULLPTR);
		tableComboBox->setProperty(kHash, hash);
		tableComboBox->setupItems(CommandGroup::toStringList());
		tableComboBox->setCurrentText(CommandGroup::toString(i2cData._commandGroup));
		connect(tableComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(onI2CGroupChanged(QString)));
		table->setCellWidget(row, kI2CCommandGroupColumn, tableComboBox);

		tableComboBox = new TableComboBox(Q_NULLPTR);
		tableComboBox->setProperty(kHash, hash);
		tableComboBox->setupItems(Tab::toStringList(_configurableTabs));
		tableComboBox->setCurrentText(i2cData._tabName);
		connect(tableComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(onI2CTabsChanged(QString)));
		table->setCellWidget(row, kI2CTabsColumn, tableComboBox);

		twi = new QTableWidgetItem();
		twi->setData(kHashRole, hash);
		twi->setText(QString::number(i2cData._cellLocation.x()) + "," + QString::number(i2cData._cellLocation.y()));
		twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		table->setItem(row, kI2CCellLocationColumn, twi);

		row++;
	}
}

void PSOCEditorView::applyColumnResizeSpecs(QTableWidget* table, const ColumnResizeSpec* specs, int count)
{
	QHeaderView* headerView = table->horizontalHeader();

	for (int i = 0; i < count; ++i)
	{
		if (specs[i].width < 0)
			table->resizeColumnToContents(specs[i].column);
		else
			table->setColumnWidth(specs[i].column, specs[i].width);

		headerView->setSectionResizeMode(specs[i].column, specs[i].mode);
	}
}

void PSOCEditorView::setupColumnProperties()
{
	applyColumnResizeSpecs(_psocTable, kGPIOColumnSpecs, std::size(kGPIOColumnSpecs));

	applyColumnResizeSpecs(_i2cTable, kI2CColumnSpecs, std::size(kI2CColumnSpecs));
	
	for (auto* table : std::as_const(_slaveI2CTables))
		applyColumnResizeSpecs(table, kI2CColumnSpecs, std::size(kI2CColumnSpecs));
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

	if (_psocPlatformConfig != Q_NULLPTR)
	{
		_PSOCPlatformConfiguration psocConfig(_psocPlatformConfig->variant());
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
}
