#ifndef PROPERTY_PAGE_INTERFACE_H
#define PROPERTY_PAGE_INTERFACE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
