add_executable(llbo llbo.cpp)

llvm_map_components_to_libnames(REQ_LLVM_LIBRARIES ${LLVM_TARGETS_TO_BUILD}
    asmparser
    core
    linker
    bitreader
    bitwriter
    irreader
    ipo
    scalaropts
    analysis
    target
    mc
    support
)

target_link_libraries(llbo PRIVATE
    llbo-instrument
    ${REQ_LLVM_LIBRARIES}
)
set_target_properties(llbo PROPERTIES
    LINKER_LANGUAGE CXX
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
