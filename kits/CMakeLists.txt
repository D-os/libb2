include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/private
)

add_subdirectory(libcutils)
add_subdirectory(base)
add_subdirectory(libutils)

if (NOT PLATFORM_BITS EQUAL 64)
    add_definitions("-DBINDER_IPC_32BIT=1")
endif()
add_subdirectory(binder)

add_library(be SHARED "")
target_link_libraries(be
    "-Wl,--whole-archive"
    interfaces
    "-Wl,--no-whole-archive"
)
file(GLOB_RECURSE privateHeaderFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
target_sources(be PRIVATE
    ${headerFiles}
    ${interfaceFiles}
    ${privateHeaderFiles}
)
set_target_properties(be PROPERTIES LINKER_LANGUAGE CXX)
