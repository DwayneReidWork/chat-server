cmake_minimum_required(VERSION 3.22)

project(host LANGUAGES C)

set (CMAKE_C_STANDARD 99)
set (CMAKE_CXX_COMPILER gcc)
set (CMAKE_C_COMPILER gcc)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# so that it applies to both the server and server_tests
include_directories(./include)

add_subdirectory(src)
add_subdirectory(c_testing)