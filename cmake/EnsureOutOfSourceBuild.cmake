if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Please create a separate build directory and run 'cmake path_to_libbe [options]' there.")
endif()
