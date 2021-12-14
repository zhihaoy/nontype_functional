cmake_minimum_required(VERSION 3.15)
project(function_ref_nontype NONE)
enable_language(CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_library(function_ref_nontype INTERFACE)
target_sources(function_ref_nontype INTERFACE "include/function_ref.h")
target_include_directories(function_ref_nontype
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_subdirectory("tests")
