include_directories(
    ${PROJECT_SOURCE_DIR}/external/googletest/googletest/include
    ${PROJECT_SOURCE_DIR}/external/googletest/googlemock/include
)

add_executable(check
    ""
    # tests go here
)
target_link_libraries(check gtest_main gtest gmock)

add_subdirectory(binder)
