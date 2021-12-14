cmake_minimum_required(VERSION 3.17)
project(function_ref_nontype NONE)
enable_language(CXX)

add_library(function_ref_nontype INTERFACE)
target_sources(function_ref_nontype INTERFACE "include/function_ref.h")
set_target_properties(function_ref_nontype PROPERTIES
    CXX_STANDARD 20
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED YES
)
target_include_directories(function_ref_nontype
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_subdirectory("tests")
