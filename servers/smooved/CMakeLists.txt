add_executable(smooved
    smoove.cpp
)
target_link_libraries(smooved PUBLIC ob)
target_link_libraries(smooved PRIVATE ob_glue)
