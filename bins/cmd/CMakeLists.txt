add_definitions("-DXP_UNIX")
add_executable(cmd cmd.cpp)
target_link_libraries(cmd utils binder)
