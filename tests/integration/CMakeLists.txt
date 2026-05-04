add_subdirectory(Benchmarks)

add_custom_target(integration_tests
    DEPENDS BenchmarkLoader BenchmarkParseLog
    COMMAND ${CMAKE_CTEST_COMMAND} -L integration --output-on-failure
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Building and running all integration tests"
    VERBATIM
)
