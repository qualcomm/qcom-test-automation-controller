#ifndef STYLEDLABEL_H
#define STYLEDLABEL_H
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
// Copyright 2020-2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonGlobal.h"

#include <QColor>
#include <QFont>
#include <QWidget>

class QCOMMON_EXPORT StyledLabel :
	public QWidget
{
	Q_OBJECT
public:
	StyledLabel(QWidget* parent = Q_NULLPTR);

	void setColor(const QColor& foregroundColor);
	void setFont(const QFont& font);
	void setText(const QString& text);
	void setRotation(qreal rotation);

protected:
	virtual void paintEvent(QPaintEvent* paintEvt);

private:
	QString						_text{"foo"};
	QColor						_foregroundColor{Qt::black};
	QFont						_font{QFont("Arial", 10)};
	qreal						_rotation{0.0};
};

#endif // STYLEDLABEL_H
