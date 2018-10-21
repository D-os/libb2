add_compile_options(${GLOBAL_CFLAGS} -frtti -fexceptions)

add_subdirectory(libbinder)
add_subdirectory(libbinder_glue)
add_subdirectory(libbinder_component_glue)
