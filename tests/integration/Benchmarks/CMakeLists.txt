cmake_minimum_required(VERSION 3.21)
project(Benchmarks LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# BenchmarkLoader — end-to-end benchmark against a live git repository.
# Runs git log for real; measures the full load pipeline including I/O.
# ---------------------------------------------------------------------------
add_executable(BenchmarkLoader EXCLUDE_FROM_ALL BenchmarkLoader.cpp)

add_test(NAME BenchmarkLoader COMMAND BenchmarkLoader "${CMAKE_SOURCE_DIR}" --runs 1)
set_tests_properties(BenchmarkLoader PROPERTIES LABELS integration TIMEOUT 300)

target_link_libraries(BenchmarkLoader PRIVATE
    GTest::gtest
    GTest::gmock
    Qt::Core
    core
)

target_include_directories(BenchmarkLoader PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/tests
)

# ---------------------------------------------------------------------------
# BenchmarkParseLog — parsing-only benchmark.
# Captures git log output once (or reads a pre-recorded file), then replays
# the same byte-identical data through the parse + cache-build pipeline
# N times. Measures GitRepoLoader::onCommitLogReceived performance in
# isolation from disk I/O and git subprocess startup costs.
#
# Uses FakeLogRequestor (hand-written mock of AGitProcess) injected via
# GitRepoLoader::createLogRequestor() to avoid running a real git process
# during each timed iteration.
# ---------------------------------------------------------------------------
add_executable(BenchmarkParseLog EXCLUDE_FROM_ALL BenchmarkParseLog.cpp)

add_test(NAME BenchmarkParseLog COMMAND BenchmarkParseLog "${CMAKE_SOURCE_DIR}" --runs 1)
set_tests_properties(BenchmarkParseLog PROPERTIES LABELS integration TIMEOUT 300)

target_link_libraries(BenchmarkParseLog PRIVATE
    GTest::gtest
    GTest::gmock
    Qt::Core
    core
)

target_include_directories(BenchmarkParseLog PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/tests
)
