set(AIDL_SOURCE_DIR "${ANDROID_SYSTEM_TOOLS_AIDL_DIR}")

find_package(BISON)
find_package(FLEX)

BISON_TARGET(aidl_language_y
    "${AIDL_SOURCE_DIR}/aidl_language_y.yy" ${CMAKE_CURRENT_BINARY_DIR}/aidl_language_y.cpp)
FLEX_TARGET(aidl_language_l
    "${AIDL_SOURCE_DIR}/aidl_language_l.ll" ${CMAKE_CURRENT_BINARY_DIR}/aidl_language_l.cpp)
ADD_FLEX_BISON_DEPENDENCY(aidl_language_l aidl_language_y)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${AIDL_SOURCE_DIR}
    "${EXTERNAL_GOOGLETEST_DIR}/googletest/include"
    "${EXTERNAL_GOOGLETEST_DIR}/googlemock/include"
)

add_library(aidl-common STATIC
    ${BISON_aidl_language_y_OUTPUTS}
    ${FLEX_aidl_language_l_OUTPUTS}
    "${AIDL_SOURCE_DIR}/aidl.cpp"
    "${AIDL_SOURCE_DIR}/aidl_language.cpp"
    "${AIDL_SOURCE_DIR}/aidl_language_l.ll"
    "${AIDL_SOURCE_DIR}/aidl_language_y.yy"
    "${AIDL_SOURCE_DIR}/ast_cpp.cpp"
    "${AIDL_SOURCE_DIR}/ast_java.cpp"
    "${AIDL_SOURCE_DIR}/code_writer.cpp"
    "${AIDL_SOURCE_DIR}/generate_cpp.cpp"
    "${AIDL_SOURCE_DIR}/generate_java.cpp"
    "${AIDL_SOURCE_DIR}/generate_java_binder.cpp"
    "${AIDL_SOURCE_DIR}/import_resolver.cpp"
    "${AIDL_SOURCE_DIR}/line_reader.cpp"
    "${AIDL_SOURCE_DIR}/io_delegate.cpp"
    "${AIDL_SOURCE_DIR}/options.cpp"
    "${AIDL_SOURCE_DIR}/type_cpp.cpp"
    "${AIDL_SOURCE_DIR}/type_java.cpp"
    "${AIDL_SOURCE_DIR}/type_namespace.cpp"

    "${AIDL_SOURCE_DIR}/docs/aidl-cpp.md"
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
    "${AIDL_SOURCE_DIR}/main_java.cpp"
)
target_link_libraries(aidl aidl-common base)

add_executable(aidl-cpp
    "${AIDL_SOURCE_DIR}/main_cpp.cpp"
)
target_link_libraries(aidl-cpp aidl-common base)


add_executable(aidl_unittests
    "${AIDL_SOURCE_DIR}/aidl_unittest.cpp"
    "${AIDL_SOURCE_DIR}/ast_cpp_unittest.cpp"
    "${AIDL_SOURCE_DIR}/ast_java_unittest.cpp"
    "${AIDL_SOURCE_DIR}/generate_cpp_unittest.cpp"
    "${AIDL_SOURCE_DIR}/io_delegate_unittest.cpp"
    "${AIDL_SOURCE_DIR}/options_unittest.cpp"
    "${AIDL_SOURCE_DIR}/tests/end_to_end_tests.cpp"
    "${AIDL_SOURCE_DIR}/tests/fake_io_delegate.cpp"
    "${AIDL_SOURCE_DIR}/tests/main.cpp"
    "${AIDL_SOURCE_DIR}/tests/test_data_example_interface.cpp"
    "${AIDL_SOURCE_DIR}/tests/test_data_ping_responder.cpp"
    "${AIDL_SOURCE_DIR}/tests/test_util.cpp"
    "${AIDL_SOURCE_DIR}/type_cpp_unittest.cpp"
    "${AIDL_SOURCE_DIR}/type_java_unittest.cpp"
)
target_link_libraries(aidl_unittests aidl-common base gmock gtest)
