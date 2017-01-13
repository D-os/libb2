find_package(BISON)
find_package(FLEX)

BISON_TARGET(aidl_language_y
    aidl_language_y.yy ${CMAKE_CURRENT_BINARY_DIR}/aidl_language_y.cpp)
FLEX_TARGET(aidl_language_l
    aidl_language_l.ll  ${CMAKE_CURRENT_BINARY_DIR}/aidl_language_l.cpp)
ADD_FLEX_BISON_DEPENDENCY(aidl_language_l aidl_language_y)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
add_library(aidl-common STATIC
    aidl.cpp
    aidl_language.cpp
    ${BISON_aidl_language_y_OUTPUTS}
    ${FLEX_aidl_language_l_OUTPUTS}
    aidl_language_l.ll
    aidl_language_y.yy
    ast_cpp.cpp
    ast_java.cpp
    code_writer.cpp
    generate_cpp.cpp
    generate_java.cpp
    generate_java_binder.cpp
    import_resolver.cpp
    line_reader.cpp
    io_delegate.cpp
    options.cpp
    type_cpp.cpp
    type_java.cpp
    type_namespace.cpp
)
target_compile_options(aidl-common PRIVATE
# Tragically, the code is riddled with unused parameters.
-Wno-unused-parameter
# yacc dumps a lot of code *just in case*.
-Wno-unused-function
-Wno-unneeded-internal-declaration
# yacc is a tool from a more civilized age.
-Wno-deprecated-register
# yacc also has a habit of using char* over const char*.
-Wno-writable-strings
)

add_executable(aidl
    main_java.cpp
)
target_link_libraries(aidl aidl-common base)

add_executable(aidl-cpp
    main_cpp.cpp
)
target_link_libraries(aidl-cpp aidl-common base)


include_directories(
    ${PROJECT_SOURCE_DIR}/external/googletest/googletest/include
    ${PROJECT_SOURCE_DIR}/external/googletest/googlemock/include
)

add_executable(aidl_unittests
    aidl_unittest.cpp
    ast_cpp_unittest.cpp
    ast_java_unittest.cpp
    generate_cpp_unittest.cpp
    io_delegate_unittest.cpp
    options_unittest.cpp
    tests/end_to_end_tests.cpp
    tests/fake_io_delegate.cpp
    tests/main.cpp
    tests/test_data_example_interface.cpp
    tests/test_data_ping_responder.cpp
    tests/test_util.cpp
    type_cpp_unittest.cpp
    type_java_unittest.cpp
)
target_link_libraries(aidl_unittests aidl-common base gmock gtest)
