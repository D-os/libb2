add_library(BPerf SHARED
    main.cpp
    EffectIPC.cpp
    icache.cpp
)
set_target_properties(BPerf PROPERTIES PREFIX "")
target_link_libraries(BPerf PUBLIC ob)
