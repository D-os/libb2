include_directories(
	${PROJECT_SOURCE_DIR}/external/googletest/googletest/include
	${PROJECT_SOURCE_DIR}/external/googletest/googlemock/include
)

add_executable(liblog_benchmarks
	benchmark.h
	benchmark_main.cpp
	liblog_benchmark.cpp
)
target_link_libraries(liblog_benchmarks gtest log m)

add_executable(liblog_unit-tests
	liblog_test.cpp
)
target_link_libraries(liblog_unit-tests gtest_main gtest log cutils)
set_target_properties(liblog_unit-tests PROPERTIES COMPILE_FLAGS "-fstack-protector-all")
