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
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "ButtonEditor.h"
#include "CustomValidator.h"

// QCommonConsole
#include "PlatformConfigurationException.h"
#include "StringUtilities.h"

// QCommon
#include "CommandGroup.h"
#include "TableComboBox.h"

// QT
#include <QMessageBox>

const int kLabelColumn{0};
const int kTooltipColumn{1};
const int kCommandColumn{2};
const int kGroupColumn{3};
const int kTabsColumn{4};
const int kCellLocationColumn{5};

const QByteArray kHash(QByteArrayLiteral("hash"));
const QByteArray kRow(QByteArrayLiteral("row"));
const QString kDefaultLabel(QStringLiteral("<type a label name>"));
const QString kDefaultTooltip(QStringLiteral("<add a tooltip>"));
const QString kDefaultCommand(QStringLiteral("<type a command>"));
const QString kDefaultGroup(QStringLiteral("<Select Pin Group>"));
const QString kDefaultTab(QStringLiteral("General"));
const QString kDefaultCellLocation(QStringLiteral("-1,-1"));

const int kHashRole(Qt::UserRole + 1);


ButtonEditor::ButtonEditor
(
	QWidget* parent
) :
	QGroupBox(parent)
{
	setupUi(this);

	setupColumnProperties();

	_buttonsTable->setItemDelegateForColumn(kLabelColumn, new CustomValidator(_buttonsTable, eNoSemiColonValidator));
	_buttonsTable->setItemDelegateForColumn(kTooltipColumn, new CustomValidator(_buttonsTable, eNoSemiColonValidator));
	_buttonsTable->setItemDelegateForColumn(kCellLocationColumn, new CustomValidator(_buttonsTable, eCellLocationValidator));

	_buttonsTable->setFocusPolicy(Qt::NoFocus);
}

ButtonEditor::~ButtonEditor()
{
}

void ButtonEditor::setPlatformConfiguration
(
	PlatformConfiguration platformConfig
)
{
	_platformConfiguration = platformConfig;

	setupColumnProperties();

	read();
}

void ButtonEditor::resetPlatform()
{
	if (_platformConfiguration != Q_NULLPTR)
	{
		_buttonsTable->clearContents();

		read();
	}
}

void ButtonEditor::on__addButton_clicked()
{
	QTableWidgetItem* twi{Q_NULLPTR};
	TableComboBox* tableComboBox{Q_NULLPTR};
	QStringList configurableTabs;
	Button button;

	if (_platformConfiguration.isNull() == false)
	{
		Tabs tabs = _platformConfiguration->getTabs();

		int row = _buttonsTable->rowCount();

		if (row == 0)
			read();

		_buttonsTable->insertRow(row);

		twi = new QTableWidgetItem();
		twi->setText(kDefaultLabel);
		twi->setData(kHashRole, Button::makeHash(button));
		twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		_buttonsTable->setItem(row, kLabelColumn, twi);

		twi = new QTableWidgetItem();
		twi->setText(kDefaultTooltip);
		twi->setData(kHashRole, Button::makeHash(button));
		twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		_buttonsTable->setItem(row, kTooltipColumn, twi);

		twi = new QTableWidgetItem();
		twi->setText(kDefaultCommand);
		twi->setData(kHashRole, Button::makeHash(button));
		twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		_buttonsTable->setItem(row, kCommandColumn, twi);

		tableComboBox = new TableComboBox(Q_NULLPTR);
		tableComboBox->setupItems(CommandGroup::toStringList());
		tableComboBox->setProperty(kHash, Button::makeHash(button));
		tableComboBox->setProperty(kRow, row);
		tableComboBox->setCurrentText(kDefaultGroup);
		connect(tableComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(onGroupChanged(QString)));
		_buttonsTable->setCellWidget(row, kGroupColumn, tableComboBox);

		tableComboBox = new TableComboBox(Q_NULLPTR);
		tableComboBox->setupItems(Tab::toStringList(tabs));
		tableComboBox->setProperty(kHash, Button::makeHash(button));
		tableComboBox->setCurrentText(kDefaultTab);
		connect(tableComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(onTabsChanged(QString)));
		_buttonsTable->setCellWidget(row, kTabsColumn, tableComboBox);

		twi = new QTableWidgetItem();
		twi->setText(kDefaultCellLocation);
		twi->setData(kHashRole, Button::makeHash(button));
		twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		_buttonsTable->setItem(row, kCellLocationColumn, twi);

		updateEnablements();
	}
}

