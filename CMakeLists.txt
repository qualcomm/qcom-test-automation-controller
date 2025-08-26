cmake_minimum_required(VERSION 3.16)
project(qtac-workspace VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Concurrent Gui Network SerialPort Widgets Xml)

qt_standard_project_setup()


add_subdirectory(src/libraries)
add_subdirectory(src/applications)
add_subdirectory(interfaces)
