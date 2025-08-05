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

#include "FTDIEditorView.h"

#include "CustomValidator.h"
#include "PlatformConfigurationException.h"

// QCommon
#include "TableCheckBox.h"
#include "TableComboBox.h"
#include "Range.h"

// QT
#include <QCheckBox>
#include <QComboBox>
#include <QFile>
#include <QJsonObject>
#include <QMenu>
#include <QMessageBox>

const int kBusColumn{0};
const int kFunctionColumn{1};
const int kPinColumn{2};
const int kSetPinColumn{3};
const int kEnabledColumn{4};
const int kInputColumn{5};
const int kInitialPinValueColumn{6};
const int kInitializationPriorityColumn{7};
const int kInvertColumn{8};
const int kPinLabelColumn{9};
const int kPinTooltipColumn{10};
const int kPinCommandColumn{11};
const int kCommandGroupColumn{12};
const int kTabsColumn{13};
const int kCellLocationColumn{14};

const int kHashRole(Qt::UserRole + 1);

const int kARow{0};
const int kBRow{9};
const int kCRow{18};
const int kDRow{27};

const QStringList gComboBoxItems =
{
	QStringLiteral("<select a function>"),
	QStringLiteral("VCP"),
	QStringLiteral("D2XX"),
	QStringLiteral("I2C")
};

FTDIEditorView::FTDIEditorView(QWidget* parent):
	EditorView(parent)
{
	setupUi(this);
	connect(_chip1Table, &FTDIEditorView::customContextMenuRequested, this, &FTDIEditorView::onCustomContextMenuRequested);
	connect(_chip2Table, &FTDIEditorView::customContextMenuRequested, this, &FTDIEditorView::onCustomContextMenuRequested);
	connect(_chip3Table, &FTDIEditorView::customContextMenuRequested, this, &FTDIEditorView::onCustomContextMenuRequested);
	connect(_chip4Table, &FTDIEditorView::customContextMenuRequested, this, &FTDIEditorView::onCustomContextMenuRequested);
}

FTDIEditorView::~FTDIEditorView()
{
	// do not delete, belongs to a smart pointer that will do the job
	_ftdiPlatformConfiguration = Q_NULLPTR;
}

void FTDIEditorView::setPlatformConfiguration
(
	PlatformConfiguration platformConfiguration
)
{
	if (platformConfiguration.isNull() == false)
	{
		EditorView::setPlatformConfiguration(platformConfiguration);

		_ftdiPlatformConfiguration = static_cast<_FTDIPlatformConfiguration*>(platformConfiguration.data());

		setupChipTabs();

		read();
	}
}

void FTDIEditorView::resetPlatform()
{
	if (_ftdiPlatformConfiguration != Q_NULLPTR)
	{
		// clear your ui
		read();
	}
}

void FTDIEditorView::onBusFunctionChanged(const QString &text)
{
	TableComboBox* comboBox = qobject_cast<TableComboBox*>(sender());
	if (comboBox)
	{
		HashType hash = comboBox->property(kHash).toULongLong();
		ChipIndex chipIndex = comboBox->property(kChipIndex).toInt();
		_ftdiPlatformConfiguration->setBusFunction(hash, FTDIBusData::fromString(text));

		if (comboBox->currentText().compare(FTDIBusData::toString(eBusFunctionVCP)) ==0 || comboBox->currentText().compare(FTDIBusData::toString(eBusFunctionI2C)) == 0)
		{
			updateBusState(chipIndex, hash, false);
		}
		else
		{
			updateBusState(chipIndex, hash, true);
		}
	}
}

void FTDIEditorView::onEnableCheckChanged(bool newState)
{
	TableCheckBox* obj = qobject_cast<TableCheckBox*>(sender());
	if (obj != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();

		_ftdiPlatformConfiguration->setPinEnableState(hash, newState);
	}
}

void FTDIEditorView::onInputCheckChanged(bool newState)
{
	TableCheckBox* obj = qobject_cast<TableCheckBox*>(sender());
	if (obj != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();

		_ftdiPlatformConfiguration->setPinInputState(hash, newState);
	}
}

