set(LOG_SOURCE_DIR "${ANDROID_SYSTEM_CORE_DIR}/liblog")

add_library(log SHARED
    "${LOG_SOURCE_DIR}/log_read_kern.c"
    "${LOG_SOURCE_DIR}/logd_write_kern.c"
)

add_subdirectory(tests)
