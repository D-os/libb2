add_library(base SHARED
    main_base.cpp
)
set_target_properties(base PROPERTIES PREFIX "")
target_link_libraries(base PUBLIC ob)
