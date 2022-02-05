add_library(kris-ut INTERFACE)
target_sources(kris-ut INTERFACE "include/boost/ut.hpp")
target_include_directories(kris-ut
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_compile_definitions(kris-ut INTERFACE BOOST_UT_DISABLE_MODULE)

add_subdirectory(function_ref)
add_subdirectory(move_only_function)
add_subdirectory(function)
