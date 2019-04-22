add_subdirectory(base)
add_subdirectory(libcutils)
add_subdirectory(libutils)

add_definitions("-D__ANDROID__")
if (NOT PLATFORM_BITS EQUAL 64)
    add_definitions("-DBINDER_IPC_32BIT=1")
endif()
add_subdirectory(libbinder)
