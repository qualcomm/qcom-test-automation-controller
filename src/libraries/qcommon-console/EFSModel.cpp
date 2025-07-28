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
	$Id: //depot/QDL/QCommon/EFSModel.cpp#1 $
	$Header: //depot/QDL/QCommon/EFSModel.cpp#1 $
	$Date: 2016/11/10 $
	$DateTime: 2016/11/10 09:48:15 $
	$Change: 939820 $
	$File: //depot/QDL/QCommon/EFSModel.cpp $
	$Revision: #1 $
	%Author: msimpson %
*/

// QCommon
#include "EFSModel.h"
#include "EFSSubSystem.h"

EFSModel::EFSModel(QObject* parent)
	: QAbstractItemModel(parent)
{

}

EFSModel::~EFSModel()
{

}

void EFSModel::SetEFSSubSystem
(
	EFSSubSystem* eFSSubSystem
)
{
	_eFSSubSystem = eFSSubSystem;
}