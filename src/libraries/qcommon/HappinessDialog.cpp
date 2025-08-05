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
