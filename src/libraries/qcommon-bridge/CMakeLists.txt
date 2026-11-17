# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_SOURCE_DIR}/src/libraries/qcommon/Common.cmake)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LIBRARY_NAME QCommonBridged)
    set(CONSOLE_LIB QCommonConsoled)
else()
    set(LIBRARY_NAME QCommonBridge)
    set(CONSOLE_LIB QCommonConsole)
endif()

qt_add_library(${LIBRARY_NAME} STATIC
    QAlpacaDevice.cpp
    QAlpacaDevice.h
    QAlpacaSettings.h
    QAppPaths.cpp
    QAppPaths.h
    QCommandGroup.h
    QCommonBridgeGlobal.h
    QCommonTypes.cpp
    QCommonTypes.h
    QFTDIBusData.h
    QPlatformConfigBridge.h
    QRecentFiles.h
    QTab.h
    QTACDefines.h
    QTACPreferences.h
    QUSBDescriptors.cpp
    QUSBDescriptors.h
)

set_target_properties(${LIBRARY_NAME} PROPERTIES
    VERSION ${QTAC_VERSION}
)

QCommonSettings(${LIBRARY_NAME})

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../qcommon-console
)

target_link_directories(${LIBRARY_NAME} PRIVATE
    ${STATIC_LIBPATH}
)

target_compile_definitions(${LIBRARY_NAME} PUBLIC
    QTAC_STATIC
    QCOMMONBRIDGE_LIBRARY
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_UP_TO=0x060900
    $<$<CONFIG:Debug>:DEBUG;_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG;_NDEBUG>
)

target_link_libraries(${LIBRARY_NAME} PUBLIC
    Qt6::Core
    Qt6::Gui
    ${CONSOLE_LIB}
)

install(TARGETS ${LIBRARY_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
