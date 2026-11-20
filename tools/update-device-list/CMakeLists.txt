# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause
#
# Qt-free tool that regenerates configurations/devicelist.json from *.tcnf files.
# This is a port of src/applications/updatedevicelist/UpdateDeviceList for the
# source/ subtree.  It has no Qt or qtac-core dependency — only nlohmann/json.

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
# nlohmann/json (header-only)
# ---------------------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(json)

# ---------------------------------------------------------------------------
# Executable
# ---------------------------------------------------------------------------
add_executable(update-device-list main.cpp)

target_link_libraries(update-device-list PRIVATE nlohmann_json::nlohmann_json)

if(MSVC)
    target_compile_options(update-device-list PRIVATE /W3)
endif()

install(TARGETS update-device-list RUNTIME DESTINATION bin)
