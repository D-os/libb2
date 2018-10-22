add_library(Printf SHARED
    printf.c
    PrintfCommand.cpp
)
set_target_properties(Printf PROPERTIES PREFIX "")
target_link_libraries(Printf PUBLIC ob)
