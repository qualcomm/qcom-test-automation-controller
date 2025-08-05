// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
