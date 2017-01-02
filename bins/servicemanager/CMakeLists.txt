set(SERVICEMANAGER_SOURCE_DIR "${CMDS_SOURCE_DIR}/servicemanager")

add_executable(bctest
    "${SERVICEMANAGER_SOURCE_DIR}/bctest.c"
    "${SERVICEMANAGER_SOURCE_DIR}/binder.c"
)
target_link_libraries(bctest cutils)

add_executable(servicemanager
    "${SERVICEMANAGER_SOURCE_DIR}/service_manager.c"
    "${SERVICEMANAGER_SOURCE_DIR}/binder.c"
)
target_link_libraries(servicemanager cutils selinux)
