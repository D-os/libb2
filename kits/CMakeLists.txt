include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/private
)

add_definitions(-fno-rtti -fno-exceptions)
set(CMAKE_CXX_IMPLICIT_LINK_LIBRARIES "")
set(CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES "")
#set(CMAKE_SHARED_LINKER_FLAGS "-nodefaultlibs")
#if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
#    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lgcc")
#endif()

# OpenBinder defines. TODO get rid of these :)
add_definitions(
-DBUILD_TYPE=BUILD_TYPE_DEBUG
-DTARGET_PLATFORM=TARGET_PLATFORM_PALMSIM_LINUX
-DTARGET_HOST=TARGET_HOST_LINUX
-D_SUPPORTS_EXCEPTIONS=1
-DVALIDATES_REGION=0
-DVALIDATES_VALUE=0
-DSUPPORTS_LOCK_DEBUG=1
-DSUPPORTS_ATOM_DEBUG=1
-D_SUPPORTS_NAMESPACE=1
-DSUPPORTS_UNIX_FILE_PATH=1
-DSUPPORTS_TEXT_STREAM=0 # TODO should be 1
-DSUPPORTS_CALL_STACK=1
-DLINUX_DEMO_HACK=1
-Wno-multichar
)

add_subdirectory(liblog)
add_subdirectory(libcutils)
add_subdirectory(base)
add_subdirectory(libutils)

if (NOT PLATFORM_BITS EQUAL 64)
    add_definitions("-DBINDER_IPC_32BIT=1")
endif()
add_subdirectory(binder)

add_library(b2 SHARED
    support/Value.cpp
)
set_target_properties(b2 PROPERTIES LINKER_LANGUAGE CXX)

# pull header files to sources, so IDE shows them
file(GLOB_RECURSE privateHeaderFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
target_sources(b2 PRIVATE
        ${headerFiles}
        ${privateHeaderFiles}
)
