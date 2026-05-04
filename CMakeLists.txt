cmake_minimum_required(VERSION 3.21)
project(GitQlient VERSION 2.0 LANGUAGES CXX)

option(USE_NINJA "Use the Ninja generator (recommended on all platforms)" ON)
if(USE_NINJA AND NOT CMAKE_GENERATOR MATCHES "Ninja")
    message(WARNING
        "Generator '${CMAKE_GENERATOR}' is active but USE_NINJA=ON. "
        "Re-configure with -G Ninja or use a Ninja preset (cmake --preset debug). "
        "Pass -DUSE_NINJA=OFF to suppress this warning.")
endif()

option(EXCLUDE_TESTS "Exclude the tests from the build" OFF)
option(GIT_SUBMODULE "Automatically initialise git submodules if missing" ON)

add_subdirectory(app)

if(NOT EXCLUDE_TESTS)
    add_subdirectory(tests)
endif()

include(cmake/Doxygen.cmake)
