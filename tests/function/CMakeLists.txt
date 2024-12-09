add_executable(run-function)
target_sources(run-function PRIVATE
 "main.cpp"
 "test_basics.cpp"
 "common_callables.h"
 "common_callables.cpp"
 "test_value_semantics.cpp"
 "test_ctad.cpp"
 "test_reference_semantics.cpp"
 "test_nullable.cpp"
 "test_nontype.cpp"
)
target_link_libraries(run-function PRIVATE nontype_functional kris-ut)
set_target_properties(run-function PROPERTIES OUTPUT_NAME run)
add_test(function run)
