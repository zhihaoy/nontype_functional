cmake_minimum_required(VERSION 3.15)

get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(is_multi_config)
    set(CMAKE_CONFIGURATION_TYPES Debug RelWithDebInfo CACHE STRING
        "Semicolon separated list of supported configuration types")
    mark_as_advanced(CMAKE_CONFIGURATION_TYPES)
elseif(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CXX_FLAGS)
    message(WARNING "No CMAKE_BUILD_TYPE is selected")
endif()

project(nontype_functional VERSION 1.0.1 LANGUAGES CXX)

if(NOT DEFINED PROJECT_IS_TOP_LEVEL)
    if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
        set(PROJECT_IS_TOP_LEVEL TRUE)
    else()
        set(PROJECT_IS_TOP_LEVEL FALSE)
    endif()
endif()

include(CTest)
include(CMakePackageConfigHelpers)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

if(MSVC)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/W3>)
    string(REPLACE "/GR" "/GR-" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
else()
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wall>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wconversion>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wsign-conversion>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wsign-compare>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)
endif()

add_library(nontype_functional INTERFACE)
add_library(std23::nontype_functional ALIAS nontype_functional)
target_sources(nontype_functional INTERFACE
 "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/std23/function_ref.h>"
 "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/std23/function.h>"
 "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/std23/move_only_function.h>"
 "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/std23/__functional_base.h>"
 "$<INSTALL_INTERFACE:include/std23/function_ref.h>"
 "$<INSTALL_INTERFACE:include/std23/function.h>"
 "$<INSTALL_INTERFACE:include/std23/move_only_function.h>"
 "$<INSTALL_INTERFACE:include/std23/__functional_base.h>"
)
target_include_directories(nontype_functional
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
              $<INSTALL_INTERFACE:include>
)
target_compile_features(nontype_functional INTERFACE cxx_std_20)

if(PROJECT_IS_TOP_LEVEL)
    set(_version_filename "nontype_functional-config-version.cmake")
    write_basic_package_version_file(
        ${_version_filename}
        COMPATIBILITY SameMajorVersion
    )
    install(TARGETS nontype_functional
            EXPORT nontype_functional-config)
    install(EXPORT nontype_functional-config
            NAMESPACE std23::
            DESTINATION lib/cmake/${PROJECT_NAME})
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${_version_filename}"
            DESTINATION lib/cmake/${PROJECT_NAME})
    install(FILES "$<TARGET_PROPERTY:nontype_functional,INTERFACE_SOURCES>"
            DESTINATION include/std23)
endif()

if(PROJECT_IS_TOP_LEVEL AND BUILD_TESTING)
    add_subdirectory("tests")
endif()