void FTDIEditorView::onInitialPinValueChanged(bool newState)
{
	TableCheckBox* obj = qobject_cast<TableCheckBox*>(sender());
	if (obj != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();

		_ftdiPlatformConfiguration->setInitialPinValue(hash, newState);
	}
}

void FTDIEditorView::onInvertCheckChanged(bool newState)
{
	TableCheckBox* obj = qobject_cast<TableCheckBox*>(sender());
	if (obj != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();

		_ftdiPlatformConfiguration->setPinInvertedState(hash, newState);
	}
}

void FTDIEditorView::onGroupChanged(QString newText)
{
	TableComboBox* obj = qobject_cast<TableComboBox*>(sender());
	if (obj != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();

		try
		{
			_ftdiPlatformConfiguration->setPinGroup(hash, CommandGroup::fromString(newText));
		}
		catch (const PlatformConfigurationException& e)
		{
			QMessageBox::critical(this, "TAC Configuration Editor - cannot set pin group", e.getMessage());
		}
	}
}

void FTDIEditorView::onTabsChanged(QString newText)
{
	TableComboBox* obj = qobject_cast<TableComboBox*>(sender());
	if (obj != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();

		_ftdiPlatformConfiguration->setTabName(hash, newText);
	}
}

void FTDIEditorView::onTableItemChanged(QTableWidgetItem* twi)
{
	HashType hash = twi->data(kHashRole).toInt();
	QString itemText = twi->text();
	if (hash != 0)
	{
		switch(twi->column())
		{
		case kInitializationPriorityColumn:
			_ftdiPlatformConfiguration->setPinInitializationPriority(hash, itemText.toInt());
			break;
		case kPinLabelColumn:
			_ftdiPlatformConfiguration->setPinLabel(hash, sanitizeText(itemText));
			break;
		case kPinTooltipColumn:
			_ftdiPlatformConfiguration->setPinTooltip(hash, sanitizeText(itemText));
			break;
		case kPinCommandColumn:
			_ftdiPlatformConfiguration->setPinCommand(hash, sanitizeText(itemText));
			break;
		case kCellLocationColumn:
		{
			QStringList rowColString = itemText.split(",");
			if (rowColString.size() == 2 && rowColString[0].isEmpty() == false && rowColString[1].isEmpty() == false)
			{
				bool okay{false};

				int row = rowColString.at(0).toInt(&okay);

				if (okay)
				{
					int col = rowColString.at(1).toInt(&okay);

					if (okay)
					{
						if (row < 0 || col < 0)
						{
							twi->setText(kDefaultCellLocation);
							_ftdiPlatformConfiguration->setPinCellLocation(hash, QPoint(-1,-1));
						}
						else
							_ftdiPlatformConfiguration->setPinCellLocation(hash, QPoint(row, col));
					}
					else
					{
						twi->setText(kDefaultCellLocation);
						_ftdiPlatformConfiguration->setPinCellLocation(hash, QPoint(-1,-1));
					}
				}
				else
				{
					twi->setText(kDefaultCellLocation);
					_ftdiPlatformConfiguration->setPinCellLocation(hash, QPoint(-1,-1));
				}
			}
			else
			{
				twi->setText(kDefaultCellLocation);
				_ftdiPlatformConfiguration->setPinCellLocation(hash, QPoint(-1,-1));
			}
			break;
		}
		default:
			break;
		}
	}
}

bool FTDIEditorView::read()
{
	bool result{true};

	EditorView::read();

	if (_ftdiPlatformConfiguration != Q_NULLPTR)
	{
		int chipCount = _ftdiPlatformConfiguration->getChipCount();

		for (auto chipIndex: range(chipCount))
		{
			QTableWidget* chipTable = selectChipTable(chipIndex);
			chipTable->clearContents();

			setupPinRows(chipIndex, 'A');
			setupPinRows(chipIndex, 'B');
			setupPinRows(chipIndex, 'C');
			setupPinRows(chipIndex, 'D');

			setupFunctionRows(chipIndex);

			setupDelegates(chipIndex);
		}
	}

	return result;
}

