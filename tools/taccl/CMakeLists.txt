# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
#
# Standalone CMake project for the `taccl` CLI.
# This project deliberately does NOT depend on Qt or the rest of the
# qtac-cli build. It only links against the TACDev shared library.

cmake_minimum_required(VERSION 3.16)
project(taccl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------

# Where to find TACDev.h. Defaults to the sibling interfaces directory in the
# qtac-cli-main repo, but can be overridden when building outside the tree.
set(TACDEV_INCLUDE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/C++/TACDev"
    CACHE PATH "Directory containing TACDev.h")

# Where to find TACDev.lib (import library on Windows) or libTACDev.so (Linux).
# On Windows the import lib (TACDev.lib / TACDevd.lib) is typically produced
# next to the DLL under the main qtac-cli build output.
set(TACDEV_LIBRARY_DIR
    ""
    CACHE PATH "Directory containing TACDev import library (TACDev.lib / libTACDev.so)")

set(TACDEV_LIBRARY_NAME
    "TACDev"
    CACHE STRING "Name of the TACDev library to link against (TACDev or TACDevd)")

# ---------------------------------------------------------------------------
# Third-party (header-only) dependencies via FetchContent
# ---------------------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG        v2.4.2
)
FetchContent_MakeAvailable(cli11)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(json)

# ---------------------------------------------------------------------------
# Executable
# ---------------------------------------------------------------------------
add_executable(taccl
    main.cpp
    Device.cpp
    Device.h
    Output.cpp
    Output.h
    QuickCommands.cpp
    QuickCommands.h
    StateControls.cpp
    StateControls.h
    Errors.h
)

target_include_directories(taccl PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${TACDEV_INCLUDE_DIR}
)

if(TACDEV_LIBRARY_DIR)
    target_link_directories(taccl PRIVATE ${TACDEV_LIBRARY_DIR})
endif()

target_link_libraries(taccl PRIVATE
    CLI11::CLI11
    nlohmann_json::nlohmann_json
    ${TACDEV_LIBRARY_NAME}
)

if(WIN32)
    # TACDev headers assume DLL import on the client side (default when
    # TACDEV_LIBRARY is not defined) - nothing extra needed here.
endif()

install(TARGETS taccl RUNTIME DESTINATION bin)