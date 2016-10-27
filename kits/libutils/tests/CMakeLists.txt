include_directories(
    ${PROJECT_SOURCE_DIR}/external/googletest/googletest/include
    ${PROJECT_SOURCE_DIR}/external/googletest/googlemock/include
)

add_executable(libutils_tests
    BasicHashtable_test.cpp
#    BlobCache_test.cpp
    BitSet_test.cpp
    Looper_test.cpp
    LruCache_test.cpp
    String8_test.cpp
    Unicode_test.cpp
    Vector_test.cpp
)

target_link_libraries(libutils_tests gtest_main gtest z log cutils utils)
