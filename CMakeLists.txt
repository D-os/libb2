cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(LIBB2 VERSION 0.1.0 LANGUAGES CXX C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Policies
if(POLICY CMP0076)
  cmake_policy(SET CMP0076 OLD)
endif()

# Defaults
set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)
set(CMAKE_COLOR_MAKEFILE ON)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Ensure env
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Create a separate build directory and run 'cmake path_to_libb2 [options]' there.")
endif()
if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
    message(FATAL_ERROR "Only Ninja generator is supported: cmake ... -GNinja")
endif()
if (NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "Only Linux is supported.")
endif()

# Color diagnostics
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    SET(DIAG_FLAGS "-fcolor-diagnostics")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    SET(DIAG_FLAGS "-fdiagnostics-color")
endif()

# Build type flags.
set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g0")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

# Common compiler flags.
set(COMMON_FLAGS "-Wall -Wpointer-arith -Wcast-align -Wsign-compare -Wstrict-aliasing -Wno-multichar -Wno-reserved-id-macro -fPIC ${DIAG_FLAGS}")

# OpenBinder defines
set(GLOBAL_CFLAGS
    -DBUILD_TYPE=BUILD_TYPE_DEBUG
    -DTARGET_PLATFORM=TARGET_PLATFORM_PALMSIM_LINUX
    -DTARGET_HOST=TARGET_HOST_LINUX
    -D_SUPPORTS_EXCEPTIONS=1
    -DVALIDATES_REGION=0
    -DVALIDATES_VALUE=0
    -DSUPPORTS_LOCK_DEBUG=1
    -DSUPPORTS_ATOM_DEBUG=0
    -DSUPPORTS_NAMESPACE=1
    -DSUPPORTS_UNIX_FILE_PATH=1
    -DSUPPORTS_TEXT_STREAM=1
    -DSUPPORTS_CALL_STACK=1
    -DLINUX_DEMO_HACK=1
)

set(SOURCE_HEADERS_DIR "${CMAKE_SOURCE_DIR}/headers/android" "${CMAKE_SOURCE_DIR}/headers/os")
set(INTERFACE_HEADERS_DIR "${CMAKE_BINARY_DIR}/headers")

# Default include directories.
include_directories(
    ${SOURCE_HEADERS_DIR}
    ${INTERFACE_HEADERS_DIR}
    ${INTERFACE_HEADERS_DIR}/os
)

# Set flags.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS} -Woverloaded-virtual -Wno-ctor-dtor-privacy -Wnon-virtual-dtor -fno-rtti -fno-exceptions -fpermissive")

# Goal is to work without libctdc++ installed
set(CMAKE_CXX_IMPLICIT_LINK_LIBRARIES "")
set(CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES "")
#set(CMAKE_SHARED_LINKER_FLAGS "-nodefaultlibs")
#if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
#    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lgcc")
#endif()

# Compute platform bits.
math(EXPR PLATFORM_BITS "8*${CMAKE_SIZEOF_VOID_P}")

# Log some information.
message(STATUS "")
message(STATUS "============= Build environment =============")
message(STATUS "CMAKE_SYSTEM:                    ${CMAKE_SYSTEM}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR:          ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "CMAKE_SIZEOF_VOID_P:             ${CMAKE_SIZEOF_VOID_P}")
message(STATUS "PLATFORM_BITS:                   ${PLATFORM_BITS}")
message(STATUS "CMAKE_C_FLAGS:                   ${CMAKE_C_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS:                 ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_BUILD_TYPE:                ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_C_COMPILER:                ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER:              ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_AR:                        ${CMAKE_AR}")
message(STATUS "CMAKE_RANLIB:                    ${CMAKE_RANLIB}")
message(STATUS "INTERFACE_HEADERS_DIR:           ${INTERFACE_HEADERS_DIR}")
message(STATUS "=============================================")
message(STATUS "")

set(EXTERNAL_GOOGLETEST_DIR "${CMAKE_SOURCE_DIR}/external/googletest")
set(EXTERNAL_SAFE_IOP_DIR "${CMAKE_SOURCE_DIR}/external/safe-iop")

# Tests
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(DBUILD_GMOCK OFF)
set(INSTALL_GTEST OFF)
add_subdirectory("${EXTERNAL_GOOGLETEST_DIR}" EXCLUDE_FROM_ALL)

include(GoogleTest)
include(CTest)
enable_testing()

# Sub directories.
add_subdirectory(headers)
add_subdirectory(interfaces)
add_subdirectory(libs)
add_subdirectory(libraries)
add_subdirectory(cmds)
add_subdirectory(tools)
add_subdirectory(servers)
add_subdirectory(commands)
add_subdirectory(components)
#add_subdirectory(samples)

#add_subdirectory(tests)
