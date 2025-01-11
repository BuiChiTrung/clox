cmake_minimum_required(VERSION 3.14)
project(clox CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(clox main.cpp clox/scanner.cpp clox/parser.cpp clox/error_manager.cpp)

target_include_directories(clox PRIVATE .)


# Enable testing
enable_testing()

# Add Google Test
add_subdirectory(third_party/googletest)

# Add your tests
add_executable(test_scanner tests/test_scanner.cpp clox/scanner.cpp clox/error_manager.cpp)
target_link_libraries(test_scanner gtest gtest_main)

add_executable(test_helper tests/test_helper.cpp utils/helper.cpp clox/error_manager.cpp)
target_link_libraries(test_helper gtest gtest_main)

add_executable(test_printer_visitor tests/test_printer_visitor.cpp)
target_link_libraries(test_printer_visitor gtest gtest_main)

add_executable(test_interpreter_visitor tests/test_interpreter_visitor.cpp clox/error_manager.cpp clox/parser.cpp clox/scanner.cpp)
target_link_libraries(test_interpreter_visitor gtest gtest_main)