add_library(llbo-utils)
target_sources(llbo-utils PRIVATE
    code_gen.cpp
)
target_compile_features(llbo-utils PUBLIC cxx_std_20)
set_target_properties(llbo-utils PROPERTIES
    LINKER_LANGUAGE CXX
    POSITION_INDEPENDENT_CODE ON
)