void FTDIEditorView::setupChipTabs()
{
	int chipCount = _ftdiPlatformConfiguration->getChipCount();

	switch (chipCount)
	{
	case 1:
		// removing the tabs in descending order is necessary because
		// the index of tabs will change we remove from beginning. Thus,
		// making the removeTab() for the last index ineffective.
		_chipTabs->removeTab(3);
		_chipTabs->removeTab(2);
		_chipTabs->removeTab(1);
		break;

	case 2:
		_chipTabs->removeTab(3);
		_chipTabs->removeTab(2);
		break;

	case 3:
		_chipTabs->removeTab(3);
		break;

	default:
		break;
	}
}

void FTDIEditorView::setupFunctionRows(ChipIndex chipIndex)
{
	FTDIBusData busFunction;
	TableComboBox* tableComboBox;
	QTableWidget* chipTable{selectChipTable(chipIndex)};

	if (chipTable != Q_NULLPTR)
	{
		QString busFunctionString;
		QTableWidgetItem* twi{Q_NULLPTR};

		twi = new QTableWidgetItem("A");
		chipTable->setItem(kARow, kBusColumn, twi);

		busFunction = _ftdiPlatformConfiguration->getBusFunction(chipIndex, Bus('A'));
		tableComboBox = new TableComboBox(chipTable);
		busFunctionString = FTDIBusData::toString(busFunction._busFunction);
		tableComboBox->setupItems(gComboBoxItems, busFunctionString);
		tableComboBox->setProperty(kHash, busFunction._hash);
		tableComboBox->setProperty(kChipIndex, chipIndex);
		connect(tableComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(onBusFunctionChanged(QString)));
		chipTable->setCellWidget(kARow, kFunctionColumn, tableComboBox);

		updateBusState(chipIndex, busFunction._hash, busFunction._busFunction != eBusFunctionVCP);

		twi = new QTableWidgetItem("B");
		chipTable->setItem(kBRow, kBusColumn, twi);

		busFunction = _ftdiPlatformConfiguration->getBusFunction(chipIndex, Bus('B'));
		tableComboBox = new TableComboBox(chipTable);
		busFunctionString = FTDIBusData::toString(busFunction._busFunction);
		tableComboBox->setupItems(gComboBoxItems, busFunctionString);
		tableComboBox->setProperty(kHash, busFunction._hash);
		tableComboBox->setProperty(kChipIndex, chipIndex);
		connect(tableComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(onBusFunctionChanged(QString)));
		chipTable->setCellWidget(kBRow, kFunctionColumn, tableComboBox);

		updateBusState(chipIndex, busFunction._hash, busFunction._busFunction != eBusFunctionVCP);

		twi = new QTableWidgetItem("C");
		chipTable->setItem(kCRow, kBusColumn, twi);

		busFunction = _ftdiPlatformConfiguration->getBusFunction(chipIndex, Bus('C'));
		tableComboBox = new TableComboBox(chipTable);
		busFunctionString = FTDIBusData::toString(busFunction._busFunction);
		tableComboBox->setupItems(gComboBoxItems, busFunctionString);
		tableComboBox->setProperty(kHash, busFunction._hash);
		tableComboBox->setProperty(kChipIndex, chipIndex);
		connect(tableComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(onBusFunctionChanged(QString)));
		chipTable->setCellWidget(kCRow, kFunctionColumn, tableComboBox);

		updateBusState(chipIndex, busFunction._hash, busFunction._busFunction != eBusFunctionVCP);

		twi = new QTableWidgetItem("D");
		chipTable->setItem(kDRow, kBusColumn, twi);

		busFunction = _ftdiPlatformConfiguration->getBusFunction(chipIndex, Bus('D'));
		tableComboBox = new TableComboBox(chipTable);
		busFunctionString = FTDIBusData::toString(busFunction._busFunction);
		tableComboBox->setupItems(gComboBoxItems, busFunctionString);
		tableComboBox->setProperty(kHash, busFunction._hash);
		tableComboBox->setProperty(kChipIndex, chipIndex);
		connect(tableComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(onBusFunctionChanged(QString)));
		chipTable->setCellWidget(kDRow, kFunctionColumn, tableComboBox);

		updateBusState(chipIndex, busFunction._hash, busFunction._busFunction != eBusFunctionVCP);
	}
}

