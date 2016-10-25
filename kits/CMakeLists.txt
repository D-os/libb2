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

add_subdirectory(liblog)
add_subdirectory(libcutils)
add_subdirectory(libutils)

if (NOT PLATFORM_BITS EQUAL 64)
    add_definitions("-DBINDER_IPC_32BIT=1")
endif()
add_subdirectory(binder)

add_library(be SHARED "")
target_sources(be PRIVATE
        ${headerFiles}
)
set_target_properties(be PROPERTIES LINKER_LANGUAGE CXX)
