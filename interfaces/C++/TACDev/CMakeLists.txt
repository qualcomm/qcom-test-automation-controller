# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_SOURCE_DIR}/src/libraries/qcommon-console/Common.cmake)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LIBRARY_NAME TACDevd)
else()
    set(LIBRARY_NAME TACDev)
endif()

qt_add_library(${LIBRARY_NAME} SHARED
    TACDev.cpp TACDev.h
    TACDevCore.cpp TACDevCore.h
)

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/src/libraries/qcommon-console
)

target_link_directories(${LIBRARY_NAME} PRIVATE
    ${STATIC_LIBPATH}
)

target_compile_definitions(${LIBRARY_NAME} PUBLIC
    QTAC_STATIC
    QT_DISABLE_DEPRECATED_UP_TO=0x060900
    TACDEV_LIBRARY
    $<$<CONFIG:Debug>:DEBUG;_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG;_NDEBUG>
)

target_link_libraries(${LIBRARY_NAME} PUBLIC
    Qt6::Core
    Qt6::SerialPort
    ftd2xx
    $<$<CONFIG:Debug>:QCommonConsoled>
    $<$<NOT:$<CONFIG:Debug>>:QCommonConsole>
)

if(WIN32)
    target_link_libraries(${LIBRARY_NAME} PUBLIC
        Advapi32
        Ole32
        SetupAPI
        Shell32
        Winusb
        Wtsapi32
    )

    target_compile_options(${LIBRARY_NAME} PRIVATE
        $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Zi>
    )
endif()

if(UNIX AND QCOMMONCONSOLE_LINK_FLAGS)
    # Search the library's own folder ($ORIGIN) for the Qt and other shared libraries
    # installed next to it. Without this, libTACDev.so only carries the build-machine
    # paths, so loading it from Python or another program fails with
    # "libQt6SerialPort.so.6: cannot open shared object file".
    target_link_options(${LIBRARY_NAME} PRIVATE ${QCOMMONCONSOLE_LINK_FLAGS})
endif()

if(UNIX)
    # Record only relative library search paths so the packaged library does not
    # retain absolute paths from the build machine.
    if(APPLE)
        set(TACDEV_INSTALL_RPATH "@loader_path;@loader_path/../lib")
    else()
        set(TACDEV_INSTALL_RPATH "$ORIGIN;$ORIGIN/../lib")
    endif()

    if(APPLE)
        list(GET CMAKE_PREFIX_PATH 0 QTAC_QT_PREFIX)
        set_target_properties(${LIBRARY_NAME} PROPERTIES
            BUILD_WITH_INSTALL_RPATH OFF
            BUILD_RPATH "${QTAC_QT_PREFIX}/lib"
            INSTALL_RPATH "${TACDEV_INSTALL_RPATH}"
        )
    else()
        set_target_properties(${LIBRARY_NAME} PROPERTIES
            BUILD_WITH_INSTALL_RPATH ON
            INSTALL_RPATH "${TACDEV_INSTALL_RPATH}"
        )
    endif()
endif()

if(UNIX)
    target_compile_options(${LIBRARY_NAME} PRIVATE
        -Wno-comment
        -Wno-conversion-null
        -Wno-reorder
        -Wno-unknown-pragmas
        -Wno-unused-function
        -Wno-unused-result
        -Wno-write-strings
    )
endif()

install(TARGETS ${LIBRARY_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Windows file version resource
qtac_add_version_info(${LIBRARY_NAME} "TAC device interface library" "${QTAC_VERSION}")
