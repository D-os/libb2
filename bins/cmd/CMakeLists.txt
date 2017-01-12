set(CMD_SOURCE_DIR "${CMDS_SOURCE_DIR}/cmd")
add_definitions("-DXP_UNIX")
add_executable(cmd "${CMD_SOURCE_DIR}/cmd.cpp")
target_link_libraries(cmd utils binder)
