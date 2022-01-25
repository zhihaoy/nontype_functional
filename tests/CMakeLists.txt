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
target_link_libraries(run PRIVATE function_ref_nontype)
