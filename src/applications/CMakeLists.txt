# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.22)
project(applications VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup(REQUIRES 6.9)

add_subdirectory(devlist)
add_subdirectory(tacdump)
add_subdirectory(ftdi-check)
add_subdirectory(updatedevicelist)
add_subdirectory(device-catalog)
add_subdirectory(test-automation-controller)
add_subdirectory(tac-configuration-editor)
add_subdirectory(programmers)
