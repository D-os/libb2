add_executable(bsh
    main.cpp
)
target_link_libraries(bsh PUBLIC ob)
target_link_libraries(bsh PRIVATE ob_glue)
