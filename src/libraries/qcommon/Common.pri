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
include($$PWD/../qcommon-console/version.pri)

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QCOMMON = "$$PWD"

QCOMMONCONSOLE = "$$PWD/../qcommon-console"
LIBUI = "$$PWD/../ui-common"

TACDEV = "$$PWD/../../../interfaces/C++/TACDev"

TACLOC = "$$PWD/../TAC"

CONFIG += skip_target_version_ext

DEFINES += QTAC_STATIC

message($$QCOMMONCONSOLE)

win32 {
	CONFIG += c++20
	QMAKE_CXXFLAGS += /Zi
    QMAKE_LFLAGS += /INCREMENTAL:NO
}

linux {
    CONFIG += c++2a c++1z c++14
}

CONFIG(x32) {
    message(Building 32 bit)
	win32: DEFINES += _WIN32
    linux: DEFINES += __i386__
	WINTARGET = "Win32"
} else {
    message(Building 64 bit)
    linux: DEFINES += __X86_64__
	WINTARGET = "x64"
}

CONFIG(debug, debug|release) {
	DEFINES += _DEBUG DEBUG
	win32: CONFIGURATION = "$$WINTARGET/Debug"
	linux: CONFIGURATION = "Linux/Debug"
}
else {
	DEFINES += _NDEBUG NDEBUG
	win32: CONFIGURATION = "$$WINTARGET/Release"
	linux: CONFIGURATION = "Linux/Release"
}

BUILDROOT = $$PWD/../../../__Builds/$$CONFIGURATION
BINPATH = "$$BUILDROOT/bin"
DESTDIR = $$BINPATH
OBJECTS_DIR = "$$BUILDROOT/obj/$$TARGET"
STATIC_LIBPATH = "$$BUILDROOT/lib"
MOC_DIR = "$$BUILDROOT/moc/$$TARGET"
UI_DIR = "$$BUILDROOT/ui/$$TARGET"

INCLUDEPATH += "$$BUILDROOT/ui/QCommon"


unix: {
	QMAKE_CXXFLAGS += -Werror -Wno-unused-result -Wno-write-strings -Wno-comment -Wno-unused-function -Wno-reorder -Wno-unknown-pragmas -Wno-conversion-null -std=c++0x
	QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder -Wno-unused-parameter -Wno-overloaded-virtual -Wno-unused-function -Wno-unused-variable -Wno-date-time
}

QMAKE_TARGET_COMPANY="Qualcomm, Inc."
QMAKE_TARGET_COPYRIGHT="2025 Qualcomm, Inc."
QMAKE_TARGET_DESCRIPTION="Qualcomm Test Automation Controller Application Suite"
