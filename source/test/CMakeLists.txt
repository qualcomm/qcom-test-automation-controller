cmake_minimum_required(VERSION 3.16)
project(qtac-tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build the library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../library ${CMAKE_CURRENT_BINARY_DIR}/library)

add_executable(test_bytearray test_bytearray.cpp)
target_include_directories(test_bytearray PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../library/include
)

add_executable(test_string test_string.cpp)
target_link_libraries(test_string PRIVATE qtac-core)

add_executable(test_containers test_containers.cpp)
target_link_libraries(test_containers PRIVATE qtac-core)
