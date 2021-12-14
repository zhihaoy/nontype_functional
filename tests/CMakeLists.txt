add_executable(run)
target_sources(run PRIVATE "main.cpp")
target_link_libraries(run PRIVATE function_ref_nontype)
