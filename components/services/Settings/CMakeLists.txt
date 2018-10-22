add_library(Settings SHARED
    SettingsCatalog.cpp
    main.cpp
)
set_target_properties(Settings PROPERTIES PREFIX "")
target_compile_definitions(Settings PUBLIC -DOPENBINDER_SETTINGS_BUILD=1)
target_link_libraries(Settings PUBLIC ob)
