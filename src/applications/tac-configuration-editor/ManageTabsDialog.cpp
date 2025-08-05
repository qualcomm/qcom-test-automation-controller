// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "ManageTabsDialog.h"
#include "Range.h"
#include "TabEditorDialog.h"

// QCommon
#include "TableCheckBox.h"

// QT
#include <QMessageBox>

const int kMaxTabCount{7};

const int kTabNameColumn{0};
const int kVisibleColumn{1};
const int kUserDefinedColumn{2};
const int kConfigureableColumn{3};
const int kMoveableColumn{4};

const int kHashRole(Qt::UserRole);

const QByteArray kHash(QByteArrayLiteral("hash"));

const QString kDefaultTabString(QStringLiteral("<type the tab name>"));

ManageTabsDialog::ManageTabsDialog
(
	PlatformConfiguration platformConfiguration,
	QWidget* parent
) :
	QDialog(parent),
	_platformConfiguration(platformConfiguration)
{
	setupUi(this);

	connect(_tabsTable, &QTableWidget::currentCellChanged, this, &ManageTabsDialog::onCurrentRowChanged);

	Tabs tabs = _platformConfiguration->getTabs();
	for (const auto& tab: tabs)
	{
		Tab managedTab;

		managedTab = tab;
		managedTab._hash = Tab::makeHash(tab);

		_tabs.append(managedTab);
	}

	setupTabs();
}

ManageTabsDialog::~ManageTabsDialog()
{
}

Tabs ManageTabsDialog::getDeletedTabs()
{
	return _deletedTabs;
}

Tabs ManageTabsDialog::getTabs()
{
	Tabs result;

	for (auto& tab: _platformConfiguration->getTabs())
	{
		result.push_back(tab);
	}

	return result;
}

void ManageTabsDialog::on__addTabButton_clicked()
{
	int rowCount = _tabsTable->rowCount();
	if (rowCount < kMaxTabCount)
	{
		Tab tab;

		tab._visible = true;
		tab._added = true;
		tab._userTab = true;
		tab._configurable = true;
		tab._moveable = true;
		tab._ordinal = rowCount + 1;

		TabEditorDialog tabEditor(tab, this);

		if (tabEditor.exec() == QDialog::Accepted)
		{
			// check to see if the tab name is a duplicate
			// if not...
			tab = tabEditor.getTab();
			tab._name = tab._newText;
			tab._newText.clear();

			tab._hash = Tab::makeHash(tab);
			_tabs.append(tab);

			_platformConfiguration->updateTabs(_tabs);

			setupTabs();
		}

		updateEnablements();
	}
}

void ManageTabsDialog::on__removeTabButton_clicked()
{
	int row = _tabsTable->currentRow();
	if (row >= 0)
	{
		QTableWidgetItem* listItem = _tabsTable->item(row, 0);
		HashType hash = listItem->data(kHashRole).toULongLong();
		Tab tab = Tab::find(hash, _tabs);
		if (tab._hash != 0)
		{
			_deletedTabs.push_back(tab);

			_tabsTable->removeRow(row);
			_tabsTable->setCurrentCell(row - 1, 0, QItemSelectionModel::Rows);
		}
	}

	updateEnablements();
}

void ManageTabsDialog::onCurrentRowChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
	Q_UNUSED(currentRow);
	Q_UNUSED(currentColumn);
	Q_UNUSED(previousRow);
	Q_UNUSED(previousColumn);

	updateEnablements();
}

void ManageTabsDialog::onTabVisibleChecked(bool checkedState)
{
	TableCheckBox* tableCheckBox = qobject_cast<TableCheckBox*>(sender());
	if (tableCheckBox != Q_NULLPTR)
	{
		HashType hash = tableCheckBox->property(kHash).toULongLong();
		if (hash != 0)
		{
			Tab::setTabVisible(hash, checkedState, _tabs);
		}
	}
}

Tab ManageTabsDialog::getTab(QTableWidgetItem *twi)
{
	Tab result;

	if (twi != Q_NULLPTR)
	{
		HashType hash = twi->data(kHashRole).toULongLong();
		if (hash)
		{
			result = Tab::find(hash, _tabs);
		}
	}

	return result;
}

void ManageTabsDialog::updateEnablements()
{
	int listCount{_tabsTable->rowCount()};

	int row = _tabsTable->currentRow();
	if (row >= 0)
	{
		Tab tab = getTab(_tabsTable->item(row, 0));
		if (tab._hash != 0)
		{
			if (row == 0)
				_moveTabUpButton->setEnabled(false);
			else
			{
				if (testTabForMove(row - 1))
					_moveTabUpButton->setEnabled(tab._moveable);
				else
					_moveTabUpButton->setEnabled(false);
			}

			if (row == listCount)
				_moveTabDownButton->setEnabled(false);
			else
				if (testTabForMove(row + 1))
					_moveTabDownButton->setEnabled(tab._moveable);
				else
					_moveTabDownButton->setEnabled(false);

			_removeTabButton->setEnabled(tab._userTab);
			_renameTabButton->setEnabled(tab._userTab);
		}
	}
	else
	{
		_moveTabUpButton->setEnabled(false);
		_moveTabDownButton->setEnabled(false);
		_removeTabButton->setEnabled(false);
		_renameTabButton->setEnabled(false);
	}	

	_addTabButton->setEnabled(listCount < kMaxTabCount);
}

