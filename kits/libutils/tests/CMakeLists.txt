include_directories(
    "${EXTERNAL_GOOGLETEST_DIR}/googletest/include"
    "${EXTERNAL_GOOGLETEST_DIR}/googlemock/include"
)

add_executable(libutils_tests
    "${UTILS_SOURCE_DIR}/tests/BasicHashtable_test.cpp"
#    "${UTILS_SOURCE_DIR}/tests/BlobCache_test.cpp"
    "${UTILS_SOURCE_DIR}/tests/BitSet_test.cpp"
    "${UTILS_SOURCE_DIR}/tests/Looper_test.cpp"
    "${UTILS_SOURCE_DIR}/tests/LruCache_test.cpp"
    "${UTILS_SOURCE_DIR}/tests/String8_test.cpp"
    "${UTILS_SOURCE_DIR}/tests/Unicode_test.cpp"
    "${UTILS_SOURCE_DIR}/tests/Vector_test.cpp"
)

#target_link_libraries(libutils_tests gtest z log cutils utils)
target_link_libraries(libutils_tests gtest_main gtest z cutils utils)
