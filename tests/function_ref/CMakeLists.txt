add_executable(run-function_ref)
target_sources(run-function_ref PRIVATE
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
 "test_call_pattern.cpp"
 "test_constinit.cpp"
 "test_return_reference.cpp"
)
target_link_libraries(run-function_ref PRIVATE nontype_functional kris-ut)
set_target_properties(run-function_ref PROPERTIES OUTPUT_NAME run)
add_test(function_ref run)
