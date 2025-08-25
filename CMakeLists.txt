# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted (subject to the limitations in the
# disclaimer below) provided that the following conditions are met:
#
# 	* Redistributions of source code must retain the above copyright
# 	  notice, this list of conditions and the following disclaimer.
#
# 	* Redistributions in binary form must reproduce the above
# 	  copyright notice, this list of conditions and the following
# 	  disclaimer in the documentation and/or other materials provided
# 	  with the distribution.
#
# 	* Neither the name of Qualcomm Technologies, Inc. nor the names of its
# 	  contributors may be used to endorse or promote products derived
# 	  from this software without specific prior written permission.
#
# NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
# GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
# HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
# GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
# IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Author: Biswajit Roy <biswroy@qti.qualcomm.com>

cmake_minimum_required(VERSION 3.16)

# Include version management
include(cmake/CoreLibraryVersions.cmake)
include(cmake/ApplicationVersions.cmake)
include(cmake/GenerateVersionHeader.cmake)
include(cmake/QTACApplicationHelpers.cmake)

# Third-party dependencies
include(cmake/ConfigureFTDI.cmake)

# Project definition
project(qtac-workspace
    VERSION ${QTAC_CORE_VERSION}
    DESCRIPTION "Qualcomm Test Automation Controller Application Suite"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Platform-specific configurations
if(WIN32)
    add_compile_options(/INCREMENTAL:NO)
    add_compile_definitions(_WIN32 QTAC_STATIC FTd2XX_STATIC)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_definitions(_DEBUG DEBUG)
        add_compile_options(/Zi)
    else()
        add_compile_definitions(_NDEBUG NDEBUG)
    endif()
elseif(UNIX AND NOT APPLE)
    add_compile_options(-fPIC -Werror -Wno-unused-result -Wno-write-strings -Wno-comment 
                       -Wno-unused-function -Wno-reorder -Wno-unknown-pragmas -Wno-conversion-null
                       -Wno-unused-parameter -Wno-overloaded-virtual -Wno-unused-variable 
                       -Wno-date-time -Wno-pragmas)
    add_compile_definitions(__X86_64__ QTAC_STATIC)
    add_link_options(-Wl,--rpath=$ORIGIN -Wl,--rpath=$ORIGIN/lib -Wl,--rpath=$ORIGIN/../lib)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_definitions(_DEBUG DEBUG)
    else()
        add_compile_definitions(_NDEBUG NDEBUG)
    endif()
endif()

# Qt configuration with fallback paths
if(NOT Qt6_FOUND)
    if(DEFINED ENV{QTBIN})
        list(PREPEND CMAKE_PREFIX_PATH "$ENV{QTBIN}")
    elseif(DEFINED ENV{QTDIR})
        list(PREPEND CMAKE_PREFIX_PATH 
            "$ENV{QTDIR}/msvc2022_64" 
            "$ENV{QTBIN}" 
            "$ENV{QTDIR}")
    elseif(WIN32)
        list(PREPEND CMAKE_PREFIX_PATH 
            "C:/Qt/6.8.3/msvc2022_64" 
            "C:/Qt/6.9.0/msvc2022_64")
    endif()
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Xml SerialPort Concurrent Network)

# Disable deprecated Qt APIs
add_definitions(-DQT_DISABLE_DEPRECATED_UP_TO=0x060600)

# Version definitions
add_definitions(
    -DALPACA_VERSION="${ALPACA_VERSION}"
    -DBUG_WRITER_VERSION="${BUG_WRITER_VERSION}"
    -DDEVICE_CATALOG_VERSION="${DEVICE_CATALOG_VERSION}"
    -DEPM_SERVER_VERSION="${EPM_SERVER_VERSION}"
    -DFTDI_CHECK_VERSION="${FTDI_CHECK_VERSION}"
    -DFW_UPDATE_VERSION="${FW_UPDATE_VERSION}"
    -DQCOMMONCOMMAND_VERSION="${QCOMMONCOMMAND_VERSION}"
    -DEEPROM_COMMAND_VERSION="${EEPROM_COMMAND_VERSION}"
    -DTAC_VERSION="${TAC_VERSION}"
)

# Generate version headers
generate_version_header(
    OUTPUT_FILE "include/QTACCoreVersions.h"
    GUARD_NAME "QTAC_CORE_VERSIONS_H"
    VERSIONS
        QTAC_CORE_VERSION
        QCOMMON_CONSOLE_VERSION
        QCOMMON_VERSION
        UI_COMMON_VERSION
        ALPACA_VERSION
        TAC_VERSION
        TAC_LIB_VERSION
        EPM_SERVER_VERSION
        EEPROM_COMMAND_VERSION
        QCOMMONCOMMAND_VERSION
)

