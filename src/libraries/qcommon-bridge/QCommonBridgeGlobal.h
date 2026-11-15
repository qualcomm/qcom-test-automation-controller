#ifndef QCOMMONBRIDGEGLOBAL_H
#define QCOMMONBRIDGEGLOBAL_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include <QtCore/qglobal.h>

#ifndef QTAC_STATIC
	#if defined(QCOMMONBRIDGE_LIBRARY)
		#define QCOMMONBRIDGE_EXPORT Q_DECL_EXPORT
	#else
		#define QCOMMONBRIDGE_EXPORT Q_DECL_IMPORT
	#endif
#else
	#define QCOMMONBRIDGE_EXPORT
#endif

#endif // QCOMMONBRIDGEGLOBAL_H
