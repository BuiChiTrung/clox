cmake_minimum_required(VERSION 3.14)
project(clox CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(clox main.cpp scanner.cpp error_manager.cpp)
# file(GLOB TEST_SOURCES tests/*.cpp)
# foreach(TEST_SOURCE ${TEST_SOURCES})
#     get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
#     add_executable(${TEST_NAME} ${TEST_SOURCE} scanner.cpp error_manager.cpp)
#     target_include_directories(${TEST_NAME} PRIVATE .)
# endforeach()

target_include_directories(clox PRIVATE .)
