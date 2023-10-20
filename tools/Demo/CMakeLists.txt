add_executable(llbo test_entry.cpp)
target_link_libraries(llbo PRIVATE
    llbo-instrument
)
set_target_properties(llbo PROPERTIES
    LINKER_LANGUAGE CXX
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