bool ManageTabsDialog::testTabForMove(int tabIndex)
{
	bool result{false};

	Tab tab = getTab(_tabsTable->item(tabIndex, 0));
	if (tab._hash != 0)
	{
		result = tab._moveable;
	}

	return result;
}

void ManageTabsDialog::setupTabs()
{
	QString selectedText;
	int ordinal{1};
	int row{0};

	QList<QTableWidgetItem*> selectedItems = _tabsTable->selectedItems();
	if (selectedItems.count() == 1)
	{
		Tab tab = getTab(selectedItems.at(0));
		if (tab._hash != 0)
		{
			selectedText = tab._newText.isEmpty() ? tab._name : tab._newText;
		}
	}

	_tabsTable->clearContents();
	_tabsTable->setRowCount(0);

	_tabsTable->setRowCount(_tabs.count());

	for (auto& tab : _tabs)
	{
		if (tab._deleted == false)
		{
			tab._ordinal = ordinal++;

			QTableWidgetItem* lwi = new QTableWidgetItem;

			lwi->setText(tab._newText.isEmpty() ? tab._name : tab._newText);
			lwi->setData(kHashRole, tab._hash);
            lwi->setFlags(lwi->flags() & ~Qt::ItemIsEditable);
			_tabsTable->setItem(row, kTabNameColumn, lwi);

			TableCheckBox* tableCheckBox;

			tableCheckBox = new TableCheckBox(Q_NULLPTR);
			tableCheckBox->setCheckState(tab._visible);
			tableCheckBox->setProperty(kHash, tab._hash);
			connect(tableCheckBox, SIGNAL(checkStateChanged(bool)), this, SLOT(onTabVisibleChecked(bool)));
			_tabsTable->setCellWidget(row, kVisibleColumn, tableCheckBox);

			tableCheckBox = new TableCheckBox(Q_NULLPTR);
			tableCheckBox->setEnabled(false);
			tableCheckBox->setCheckState(tab._userTab);
			tableCheckBox->setProperty(kHash, tab._hash);
			_tabsTable->setCellWidget(row, kUserDefinedColumn, tableCheckBox);

			tableCheckBox = new TableCheckBox(Q_NULLPTR);
			tableCheckBox->setEnabled(false);
			tableCheckBox->setCheckState(tab._configurable);
			tableCheckBox->setProperty(kHash, tab._hash);
			_tabsTable->setCellWidget(row, kConfigureableColumn, tableCheckBox);

			tableCheckBox = new TableCheckBox(Q_NULLPTR);
			tableCheckBox->setEnabled(false);
			tableCheckBox->setCheckState(tab._moveable);
			tableCheckBox->setProperty(kHash, tab._hash);
			_tabsTable->setCellWidget(row, kMoveableColumn, tableCheckBox);

			row++;
		}
	}

	_tabsTable->setColumnWidth(kTabNameColumn, 120);
	_tabsTable->resizeColumnToContents(kVisibleColumn);
	_tabsTable->resizeColumnToContents(kUserDefinedColumn);
	_tabsTable->resizeColumnToContents(kConfigureableColumn);
	_tabsTable->resizeColumnToContents(kMoveableColumn);

	if (selectedText.isEmpty() == false)
	{
		for (auto tabIndex: range(_tabs.count()))
		{
			QTableWidgetItem* twi = _tabsTable->item(tabIndex, kTabNameColumn);
			if (twi != Q_NULLPTR)
			{
				QString itemText = _tabsTable->item(tabIndex, kTabNameColumn)->text();
				if (itemText == selectedText)
				{
					_tabsTable->selectRow(tabIndex);
					break;
				}
			}
		}
	}
}

void ManageTabsDialog::on__renameTabButton_clicked()
{
	int row = _tabsTable->currentRow();

	if (row >= 0)
	{
		Tab tab = getTab(_tabsTable->item(row, 0));
		if (tab._hash != 0)
		{
			TabEditorDialog tabEditor(tab, this);

			if (tabEditor.exec() == QDialog::Accepted)
			{
				QTableWidgetItem* tableItem = _tabsTable->item(row, 0);
				tab = tabEditor.getTab();
				if (tab._hash != 0)
				{
					if (tab._name != tab._newText)
					{
						tableItem->setText(tab._newText);

						Tab::updateTabName(tab._hash, tab._newText, _tabs);
					}
				}

                _platformConfiguration->updateTabs(_tabs);
			}
		}
	}
}

void ManageTabsDialog::on__moveTabUpButton_clicked()
{
	int row = _tabsTable->currentRow();

	if (row >= 0)
	{
		_tabs.swapItemsAt(row, row - 1);

		setupTabs();
	}
}

void ManageTabsDialog::on__moveTabDownButton_clicked()
{
	int row = _tabsTable->currentRow();

	if (row >= 0)
	{
		_tabs.swapItemsAt(row, row + 1);

		setupTabs();
	}
}

