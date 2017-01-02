set(BASE_SOURCE_DIR "${ANDROID_SYSTEM_CORE_DIR}/base")

add_library(base SHARED
    "${BASE_SOURCE_DIR}/file.cpp"
    "${BASE_SOURCE_DIR}/logging.cpp"
    "${BASE_SOURCE_DIR}/parsenetaddress.cpp"
    "${BASE_SOURCE_DIR}/stringprintf.cpp"
    "${BASE_SOURCE_DIR}/strings.cpp"
    "${BASE_SOURCE_DIR}/test_utils.cpp"

    "${BASE_SOURCE_DIR}/errors_unix.cpp"
)
target_link_libraries(base cutils)
target_compile_options(base PRIVATE -Wexit-time-destructors)

include_directories(
    "${EXTERNAL_GOOGLETEST_DIR}/googletest/include"
    "${EXTERNAL_GOOGLETEST_DIR}/googlemock/include"
)

add_executable(libbase_test
    "${BASE_SOURCE_DIR}/errors_test.cpp"
    "${BASE_SOURCE_DIR}/file_test.cpp"
    "${BASE_SOURCE_DIR}/logging_test.cpp"
    "${BASE_SOURCE_DIR}/parseint_test.cpp"
    "${BASE_SOURCE_DIR}/parsenetaddress_test.cpp"
    "${BASE_SOURCE_DIR}/stringprintf_test.cpp"
    "${BASE_SOURCE_DIR}/strings_test.cpp"
    "${BASE_SOURCE_DIR}/test_main.cpp"
)
target_link_libraries(libbase_test base gtest)
