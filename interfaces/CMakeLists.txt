cmake_minimum_required(VERSION 3.16)
project(interfaces VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()


add_subdirectory(C++/TACDev)
