add_library(ServiceProcess SHARED
    Service.cpp
    ServiceCommand.cpp
    main_serviceprocess.cpp
)
set_target_properties(ServiceProcess PROPERTIES PREFIX "")
target_link_libraries(ServiceProcess PUBLIC ob)
