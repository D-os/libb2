add_library(support SHARED
    SupportComponents.cpp
)
set_target_properties(support PROPERTIES PREFIX "")
target_link_libraries(support PUBLIC ob)
