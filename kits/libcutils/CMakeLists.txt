add_library(cutils SHARED
    hashmap.c
    native_handle.c
    config_utils.c
    load_file.c
    strlcpy.c
    open_memstream.c
#	strdup16to8.c
#	strdup8to16.c
    record_stream.c
    process_name.c
    threads.c
    sched_policy.c
    iosched_policy.c
    str_parms.c
#    fs_config.c

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
