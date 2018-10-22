add_library(MemoryDealer SHARED
    Heap.cpp
    main.cpp
)
set_target_properties(MemoryDealer PROPERTIES PREFIX "")
target_link_libraries(MemoryDealer PUBLIC ob)
