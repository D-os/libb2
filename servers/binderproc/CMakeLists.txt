add_executable(binderproc
    main.cpp
)
target_link_libraries(binderproc PUBLIC ob)
target_link_libraries(binderproc PRIVATE ob_glue)
