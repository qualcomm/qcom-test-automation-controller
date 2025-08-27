cmake_minimum_required(VERSION 3.16)
project(QTAC-Workspace VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Concurrent Gui Network SerialPort Widgets Xml)

qt_standard_project_setup(REQUIRES 6.9)

add_subdirectory(src/libraries)
add_subdirectory(src/applications)
add_subdirectory(interfaces)
