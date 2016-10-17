include_directories(
    ${CMAKE_SOURCE_DIR}/external/googletest/googletest/include
    ${CMAKE_SOURCE_DIR}/external/googletest/googlemock/include
)

add_executable(binderDriverInterfaceTest "${BINDER_SOURCE}/tests/binderDriverInterfaceTest.cpp")
#target_link_libraries(binderDriverInterfaceTest root)

add_executable(binderLibTest "${BINDER_SOURCE}/tests/binderLibTest.cpp")
target_link_libraries(binderLibTest binder)

#set_target_properties(checks PROPERTIES COMPILE_FLAGS "-DBINDER_IPC_32BIT=1")
#target_sources(checks PRIVATE
#    ${CMAKE_CURRENT_SOURCE_DIR}/binderDriverInterfaceTest.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/binderLibTest.cpp
#)
