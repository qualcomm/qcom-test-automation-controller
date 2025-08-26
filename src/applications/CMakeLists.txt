cmake_minimum_required(VERSION 3.16)
project(applications VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()


add_subdirectory(devlist)
add_subdirectory(tacdump)
add_subdirectory(ftdi-check)
add_subdirectory(updatedevicelist)
add_subdirectory(device-catalog)
add_subdirectory(test-automation-controller)
add_subdirectory(tac-configuration-editor)
add_subdirectory(programmers)
