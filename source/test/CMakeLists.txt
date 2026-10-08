cmake_minimum_required(VERSION 3.16)

# Allow standalone invocation (build_test/) or inclusion from root.
if(NOT TARGET qtac-core)
    project(qtac-tests LANGUAGES CXX)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../library ${CMAKE_CURRENT_BINARY_DIR}/library)
endif()

add_executable(test_bytearray test_bytearray.cpp)
target_include_directories(test_bytearray PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../library/include
)

add_executable(test_string test_string.cpp)
target_link_libraries(test_string PRIVATE qtac-core)

add_executable(test_containers test_containers.cpp)
target_link_libraries(test_containers PRIVATE qtac-core)

add_executable(test_stringutils test_stringutils.cpp)
target_link_libraries(test_stringutils PRIVATE qtac-core)

add_executable(test_signal test_signal.cpp)
target_link_libraries(test_signal PRIVATE qtac-core)

add_executable(test_platform_configs test_platform_configs.cpp)
target_link_libraries(test_platform_configs PRIVATE qtac-core)

add_executable(test_coders_commands test_coders_commands.cpp)
target_link_libraries(test_coders_commands PRIVATE qtac-core)

add_executable(test_hardware_ftdi test_hardware_ftdi.cpp)
target_link_libraries(test_hardware_ftdi PRIVATE qtac-core)

add_executable(test_hardware_psoc test_hardware_psoc.cpp)
target_link_libraries(test_hardware_psoc PRIVATE qtac-core)

add_executable(test_tacdev_api test_tacdev_api.cpp)
target_include_directories(test_tacdev_api PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../tacdev
)
target_link_libraries(test_tacdev_api PRIVATE TACDev)
