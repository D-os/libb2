if (NOT PLATFORM_BITS EQUAL 64)
    add_definitions(-DBINDER_IPC_32BIT=1)
endif()

add_executable(binderDriverInterfaceTest binderDriverInterfaceTest.cpp)
#target_link_libraries(binderDriverInterfaceTest root)

add_executable(binderLibTest binderLibTest.cpp)
target_link_libraries(binderLibTest libbinder)

#set_target_properties(checks PROPERTIES COMPILE_FLAGS "-DBINDER_IPC_32BIT=1")
#target_sources(checks PRIVATE
#    ${CMAKE_CURRENT_SOURCE_DIR}/binderDriverInterfaceTest.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/binderLibTest.cpp
#)
