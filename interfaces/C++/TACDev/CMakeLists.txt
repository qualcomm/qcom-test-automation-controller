# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../src/libraries/qcommon-console/Common.cmake")
    include(${CMAKE_CURRENT_LIST_DIR}/../../../src/libraries/qcommon-console/Common.cmake)
else()
    include(${CMAKE_SOURCE_DIR}/src/libraries/qcommon-console/Common.cmake)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LIBRARY_NAME TACDevd)
    set(LIB_TARGET QCommonConsoled)
else()
    set(LIBRARY_NAME TACDev)
    set(LIB_TARGET QCommonConsole)
endif()

add_library(${LIBRARY_NAME} SHARED
    TACDev.cpp TACDev.h
    TACDevCore.cpp TACDevCore.h
)

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_directories(${LIBRARY_NAME} PRIVATE
    ${STATIC_LIBPATH}
)

target_compile_definitions(${LIBRARY_NAME} PUBLIC
    QTAC_STATIC
    TACDEV_LIBRARY
    $<$<CONFIG:Debug>:DEBUG;_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG;_NDEBUG>
)

target_link_libraries(${LIBRARY_NAME} PUBLIC
    ${LIB_TARGET}
    ftd2xx
)


if(WIN32)
    target_link_libraries(${LIBRARY_NAME} PUBLIC
        Advapi32
        Ole32
        SetupAPI
        Shell32
        Winusb
        Wtsapi32
        version
        user32
    )

    target_compile_options(${LIBRARY_NAME} PRIVATE
        $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Zi>
    )
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
