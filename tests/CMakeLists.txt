add_library(kris-ut INTERFACE "include/boost/ut.hpp")
target_include_directories(kris-ut
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_compile_definitions(kris-ut INTERFACE BOOST_UT_DISABLE_MODULE)

add_executable(run)
target_sources(run PRIVATE
 "main.cpp"
 "common_callables.h"
 "test_constexpr.cpp"
 "test_basics.cpp"
 "test_nontype.cpp"
 "common_callables.cpp"
 "test_noexcept.cpp"
 "test_const.cpp"
 "test_safety.cpp"
 "test_const_noexcept.cpp"
 "test_ctad.cpp"
 "test_call_pattern.cpp")
target_link_libraries(run PRIVATE nontype_functional kris-ut)
