add_subdirectory(cache)
add_subdirectory(graph)
add_subdirectory(system)

add_library(
    core
    STATIC
    ${SRC_FILES}
    ${UI_SRC}
)

target_link_libraries(core
    PUBLIC
    Qt::Core
    Qt::Network
    QLogger
    git
    cache
    graph
    system
)

target_include_directories(
    core
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/cache/include
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/include
    ${CMAKE_CURRENT_SOURCE_DIR}/system/include)

if(BUILD_TESTING)
    target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src)
endif()
