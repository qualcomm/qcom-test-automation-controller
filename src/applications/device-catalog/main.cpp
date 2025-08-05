// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "DeviceCatalogApplication.h"

// QCommon
#include "QCommon.h"

int main(int argc, char *argv[])
{
	DeviceCatalogApplication a(argc, argv);

	InitializeQCommon();

	DeviceCatalogApplication::createCatalogWindow();

	return a.exec();
}
