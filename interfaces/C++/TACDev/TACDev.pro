# Confidential and Proprietary – Qualcomm Technologies, Inc.

# NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
# to: DocCtrlAgent@qualcomm.com.

# RESTRICTED USE AND DISCLOSURE:
# This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
# or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
# of Qualcomm Technologies, Inc.

# Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
# QUALCOMM Incorporated trademarks are used with permission.

# This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
# and international law is strictly prohibited.

# Qualcomm Technologies, Inc.
# 5775 Morehouse Drive
# San Diego, CA 92121 U.S.A.
# Copyright ©2022 Qualcomm Technologies, Inc.
# All rights reserved.
# Qualcomm Technologies Confidential and Proprietary

# Author: msimpson

include(../../../src/libraries/qcommon-console/Common.pri)

QT += core serialport network

TEMPLATE = lib
CONFIG += dll
DESTDIR = $$STATIC_LIBPATH

DEFINES += TACDEV_LIBRARY

INCLUDEPATH += $$QCOMMONCONSOLE

DEPENDPATH += $$QCOMMONCONSOLE

CONFIG(debug, debug|release) {
	TARGET = TACDevd
} else {
	TARGET = TACDev
}

DLLDESTDIR = $$BINPATH

win32 {
	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/qcommon-consoled.lib
	} else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/qcommon-console.lib
	}
}

linux {
	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon-consoled.a
	} else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon-console.a
	}
}

SOURCES += \
	TACDevCore.cpp \
	TACDev.cpp

HEADERS += \
	TACDevCore.h \
	TACDev.h

CONFIG(debug, debug|release) {
	LIBS += -L$$STATIC_LIBPATH -lQCommon-consoled
} else {
	LIBS += -L$$STATIC_LIBPATH -lQCommon-console
}

LIBS += -L$$STATIC_LIBPATH -lftd2xx

win32 {
	LIBS += -lSetupAPI -lWinusb -lOle32 -lAdvapi32 -lShell32 -lWtsapi32
}
