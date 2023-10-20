add_library(llbo-instrument)
target_sources(llbo-instrument PRIVATE
    instrumentation.cpp
)
target_compile_features(llbo-instrument PUBLIC cxx_std_20)
set_target_properties(llbo-instrument PROPERTIES
    LINKER_LANGUAGE CXX
    POSITION_INDEPENDENT_CODE ON
)
