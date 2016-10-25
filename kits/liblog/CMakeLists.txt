add_definitions(-DFAKE_LOG_DEVICE=1)
add_library(log SHARED
    fake_log_device.h
    fake_log_device.c
    logd_write_kern.c
    uio.c
    log_read_kern.c
)
