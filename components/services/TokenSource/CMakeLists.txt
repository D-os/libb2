add_library(TokenSource SHARED
    main_token_source.cpp
)
set_target_properties(TokenSource PROPERTIES PREFIX "")
target_link_libraries(TokenSource PUBLIC ob)