int FTDIEditorView::getPinRowOffset(const Bus& bus)
{
	int result{-1};

	switch (bus.toLatin1())
	{
	case 'A':
		result = kARow;
		break;
	case 'B':
		result = kBRow;
		break;
	case 'C':
		result = kCRow;
		break;
	case 'D':
		result = kDRow;
		break;
	default:
		break;
	}

	return result;
}

void FTDIEditorView::setupPinRows(ChipIndex chipIndex, const Bus& bus)
{
	auto chipTable = selectChipTable(chipIndex);
	if (chipTable != Q_NULLPTR)
	{
		int row = getPinRowOffset(bus);
		if (row != -1)
		{
			TableCheckBox* tableCheckBox{Q_NULLPTR};
			TableComboBox* tableComboBox{Q_NULLPTR};
			QTableWidgetItem* twi{Q_NULLPTR};

			for (PinID pinId : range(kMaxPinIndex + 1))
			{
				row++;

				FTDIPinData pinData =_ftdiPlatformConfiguration->getPinData(chipIndex, bus, pinId);

				if (pinData._hash != 0)
				{
					HashType hash{static_cast<HashType>(pinData._hash)};

					twi = new QTableWidgetItem();
					twi->setData(kHashRole, hash);
					twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					twi->setText(QString::number(pinData._chipPin));
					twi->setFlags(twi->flags() & ~Qt::ItemIsEditable);
					chipTable->setItem(row, kPinColumn, twi);

					twi = new QTableWidgetItem();
					twi->setData(kHashRole, hash);
					twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					twi->setText(getSetPinIndex(chipIndex, bus, pinId));
					twi->setFlags(twi->flags() & ~Qt::ItemIsEditable);
					chipTable->setItem(row, kSetPinColumn, twi);

					tableCheckBox = new TableCheckBox(Q_NULLPTR);
					tableCheckBox->setProperty(kHash, hash);
					tableCheckBox->setCheckState(pinData._enabled);
					connect(tableCheckBox, SIGNAL(checkStateChanged(bool)), this, SLOT(onEnableCheckChanged(bool)));
					chipTable->setCellWidget(row, kEnabledColumn, tableCheckBox);

					tableCheckBox = new TableCheckBox(Q_NULLPTR);
					tableCheckBox->setProperty(kHash, hash);
					tableCheckBox->setCheckState(pinData._input);
					connect(tableCheckBox, SIGNAL(checkStateChanged(bool)), this, SLOT(onInputCheckChanged(bool)));
					chipTable->setCellWidget(row, kInputColumn, tableCheckBox);

					tableCheckBox = new TableCheckBox(Q_NULLPTR);
					tableCheckBox->setProperty(kHash, hash);
					tableCheckBox->setCheckState(pinData._initialValue);
					connect(tableCheckBox, SIGNAL(checkStateChanged(bool)), this, SLOT(onInitialPinValueChanged(bool)));
					chipTable->setCellWidget(row, kInitialPinValueColumn, tableCheckBox);

					twi = new QTableWidgetItem();
					twi->setData(kHashRole, hash);
					twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					twi->setText(QString::number(pinData._initializationPriority));
					chipTable->setItem(row, kInitializationPriorityColumn, twi);

					tableCheckBox = new TableCheckBox(Q_NULLPTR);
					tableCheckBox->setCheckState(pinData._inverted);
					tableCheckBox->setProperty(kHash, hash);
					connect(tableCheckBox, SIGNAL(checkStateChanged(bool)), this, SLOT(onInvertCheckChanged(bool)));
					chipTable->setCellWidget(row, kInvertColumn, tableCheckBox);

					twi = new QTableWidgetItem();
					twi->setData(kHashRole, hash);
					twi->setText(pinData._pinLabel.isEmpty() ? kDefaultPinLabel : pinData._pinLabel);
					twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					chipTable->setItem(row, kPinLabelColumn, twi);

					twi = new QTableWidgetItem();
					twi->setData(kHashRole, hash);
					twi->setText(pinData._pinTooltip.isEmpty() ? kDefaultPinTooltip : pinData._pinTooltip);
					twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					chipTable->setItem(row, kPinTooltipColumn, twi);

					twi = new QTableWidgetItem();
					twi->setData(kHashRole, hash);
					twi->setText(pinData._pinCommand.isEmpty() ? kDefaultPinCommand : pinData._pinCommand);
					twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					chipTable->setItem(row, kPinCommandColumn, twi);

					tableComboBox = new TableComboBox(Q_NULLPTR);
					tableComboBox->setProperty(kHash, hash);
					tableComboBox->setupItems(CommandGroup::toStringList());
					tableComboBox->setCurrentText(CommandGroup::toString(pinData._commandGroup));
					connect(tableComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(onGroupChanged(QString)));
					chipTable->setCellWidget(row, kCommandGroupColumn, tableComboBox);

					tableComboBox = new TableComboBox(Q_NULLPTR);
					tableComboBox->setProperty(kHash, hash);
					tableComboBox->setupItems(Tab::toStringList(_configurableTabs));
					tableComboBox->setCurrentText(pinData._tabName);
					connect(tableComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(onTabsChanged(QString)));
					chipTable->setCellWidget(row, kTabsColumn, tableComboBox);

					twi = new QTableWidgetItem();
					twi->setData(kHashRole, hash);
					twi->setText(QString::number(pinData._cellLocation.x()) + "," + QString::number(pinData._cellLocation.y()));
					twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					chipTable->setItem(row, kCellLocationColumn, twi);
				}
			}
		}
	}
}

