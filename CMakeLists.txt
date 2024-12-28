cmake_minimum_required(VERSION 3.14)
project(clox CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(clox lox.cpp scanner.cpp parser.cpp)
# file(GLOB TEST_SOURCES tests/*.cpp)
# foreach(TEST_SOURCE ${TEST_SOURCES})
#     get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
#     add_executable(${TEST_NAME} ${TEST_SOURCE} scanner.cpp error_manager.cpp)
#     target_include_directories(${TEST_NAME} PRIVATE .)
# endforeach()

target_include_directories(clox PRIVATE .)


# Enable testing
enable_testing()

# Add Google Test
add_subdirectory(third_party/googletest)

# Add your tests
add_executable(test_scanner tests/test_scanner.cpp scanner.cpp)
target_link_libraries(test_scanner gtest gtest_main)
