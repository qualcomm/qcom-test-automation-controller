# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.22)
project(QTAC-CLI VERSION 6.9.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Ensure src is accessible from cli/ for dependencies expecting ${CMAKE_SOURCE_DIR}/src
if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src")
    if (WIN32)
        file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src" _CLI_SRC_LINK)
        file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../src" _CLI_SRC_TARGET)
        execute_process(COMMAND cmd /c mklink /J "${_CLI_SRC_LINK}" "${_CLI_SRC_TARGET}" OUTPUT_QUIET ERROR_QUIET)
    else()
        execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CURRENT_SOURCE_DIR}/../src" "${CMAKE_CURRENT_SOURCE_DIR}/src" OUTPUT_QUIET ERROR_QUIET)
    endif()
endif()

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

