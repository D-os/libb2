add_executable(makestrings
    Strings.cpp
)
target_compile_options(makestrings PRIVATE ${GLOBAL_CFLAGS})
target_link_libraries(makestrings PUBLIC ob)
target_link_libraries(makestrings PRIVATE binder_glue)
