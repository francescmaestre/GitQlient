# GitQlient Library Helper Functions

# Usage: create_gitqlient_library(TARGET_NAME [HAS_UI] [DEPENDENCIES dep1 dep2 ...])
function(create_gitqlient_library TARGET_NAME)
    cmake_parse_arguments(ARGS "HAS_UI" "" "DEPENDENCIES" ${ARGN})

    # Collect source files
    file(GLOB_RECURSE SRC_FILES
        include/*.h
        src/*.h
        src/*.cpp
    )

    set(ALL_SOURCES ${SRC_FILES})

    # Handle UI files if needed
    if(ARGS_HAS_UI)
        file(GLOB_RECURSE UI_SRC ui/*.ui)
        set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)
        source_group("Forms" FILES ${UI_SRC})
        list(APPEND ALL_SOURCES ${UI_SRC})
    endif()

    # Create the library
    add_library(${TARGET_NAME} STATIC ${ALL_SOURCES})

    # Set standard include directories
    target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

    # Add testing include directories if BUILD_TESTING is enabled
    if(BUILD_TESTING)
        target_include_directories(${TARGET_NAME} PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/src
        )
    endif()

    # Link dependencies if provided
    if(ARGS_DEPENDENCIES)
        target_link_libraries(${TARGET_NAME} PUBLIC ${ARGS_DEPENDENCIES})
    endif()
endfunction()
