# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
#  
# Redistribution and use in source and binary forms, with or without
# modification, are permitted (subject to the limitations in the
# disclaimer below) provided that the following conditions are met:
#  
# 	* Redistributions of source code must retain the above copyright
# 	  notice, this list of conditions and the following disclaimer.
#  
# 	* Redistributions in binary form must reproduce the above
# 	  copyright notice, this list of conditions and the following
# 	  disclaimer in the documentation and/or other materials provided
# 	  with the distribution.
#  
# 	* Neither the name of Qualcomm Technologies, Inc. nor the names of its
# 	  contributors may be used to endorse or promote products derived
# 	  from this software without specific prior written permission.
#  
# NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
# GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
# HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
# GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
# IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Author: msimpson, biswroy

include(../qcommon/Common.pri)

QT += core gui widgets serialport

TEMPLATE = lib
CONFIG += staticlib
DESTDIR = $$STATIC_LIBPATH

INCLUDEPATH += $$QCOMMON
INCLUDEPATH += $$QCOMMONCONSOLE

DEPENDPATH += $$QCOMMON
DEPENDPATH += $$QCOMMONCONSOLE
DEPENDPATH += $$LIBEPM

CONFIG(debug, debug|release) {
	TARGET = UId
} else {
	TARGET = UI
}

win32 {
	CONFIG(debug, debug|release) {
	        PRE_TARGETDEPS += $$STATIC_LIBPATH/qcommond.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/qcommon-consoled.lib
	} else {
	        PRE_TARGETDEPS += $$STATIC_LIBPATH/qcommon.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/qcommon-console.lib
	}
}

linux {

	CONFIG(debug, debug|release) {
	        PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommond.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon-consoled.a
	} else {
	        PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon-console.a
	}
}

SOURCES += \
	ColorConversion.cpp \
	CommandButton.cpp \
	CustomValidator.cpp \
	HoverAwareQWindow.cpp \
	I2CWidget.cpp \
	NotificationWidget.cpp \
	TACFrame.cpp \
	TerminalWidget.cpp \
	UserWidget.cpp \
	UILib.cpp \
	VariableInput.cpp

HEADERS += \
	ColorConversion.h \
	CommandButton.h \
	CustomValidator.h \
	HoverAwareQWindow.h \
	I2CWidget.h \
	NotificationWidget.h \
	TACFrame.h \
	TerminalWidget.h \
	UserWidget.h \
	UILib.h \
	UIGlobalLib.h \
	VariableInput.h

RESOURCES += \
	resources/ui-common.qrc

FORMS += \
	HoverAwareQWindow.ui \
	I2CWidget.ui \
	NotificationWidget.ui \
	TACFrame.ui \
	TerminalWidget.ui \
	UserWidget.ui \
	VariableInput.ui
