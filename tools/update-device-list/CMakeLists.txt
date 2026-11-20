# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause
#
# Qt-free tool that regenerates configurations/devicelist.json from *.tcnf files.
# This is a port of src/applications/updatedevicelist/UpdateDeviceList for the
# source/ subtree.  It depends on qtac-core (for qtac::JsonValue / boost::json).
# Build as part of source/CMakeLists.txt (add_subdirectory).

# Guard standalone-only preamble so this file can also be used via
# add_subdirectory() from a parent build (e.g. source/CMakeLists.txt).
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.17)
    project(update-device-list LANGUAGES CXX)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)

# ---------------------------------------------------------------------------
# Executable
# ---------------------------------------------------------------------------
add_executable(update-device-list main.cpp)

# qtac-core provides qtac::JsonValue and propagates its include path PUBLIC.
target_link_libraries(update-device-list PRIVATE qtac-core)

if(MSVC)
    target_compile_options(update-device-list PRIVATE /W3)
endif()

install(TARGETS update-device-list RUNTIME DESTINATION bin)
