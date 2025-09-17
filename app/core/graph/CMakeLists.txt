file(GLOB_RECURSE SRC_FILES
    include/*.h
    src/*.cpp
)

add_library(
    graph
    STATIC
    ${SRC_FILES}
    ${UI_SRC}
)

target_link_libraries(graph
    PUBLIC
    Qt::Core
    Qt::Network
    QLogger
    git
)

target_include_directories(graph PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(BUILD_TESTING)
    target_include_directories(graph PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src)
endif()
