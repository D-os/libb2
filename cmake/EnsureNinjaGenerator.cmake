if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
    message(FATAL_ERROR "Only Ninja generator is supported: cmake ... -GNinja")
endif()
