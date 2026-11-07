# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.22)
project(qtac-napi)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# TACDev and the main QTAC libraries are built with the dynamic MSVC runtime.
# Keep the addon ABI-compatible with those libraries (/MD and /MDd).
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# Detect if we're being built standalone (cmake-js) or as part of main project
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(STANDALONE_BUILD TRUE)
    # When built standalone, find the repository root
    get_filename_component(REPO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
else()
    set(STANDALONE_BUILD FALSE)
    set(REPO_ROOT ${CMAKE_SOURCE_DIR})
endif()

message(STATUS "Standalone build: ${STANDALONE_BUILD}")
message(STATUS "Repository root: ${REPO_ROOT}")

# Find Node.js and node-addon-api. Use include_dir instead of include because
# node-addon-api.include contains surrounding quotes on Windows; passing that
# value directly to CMake produces a malformed MSBuild include path.
execute_process(
    COMMAND node -p "require('node-addon-api').include_dir"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE NODE_ADDON_API_RELATIVE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE NODE_ADDON_API_RESULT
)

if(NOT NODE_ADDON_API_RESULT EQUAL 0)
    message(FATAL_ERROR "node-addon-api not found. Run 'npm install' in interfaces/Node.js/")
endif()

get_filename_component(
    NODE_ADDON_API_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/${NODE_ADDON_API_RELATIVE_DIR}"
    ABSOLUTE
)

execute_process(
    COMMAND node -p "process.versions.node"
    OUTPUT_VARIABLE NODE_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "Building NAPI module for Node.js ${NODE_VERSION}")
message(STATUS "node-addon-api include: ${NODE_ADDON_API_DIR}")

# Define NAPI module target
add_library(qtac_napi SHARED
    src/addon.cc
    src/tacdev_wrapper.cc
    src/utils.cc
    # cmake-js supplies the Windows delay-load hook here. Without it the
    # addon tries to load a literal node.exe, which works in Node.js but
    # terminates Electron because Electron's executable is electron.exe.
    ${CMAKE_JS_SRC}
)

# Set output name to qtac.node
set_target_properties(qtac_napi PROPERTIES
    PREFIX ""
    SUFFIX ".node"
    OUTPUT_NAME "qtac"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# Include directories
target_include_directories(qtac_napi PRIVATE
    ${NODE_ADDON_API_DIR}
    ${CMAKE_JS_INC}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${REPO_ROOT}/interfaces/C++/TACDev
    ${REPO_ROOT}/src/libraries/qcommon-console
)

# Find TACDev library
if(STANDALONE_BUILD)
    # In standalone mode, find the pre-built TACDev library
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(TACDEV_LIB_NAME "TACDevd")
    else()
        set(TACDEV_LIB_NAME "TACDev")
    endif()

    # Detect the library architecture. On Windows, use the generator platform
    # because CMAKE_SYSTEM_PROCESSOR can describe the host rather than the
    # target selected by cmake-js/MSBuild.
    if(WIN32 AND CMAKE_GENERATOR_PLATFORM MATCHES "^(ARM64|ARM64EC)$")
        set(BUILD_ARCH "ARM64")
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(BUILD_ARCH "Linux")
    else()
        set(BUILD_ARCH "x64")
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(BUILD_CONFIG "Debug")
    else()
        set(BUILD_CONFIG "Release")
    endif()

    # Look for TACDev library in build output (static library)
    set(TACDEV_LIB_PATH "${REPO_ROOT}/__Builds/${BUILD_ARCH}/${BUILD_CONFIG}")

    if(WIN32)
        set(TACDEV_LIB_FILE "${TACDEV_LIB_PATH}/lib/${TACDEV_LIB_NAME}.lib")
    else()
        set(TACDEV_LIB_FILE "${TACDEV_LIB_PATH}/lib/lib${TACDEV_LIB_NAME}.a")
    endif()

    message(STATUS "Looking for TACDev library at: ${TACDEV_LIB_FILE}")

    if(NOT EXISTS "${TACDEV_LIB_FILE}")
        message(FATAL_ERROR "TACDev library not found at ${TACDEV_LIB_FILE}. Please build the main QTAC project first using build.bat or build.sh")
    endif()

    # Create imported target for static library
    add_library(TACDev STATIC IMPORTED)
    set_target_properties(TACDev PROPERTIES
        IMPORTED_LOCATION "${TACDEV_LIB_FILE}"
    )

    target_link_libraries(qtac_napi PRIVATE TACDev)

    # Since TACDev is static, link the libraries it depends on as well.
    if(WIN32)
        # build.bat exposes the selected Qt installation through QTBIN.
        if(DEFINED ENV{QTBIN})
            get_filename_component(QT_PREFIX "$ENV{QTBIN}/.." ABSOLUTE)
            list(PREPEND CMAKE_PREFIX_PATH "${QT_PREFIX}")
        endif()

        find_package(Qt6 REQUIRED COMPONENTS Concurrent Core Network SerialPort Xml)

        set(QCOMMONCONSOLE_LIB_FILE "${TACDEV_LIB_PATH}/lib/QCommonConsole.lib")
        set(FTD2XX_LIB_FILE "${TACDEV_LIB_PATH}/lib/ftd2xx.lib")
        set(HIDAPI_LIB_FILE "${REPO_ROOT}/build/${BUILD_CONFIG}/_deps/hidapi-build/src/windows/hidapi.lib")

        foreach(DEPENDENCY_FILE IN ITEMS QCOMMONCONSOLE_LIB_FILE FTD2XX_LIB_FILE HIDAPI_LIB_FILE)
            if(NOT EXISTS "${${DEPENDENCY_FILE}}")
                message(FATAL_ERROR "Required TACDev dependency not found: ${${DEPENDENCY_FILE}}")
            endif()
        endforeach()

        add_library(QTAC_QCommonConsole STATIC IMPORTED)
        set_target_properties(QTAC_QCommonConsole PROPERTIES
            IMPORTED_LOCATION "${QCOMMONCONSOLE_LIB_FILE}"
        )

        add_library(QTAC_ftd2xx STATIC IMPORTED)
        set_target_properties(QTAC_ftd2xx PROPERTIES
            IMPORTED_LOCATION "${FTD2XX_LIB_FILE}"
        )

        add_library(QTAC_hidapi STATIC IMPORTED)
        set_target_properties(QTAC_hidapi PROPERTIES
            IMPORTED_LOCATION "${HIDAPI_LIB_FILE}"
        )

        target_link_libraries(qtac_napi PRIVATE
            QTAC_QCommonConsole
            QTAC_ftd2xx
            QTAC_hidapi
            Qt6::Concurrent
            Qt6::Core
            Qt6::Network
            Qt6::SerialPort
            Qt6::Xml
            Advapi32
            Ole32
            SetupAPI
            Shell32
            Winusb
            Wtsapi32
        )
    else()
        find_package(Qt6 REQUIRED COMPONENTS Concurrent Core Network SerialPort Xml)

        set(QCOMMONCONSOLE_LIB_FILE "${TACDEV_LIB_PATH}/lib/libQCommonConsole.a")
        set(FTD2XX_LIB_FILE "${TACDEV_LIB_PATH}/lib/libftd2xx.a")
        set(HIDAPI_LIB_FILE "${REPO_ROOT}/build/${BUILD_CONFIG}/_deps/hidapi-build/src/linux/libhidapi-hidraw.a")

        foreach(DEPENDENCY_FILE IN ITEMS QCOMMONCONSOLE_LIB_FILE FTD2XX_LIB_FILE HIDAPI_LIB_FILE)
            if(NOT EXISTS "${${DEPENDENCY_FILE}}")
                message(FATAL_ERROR "Required TACDev dependency not found: ${${DEPENDENCY_FILE}}")
            endif()
        endforeach()

        add_library(QTAC_QCommonConsole STATIC IMPORTED)
        set_target_properties(QTAC_QCommonConsole PROPERTIES
            IMPORTED_LOCATION "${QCOMMONCONSOLE_LIB_FILE}"
        )

        add_library(QTAC_ftd2xx STATIC IMPORTED)
        set_target_properties(QTAC_ftd2xx PROPERTIES
            IMPORTED_LOCATION "${FTD2XX_LIB_FILE}"
        )

        add_library(QTAC_hidapi STATIC IMPORTED)
        set_target_properties(QTAC_hidapi PROPERTIES
            IMPORTED_LOCATION "${HIDAPI_LIB_FILE}"
        )

        target_link_libraries(qtac_napi PRIVATE
            QTAC_QCommonConsole
            QTAC_ftd2xx
            QTAC_hidapi
            Qt6::Concurrent
            Qt6::Core
            Qt6::Network
            Qt6::SerialPort
            Qt6::Xml
            pthread
            udev
            dl
        )
    endif()
else()
    # In integrated build, link against the target
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_libraries(qtac_napi PRIVATE TACDevd)
    else()
        target_link_libraries(qtac_napi PRIVATE TACDev)
    endif()
endif()

# N-API functions are supplied by the Node.js runtime. CMake.js exposes the
# correct platform import library through CMAKE_JS_LIB.
if(CMAKE_JS_LIB)
    target_link_libraries(qtac_napi PRIVATE ${CMAKE_JS_LIB})
endif()

# cmake-js supplies the N-API export definitions, but MSVC requires an import
# library when linking the addon. Generate node.lib in the build directory.
if(MSVC AND CMAKE_JS_NODELIB_DEF AND CMAKE_JS_NODELIB_TARGET)
    execute_process(
        COMMAND ${CMAKE_AR}
            /def:${CMAKE_JS_NODELIB_DEF}
            /out:${CMAKE_JS_NODELIB_TARGET}
            ${CMAKE_STATIC_LINKER_FLAGS}
    )
endif()

# Platform-specific settings
if(WIN32)
    target_compile_definitions(qtac_napi PRIVATE
        NAPI_VERSION=8
        TACDEV_STATIC
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )

    # Suppress node-gyp warnings
    target_compile_options(qtac_napi PRIVATE
        /wd4251  # class needs to have dll-interface
        /wd4275  # non dll-interface class used as base
    )
elseif(UNIX)
    target_compile_definitions(qtac_napi PRIVATE
        NAPI_VERSION=8
    )

    target_link_libraries(qtac_napi PRIVATE
        ${CMAKE_DL_LIBS}
    )

    target_compile_options(qtac_napi PRIVATE
        -fexceptions
        -Wno-unused-result
    )
endif()

# Install to build directory for npm to find
install(TARGETS qtac_napi
    LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/build/Release
    RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/build/Release
)
