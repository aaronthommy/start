cmake_minimum_required(VERSION 3.18)
project(start LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
list(APPEND CMAKE_PREFIX_PATH "C:/msys64/ucrt64")

find_package(raylib CONFIG REQUIRED)

add_executable(start src/main.cpp)

target_include_directories(start PRIVATE
    $<TARGET_PROPERTY:raylib,INTERFACE_INCLUDE_DIRECTORIES>)

target_link_libraries(start PRIVATE raylib)
