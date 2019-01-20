add_compile_options(${GLOBAL_CFLAGS})
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-std=c++17>)

add_subdirectory(libbinder)
#add_subdirectory(libbinder_glue)
#add_subdirectory(libbinder_component_glue)
