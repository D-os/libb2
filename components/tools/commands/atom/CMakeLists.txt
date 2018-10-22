add_library(Atom SHARED
    Atom.cpp
)
set_target_properties(Atom PROPERTIES PREFIX "")
target_link_libraries(Atom PUBLIC ob)
