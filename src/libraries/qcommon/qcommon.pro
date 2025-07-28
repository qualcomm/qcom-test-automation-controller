#-------------------------------------------------
#
# Project created by QtCreator 2018-08-30T09:50:56  <<< --- Wow, this is the day I started the QTAC project
#
#-------------------------------------------------

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
# Copyright ©2018-2023 Qualcomm Technologies, Inc.
# All rights reserved.
# Qualcomm Technologies Confidential and Proprietary

# Author: msimpson

include(Common.pri)

QT += gui widgets xml serialport concurrent network

CONFIG(debug, debug|release) {
    TARGET = QCommond
} else {
    TARGET = QCommon
}

TEMPLATE = lib
CONFIG += staticlib
DESTDIR = $$STATIC_LIBPATH
DEFINES += QCOMMON_LIBRARY

INCLUDEPATH += $$QCOMMONCONSOLE
DEPENDPATH += $$QCOMMONCONSOLE

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
    AboutDialog.cpp \
    AlpacaApplication.cpp \
    ApplicationEnhancements.cpp \
	ColorSwatch.cpp \
    HappinessDialog.cpp \
    IntFilteredEditor.cpp \
    LEDWidget.cpp \
    PainterEffects.cpp \
	PinLED.cpp \
    ProgressDialog.cpp \
    QCommon.cpp \
	StyledLabel.cpp \
    TableCheckBox.cpp \
	TableComboBox.cpp

HEADERS += \
    AboutDialog.h \
    AlpacaApplication.h \
    ApplicationEnhancements.h \
	ColorSwatch.h \
	CustomComboBox.h \
    HappinessDialog.h \
    IntFilteredEditor.h \
    LEDWidget.h \
    PainterEffects.h \
	PinLED.h \
	ProgressDialog.h \
    QCommon.h \
    QCommonGlobal.h \
	StyledLabel.h \
    TableCheckBox.h \
	TableComboBox.h

FORMS += \
	AboutDialog.ui \
    HappinessDialog.ui \
    PinLED.ui \
    ProgressDialog.ui

RESOURCES += \
    QCommon.qrc
