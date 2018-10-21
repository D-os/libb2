set(INTERFACE_HEADERS_DIR "${CMAKE_BINARY_DIR}/headers")
include_directories("${INTERFACE_HEADERS_DIR}")

add_compile_options(${GLOBAL_CFLAGS} -frtti -fexceptions)

add_subdirectory(libbinder)
add_subdirectory(libbinder_glue)
add_subdirectory(libbinder_component_glue)
