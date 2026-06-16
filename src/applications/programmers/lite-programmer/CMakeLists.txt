# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_SOURCE_DIR}/src/libraries/qcommon-console/Common.cmake)

set(APP_NAME LITEProgrammer)

qt_add_executable(${APP_NAME}
    ProgrammerCommandLine.cpp ProgrammerCommandLine.h
    main.cpp
)

target_include_directories(${APP_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/libraries/qcommon-console
)

target_compile_definitions(${APP_NAME} PRIVATE
    QTAC_STATIC
    QT_DISABLE_DEPRECATED_UP_TO=0x060900
    $<$<CONFIG:Debug>:DEBUG;_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG;_NDEBUG>
)

target_link_directories(${APP_NAME} PRIVATE
    ${STATIC_LIBPATH}
)

target_link_libraries(${APP_NAME} PRIVATE
    Qt6::Core
    Qt6::SerialPort
    ftd2xx
    $<$<CONFIG:Debug>:QCommonConsoled>
    $<$<NOT:$<CONFIG:Debug>>:QCommonConsole>
)

target_compile_options(${APP_NAME} PRIVATE
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Zi>
)

if(UNIX)
    target_compile_options(${APP_NAME} PRIVATE
        -Wno-comment
        -Wno-conversion-null
        -Wno-reorder
        -Wno-unknown-pragmas
        -Wno-unused-function
        -Wno-unused-result
        -Wno-write-strings
    )
endif()

install(TARGETS ${APP_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET ${APP_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
