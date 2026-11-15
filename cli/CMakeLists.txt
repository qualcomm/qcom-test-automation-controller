# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.22)
project(QTAC-CLI VERSION 6.9.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Configure output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add third-party dependencies (FTDI, HIDAPI)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../third-party ${CMAKE_CURRENT_BINARY_DIR}/third-party)

# Add QCommonConsole static library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../src/libraries/qcommon-console ${CMAKE_CURRENT_BINARY_DIR}/qcommon-console)

# Add TACDev C++ interface shared library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../interfaces/C++/TACDev ${CMAKE_CURRENT_BINARY_DIR}/TACDev)

# Target library names based on build type
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(TACDEV_TARGET TACDevd)
    set(QCOMMON_TARGET QCommonConsoled)
else()
    set(TACDEV_TARGET TACDev)
    set(QCOMMON_TARGET QCommonConsole)
endif()

# Build TACTerminal CLI application
add_executable(TACTerminal
    ${CMAKE_CURRENT_SOURCE_DIR}/../examples/C++/TACTerminal.cpp
)

target_include_directories(TACTerminal PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../interfaces/C++/TACDev
)

target_link_libraries(TACTerminal PRIVATE
    ${TACDEV_TARGET}
)

# Copy TACDev DLL next to executables on Windows
if (WIN32)
    add_custom_command(TARGET TACTerminal POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:${TACDEV_TARGET}>
            $<TARGET_FILE_DIR:TACTerminal>
    )
endif()

# Build tests if enabled
option(BUILD_TESTING "Build test executables" ON)
if (BUILD_TESTING AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/standalone/test_main.cpp")
    add_executable(qcommon_console_test
        ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/standalone/test_main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/standalone/hardware_stubs.cpp
    )
    target_include_directories(qcommon_console_test PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/standalone
        ${CMAKE_CURRENT_SOURCE_DIR}/../interfaces/C++/TACDev
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/libraries/qcommon-console
    )
    target_link_libraries(qcommon_console_test PRIVATE
        ${QCOMMON_TARGET}
        ${TACDEV_TARGET}
    )
    if (WIN32)
        target_link_libraries(qcommon_console_test PRIVATE setupapi advapi32 version user32 shell32)
        add_custom_command(TARGET qcommon_console_test POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:${TACDEV_TARGET}>
                $<TARGET_FILE_DIR:qcommon_console_test>
        )
    endif()
endif()
