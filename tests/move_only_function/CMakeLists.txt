add_executable(run-move_only_function)
target_sources(run-move_only_function PRIVATE
 "main.cpp"
)
target_link_libraries(run-move_only_function PRIVATE nontype_functional kris-ut)
set_target_properties(run-move_only_function PROPERTIES OUTPUT_NAME run)
add_test(move_only_function run)
