add_executable(bctest bctest.c binder.c)
target_link_libraries(bctest cutils)

add_executable(servicemanager service_manager.c binder.c)
target_link_libraries(servicemanager cutils selinux)