void FTDIEditorView::updateBusState(ChipIndex chipIndex, HashType actualBusHash, bool newState)
{
	// TODO: Request for an unhash(HashType) that can provide the chipIndex and the busName value
	// so that subsequent pins can be disabled without looping

	QTableWidget* chipTable{Q_NULLPTR};

	chipTable = selectChipTable(chipIndex);

	if (chipTable != Q_NULLPTR)
	{
		for (char busId = 'A'; busId < 'E'; busId++ )
		{
			HashType hash{0};
			hash = FTDIBusData::makeFTDIHash(chipIndex, Bus(busId));

			if (hash != 0 && actualBusHash == hash)
			{
				for(int pinId : range(kMaxPinIndex + 1))
				{
					updatePinsState(chipTable, chipIndex, busId, pinId, newState);
				}

				break;
			}
		}
	}
}

void FTDIEditorView::updatePinsState(QTableWidget* chipTable, ChipIndex chipIndex, Bus busId, PinID pinId, bool newState)
{
	if (chipTable != Q_NULLPTR)
	{
		for (auto row : range(chipTable->rowCount()))
		{
			row++;

			HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busId, pinId);

			QTableWidgetItem* twi{Q_NULLPTR};
			TableCheckBox* checkBox{Q_NULLPTR};
			TableComboBox* comboBox{Q_NULLPTR};

			twi = chipTable->item(row, kSetPinColumn);
			if (twi != Q_NULLPTR && twi->data(kHashRole).toULongLong() == hash)
				setTableWidgetItemEnabled(twi, newState);

			twi = chipTable->item(row, kPinColumn);
			if (twi != Q_NULLPTR && twi->data(kHashRole).toULongLong() == hash)
				setTableWidgetItemEnabled(twi, newState);

			checkBox = qobject_cast<TableCheckBox*>(chipTable->cellWidget(row, kEnabledColumn));
			if (checkBox)
			{
				if (checkBox->property(kHash).toULongLong() == hash)
				{
					checkBox->setEnabled(newState);
				}
			}

			checkBox = qobject_cast<TableCheckBox*>(chipTable->cellWidget(row, kInputColumn));
			if (checkBox)
			{
				if (checkBox->property(kHash).toULongLong() == hash)
				{
					checkBox->setEnabled(newState);
				}
			}

			checkBox = qobject_cast<TableCheckBox*>(chipTable->cellWidget(row, kInitialPinValueColumn));
			if (checkBox)
			{
				if (checkBox->property(kHash).toULongLong() == hash)
				{
					checkBox->setEnabled(newState);
				}
			}

			twi = chipTable->item(row, kInitializationPriorityColumn);
			if (twi != Q_NULLPTR && twi->data(kHashRole).toULongLong() == hash)
				setTableWidgetItemEnabled(twi, newState);

			checkBox = qobject_cast<TableCheckBox*>(chipTable->cellWidget(row, kInvertColumn));
			if (checkBox)
			{
				if (checkBox->property(kHash).toULongLong() == hash)
				{
					checkBox->setEnabled(newState);
				}
			}

			twi = chipTable->item(row, kPinLabelColumn);

			if (twi != Q_NULLPTR && twi->data(kHashRole).toULongLong() == hash)
				setTableWidgetItemEnabled(twi, newState);

			twi = chipTable->item(row, kPinTooltipColumn);

			if (twi != Q_NULLPTR && twi->data(kHashRole).toULongLong() == hash)
				setTableWidgetItemEnabled(twi, newState);

			twi = chipTable->item(row, kPinCommandColumn);

			if (twi != Q_NULLPTR && twi->data(kHashRole).toULongLong() == hash)
				setTableWidgetItemEnabled(twi, newState);

			comboBox = qobject_cast<TableComboBox*>(chipTable->cellWidget(row, kCommandGroupColumn));
			if (comboBox)
			{
				if (comboBox->property(kHash).toULongLong() == hash)
				{
					comboBox->setEnabled(newState);
				}
			}

			comboBox = qobject_cast<TableComboBox*>(chipTable->cellWidget(row, kTabsColumn));
			if (comboBox)
			{
				if (comboBox->property(kHash).toULongLong() == hash)
				{
					comboBox->setEnabled(newState);
				}
			}

			twi = chipTable->item(row, kCellLocationColumn);

			if (twi != Q_NULLPTR && twi->data(kHashRole).toULongLong() == hash)
				setTableWidgetItemEnabled(twi, newState);
		}
	}
}

