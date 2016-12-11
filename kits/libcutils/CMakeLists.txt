add_library(cutils SHARED
#    atomic.c.arm
    config_utils.c
#    fs_config.c
#    canned_fs_config.c
    hashmap.c
    iosched_policy.c
    load_file.c
    native_handle.c
    open_memstream.c
    process_name.c
    record_stream.c
    sched_policy.c
#    sockets.cpp
#    strdup16to8.c
#    strdup8to16.c
    strlcpy.c
    threads.c

    fs.c
#    multiuser.c
#    socket_inaddr_any_server_unix.c
#    socket_local_client_unix.c
#    socket_local_server_unix.c
#    socket_loopback_client_unix.c
#    socket_loopback_server_unix.c
#    socket_network_client_unix.c
#    sockets_unix.cpp
    str_parms.c

#    android_reboot.c
#    ashmem-dev.c
#    debugger.c
    klog.c
#    partition_utils.c
#    properties.c
#    qtaguid.c
#    trace-dev.c
#    uevent.c

    safe_iop.c
)
target_link_libraries(cutils log)
set_target_properties(cutils PROPERTIES LINKER_LANGUAGE C)

add_subdirectory(tests)
