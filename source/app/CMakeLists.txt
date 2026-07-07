# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted (subject to the limitations in the
# disclaimer below) provided that the following conditions are met:
#
#     * Redistributions of source code must retain the above copyright
#         notice, this list of conditions and the following disclaimer.
#
#     * Redistributions in binary form must reproduce the above
#         copyright notice, this list of conditions and the following
#         disclaimer in the documentation and/or other materials provided
#         with the distribution.
#
#     * Neither the name of Qualcomm Technologies, Inc. nor the names of its
#         contributors may be used to endorse or promote products derived
#         from this software without specific prior written permission.
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

cmake_minimum_required(VERSION 3.16)
project(qtac-app VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()

qt_add_resources(QTAC_APP_RESOURCES resources/TAC.qrc)

qt_add_executable(qtac-app WIN32
    main.cpp
    TACApplication.h
    TACApplication.cpp
    TACWindow.h
    TACWindow.cpp
    TACWindow.ui
    TACDeviceSelection.h
    TACDeviceSelection.cpp
    TACDeviceSelection.ui
    TACPinFrame.h
    TACPinFrame.cpp
    ${QTAC_APP_RESOURCES}
)

target_include_directories(qtac-app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../library/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/libraries/qt-adapter/include
)

if(WIN32)
    target_link_directories(qtac-app PRIVATE
        "${CMAKE_SOURCE_DIR}/__Builds/x64/Release/lib"
        "${CMAKE_SOURCE_DIR}/__Builds/x64/Debug/lib"
    )
endif()

target_link_libraries(qtac-app PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    qt-adapter
    qtac-core
)

if(WIN32)
    target_link_libraries(qtac-app PRIVATE
        ftd2xx
        Advapi32
        SetupAPI
    )
    # RC compilation with Ninja requires special handling; disabled for now.
    # target_sources(qtac-app PRIVATE resources/TAC.rc)
endif()

install(TARGETS qtac-app
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
