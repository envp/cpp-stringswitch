cmake_minimum_required(VERSION 3.25)
project(stringswitch CXX)

add_subdirectory(src)

set(CMAKE_CXX_STANDARD 20)

enable_testing()
add_subdirectory(tests)
