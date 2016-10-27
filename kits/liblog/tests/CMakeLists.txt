include_directories(
    "${EXTERNAL_GOOGLETEST_DIR}/googletest/include"
    "${EXTERNAL_GOOGLETEST_DIR}/googlemock/include"
)

add_executable(liblog_benchmarks
    "${LOG_SOURCE_DIR}/tests/benchmark.h"
    "${LOG_SOURCE_DIR}/tests/benchmark_main.cpp"
    "${LOG_SOURCE_DIR}/tests/liblog_benchmark.cpp"
)
target_link_libraries(liblog_benchmarks gtest log m)

add_executable(liblog_unit-tests
    "${LOG_SOURCE_DIR}/tests/liblog_test.cpp"
)
target_link_libraries(liblog_unit-tests gtest_main gtest log cutils)
set_target_properties(liblog_unit-tests PROPERTIES COMPILE_FLAGS "-fstack-protector-all")
