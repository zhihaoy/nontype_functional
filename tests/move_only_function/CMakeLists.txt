add_executable(run-move_only_function)
target_sources(run-move_only_function PRIVATE
 "main.cpp"
 "test_basics.cpp"
 "common_callables.h"
 "common_callables.cpp"
 "test_cvref.cpp"
 "test_nullable.cpp"
 "test_value_semantics.cpp"
 "test_inplace.cpp"
 "test_reference_semantics.cpp"
 "test_noexcept.cpp"
 "test_nontype.cpp"
 "test_return_reference.cpp"
 "test_unique.cpp"
)
target_compile_options(run-move_only_function PRIVATE
    $<$<COMPILE_LANG_AND_ID:CXX,Clang>:-fsized-deallocation>)
target_link_libraries(run-move_only_function PRIVATE nontype_functional kris-ut)
set_target_properties(run-move_only_function PROPERTIES OUTPUT_NAME run)
add_test(move_only_function run)
