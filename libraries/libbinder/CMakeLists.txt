add_library(ob_bootstrap STATIC "")
target_link_libraries(ob_bootstrap PUBLIC binder)
set_target_properties(ob_bootstrap PROPERTIES LINKER_LANGUAGE CXX)
target_compile_definitions(ob_bootstrap PRIVATE -DLIBBE_BOOTSTRAP=1)

add_library(ob SHARED "")
target_link_libraries(ob PUBLIC binder)
#target_link_libraries(ob PRIVATE
#    "-Wl,--whole-archive"
#    interfaces
#    "-Wl,--no-whole-archive"
#)
file(GLOB_RECURSE privateHeaderFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
target_sources(ob PRIVATE
    ${headerFiles}
#    ${interfaceFiles}
    ${privateHeaderFiles}
)
set_target_properties(ob PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(app)
#add_subdirectory(package)
add_subdirectory(services)
add_subdirectory(storage)
add_subdirectory(support)
#add_subdirectory(xml)