QString FTDIEditorView::getSetPinIndex
(
	const ChipIndex chipIndex, // 1 to 4
	const Bus &bus,
	PinID pinId
)
{
	int pinIndex{0};

	switch (bus.toLatin1())
	{
	case 'A': pinIndex = 0; break;
	case 'B': pinIndex = 8; break;
	case 'C': pinIndex = 16; break;
	case 'D': pinIndex = 24; break;
	}

	pinIndex += pinId;
	pinIndex *= (chipIndex + 1);

	return QString::number(pinIndex);
}

void FTDIEditorView::setTableWidgetItemEnabled
(
	QTableWidgetItem* twi,
	bool enable
)
{
	if (enable)
		twi->setFlags(twi->flags() | Qt::ItemIsEnabled);
	else
		twi->setFlags(twi->flags() & ~Qt::ItemIsEnabled);
}

QTableWidget *FTDIEditorView::selectChipTable(ChipIndex chipIndex)
{
	QTableWidget* chipTable{Q_NULLPTR};

	switch(chipIndex)
	{
	case 0: chipTable= _chip1Table; break;
	case 1: chipTable= _chip2Table; break;
	case 2: chipTable= _chip3Table; break;
	case 3: chipTable= _chip4Table; break;
	default: break;
	}

	return chipTable;
}

