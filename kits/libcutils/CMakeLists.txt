set(CUTILS_SOURCE_DIR "${ANDROID_SYSTEM_CORE_DIR}/libcutils")

add_library(cutils SHARED
    "${CUTILS_SOURCE_DIR}/hashmap.c"
    "${CUTILS_SOURCE_DIR}/native_handle.c"
    "${CUTILS_SOURCE_DIR}/config_utils.c"
    "${CUTILS_SOURCE_DIR}/load_file.c"
    "${CUTILS_SOURCE_DIR}/strlcpy.c"
    "${CUTILS_SOURCE_DIR}/open_memstream.c"
#    "${CUTILS_SOURCE_DIR}/strdup16to8.c"
#    "${CUTILS_SOURCE_DIR}/strdup8to16.c"
    "${CUTILS_SOURCE_DIR}/record_stream.c"
    "${CUTILS_SOURCE_DIR}/process_name.c"
    "${CUTILS_SOURCE_DIR}/threads.c"
    "${CUTILS_SOURCE_DIR}/sched_policy.c"
    "${CUTILS_SOURCE_DIR}/iosched_policy.c"
    "${CUTILS_SOURCE_DIR}/str_parms.c"
#    "${CUTILS_SOURCE_DIR}/fs_config.c"

#    "${CUTILS_SOURCE_DIR}/android_reboot.c"
#    "${CUTILS_SOURCE_DIR}/ashmem-dev.c"
#    "${CUTILS_SOURCE_DIR}/debugger.c"
    "${CUTILS_SOURCE_DIR}/klog.c"
#    "${CUTILS_SOURCE_DIR}/partition_utils.c"
#    "${CUTILS_SOURCE_DIR}/properties.c"
#    "${CUTILS_SOURCE_DIR}/qtaguid.c"
#    "${CUTILS_SOURCE_DIR}/trace-dev.c"
#    "${CUTILS_SOURCE_DIR}/uevent.c"
)
target_link_libraries(cutils log)
set_target_properties(cutils PROPERTIES LINKER_LANGUAGE C)

add_subdirectory(tests)
