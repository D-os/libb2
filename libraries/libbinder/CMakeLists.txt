add_library(ob_bootstrap STATIC "")
target_link_libraries(ob_bootstrap pthread dl)
set_target_properties(ob_bootstrap PROPERTIES LINKER_LANGUAGE CXX)
target_compile_options(ob_bootstrap PRIVATE ${GLOBAL_CFLAGS} -frtti -fexceptions -DLIBBE_BOOTSTRAP=1)

set(INTERFACE_HEADERS_DIR "${CMAKE_BINARY_DIR}/headers")

add_library(ob SHARED "")
target_include_directories(ob PRIVATE "${INTERFACE_HEADERS_DIR}")
target_link_libraries(ob PUBLIC binder)
target_link_libraries(ob PRIVATE
    "-Wl,--whole-archive"
    interfaces
    "-Wl,--no-whole-archive"
)
set_target_properties(ob PROPERTIES LINKER_LANGUAGE CXX)
target_compile_options(ob PRIVATE ${GLOBAL_CFLAGS} -frtti -fexceptions)

#add_subdirectory(app)
#add_subdirectory(package)
#add_subdirectory(services)
add_subdirectory(storage)
add_subdirectory(support)
add_subdirectory(xml)