void FTDIEditorView::setupDelegates(ChipIndex chipIndex)
{
	auto chipTable = selectChipTable(chipIndex);

	if (chipTable != Q_NULLPTR)
	{
		chipTable->setItemDelegateForColumn(kPinLabelColumn, new CustomValidator(chipTable, eNoSemiColonValidator));
		chipTable->setItemDelegateForColumn(kPinTooltipColumn, new CustomValidator(chipTable, eNoSemiColonValidator));
		chipTable->setItemDelegateForColumn(kPinCommandColumn, new CustomValidator(chipTable, eNoSpaceValidator));
		chipTable->setItemDelegateForColumn(kCellLocationColumn, new CustomValidator(chipTable, eCellLocationValidator));

		connect(chipTable, &QTableWidget::itemChanged, this, &FTDIEditorView::onTableItemChanged);

		chipTable->setFocusPolicy(Qt::NoFocus);
		QHeaderView *headerView = chipTable->horizontalHeader();
		chipTable->resizeColumnToContents(kSetPinColumn);
		headerView->setSectionResizeMode(kSetPinColumn, QHeaderView::Fixed);
		chipTable->resizeColumnToContents(kPinColumn);
		headerView->setSectionResizeMode(kPinColumn, QHeaderView::Fixed);
		chipTable->setColumnWidth(kFunctionColumn, 135);
		headerView->setSectionResizeMode(kFunctionColumn, QHeaderView::Fixed);
		chipTable->resizeColumnToContents(kEnabledColumn);
		headerView->setSectionResizeMode(kEnabledColumn, QHeaderView::Fixed);
		chipTable->resizeColumnToContents(kInputColumn);
		headerView->setSectionResizeMode(kInputColumn, QHeaderView::Fixed);
		chipTable->resizeColumnToContents(kInitialPinValueColumn);
		headerView->setSectionResizeMode(kInitialPinValueColumn, QHeaderView::Fixed);
		chipTable->resizeColumnToContents(kInitializationPriorityColumn);
		headerView->setSectionResizeMode(kInitializationPriorityColumn, QHeaderView::Fixed);
		chipTable->resizeColumnToContents(kInvertColumn);
		headerView->setSectionResizeMode(kInvertColumn, QHeaderView::Fixed);
		chipTable->setColumnWidth(kPinLabelColumn, 160);
		headerView->setSectionResizeMode(kPinLabelColumn, QHeaderView::Interactive);
		chipTable->setColumnWidth(kPinTooltipColumn, 220);
		headerView->setSectionResizeMode(kPinTooltipColumn, QHeaderView::Stretch);
		chipTable->setColumnWidth(kPinCommandColumn, 160);
		headerView->setSectionResizeMode(kPinCommandColumn, QHeaderView::Interactive);
		chipTable->setColumnWidth(kCommandGroupColumn, 120);
		headerView->setSectionResizeMode(kCommandGroupColumn, QHeaderView::Fixed);
		chipTable->setColumnWidth(kTabsColumn, 120);
		headerView->setSectionResizeMode(kTabsColumn, QHeaderView::Fixed);
		chipTable->setColumnWidth(kCellLocationColumn, 100);
		headerView->setSectionResizeMode(kCellLocationColumn, QHeaderView::Fixed);
	}
}

void FTDIEditorView::onCustomContextMenuRequested(const QPoint& pos)
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
				clearRow(tableWidget);
			else if (result == resetAction)
				resetToDefault(tableWidget);
		}
	}
}

void FTDIEditorView::clearRow(QTableWidget* tableWidget)
{
	QTableWidgetItem* twi{Q_NULLPTR};
	TableCheckBox* checkBoxWidget{Q_NULLPTR};
	TableComboBox* comboBoxWidget{Q_NULLPTR};

	checkBoxWidget = static_cast<TableCheckBox*>(tableWidget->cellWidget(tableWidget->currentRow(), kEnabledColumn));
	if (checkBoxWidget)
		checkBoxWidget->setCheckState(false);

	checkBoxWidget = static_cast<TableCheckBox*>(tableWidget->cellWidget(tableWidget->currentRow(), kInputColumn));
	if (checkBoxWidget)
		checkBoxWidget->setCheckState(false);

	checkBoxWidget = static_cast<TableCheckBox*>(tableWidget->cellWidget(tableWidget->currentRow(), kInitialPinValueColumn));
	if (checkBoxWidget)
		checkBoxWidget->setCheckState(false);

	checkBoxWidget = static_cast<TableCheckBox*>(tableWidget->cellWidget(tableWidget->currentRow(), kInvertColumn));
	if (checkBoxWidget)
		checkBoxWidget->setCheckState(false);

	twi = tableWidget->item(tableWidget->currentRow(), kInitializationPriorityColumn);
	if (twi)
		twi->setText(kDefaultPinPriority);

	twi = tableWidget->item(tableWidget->currentRow(), kPinLabelColumn);
	if (twi)
		twi->setText(kDefaultPinLabel);

	twi = tableWidget->item(tableWidget->currentRow(), kPinTooltipColumn);
	if (twi)
		twi->setText(kDefaultPinTooltip);

	twi = tableWidget->item(tableWidget->currentRow(), kPinCommandColumn);
	if (twi)
		twi->setText(kDefaultPinCommand);

	comboBoxWidget = static_cast<TableComboBox*>(tableWidget->cellWidget(tableWidget->currentRow(), kCommandGroupColumn));
	if (comboBoxWidget)
		comboBoxWidget->setCurrentIndex(0);

	comboBoxWidget = static_cast<TableComboBox*>(tableWidget->cellWidget(tableWidget->currentRow(), kTabsColumn));
	if (comboBoxWidget)
		comboBoxWidget->setCurrentText(kDefaultTabString);

	twi = tableWidget->item(tableWidget->currentRow(), kCellLocationColumn);
	if (twi)
		twi->setText(kDefaultCellLocation);
}

