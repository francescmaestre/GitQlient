add_subdirectory(cache)
add_subdirectory(graph)
add_subdirectory(system)

add_library(
    core
    STATIC
    ${SRC_FILES}
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
    set_source_files_properties(ThemeManager_mac.mm PROPERTIES COMPILE_FLAGS "-fobjc-arc")
    target_link_libraries(core "-framework AppKit")
    target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src)
endif()
