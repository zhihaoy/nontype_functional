cmake_minimum_required(VERSION 3.14)
project(std23-functional-examples CXX)

find_package(nontype_functional 1.0 CONFIG)

add_library(examples OBJECT)
target_sources(examples PRIVATE
 "listing_1.cpp"
 "listing_2.cpp"
 "listing_3.cpp"
)
target_compile_definitions(examples
    PRIVATE $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
target_link_libraries(examples PRIVATE std23::nontype_functional)
