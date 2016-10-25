set(LOG_SOURCE_DIR "${ANDROID_SYSTEM_CORE_DIR}/liblog")
add_definitions(-DFAKE_LOG_DEVICE=1)
add_library(log SHARED
    "${LOG_SOURCE_DIR}/fake_log_device.h"
    "${LOG_SOURCE_DIR}/fake_log_device.c"
    "${LOG_SOURCE_DIR}/logd_write_kern.c"
    "${LOG_SOURCE_DIR}/uio.c"
    "${LOG_SOURCE_DIR}/log_read_kern.c"
)
