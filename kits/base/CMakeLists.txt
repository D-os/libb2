add_library(base SHARED
    file.cpp
    logging.cpp
    parsenetaddress.cpp
    stringprintf.cpp
    strings.cpp
    test_utils.cpp

    errors_unix.cpp
)
target_link_libraries(base log)
target_compile_options(base PRIVATE -Wexit-time-destructors)

include_directories(
    ${PROJECT_SOURCE_DIR}/external/googletest/googletest/include
    ${PROJECT_SOURCE_DIR}/external/googletest/googlemock/include
)

add_executable(libbase_test
    errors_test.cpp
    file_test.cpp
    logging_test.cpp
    parseint_test.cpp
    parsenetaddress_test.cpp
    stringprintf_test.cpp
    strings_test.cpp
    test_main.cpp
)
target_link_libraries(libbase_test base gtest)
