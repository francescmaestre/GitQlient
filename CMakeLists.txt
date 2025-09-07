cmake_minimum_required(VERSION 3.21)
project(GitQlient VERSION 2.0 LANGUAGES CXX)

option(EXCLUDE_TESTS "Exclude the tests fromt he build" OFF)

add_subdirectory(app)

if(!EXCLUDE_TESTS)
    add_subdirectory(tests)
endif()
