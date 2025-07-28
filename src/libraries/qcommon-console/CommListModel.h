#ifndef COMLISTMODEL_H
#define COMLISTMODEL_H

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
// Copyright © 2013-2016 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	$Id: //depot/QDL/QCommon/CommListModel.h#3 $
	$Header: //depot/QDL/QCommon/CommListModel.h#3 $
	$Date: 2016/07/06 $
	$DateTime: 2016/07/06 12:52:24 $
	$Change: 889602 $
	$File: //depot/QDL/QCommon/CommListModel.h $
	$Revision: #3 $
	%Author: msimpson $
*/

//Qt
#include <QtGui/QStandardItemModel>

class  CommListModel :
	public QStandardItemModel
{
Q_OBJECT

public:
	CommListModel(QObject* parent = NULL);

	void Refresh();

signals:
	void ModelUpdated();
};

#endif // COMLISTMODEL_H

