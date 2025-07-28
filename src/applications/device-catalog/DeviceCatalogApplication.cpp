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
// Copyright 2024 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/


#include "DeviceCatalogApplication.h"

DeviceCatalogApplication::DeviceCatalogApplication(int &argc, char **argv):
	AlpacaApplication (argc, argv, kAppName, kAppVersion)
{
	_preferences.setAppName(kAppName, kAppVersion);
	initialize(&_preferences);
}

DeviceCatalogApplication::~DeviceCatalogApplication()
{

}

DeviceCatalog *DeviceCatalogApplication::createCatalogWindow()
{
	DeviceCatalog* result;

	result = new DeviceCatalog;
	result->show();

	return result;
}

DeviceCatalogApplication *DeviceCatalogApplication::appInstance()
{
	return qobject_cast<DeviceCatalogApplication*>(QCoreApplication::instance());
}

void DeviceCatalogApplication::shutDown()
{
	quit();
}