void FTDIEditorView::resetToDefault(QTableWidget* tableWidget)
{
	QTableWidgetItem* twi{Q_NULLPTR};
	TableCheckBox* checkBoxWidget{Q_NULLPTR};
	TableComboBox* comboBoxWidget{Q_NULLPTR};

	_FTDIPlatformConfiguration ftdiConfig(_ftdiPlatformConfiguration->getChipCount());
	for (auto& pinData : ftdiConfig.getPins())
	{
		PinID pin{0};

		twi = tableWidget->item(tableWidget->currentRow(), kSetPinColumn);
		if (twi)
			pin = twi->text().toUInt();

		if (pin == pinData._pin)
		{
			checkBoxWidget = static_cast<TableCheckBox*>(tableWidget->cellWidget(tableWidget->currentRow(), kEnabledColumn));
			if (checkBoxWidget)
				checkBoxWidget->setCheckState(pinData._enabled);

			checkBoxWidget = static_cast<TableCheckBox*>(tableWidget->cellWidget(tableWidget->currentRow(), kInputColumn));
			if (checkBoxWidget)
				checkBoxWidget->setCheckState(false);

			checkBoxWidget = static_cast<TableCheckBox*>(tableWidget->cellWidget(tableWidget->currentRow(), kInitialPinValueColumn));
			if (checkBoxWidget)
				checkBoxWidget->setCheckState(pinData._initialValue);

			twi = tableWidget->item(tableWidget->currentRow(), kInitializationPriorityColumn);
			if (twi)
				twi->setText(QString::number(pinData._initializationPriority));

			checkBoxWidget = static_cast<TableCheckBox*>(tableWidget->cellWidget(tableWidget->currentRow(), kInvertColumn));
			if (checkBoxWidget)
				checkBoxWidget->setCheckState(pinData._inverted);

			twi = tableWidget->item(tableWidget->currentRow(), kPinLabelColumn);
			if (twi)
				twi->setText(pinData._pinLabel);

			twi = tableWidget->item(tableWidget->currentRow(), kPinTooltipColumn);
			if (twi)
				twi->setText(pinData._pinTooltip);

			twi = tableWidget->item(tableWidget->currentRow(), kPinCommandColumn);
			if (twi)
				twi->setText(pinData._pinCommand);

			comboBoxWidget = static_cast<TableComboBox*>(tableWidget->cellWidget(tableWidget->currentRow(), kCommandGroupColumn));
			if (comboBoxWidget)
				comboBoxWidget->setCurrentText(CommandGroup::toString(pinData._commandGroup));

			comboBoxWidget = static_cast<TableComboBox*>(tableWidget->cellWidget(tableWidget->currentRow(), kTabsColumn));
			if (comboBoxWidget)
				comboBoxWidget->setCurrentText(pinData._tabName);

			twi = tableWidget->item(tableWidget->currentRow(), kCellLocationColumn);
			if (twi)
			{
				QString cellLocationString = QString::number(pinData._cellLocation.x()) + "," + QString::number(pinData._cellLocation.y());
				twi->setText(cellLocationString);
			}
		}
	}
}
