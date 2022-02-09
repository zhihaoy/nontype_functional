cmake_minimum_required(VERSION 3.15)

get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(is_multi_config)
    set(CMAKE_CONFIGURATION_TYPES Debug RelWithDebInfo CACHE STRING
        "Semicolon separated list of supported configuration types")
    mark_as_advanced(CMAKE_CONFIGURATION_TYPES)
elseif(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CXX_FLAGS)
    message(WARNING "No CMAKE_BUILD_TYPE is selected")
endif()

project(nontype_functional LANGUAGES CXX)

include(CTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

if(MSVC)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/W3>)
else()
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wall>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wconversion>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wsign-conversion>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wsign-compare>)
endif()

add_library(nontype_functional INTERFACE)
target_sources(nontype_functional INTERFACE
 "include/std23/function_ref.h"
 "include/std23/function.h"
 "include/std23/__functional_base.h"
)
target_include_directories(nontype_functional
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if(BUILD_TESTING)
    add_subdirectory("tests")
endif()
