// Confidential and Proprietary – Qualcomm Technologies, Inc.

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
// Copyright © 2013-2015 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	$Id: //depot/QDL/QCommon/CommListModel.cpp#3 $
	$Header: //depot/QDL/QCommon/CommListModel.cpp#3 $
	$Date: 2016/07/06 $
	$DateTime: 2016/07/06 12:52:24 $
	$Change: 889602 $
	$File: //depot/QDL/QCommon/CommListModel.cpp $
	$Revision: #3 $
	%Author: msimpson %
*/

#include "CommListModel.h"

// Qt
#include <QtCore/QSize>
#include <QtGui/QColor>

CommListModel::CommListModel
(
	QObject* parent // = NULL
) :
	QStandardItemModel(parent)
{
	QStringList headers;

	headers << tr("Port Number") << tr("Port Description") ;

	setHorizontalHeaderLabels(headers);

//	if (_deviceServer.isNull() == false)
//		Refresh();
}

void CommListModel::Refresh()
{
	removeRows(0, rowCount());

/*	SerialPorts serialPorts;

	serialPorts = _deviceServer->GetSerialPorts();

	if (serialPorts.empty() == false)
	{
		SerialPortIter serialPort = serialPorts.begin();
		while (serialPort != serialPorts.end())
		{	
			QStandardItem* standardItem;
		
			QList<QStandardItem*> standardItems;

			standardItem = new QStandardItem(serialPort->_portLocation);
			standardItem->setData(serialPort->_portLocation);
			standardItems.push_back(standardItem);

			standardItem = new QStandardItem(serialPort->_description);
			standardItems.push_back(standardItem);

			appendRow(standardItems);

			emit ModelUpdated();

			serialPort++;
		}
	}
*/
}
