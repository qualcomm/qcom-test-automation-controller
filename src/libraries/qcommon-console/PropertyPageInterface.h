#ifndef PROPERTY_PAGE_INTERFACE_H
#define PROPERTY_PAGE_INTERFACE_H
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
// Copyright 2020 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// QT 

#include <QtPlugin>
#include <QIcon>
#include <QString>
#include <QWidget>

class PropertyPageInterface :
	public QWidget
{
Q_OBJECT

public:
	PropertyPageInterface(QWidget* parent) :
		QWidget(parent)
	{
	}

	~PropertyPageInterface()
	{

	}
	
	QIcon Icon()
	{
		return _icon;
	}

	QString Name()
	{
		return _name;
	}

	virtual bool IsDirty() = 0;
	virtual void SaveSettings() = 0;
	virtual void RestoreSettings() = 0;

signals:
	void SettingsChanged(const QString& groupName);

protected:
	QIcon						_icon;
	QString						_name;
};

#define PropertyPageInterface_iid "com.qualcomm.simpson.PropertyPageInterface"

Q_DECLARE_INTERFACE(PropertyPageInterface, PropertyPageInterface_iid)

#endif // PROPERTY_PAGE_INTERFACE_H
