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

# Author: Biswajit Roy (biswroy@qti.qualcomm.com)

include(${CMAKE_SOURCE_DIR}/src/libraries/qcommon/Common.cmake)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LIBRARY_NAME QCommond)
else()
    set(LIBRARY_NAME QCommon)
endif()

qt_add_resources(QCOMMON_RESOURCES
    resources/QCommon.qrc
)

qt_add_library(${LIBRARY_NAME} STATIC
    AboutDialog.cpp AboutDialog.h AboutDialog.ui
    AlpacaApplication.cpp AlpacaApplication.h
    ApplicationEnhancements.cpp ApplicationEnhancements.h
    ColorSwatch.cpp ColorSwatch.h
    CustomComboBox.h
    HappinessDialog.cpp HappinessDialog.h HappinessDialog.ui
    IntFilteredEditor.cpp IntFilteredEditor.h
    LEDWidget.cpp LEDWidget.h
    PainterEffects.cpp PainterEffects.h
    PinLED.cpp PinLED.h PinLED.ui
    ProgressDialog.cpp ProgressDialog.h ProgressDialog.ui
    QCommon.cpp QCommon.h
    QCommonGlobal.h
    StyledLabel.cpp StyledLabel.h
    TableCheckBox.cpp TableCheckBox.h
    TableComboBox.cpp TableComboBox.h

    ${QCOMMON_RESOURCES}
)

set_target_properties(${LIBRARY_NAME} PROPERTIES
    VERSION ALPACA_VERSION
)

QCommonSettings(${LIBRARY_NAME})

target_include_directories(${LIBRARY_NAME} PUBLIC
    ../qcommon-console
)

target_link_directories(${LIBRARY_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/__Builds/x64/$<CONFIG>/lib
)

target_compile_definitions(${LIBRARY_NAME} PUBLIC
    QTAC_STATIC
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_UP_TO=0x060600
    $<$<CONFIG:Debug>:DEBUG;_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG;_NDEBUG>
)

target_link_libraries(${LIBRARY_NAME} PUBLIC
    Qt6::Concurrent
    Qt6::Core
    Qt6::Gui
    Qt6::SerialPort
    Qt6::Widgets
    Qt6::Xml
)

install(TARGETS ${LIBRARY_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
