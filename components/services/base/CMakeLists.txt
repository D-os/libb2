add_library(components-base SHARED
    main_base.cpp
)
set_target_properties(components-base PROPERTIES PREFIX "")
target_link_libraries(components-base PUBLIC ob)
