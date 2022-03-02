add_library(examples OBJECT)
target_sources(examples PRIVATE
 "listing_1.cpp"
 "listing_2.cpp"
 "listing_3.cpp"
)
target_compile_definitions(examples
    PRIVATE $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
target_link_libraries(examples PRIVATE nontype_functional)
