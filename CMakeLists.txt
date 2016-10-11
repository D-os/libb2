cmake_minimum_required(VERSION 3.1)
project(libbe CXX C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(Defaults)
include(EnsureOutOfSourceBuild)
include(EnsureNinjaGenerator)
include(ColorDiagnostics)

# Test platform.
if (NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "Only Linux is supported.")
endif()

# Build type flags.
set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -ggdb -DDEBUG=1")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g0")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

# Common compiler flags.
set(LIBBE_FLAGS "${LIBBE_FLAGS} -Wall -Wpointer-arith -Wcast-align -Wsign-compare -Wstrict-aliasing -Wno-multichar")

# Default include directories.
include_directories(
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_SOURCE_DIR}/headers/support
    ${CMAKE_SOURCE_DIR}/headers/app
    ${CMAKE_SOURCE_DIR}/headers/interface
    ${CMAKE_SOURCE_DIR}/headers/storage
    ${CMAKE_SOURCE_DIR}/headers/kernel
)

# Set flags.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${LIBBE_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIBBE_FLAGS} -Woverloaded-virtual -Wno-ctor-dtor-privacy -Wnon-virtual-dtor -fexceptions -fpermissive")

# Compute platform bits .
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
message(STATUS "=============================================")
message(STATUS "")

# Sub directories.
add_subdirectory(headers)
add_subdirectory(kits)

# Tests
add_subdirectory(external/googletest)
add_subdirectory(tests)