void ButtonEditor::on__removeButton_clicked()
{
	int row = _buttonsTable->rowCount();
	QItemSelectionModel* model = _buttonsTable->selectionModel();

	if (model->hasSelection())
	{
		if (row > 0)
		{
			for (auto& tableRow : model->selectedRows())
			{
				Buttons buttons = _platformConfiguration->getButtonsMap();
				for(auto& button : buttons)
				{
					if (button._label == _buttonsTable->item(tableRow.row(), kLabelColumn)->text())
					{
						_platformConfiguration->deleteButton(button._hash);
					}
				}

				_buttonsTable->removeRow(tableRow.row());
			}
		}
		else
			_removeButton->setEnabled(false);
	}
	else
	{
		_removeButton->setEnabled(false);
	}

	updateEnablements();
}

void ButtonEditor::on__buttonsTable_itemClicked(QTableWidgetItem *item)
{
	_removeButton->setEnabled(item != Q_NULLPTR);

	updateEnablements();
	emit buttonsTableUpdated();
}

void ButtonEditor::read()
{
	QTableWidgetItem* twi{Q_NULLPTR};
	TableComboBox* tableComboBox{Q_NULLPTR};
	QStringList configurableTabs;

	if (_platformConfiguration != Q_NULLPTR)
	{
		Tabs tabs = _platformConfiguration->getTabs();

		Buttons buttons = _platformConfiguration->getButtonsMap();
		_buttonsTable->setRowCount(buttons.count());

		int row{0};

		for (const auto& button : buttons)
		{
			twi = new QTableWidgetItem();
			twi->setText(button._label);
			twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			twi->setData(kHashRole, button._hash);
			_buttonsTable->setItem(row, kLabelColumn, twi);

			twi = new QTableWidgetItem();
			twi->setText(button._toolTip);
			twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			twi->setData(kHashRole, button._hash);
			_buttonsTable->setItem(row, kTooltipColumn, twi);

			twi = new QTableWidgetItem();
			twi->setText(button._command);
			twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			twi->setData(kHashRole, button._hash);
			_buttonsTable->setItem(row, kCommandColumn, twi);

			tableComboBox = new TableComboBox(Q_NULLPTR);
			tableComboBox->setupItems(CommandGroup::toStringList());
			tableComboBox->setCurrentText(CommandGroup::toString(button._commandGroup));
			tableComboBox->setProperty(kHash, button._hash);
			connect(tableComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(onGroupChanged(QString)));
			_buttonsTable->setCellWidget(row, kGroupColumn, tableComboBox);

			tableComboBox = new TableComboBox(Q_NULLPTR);
			tableComboBox->setupItems(Tab::toStringList(tabs));
			tableComboBox->setCurrentText(button._tab);
			tableComboBox->setProperty(kHash, button._hash);
			connect(tableComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(onTabsChanged(QString)));
			_buttonsTable->setCellWidget(row, kTabsColumn, tableComboBox);

			twi = new QTableWidgetItem();
			twi->setText(fromQPoint(button._cellLocation));
			twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			twi->setData(kHashRole, button._hash);
			_buttonsTable->setItem(row, kCellLocationColumn, twi);

			row++;
		}

		_buttonsTable->resizeRowsToContents();
		_addButton->setEnabled(true);

		connect(_buttonsTable, &QTableWidget::itemChanged, this, &ButtonEditor::onTableItemChanged);
	}
}

void ButtonEditor::updateEnablements()
{
	int row = _buttonsTable->rowCount();
	QItemSelectionModel* model = _buttonsTable->selectionModel();

	if (model->hasSelection())
	{
		if (row > 0)
			_removeButton->setEnabled(true);
		else
			_removeButton->setEnabled(false);
	}
	else
	{
		_removeButton->setEnabled(false);
	}
}

void ButtonEditor::setupColumnProperties()
{
	QHeaderView* headerView = _buttonsTable->horizontalHeader();
	if (headerView != Q_NULLPTR)
	{
		_buttonsTable->setColumnWidth(kLabelColumn, 160);
		headerView->setSectionResizeMode(kLabelColumn, QHeaderView::Interactive);

		_buttonsTable->setColumnWidth(kTooltipColumn, 220);
		headerView->setSectionResizeMode(kTooltipColumn, QHeaderView::Stretch);

		_buttonsTable->setColumnWidth(kCommandColumn, 160);
		headerView->setSectionResizeMode(kCommandColumn, QHeaderView::Interactive);

		_buttonsTable->setColumnWidth(kGroupColumn, 120);
		headerView->setSectionResizeMode(kGroupColumn, QHeaderView::Fixed);

		_buttonsTable->setColumnWidth(kTabsColumn, 120);
		headerView->setSectionResizeMode(kTabsColumn, QHeaderView::Fixed);

		_buttonsTable->setColumnWidth(kCellLocationColumn, 100);
		headerView->setSectionResizeMode(kCellLocationColumn, QHeaderView::Fixed);
	}
}

