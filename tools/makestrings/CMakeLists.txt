add_executable(makestrings
    Strings.cpp
)
target_compile_options(makestrings PRIVATE ${GLOBAL_CFLAGS})
target_link_libraries(makestrings PUBLIC ob_bootstrap)
#target_link_libraries(makestrings PRIVATE ob_glue) FIXME: link with real libob
