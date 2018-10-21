add_custom_command(
    OUTPUT ISampleService.h ISampleService.cpp
    COMMAND pidgen_bs "${CMAKE_CURRENT_SOURCE_DIR}/ISampleService.idl"
    MAIN_DEPENDENCY ISampleService.idl
    DEPENDS pidgen_bs
    COMMENT "Compiling IDL ISampleService.idl"
)

add_library(sampleservice SHARED
    SampleServiceLib.cpp
    ISampleService.idl
    ISampleService.h ISampleService.cpp
)
target_include_directories(sampleservice PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(sampleservice PUBLIC ob)

add_library(Service SHARED
    SampleService.cpp
    SampleService_components.cpp
)
set_target_properties(Service PROPERTIES PREFIX "")
target_include_directories(Service PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(Service PUBLIC ob sampleservice)
