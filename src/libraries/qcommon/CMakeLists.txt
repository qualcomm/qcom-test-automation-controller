# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_SOURCE_DIR}/src/libraries/qcommon/Common.cmake)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LIBRARY_NAME QCommond)
else()
    set(LIBRARY_NAME QCommon)
endif()

set(CMAKE_AUTORCC ON)

qt_add_resources(QCOMMON_RESOURCES
    resources/QCResource.qrc
)

qt_add_library(${LIBRARY_NAME} STATIC
    AboutDialog.cpp AboutDialog.h AboutDialog.ui
    AlpacaApplication.cpp AlpacaApplication.h
    ApplicationEnhancements.cpp ApplicationEnhancements.h
    ColorSwatch.cpp ColorSwatch.h
    CustomComboBox.h
    IntFilteredEditor.cpp IntFilteredEditor.h
    LEDWidget.cpp LEDWidget.h
    PainterEffects.cpp PainterEffects.h
    PinLED.cpp PinLED.h PinLED.ui
    QCommon.cpp QCommon.h
    QCommonGlobal.h
    StyledLabel.cpp StyledLabel.h
    TableCheckBox.cpp TableCheckBox.h
    TableComboBox.cpp TableComboBox.h

    ${QCOMMON_RESOURCES}
)

set_target_properties(${LIBRARY_NAME} PROPERTIES
    VERSION ${QTAC_VERSION}
)

QCommonSettings(${LIBRARY_NAME})

target_include_directories(${LIBRARY_NAME} PUBLIC
    ../qcommon-console
)

target_link_directories(${LIBRARY_NAME} PRIVATE
    ${STATIC_LIBPATH}
)

target_compile_definitions(${LIBRARY_NAME} PUBLIC
    QTAC_STATIC
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_UP_TO=0x060900
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
