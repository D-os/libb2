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

add_library(b2 SHARED ""
    "${EXTERNAL_MSGPACK11_DIR}/msgpack11.cpp"
    support/Context.cpp
    support/Node.cpp
    support/String.cpp
    support/Value.cpp
)
target_link_libraries(b2 PUBLIC utils binder)
target_link_libraries(b2 PRIVATE
    "-Wl,--whole-archive"
    interfaces
    "-Wl,--no-whole-archive"
)
file(GLOB_RECURSE privateHeaderFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
target_sources(b2 PRIVATE
    ${headerFiles}
    ${interfaceFiles}
    ${privateHeaderFiles}
)
set_target_properties(b2 PROPERTIES LINKER_LANGUAGE CXX)
