
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
	$Id: //depot/QDL/QCommon/LogInterface.cpp#1 $
	$Header: //depot/QDL/QCommon/LogInterface.cpp#1 $
	$Date: 2016/08/30 $
	$DateTime: 2016/08/30 12:45:17 $
	$Change: 902319 $
	$File: //depot/QDL/QCommon/LogInterface.cpp $
	$Revision: #1 $
	%Author: msimpson %
*/

#include "LogInterface.h"

// QCommon
#include "Hexify.h"

class _Category
{
public:
	_Category()
	{

	}

	_Category(const _Category& copyMe)
	{
		_categoryName = copyMe._categoryName;
		_debugLevels = copyMe._debugLevels;
	}

	QByteArray					_categoryName;
	_LogInterface::DebugLevels	_debugLevels;
};


_LogInterface::_LogInterface() :
	_nextCategoryID(1)
{

}

_LogInterface::~_LogInterface()
{
	_categories.clear();
	_activeCategories.clear();
}

quint32 _LogInterface::AddCategory
(
	const QByteArray& categoryName
)
{
	quint32 categoryID(-1);

	QMutexLocker lock(&_mutex);

	Categories::iterator category = _categories.begin();
	while (category != _categories.end())
	{
		if ((*category)->_categoryName == categoryName)
		{
			categoryID = category.key();
			break;
		}
	}
	if (categoryID == -1)
	{
		categoryID = _nextCategoryID++;
		Category category = Category(new _Category);
		category->_categoryName = categoryName;
		_categories.insert(categoryID, category);
	}

	return categoryID;
}

QStringList _LogInterface::CategoryNames(void)
{
	QStringList result;

	return result;
}

QByteArray _LogInterface::CategoryName(quint32 categoryID)
{
	QByteArray result;

	return result;
}

bool _LogInterface::GetDebugLevels
(
	quint32 categoryID, 
	bool& informative, 
	bool& warning, 
	bool& critical, 
	bool& fatal
)
{
	bool result(false);

	return result;
}

bool _LogInterface::SetDebugLevels
(
	quint32 categoryID, 
	bool& informative, 
	bool& warning, 
	bool& critical, 
	bool& fatal
)
{
	bool result(false);

	return result;
}

void _LogInterface::Log
(
	quint32 categoryID, 
	DebugLevel debugLevel, 
	const QString& message,
	bool newLine // = true
)
{
   Log(categoryID, debugLevel, message.toLatin1(), newLine);
}

void _LogInterface::Log
(
	quint32 categoryID, 
	DebugLevel debugLevel, 
	const QByteArray& message,
	bool newLine // = true
)
{
	QMutexLocker lock(&_mutex);

	if (_debugStream.device() != nullptr)
	{
	}
}

void _LogInterface::LogInHex
(
	quint32 categoryID, 
	DebugLevel debugLevel, 
	const QByteArray& message,
	bool newLine // = true
)
{
   Log(categoryID, debugLevel, Hexify(message), newLine);
}
