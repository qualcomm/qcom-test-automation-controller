// Copyright (c) 2022 Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
   Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "PythonDebug.h"

// QCommon
#include "ProcessUtilities.h"

bool IsPythonDebugging()
{
    bool result = isProcessRunning("debugpython");
    return result;
}