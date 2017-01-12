set(DUMPSYS_SOURCE_DIR "${CMDS_SOURCE_DIR}/dumpsys")
add_definitions("-DXP_UNIX")
add_executable(dumpsys "${DUMPSYS_SOURCE_DIR}/dumpsys.cpp")
target_link_libraries(dumpsys base utils binder)
