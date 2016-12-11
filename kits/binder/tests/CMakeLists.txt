include_directories(
    "${EXTERNAL_GOOGLETEST_DIR}/googletest/include"
    "${EXTERNAL_GOOGLETEST_DIR}/googlemock/include"
)

add_executable(binderDriverInterfaceTest "${BINDER_SOURCE_DIR}/tests/binderDriverInterfaceTest.cpp")
target_link_libraries(binderDriverInterfaceTest gtest)

add_executable(binderLibTest "${BINDER_SOURCE_DIR}/tests/binderLibTest.cpp")
target_link_libraries(binderLibTest binder gtest)

add_executable(binderThroughputTest "${BINDER_SOURCE_DIR}/tests/binderThroughputTest.cpp")
target_link_libraries(binderThroughputTest binder utils gtest)

#set_target_properties(checks PROPERTIES COMPILE_FLAGS "-DBINDER_IPC_32BIT=1")
#target_sources(checks PRIVATE
#    ${CMAKE_CURRENT_SOURCE_DIR}/binderDriverInterfaceTest.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/binderLibTest.cpp
#)
