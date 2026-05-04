find_package(Doxygen QUIET)

if(NOT DOXYGEN_FOUND)
    message(STATUS "Doxygen not found — 'docs' target will not be available")
    return()
endif()

set(DOXYGEN_OUTPUT_DIR "${CMAKE_BINARY_DIR}/docs")

configure_file(
    "${CMAKE_SOURCE_DIR}/Doxyfile.in"
    "${CMAKE_BINARY_DIR}/Doxyfile"
    @ONLY
)

add_custom_target(docs
    COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_BINARY_DIR}/Doxyfile"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "Generating Doxygen documentation..."
    VERBATIM
)

message(STATUS "Doxygen ${DOXYGEN_VERSION} found — run 'cmake --build . --target docs' to generate documentation")
