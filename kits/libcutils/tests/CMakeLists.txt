include_directories(
    "${EXTERNAL_GOOGLETEST_DIR}/googletest/include"
    "${EXTERNAL_GOOGLETEST_DIR}/googlemock/include"
)

add_executable(libcutils_test
    "${CUTILS_SOURCE_DIR}/tests/test_str_parms.cpp"
#    "${CUTILS_SOURCE_DIR}/tests/MemsetTest.cpp"
#    "${CUTILS_SOURCE_DIR}/tests/PropertiesTest.cpp"
)
target_link_libraries(libcutils_test cutils gtest gtest_main)
