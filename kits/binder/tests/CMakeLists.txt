include_directories(
    ${PROJECT_SOURCE_DIR}/external/googletest/googletest/include
    ${PROJECT_SOURCE_DIR}/external/googletest/googlemock/include
)

add_executable(binderDriverInterfaceTest binderDriverInterfaceTest.cpp)
target_link_libraries(binderDriverInterfaceTest gtest)

#add_executable(binderLibTest binderLibTest.cpp)
#target_link_libraries(binderLibTest binder gtest)

#set_target_properties(checks PROPERTIES COMPILE_FLAGS "-DBINDER_IPC_32BIT=1")
#target_sources(checks PRIVATE
#    ${CMAKE_CURRENT_SOURCE_DIR}/binderDriverInterfaceTest.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/binderLibTest.cpp
#)
