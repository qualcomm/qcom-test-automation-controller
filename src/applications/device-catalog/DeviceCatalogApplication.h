#ifndef DEVICECATALOGAPPLICATION_H
#define DEVICECATALOGAPPLICATION_H

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
// Copyright 20245 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
    Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/


#include "AlpacaApplication.h"
#include "DeviceCatalog.h"
#include "version.h"

const QByteArray kAppName{QByteArrayLiteral("Device Catalog")};
const QByteArray kAppVersion{QByteArrayLiteral(DEVICE_CATALOG_VERSION)};


class DeviceCatalogApplication : public AlpacaApplication
{
    Q_OBJECT

public:
    DeviceCatalogApplication(int &argc, char **argv);
    ~DeviceCatalogApplication();

    static DeviceCatalog* createCatalogWindow();

    static DeviceCatalogApplication* appInstance();

    PreferencesBase* preferences()
    {
        return &_preferences;
    }

protected:
    void shutDown();

    PreferencesBase				_preferences;
};

#endif // DEVICECATALOGAPPLICATION_H
