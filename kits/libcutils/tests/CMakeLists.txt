include_directories(
    ${PROJECT_SOURCE_DIR}/external/googletest/googletest/include
    ${PROJECT_SOURCE_DIR}/external/googletest/googlemock/include
)

add_executable(libcutils_test
    test_str_parms.cpp
#    MemsetTest.cpp
#    PropertiesTest.cpp
#    trace-dev_test.cpp
)
target_link_libraries(libcutils_test cutils gtest gtest_main)
