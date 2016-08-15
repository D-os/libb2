cmake_minimum_required(VERSION 3.1)
project(libbe CXX C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 98)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(Defaults)
include(EnsureOutOfSourceBuild)
include(EnsureNinjaGenerator)
include(ColorDiagnostics)

# Debug option.
option(LIBBE_DEBUG "Build the project using debugging code" ON)

# Test platform.
if (NOT CMAKE_SYSTEM_NAME MATCHES "Linux")
  message(FATAL_ERROR "Only Linux is supported.")
endif (NOT CMAKE_SYSTEM_NAME MATCHES "Linux")

# Set common flags.
if (LIBBE_DEBUG)
  set(LIBBE_FLAGS "${LIBBE_FLAGS} -ggdb -DDEBUG=1")
  set(CMAKE_BUILD_TYPE Debug)
endif (LIBBE_DEBUG)
if (NOT LIBBE_DEBUG)
  set(CMAKE_BUILD_TYPE Release)
endif (NOT LIBBE_DEBUG)

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
set(CMAKE_C_FLAGS "${LIBBE_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${LIBBE_FLAGS} -Woverloaded-virtual -Wno-ctor-dtor-privacy -Wnon-virtual-dtor -fexceptions -fpermissive")

# Log some information.
message(STATUS "")
message(STATUS "============= Build environment =============")
message(STATUS "CMAKE_SYSTEM:                    ${CMAKE_SYSTEM}")
message(STATUS "CMAKE_SYSTEM_NAME:               ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_VERSION:            ${CMAKE_SYSTEM_VERSION}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR:          ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "CMAKE_SIZEOF_VOID_P:             ${CMAKE_SIZEOF_VOID_P}")
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
add_subdirectory(tests)
