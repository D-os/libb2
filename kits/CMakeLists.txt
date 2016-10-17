include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_definitions(-fno-rtti -fno-exceptions)
set(CMAKE_SHARED_LINKER_FLAGS "-nodefaultlibs -lgcc")

add_subdirectory(binder)

add_library(be SHARED "")
target_sources(be PRIVATE
        ${headerFiles}
)
set_target_properties(be PROPERTIES LINKER_LANGUAGE CXX)
