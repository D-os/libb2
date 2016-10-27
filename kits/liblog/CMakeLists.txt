add_library(log SHARED
	log_read_kern.c
	logd_write_kern.c
)

add_subdirectory(tests)
