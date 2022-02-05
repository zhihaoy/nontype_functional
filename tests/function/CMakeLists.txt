add_executable(run-function)
target_sources(run-function PRIVATE
 "main.cpp"
 "test_basics.cpp"
 "common_callables.h")
target_link_libraries(run-function PRIVATE nontype_functional kris-ut)
set_target_properties(run-function PROPERTIES OUTPUT_NAME run)
add_test(function run)
