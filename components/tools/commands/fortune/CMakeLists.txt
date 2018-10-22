add_library(Fortune SHARED
    FortuneCommand.cpp
)
set_target_properties(Fortune PROPERTIES PREFIX "")
target_link_libraries(Fortune PUBLIC ob)
