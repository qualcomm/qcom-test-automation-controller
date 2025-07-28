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
// Copyright 2018-2020 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
    Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "HappinessDialog.h"
#include "ui_HappinessDialog.h"
#include <iostream>
#include <QPointF>
#include<QMouseEvent>
#include <QPushButton>

HappinessDialog::HappinessDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    _ui(new Ui::HappinessDialogClass)
{
    if (_ui != Q_NULLPTR)
    {
        _ui->setupUi(this);
        _ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        setMouseTracking(true);
    }
}

HappinessDialog::~HappinessDialog()
{
    if (_ui != Q_NULLPTR)
    {
        delete _ui;
        _ui = Q_NULLPTR;
	}
}

HappinessRating HappinessDialog::getRating()
{
	if (_selectedLabel == _ui->_dissatisfied)
		return eDissatisfied;

	if (_selectedLabel == _ui->_unhappy)
		return eUnhappy;

	if (_selectedLabel == _ui->_satisfied)
		return eSatisfied;

	if (_selectedLabel == _ui->_happy)
		return eHappy;

	if (_selectedLabel == _ui->_love)
		return eLove;

	return eNotSet;
}

QString HappinessDialog::ratingToString(HappinessRating rating)
{
	switch (rating)
	{
	case eDissatisfied:
		return "Dissatisfied";

	case eUnhappy:
		return "Unhappy";

	case eSatisfied:
		return "Satisfied";

	case eHappy:
		return "Happy";

	case eLove:
		return "Love";

	default:
		return "Unrated";
	}
}

void HappinessDialog::mousePressEvent(QMouseEvent* event)
{
    QPoint point = _ui->_ratingFrame->mapFromParent(event->pos());
    QRect childRect;

    // lambda
    auto selectLabel = [=, this](QLabel* selectedLabel)
    {
        if (selectedLabel == _selectedLabel)
            return;

        _ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(selectedLabel != Q_NULLPTR);

        if (selectedLabel == Q_NULLPTR)
        {
            _selectedLabel->resize(_lastSize);
            _selectedLabel->move(_lastPosition);
            _selectedLabel->setPixmap(_selectedLabel->pixmap().scaledToWidth(48));
            _selectedLabel = selectedLabel;

            _ui->_dissatisfied->setEnabled(_selectedLabel == _ui->_dissatisfied);
            _ui->_unhappy->setEnabled(_selectedLabel == _ui->_unhappy);
            _ui->_satisfied->setEnabled(_selectedLabel == _ui->_satisfied);
            _ui->_happy->setEnabled(_selectedLabel == _ui->_happy);
            _ui->_love->setEnabled(_selectedLabel == _ui->_love);

            return;
        }

        if (_selectedLabel != Q_NULLPTR)
        {
            _selectedLabel->resize(_lastSize);
            _selectedLabel->move(_lastPosition);
            _selectedLabel->setPixmap(_selectedLabel->pixmap().scaledToWidth(48));
        }

        _selectedLabel = selectedLabel;
        _lastPosition = _selectedLabel->pos();
        _lastSize = _selectedLabel->size();
        _selectedLabel->setPixmap(_selectedLabel->pixmap().scaledToWidth(64));

        _ui->_dissatisfied->setEnabled(_selectedLabel == _ui->_dissatisfied);
        _ui->_unhappy->setEnabled(_selectedLabel == _ui->_unhappy);
        _ui->_satisfied->setEnabled(_selectedLabel == _ui->_satisfied);
        _ui->_happy->setEnabled(_selectedLabel == _ui->_happy);
        _ui->_love->setEnabled(_selectedLabel == _ui->_love);
    };

    childRect = QRect(_ui->_dissatisfied->pos(),_ui->_dissatisfied->rect().size());
    if (childRect.contains(point))
        selectLabel(_ui->_dissatisfied);
    else
    {
        childRect = QRect(_ui->_unhappy->pos(),_ui->_unhappy->rect().size());
        if (childRect.contains(point))
            selectLabel(_ui->_unhappy);
        else
        {
            childRect = QRect(_ui->_satisfied->pos(),_ui->_satisfied->rect().size());
            if (childRect.contains(point))
                selectLabel(_ui->_satisfied);
            else
            {
                childRect = QRect(_ui->_happy->pos(),_ui->_happy->rect().size());
                if (childRect.contains(point))
                    selectLabel(_ui->_happy);
                else
                {
                    childRect = QRect(_ui->_love->pos(),_ui->_love->rect().size());
                    if (childRect.contains(point))
                        selectLabel(_ui->_love);
                    else
                        selectLabel(Q_NULLPTR);
                }
            }
        }
    }
}

void HappinessDialog::mouseMoveEvent(QMouseEvent* event)
{
	QPoint point = _ui->_ratingFrame->mapFromParent(event->pos());
    QRect childRect;

    // lambda
    auto setActiveLabel = [=, this](QLabel* activeLabel)
    {
        if (activeLabel == _activeLabel)
            return;
        _activeLabel = activeLabel;

        _ui->_dissatisfied->setEnabled(activeLabel == _ui->_dissatisfied || _selectedLabel == _ui->_dissatisfied);
        _ui->_unhappy->setEnabled(activeLabel == _ui->_unhappy || _selectedLabel == _ui->_unhappy);
        _ui->_satisfied->setEnabled(activeLabel == _ui->_satisfied || _selectedLabel == _ui->_satisfied);
        _ui->_happy->setEnabled(activeLabel == _ui->_happy ||_selectedLabel == _ui->_happy);
        _ui->_love->setEnabled(activeLabel == _ui->_love || _selectedLabel == _ui->_love);
    };

	childRect = QRect(_ui->_dissatisfied->pos(),_ui->_dissatisfied->rect().size());
    if (childRect.contains(point))
        setActiveLabel(_ui->_dissatisfied);
    else
    {
        childRect = QRect(_ui->_unhappy->pos(),_ui->_unhappy->rect().size());
        if (childRect.contains(point))
            setActiveLabel(_ui->_unhappy);
        else
        {
            childRect = QRect(_ui->_satisfied->pos(),_ui->_satisfied->rect().size());
            if (childRect.contains(point))
                setActiveLabel(_ui->_satisfied);
            else
            {
                childRect = QRect(_ui->_happy->pos(),_ui->_happy->rect().size());
                if (childRect.contains(point))
                    setActiveLabel(_ui->_happy);
                else
                {
                    childRect = QRect(_ui->_love->pos(),_ui->_love->rect().size());
                    if (childRect.contains(point))
                        setActiveLabel(_ui->_love);
                    else
                        setActiveLabel(Q_NULLPTR);
                }
            }
        }
    }
}
