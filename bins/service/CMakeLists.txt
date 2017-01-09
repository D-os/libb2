add_definitions("-DXP_UNIX")
add_executable(service service.cpp)
target_link_libraries(service utils binder)