void ButtonEditor::onGroupChanged(const QString& newText)
{
	bool result{false};
	TableComboBox* obj = qobject_cast<TableComboBox*>(sender());
	QTableWidget* table = qobject_cast<QTableWidget*>(sender()->parent()->parent());

	if (obj != Q_NULLPTR && table != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();
		int row = obj->property(kRow).toULongLong();
		QString labelName = table->item(row, kLabelColumn)->text();
		TableComboBox* cb = qobject_cast<TableComboBox*>(table->cellWidget(row, kTabsColumn));

		QString tabName{"General"};
		if (cb != Q_NULLPTR)
		{
			tabName = cb->currentText();
		}

		try
		{
			result = _platformConfiguration->setButtonCommandGroup(hash, CommandGroup::fromString(newText));
			if (!result)
			{
				hash = _platformConfiguration->addButtonCommandGroup(labelName, tabName, CommandGroup::fromString(newText));
				obj->setProperty(kHash, hash);
			}
		}
		catch (const PlatformConfigurationException& e)
		{
			QMessageBox::critical(this, "TAC Configuration Editor - cannot set pin group", e.getMessage());
		}
	}
}

void ButtonEditor::onTabsChanged(const QString& newText)
{
	bool result{false};
	TableComboBox* obj = qobject_cast<TableComboBox*>(sender());
	QTableWidget* table = qobject_cast<QTableWidget*>(sender()->parent()->parent());

	if (obj != Q_NULLPTR && table != Q_NULLPTR)
	{
		HashType hash = obj->property(kHash).toULongLong();
		int row = obj->property(kRow).toULongLong();
		QString labelName = table->item(row, kLabelColumn)->text();

		try
		{
			result = _platformConfiguration->setButtonTab(hash, newText);
			if (!result)
			{
				hash = _platformConfiguration->addButtonTab(labelName, newText);
				obj->setProperty(kHash, hash);
			}
		}
		catch (const PlatformConfigurationException& e)
		{
			QMessageBox::critical(this, "TAC Configuration Editor - cannot set pin tab", e.getMessage());
		}
	}
}

void ButtonEditor::onTableItemChanged(QTableWidgetItem* twi)
{
	QTableWidget* table = qobject_cast<QTableWidget*>(sender());

	if (twi != Q_NULLPTR && table != Q_NULLPTR)
	{
		bool result{false};
		HashType hash = twi->data(kHashRole).toULongLong();
		QString labelName = table->item(table->row(twi), kLabelColumn)->text();
		TableComboBox* cb = qobject_cast<TableComboBox*>(table->cellWidget(table->row(twi), kTabsColumn));

		QString tabName{"General"};
		if (cb != Q_NULLPTR)
		{
			tabName = cb->currentText();
		}

		QString itemText = twi->text();

		switch(twi->column())
		{
		case kLabelColumn:
			result = _platformConfiguration->setButtonLabel(hash, itemText);
			if (!result)
			{
				hash = _platformConfiguration->addButtonLabel(itemText, tabName);
				twi->setData(kHashRole, hash);
			}
			break;
		case kTooltipColumn:
			result = _platformConfiguration->setButtonTooltip(hash, itemText);
			if (!result)
			{
				hash = _platformConfiguration->addButtonTooltip(labelName, tabName, itemText);
				twi->setData(kHashRole, hash);
			}
			break;
		case kCommandColumn:
			result = _platformConfiguration->setButtonCommand(hash, itemText);
			if (!result)
			{
				hash = _platformConfiguration->addButtonCommand(labelName, tabName, itemText);
				twi->setData(kHashRole, hash);
			}
			break;
		case kCellLocationColumn:
		{
			QStringList rowColString = itemText.split(",");
			if (rowColString.size() == 2)
			{
				int row = rowColString.at(0).trimmed().toInt();
				int col = rowColString.at(1).trimmed().toInt();

				if (row < 0 || col < 0)
				{
					twi->setText(kDefaultCellLocation);
					result = _platformConfiguration->setButtonCellLocation(hash, QPoint(-1,-1));
				}
				else
				{
					result = _platformConfiguration->setButtonCellLocation(hash, QPoint(row, col));
				}

				if (!result)
				{
					hash = _platformConfiguration->addButtonCellLocation(labelName, tabName, QPoint(row, col));
					twi->setData(kHashRole, hash);
				}
			}
			break;
		}
		default:
			break;
		}
	}
}
