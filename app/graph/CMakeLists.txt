file(GLOB SRC_FILES
    include/*.h
    src/*.cpp
)

add_library(
    Graph
    STATIC
    ${SRC_FILES}
)

target_link_libraries(Graph
    PUBLIC
    Qt::Core
    QLogger
)

target_include_directories(Graph PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
