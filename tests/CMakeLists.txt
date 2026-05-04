cmake_minimum_required(VERSION 3.21)

project(tests VERSION 0.1 LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui)

qt_standard_project_setup()

# ---------------------------------------------------------------------------
# GTest / GMock
# ---------------------------------------------------------------------------
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(gtest_build_tests OFF CACHE BOOL "" FORCE)
set(gtest_build_samples OFF CACHE BOOL "" FORCE)
add_subdirectory(deps/googletest)

# Shared GTest entry point — constructs QCoreApplication then calls RUN_ALL_TESTS().
set(TESTS_MAIN_SRC ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

# ---------------------------------------------------------------------------
# Helper: create a single-file GTest executable, register it with CTest,
# and attach a label so aggregate targets can filter by category.
#
# Usage: add_gtest_target(<TargetName> <SourceFile> <label>)
# ---------------------------------------------------------------------------
function(add_gtest_target name source label)
    add_executable(${name} ${source} ${TESTS_MAIN_SRC})
    add_test(NAME ${name} COMMAND ${name})
    set_tests_properties(${name} PROPERTIES LABELS ${label})
    target_link_libraries(${name} PRIVATE
        GTest::gtest
        GTest::gmock
        Qt::Core
        core
    )
    target_include_directories(${name} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/tests
    )
endfunction()

# ---------------------------------------------------------------------------
# Test suites
# ---------------------------------------------------------------------------
add_subdirectory(unit)
add_subdirectory(integration)
