add_subdirectory(Tests_Core)
add_subdirectory(Tests_Graph)

add_custom_target(unit_tests
    DEPENDS
        Tests_Core
        Tests_Weaver
        Tests_Strand
        Tests_Timeline
        Tests_NexusRegister
        Tests_TemporalLoom
    COMMAND ${CMAKE_CTEST_COMMAND} -L unit --output-on-failure
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Building and running all unit tests"
    VERBATIM
)
