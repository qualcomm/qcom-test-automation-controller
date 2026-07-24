#ifndef PYTHONDEBUG_H
#define PYTHONDEBUG_H

// Copyright (c) 2022 Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
   Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#if defined(PYTHONDEBUG_LIBRARY)
#ifdef __linux__
    #define PYTHONDEBUG_EXPORT __attribute__((visibility("default")))
#else
    #define PYTHONDEBUG_EXPORT __declspec(dllexport)
#endif
#else
#ifdef __linux__
    #define PYTHONDEBUG_EXPORT __attribute__((visibility("default")))
#else
    #define PYTHONDEBUG_EXPORT __declspec(dllimport)
#endif
#endif

extern "C"
{
    PYTHONDEBUG_EXPORT bool IsPythonDebugging();
}

#endif // PYTHONDEBUG_H