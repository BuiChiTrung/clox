cmake_minimum_required(VERSION 3.20.0)
project(clox)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(third_party/googletest)
add_subdirectory(clox)
add_subdirectory(tests)

add_executable(main main.cpp)

target_link_libraries(main PRIVATE clox)