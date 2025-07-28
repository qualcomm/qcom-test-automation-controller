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
# Copyright ©2023 Qualcomm Technologies, Inc.
# All rights reserved.
# Qualcomm Technologies Confidential and Proprietary

# Author: msimpson

include(../../libraries/qcommon-console/Common.pri)

QT += core serialport network xml

TARGET = UpdateDeviceList
TEMPLATE = app

CONFIG += console
CONFIG -= app_bundle

DESTDIR = $$BINPATH

INCLUDEPATH += $$QCOMMONCONSOLE
DEPENDPATH += $$QCOMMONCONSOLE

win32 {
	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/qcommon-consoled.lib
	}
	else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/qcommon-console.lib
	}
}

linux {
	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon-consoled.a
	}
	else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon-console.a
	}
}

HEADERS += \
	FTDITemplateCompiler.h \
	UDLCommandLine.h \
	UpdateDeviceList.h

SOURCES += \
	FTDITemplateCompiler.cpp \
	UDLCommandLine.cpp \
	UpdateDeviceList.cpp \
	main.cpp

linux {
	LIBS += -ldl
}

CONFIG(debug, debug|release) {
	LIBS += -L$$STATIC_LIBPATH -lQCommon-consoled
} else {
	LIBS += -L$$STATIC_LIBPATH -lQCommon-console
}
