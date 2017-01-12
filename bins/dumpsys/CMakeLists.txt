add_definitions("-DXP_UNIX")
add_executable(dumpsys dumpsys.cpp)
target_link_libraries(dumpsys base utils binder)
