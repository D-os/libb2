add_executable(bctest bctest.c binder.c)
target_link_libraries(bctest log)

add_executable(servicemanager service_manager.c binder.c)
target_link_libraries(servicemanager log selinux)
