set(SERVICE_SOURCE_DIR "${CMDS_SOURCE_DIR}/service")
add_definitions("-DXP_UNIX")
add_executable(service "${SERVICE_SOURCE_DIR}/service.cpp")
target_link_libraries(service utils binder)