generate_version_header(
    OUTPUT_FILE "include/QTACApplicationVersions.h"
    GUARD_NAME "QTAC_APPLICATION_VERSIONS_H"
    VERSIONS
        TEST_AUTOMATION_CONTROLLER_VERSION
        DEVICE_CATALOG_VERSION
        TAC_CONFIGURATION_EDITOR_VERSION
        DEVLIST_VERSION
        TACDUMP_VERSION
        FTDI_CHECK_VERSION
        UPDATE_DEVICE_LIST_VERSION
        LITE_PROGRAMMER_VERSION
        FW_UPDATE_VERSION
        BUG_WRITER_VERSION
        EPM_SCOPE_VERSION
        ALPACA_VIEWER_VERSION
        EPM_VERSION
        EPM_EDITOR_VERSION
        EEPROM_UTL_VERSION
        TACCOM_VERSION
        TAC_TERMINAL_VERSION
)

# Add generated include directory to the global include path
include_directories(${CMAKE_BINARY_DIR}/include)

# Build directories - structured as __Builds/x64/Platform/Configuration
if(WIN32)
    set(PLATFORM_NAME "Windows")
elseif(UNIX AND NOT APPLE)
    set(PLATFORM_NAME "Linux")
else()
    set(PLATFORM_NAME "Unknown")
endif()

# Set base build directory
set(BASE_BUILD_DIR "${CMAKE_SOURCE_DIR}/__Builds/x64/${PLATFORM_NAME}")

# Set output directories based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_TYPE_DIR "${BASE_BUILD_DIR}/Debug")
else()
    set(BUILD_TYPE_DIR "${BASE_BUILD_DIR}/Release")
endif()

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_TYPE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_TYPE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_TYPE_DIR}/lib)

# For multi-config generators (Visual Studio)
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG_UPPER)
    if(CONFIG STREQUAL "Debug")
        set(CONFIG_BUILD_DIR "${BASE_BUILD_DIR}/Debug")
    else()
        set(CONFIG_BUILD_DIR "${BASE_BUILD_DIR}/Release")
    endif()
    
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CONFIG_BUILD_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CONFIG_BUILD_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CONFIG_BUILD_DIR}/lib)
endforeach()

# Create additional directories for build artifacts
file(MAKE_DIRECTORY ${BUILD_TYPE_DIR}/moc)
file(MAKE_DIRECTORY ${BUILD_TYPE_DIR}/ui)

# Add subdirectories in dependency order
add_subdirectory(src/libraries)
add_subdirectory(src/applications)
add_subdirectory(interfaces)

# Configure startup projects for Qt Creator
# Set the main TAC application as the default startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TAC)

# Make all executable targets available for running in Qt Creator
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set working directory for all executables
foreach(target TAC DeviceCatalog tac-configuration-editor devlist ftdi-check tacdump updatedevicelist lite-programmer)
    if(TARGET ${target})
        set_target_properties(${target} PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        )
    endif()
endforeach()

# Qt library copying
if(Qt6_FOUND AND WIN32)
    # Get Qt binary directory
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BINARY_DIR ${QT_QMAKE_EXECUTABLE} DIRECTORY)
    get_filename_component(QT_INSTALL_DIR ${QT_BINARY_DIR} DIRECTORY)
    
    # Determine DLL suffix based on build type
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(QT_DLL_SUFFIX "d")
    else()
        set(QT_DLL_SUFFIX "")
    endif()
    
    # Copy necessary Qt DLLs to bin directory
    set(QT_DLLS_TO_COPY
        Qt6Core${QT_DLL_SUFFIX}.dll
        Qt6Gui${QT_DLL_SUFFIX}.dll
        Qt6Widgets${QT_DLL_SUFFIX}.dll
        Qt6Xml${QT_DLL_SUFFIX}.dll
        Qt6SerialPort${QT_DLL_SUFFIX}.dll
        Qt6Concurrent${QT_DLL_SUFFIX}.dll
        Qt6Network${QT_DLL_SUFFIX}.dll
    )
    
    foreach(dll ${QT_DLLS_TO_COPY})
        if(EXISTS "${QT_BINARY_DIR}/${dll}")
            configure_file("${QT_BINARY_DIR}/${dll}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${dll}" COPYONLY)
        endif()
    endforeach()
    
    # Copy platforms plugin directory
    if(EXISTS "${QT_INSTALL_DIR}/plugins/platforms")
        file(COPY "${QT_INSTALL_DIR}/plugins/platforms" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    endif()
endif()

# Company and copyright information
set(CPACK_PACKAGE_VENDOR "Qualcomm, Inc.")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Qualcomm Test Automation Controller Application Suite")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
